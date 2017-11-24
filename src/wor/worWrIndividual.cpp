/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/wor/worWrIndividual.cpp
 * Project:     wor: Web Output for The Family Pack Rec Library.
 * Purpose:     Create the Web Page text for the Individual.
 * Author:      Nick Matthews
 * Website:     http://thefamilypack.org
 * Created:     17th August 2016
 * Copyright:   Copyright (c) 2016 ~ 2017, Nick Matthews.
 * Licence:     GNU GPLv3
 *
 *  The Family Pack is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  The Family Pack is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with The Family Pack.  If not, see <http://www.gnu.org/licenses/>.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*/

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wor/worExport.h"

#include "wor/worWr.h"

#include <rec/recDb.h>


static recIdVec GetEventRefList( const wxString& refStr )
{
    recIdVec refIDs;
    size_t pos1 = 0, pos2 = 0, len;
    for(;;) {
        pos1 = refStr.find( 'R', pos1 );
        if( pos1 == wxString::npos ) {
            break;
        }
        pos1++;
        pos2 = refStr.find( ',', pos1 );
        len = pos2 == wxString::npos ? pos2 : pos2 - pos1; 
        idt refID = recGetID( refStr.substr( pos1, len ) );
        if( refID != 0 ) {
            refIDs.push_back( refID );
        }
    }

    return refIDs;
}

wxString worExport::OutputEventRow( idt eveID ) const
{
    if( eveID == 0 ) {
        return "";
    }
    recEvent eve(eveID);

    wxString htm =
        "<tr>\n"
        "<td class='core'><b>" + eve.GetTypeStr() + "</b></td>\n"
        "<td class='core'>\n" + eve.GetDetailStr() + "\n"
    ;
    if( m_includeRef ) {
        recIdVec refIDs = GetEventRefList( eve.FGetUserRef() );
        htm += WrRefList( refIDs );
    }
    if( !eve.FGetNote().empty() ) {
        htm += "<br /><b>Note:</b> " + eve.FGetNote() + "\n";
    }
    htm += "\n</td>\n</tr>\n";
    return htm;
}

wxString worExport::HtmlifyNotes( const wxString& note ) const
{
    wxString str;
    size_t pos = 0;
    size_t pos1 = note.find( "[R" );
    while( pos1 != wxString::npos ) {
        size_t pos2 = note.find( "]", pos1 );
        if( pos2 != wxString::npos ) {
            idt refID = recGetID( note.substr( pos1+2, pos2-pos1-2 ) );
            if( refID != 0 ) {
                str += note.substr( pos, pos1-pos );
                str +=
                    "<sup>[<a href='" + GetRefFileName( refID ) + 
                    "'>R" + recGetIDStr( refID ) + "</a>]</sup>"
                ;
                pos = pos2+1;
            }
        }
        pos1 = note.find( "[R", pos2 );
    }
    if( pos != 0 ) {
        str.Replace( "</sup><sup>", "" );
        str.Replace( "</sup>,<sup>", ", " );
        str.Replace( "</sup>, <sup>", ", " );
        str.Replace( "</sup> <sup>", ", " );
    }
    str += note.substr( pos );
    str.Trim( true );
    str.Trim( false );
    str.Replace( "\n", "<br />\n" );
    return str;
}

wxString worExport::GetImageText( int image ) const
{
    wxString text;
    int idir = ((image - 1) / 500 ) + 1; // Folder number
    int isub = ((image - 1) / 100 ) % 5; // Folder sub-number
    wxString fname = wxString::Format( 
        "%s\\im%02d%c\\im%05d.txt", m_imageTextPath, idir, 'a'+isub, image
    );
    wxFFile imfile;
    if( imfile.Open( fname ) ) {
        imfile.ReadAll( &text );
    }
    return text;
}

wxString worExport::GetPhotoPanel( int image, const wxString& title ) const
{
    wxString htm;
    long chkImage, gallery;

    wxString text = GetImageText( image );
    if( text.size() == 0 ) return htm;

    size_t pos1 = text.find( " G" );
    wxASSERT( pos1 != wxString::npos );
    size_t pos2 = text.find( ' ', pos1+2 );
    wxASSERT( pos2 != wxString::npos );
    size_t pos3 = text.find( '\n', pos2 );
    wxASSERT( pos3 != wxString::npos );

    wxString chkImStr = text.Mid( 2, pos1-2 );
    chkImStr.ToLong( &chkImage );
    wxASSERT( image == (int) chkImage );
    wxString galStr = text.Mid( pos1+2, pos2-pos1-2 );
    galStr.ToLong( &gallery );
    wxString desc = text.Mid( pos3 );
    desc.Trim( true );
    desc.Trim( false );
    wxString galFName = GetGalFileName( gallery );
    wxString imFName = GetImgFileName( image );

    htm << "<table class='image'>\n"
           "<tr><td colspan='2' class='pictitle'>"
           "<span class='refnum'>Im" << image 
        << " &nbsp; <a href='" << galFName << "'>G" << gallery 
        << "</a></span>" << title << "</td></tr>\n"
           "<tr><td class='image'><a href='" << imFName 
        << "'><img src='" << imFName << "' alt='' height='200' /></a></td><td>\n"
        << desc << "</td></tr></table>\n";

    return htm;
}

wxString worExport::HtmlifyPhotos( const wxString& text ) const
{
    wxString htm;
    size_t pos0 = 0, pos1, pos2, pos3;
    long image = 0;
    bool start = true;

    for(;;) {
        pos1 = text.find( "#Photo Im", pos0 );
        if( pos1 != wxString::npos ) {
            pos2 = text.find( ' ', pos1+9 );
            wxASSERT( pos2 != wxString::npos );
            wxString inum = text.Mid( pos1+9, pos2-pos1-9 );
            inum.ToLong( &image );
            pos3 = text.find( "\n", pos2 );
            wxASSERT( pos3 != wxString::npos );
            wxString title = text.Mid( pos2, pos3-pos2 );
            title.Trim( false ); // Left trim
            htm += GetPhotoPanel( image, title );
            pos0 = pos3;
        } else {
            break;
        }
    }
    htm.Trim( true );
    htm.Trim( false );
    return htm;
}

void worExport::OutputIndividualPage( idt indID ) const
{
    wxString filename = GetIndFileName( indID, 0 );
    StringVec css;
    css.push_back( "xmenu" );
    css.push_back( "ind" );
    css.push_back( "timage" );
    wxString menuBar = GetMenuBar( 1 );

    recIndividual ind(indID);
    wxString indLink = GetFamFileName( ind.FGetFamID() );

    wxString htm = 
        GetHeader( 1, "Individual " + ind.GetIdStr(), css ) +
        "<h1>" + ind.FGetName() + "</h1>\n"
        "<div id='topmenu'>\n" + menuBar + "</div>\n<br />\n"
        // Start contents
        "<table class='core'>\n"
        "<tr>\n"
        "<td class='core'><b>" + ind.GetIdStr() +
        "</b>, " + recGetSexStr( ind.FGetSex() ) + "</td>\n"
        "<td class='core " + GetSexClass( indID ) +
        "'>" + WrXMenuFamily( indID ) +
        "<a href='" + indLink + "'>" + ind.FGetName() +
        "</a> " + ind.FGetEpitaph() + "</td>\n"
        "</tr>\n"
    ;
    recNameVec names = ind.GetNames();
    for ( size_t i = 1; i < names.size(); i++ ) {
        htm +=
            "<tr>\n"
            "<td class='core'><b>" + names[i].GetTypeStr() + " Name</b></td>\n"
            "<td class='core'>" + names[i].GetNameStr() + "</td>\n"
            "</tr>\n"
        ;
    }
    htm += OutputEventRow( ind.GetBirthEvent() );
    recIdVec eveIDs = ind.FindEvents( recET_GRP_NrBirth );
    for( size_t i = 0 ; i < eveIDs.size() ; i++ ) {
        htm += OutputEventRow( eveIDs[i] );
    }
    
    htm += OutputEventRow( ind.GetDeathEvent() );
    eveIDs = ind.FindEvents( recET_GRP_NrDeath );
    for( size_t i = 0 ; i < eveIDs.size() ; i++ ) {
        htm += OutputEventRow( eveIDs[i] );
    }

    idt occID = ind.GetPersonalSummaryEvent( recEventType::ET_Occupation );
    wxString occ = recIndividualEvent::GetRoleStr( indID, recEventType::ET_Occupation );
    if( !occ.empty() ) {
        htm +=
            "<tr>\n"
            "<td class='core'><b>Occupation</b></td>\n"
            "<td class='core'>" + occ
        ;
        recEvent eve(occID);
        if(  m_includeRef ) {
            recIdVec refIDs = GetEventRefList( eve.FGetUserRef() );
            htm += WrRefList( refIDs );
        }
        htm += "</td>\n</tr>\n";
    }

    recFamilyVec fams = ind.GetParentList();
    for( size_t i = 0 ; i < fams.size() ; i++ ) {
        if( fams[i].FGetHusbID() ) {
            htm +=
                "<tr>\n<td class='core'><b>Father</b></td>\n" +
                WrIndNameCell( fams[i].FGetHusbID() ) +
                "</tr>\n"
            ;
        }
        if( fams[i].FGetWifeID() ) {
            htm +=
                "<tr>\n<td class='core'><b>Mother</b></td>\n" +
                WrIndNameCell( fams[i].FGetWifeID() ) +
                "</tr>\n"
            ;
        }
    }
    fams = ind.GetFamilyList();
    for( size_t i = 0 ; i < fams.size() ; i++ ) {
        idt spouseID = fams[i].GetSpouseID( indID );
        recIdVec childIDs = recFamily::GetChildrenIds( fams[i].FGetID() );
        if( spouseID == 0 && childIDs.empty() ) {
            continue;
        }
        htm +=
            "<tr>\n<td colspan='2' class='core core2'>\n"
            "<table class='core2'>\n"
            "<tr>\n<td class='core head' colspan='2'><b>Family " + recGetStr( i+1 ) +
            "</b></td>\n</tr>\n"
        ;

        eveIDs = recFamily::GetEventIDs( fams[i].FGetID() );
        for( size_t j = 0 ; j < eveIDs.size() ; j++ ) {
            htm += OutputEventRow( eveIDs[j] );
        }
        
        htm +=
            "<tr>\n"
            "<td class='core'><b>" + fams[i].GetIdStr() +
            "</b></td>\n" + WrIndNameCell( spouseID ) +
            "</tr>\n"
        ;

        htm += OutputEventRow( recIndividual::GetBirthEvent( spouseID ) );
        eveIDs = recIndividual::FindEvents( spouseID, recET_GRP_NrBirth );
        for( size_t j = 0 ; j < eveIDs.size() ; j++ ) {
            htm += OutputEventRow( eveIDs[j] );
        }
        
        htm += OutputEventRow( recIndividual::GetDeathEvent( spouseID ) );
        eveIDs = recIndividual::FindEvents( spouseID, recET_GRP_NrDeath );
        for( size_t j = 0 ; j < eveIDs.size() ; j++ ) {
            htm += OutputEventRow( eveIDs[j] );
        }

        idt occID = recIndividual::GetPersonalSummaryEvent( spouseID, recEventType::ET_Occupation );
        wxString occ = recIndividualEvent::GetRoleStr( spouseID, recEventType::ET_Occupation );
        if( !occ.empty() ) {
            htm +=
                "<tr>\n"
                "<td class='core'><b>Occupation</b></td>\n"
                "<td class='core'>" + occ
            ;
            if( m_includeRef ) {
                recEvent eve(occID);
                recIdVec refIDs = GetEventRefList( eve.FGetUserRef() );
                htm += WrRefList( refIDs );
            }
            htm += "</td>\n</tr>\n";
        }

        for( size_t j = 0 ; j < childIDs.size() ; j++ ) {
            htm += "<tr>\n";
            if( j == 0 ) {
                wxString rowspan;
                if( childIDs.size() > 1 ) {
                    rowspan = wxString::Format( " rowspan='%u'", childIDs.size() );
                }
                htm += "<td class='core'" + rowspan + "><b>Children</b></td>\n";
            }
            htm += WrIndNameCell( childIDs[j] ) + "</tr>\n";
        }

        htm += "</table>\n</td>\n</tr>\n";
    }

    size_t pos = ind.FGetNote().find( "#Photo Im" );
    wxString notes = HtmlifyNotes( ind.FGetNote().substr( 0, pos ) );
    if( !notes.empty() ) {
        htm +=
            "<tr>\n<td class='core head' colspan='2'><b>Notes</b></td>\n</tr>\n"
            "<tr>\n<td class='core' colspan='2'>\n" + notes + "\n</td>\n</tr>\n"
        ;
    }
    wxString photos;
    if( pos != wxString::npos ) {
        photos = HtmlifyPhotos( ind.FGetNote().substr( pos ) + "\n" );
    };
    if( !photos.empty() && !m_imageTextPath.empty() ) {
        htm +=
            "<tr>\n<td class='core head' colspan='2'><b>Photos</b></td>\n</tr>\n"
            "<tr>\n<td class='core' colspan='2'>\n<br />\n" + photos + "\n</td>\n</tr>\n"
        ;
    }
    // core done
    htm +=
        "</table><br />\n"
        // Ends contents
        "<div id='botmenu'>\n" + menuBar + "</div>\n"
        + GetFooter( ind.GetIdStr() )
    ;
    OutputHtmFile( filename, htm );
}

// End of src/wor/worWrFamily.cpp Source
