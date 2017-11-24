/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/wor/worExport.h
 * Project:     wor: Web Output for The Family Pack Rec Library.
 * Purpose:     Manage the files to be exported, class source.
 * Author:      Nick Matthews
 * Website:     http://thefamilypack.org
 * Created:     30th October 2015
 * Copyright:   Copyright (c) 2015 ~ 2017, Nick Matthews.
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
#include "worFiles.h"

#include <wx/fileconf.h>

#include <rec/recDb.h>

worExport::worExport( const wxString& outpath, const wxString& version )
    : m_outpath(outpath),
    m_includeHome(false), m_includeInd( false ), m_includeNames( false ),
    m_includeRef(false),
    m_privacy(100), m_dataPrivacy(100), m_version(version)
{
    m_homeFamPath = GetFamFileName( 1, 0 );
}

bool worExport::InitMenuBar( const wxFileConfig& config )
{
    m_menuBar[MB_Home] = config.Read( "/MenuBar/Home", "Home" );
    m_menuBar[MB_Family] = config.Read( "/MenuBar/Family", "" );
    wxString path = GetFamFileName( config.ReadLong( "/MenuBar/FamilyID", 1 ), 0 );
    if( !path.empty() ) {
        m_homeFamPath = path;
    }
    m_menuBar[MB_Name] = config.Read( "/MenuBar/Name", "" );
    m_menuBar[MB_Gallery] = config.Read( "/MenuBar/Gallery", "" );
    m_menuBar[MB_Chart] = config.Read( "/MenuBar/Chart", "" );
    m_menuBar[MB_Source] = config.Read( "/MenuBar/Source", "" );
    m_menuBar[MB_Research] = config.Read( "/MenuBar/Research", "" );
    return true;
}

bool worExport::ExportPages()
{
    if( m_includeHome ) {
        OutputHomePage();
    }
    recIdVec famIDs = recFamily::GetFamilyIdVec();
    for( size_t i = 0 ; i < famIDs.size() ; i++ ) {
        OutputFamilyPage( famIDs[i] );
    }
    if( m_includeInd ) {
        recIdVec indIDs = recIndividual::GetIdVec();
        for( size_t i = 0 ; i < indIDs.size() ; i++ ) {
            OutputIndividualPage( indIDs[i] );
        }
        OutputSysFile( "sys/ind.css", worFile_ind_css );
    }
    if ( m_includeNames ) {
        OutputSurnameIndex();
    }
    OutputSysFile( "sys/livery.css", worFile_livery_css );
    OutputSysFile( "sys/fam.css", worFile_fam_css );
    OutputSysFile( "sys/xmenu.css", worFile_xmenu_css );
    OutputSysFile( "sys/tlist.css", worFile_tlist_css );
    OutputSysFile( "sys/timage.css", worFile_timage_css );

    OutputBinFile( "sys/bg.png", wor_bg_png, wor_sizeof_bg_png );
    OutputBinFile( "sys/club.png", wor_club_png, wor_sizeof_club_png );
    OutputBinFile( "sys/diamond.png", wor_diamond_png, wor_sizeof_diamond_png );
    OutputBinFile( "sys/heart.png", wor_heart_png, wor_sizeof_heart_png );
    OutputBinFile( "sys/spade.png", wor_spade_png, wor_sizeof_spade_png );
    OutputBinFile( "sys/family.png", wor_family_png, wor_sizeof_family_png );

    return true;
}

wxString worExport::GetBasePath( unsigned level ) const
{
    wxString basepath;
    for( unsigned i = 0 ; i < level ; i++ ) {
        basepath += "../";
    }
    return basepath;
}

wxString worExport::GetIndFileName( idt indID, unsigned level ) const
{
    if( m_privacy != 100 ) {
        if( m_privacy > recIndividual::GetPrivacy( indID ) ) {
            return "";
        }
    }
    int num = ( ( indID - 1 ) % 500 ) + 1;
    int dir = ( ( indID - 1 ) / 500 ) + 1;
    return GetBasePath( level )
        + wxString::Format( "ps%02d/ps%02d_%03d.htm", dir, dir, num );
}

wxString worExport::GetFamFileName( idt famID, unsigned level ) const
{
    if( m_privacy != 100 ) {
        recFamily fam(famID);
        if( recIndividual::GetPrivacy( fam.FGetHusbID() ) > 0 ) {
            int x = 0;
        }
        if( m_privacy < recIndividual::GetPrivacy( fam.FGetHusbID() ) ||
            m_privacy < recIndividual::GetPrivacy( fam.FGetWifeID() ) ) 
        {
            return "";
        }
    }
    int num = ( ( famID - 1 ) % 500 ) + 1;
    int dir = ( ( famID - 1 ) / 500 ) + 1;
    return GetBasePath( level )
        + wxString::Format( "wc%02d/wc%02d_%03d.htm", dir, dir, num );
}

wxString worExport::GetFileName( idt id, const wxString& code, const wxString& ext, unsigned level ) const
{
    if( id == 0 ) {
        return "";
    }
    int num = int( id );
    int dir = ( ( id - 1 ) / 500 ) + 1;
    return GetBasePath( level )
        + wxString::Format( "%s%02d/%s%05d.%s", code, dir, code, num, ext );
}

wxString worExport::GetNListFileName( const wxString & letter, unsigned level ) const
{
    char ch = tolower( letter[0] );
    wxString cstr(ch);
    if ( ch == '?' ) {
        cstr = "qm";
    }
    return GetBasePath( level ) + wxString::Format( "ind/n_%s.htm", cstr );
}

wxString worExport::GetHeader(
    unsigned level, const wxString& title,
    const StringVec& css, const StringVec& js
    ) const
{
    wxString base = GetBasePath( level );
    wxString htm =
        "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\""
        " \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n"
        "<html xmlns='http://www.w3.org/1999/xhtml'>\n"
        "<head>\n<title>" + title + "</title>\n"
        "<meta http-equiv='Content-Type' content='text/html;charset=utf-8' />\n"
        "<meta name='Generator' content='The Family Pack " + m_version + "' />\n" 
        "<link rel='stylesheet' type='text/css' href='" + base + "sys/livery.css' />\n"
    ;
    for( size_t i = 0 ; i < css.size() ; i++ ) {
        htm += "<link rel='stylesheet' type='text/css' href='"
            + base + "sys/" + css[i] + ".css' />\n";
    }
    for( size_t i = 0 ; i < js.size() ; i++ ) {
        htm += "<script type='text/javascript' src='"
            + base + "sys/" + js[i] + ".js' /></script>\n";
    }
    htm += "</head>\n<body>\n";
    return htm;
}

wxString worExport::GetFooter( const wxString& pageref ) const
{
    return
        "<div id='created-by'>Created by: <br />\n"
        "<b><em><a href='http://thefamilypack.org'>The Family Pack</a></em> - "
        + m_version + "</b></div>\n"
        "<div id='page-ref'>" + pageref + "</div>\n"
        "<div id='create-date'>" + GetTodayStr() + "</div>\n"
        "</body>\n</html>\n"
    ;
}

wxString worExport::GetMenuBar( unsigned level, const wxString& thispage ) const
{
    wxString basepath;
    for( unsigned i = 0 ; i < level ; i++ ) {
        basepath += "../";
    }
    wxString menu = "<span class='hmenu'\n ><a ";

    if( m_menuBar[MB_Home] == thispage ) {
        menu += "class='dummy' href='javascript:void(0)'";
    } else {
        menu += "href='" + basepath + "index.htm'";
    }
    menu += ">" + m_menuBar[MB_Home] + "</a\n";

    if( !m_menuBar[MB_Family].empty() ) {
        menu += " ><a ";
        if( "Family" == thispage ) {
            menu += "class='dummy' href='javascript:void(0)'";
        } else {
            menu += "href='" + basepath + m_homeFamPath + "'";
        }
        menu += ">" + m_menuBar[MB_Family] + "</a\n";
    }

    if( !m_menuBar[MB_Name].empty() ) {
        menu += " ><a ";
        if( m_menuBar[MB_Name] == thispage ) {
            menu += "class='dummy' href='javascript:void(0)'";
        } else {
            menu += "href='" + basepath + "ind/n_index.htm'";
        }
        menu += ">" + m_menuBar[MB_Name] + "</a\n";
    }

    if( !m_menuBar[MB_Gallery].empty() ) {
        menu += " ><a ";
        if( m_menuBar[MB_Gallery] == thispage ) {
            menu += "class='dummy' href='javascript:void(0)'";
        } else {
            menu += "href='" + basepath + "ind/g_index.htm'";
        }
        menu += ">" + m_menuBar[MB_Gallery] + "</a\n";
    }

    if( !m_menuBar[MB_Chart].empty() ) {
        menu += " ><a ";
        if( m_menuBar[MB_Chart] == thispage ) {
            menu += "class='dummy' href='javascript:void(0)'";
        } else {
            menu += "href='" + basepath + "ind/c_index.htm'";
        }
        menu += ">" + m_menuBar[MB_Chart] + "</a\n";
    }

    if( !m_menuBar[MB_Source].empty() ) {
        menu += " ><a ";
        if( m_menuBar[MB_Source] == thispage ) {
            menu += "class='dummy' href='javascript:void(0)'";
        } else {
            menu += "href='" + basepath + "ind/r_index.htm'";
        }
        menu += ">" + m_menuBar[MB_Source] + "</a\n";
    }

    if( !m_menuBar[MB_Research].empty() ) {
        menu += " ><a ";
        if( m_menuBar[MB_Research] == thispage ) {
            menu += "class='dummy' href='javascript:void(0)'";
        } else {
            menu += "href='" + basepath + "x/index.htm'";
        }
        menu += ">" + m_menuBar[MB_Research] + "</a\n";
    }

    return menu + " >\n</span>\n";
}

wxString worExport::WrXMenuDiamond( idt indID ) const
{
    wxString mnu;
    recFamilyVec fams = recIndividual::GetParentList( indID );
    if( fams.size() ) {
        mnu << 
            "<div class='xmenu'>\n"
            "<img src='../sys/diamond.png' alt='Menu' />\n"
            "<div class='ops'>\n"
            "<div class='x2item x2title'>Parents</div>\n"
        ;
        int options = 0;
        for( size_t i = 0 ; i < fams.size() ; i++ ) {
            wxString link = GetFamFileName( fams[i].FGetID() );
            if( link.empty() ) {
                continue;
            }
            if( fams[i].FGetHusbID() ) {
                mnu <<
                    "<a href='" << link <<
                    "'>" << recIndividual::GetName( fams[i].FGetHusbID() ) <<
                    "</a>\n"
                ;
                options++;
            }
            if( fams[i].FGetWifeID() ) {
                mnu <<
                    "<a href='" << link <<
                    "'>" << recIndividual::GetName( fams[i].FGetWifeID() ) <<
                    "</a>\n"
                ;
                options++;
            }
        }
        if( options == 0 ) {
            return "";
        }
        mnu << "</div>\n</div>\n";
    }
    return mnu;
}

wxString worExport::WrXMenuSpade( idt indID ) const
{
    wxString mnu;
    recFamilyVec fams = recIndividual::GetParentList( indID );
    if( fams.size() == 0 ) {
        return "";
    }
    recIndividualList inds = fams[0].GetChildren();
    if( inds.size() < 2 ) {
        return "";
    }

    mnu << 
        "<div class='xmenu'>\n"
        "<img src='../sys/spade.png' alt='Menu' />\n"
        "<div class='ops'>\n"
        "<div class='x2item x2title'>Siblings</div>\n"
    ;
    int priv = 0, options = 0;
    for( size_t i = 0 ; i < inds.size() ; i++ ) {
        wxString link = GetFamFileName( inds[i].FGetFamID() );
        if( link.empty() ) {
            priv++;
            continue;
        }
        if( inds[i].FGetID() == indID ) {
            mnu << "<span class='dummy'>" << inds[i].FGetName() << "</span>\n";
        } else {
            mnu <<
                "<a href='" << link <<
                "'>" << inds[i].FGetName() <<
                "</a>\n"
            ;
            options++;
        }
    }
    if( options == 0 ) {
        return "";
    }
    if( priv > 0 ) {
        wxString name = priv == 1 ? "Individual" : "Individuals";
        mnu << "<span class='dummy'>+" << priv << " Private " << name << "</span>\n";
    }
    mnu << "</div>\n</div>\n";
    return mnu;
}

wxString worExport::WrXMenuHeart( idt indID, size_t min_size ) const
{
    recFamilyVec fams = recIndividual::GetFamilyList( indID );
    recIdVec partners;
    recIdVec famIDs;
    for( size_t i = 0 ; i < fams.size() ; i++ ) {
        idt id = fams[i].FGetHusbID();
        if( id != indID && id != 0 ) {
            partners.push_back( id );
            famIDs.push_back( fams[i].FGetID() );
        }
        id = fams[i].FGetWifeID();
        if( id != indID && id != 0  ) {
            partners.push_back( id );
            famIDs.push_back( fams[i].FGetID() );
        }
    }
    wxString mnu;
    if( partners.size() >= min_size ) {
        mnu += 
            "<div class='xmenu'>\n"
            "<img src='../sys/heart.png' alt='Menu' />\n"
            "<div class='ops'>\n"
            "<div class='x2item x2title'>Partners</div>\n"
        ;

        for( size_t i = 0 ; i < partners.size() ; i++ ) {
            wxString name, link = GetFamFileName( famIDs[i] );
            if( partners[i] == 0 ) {
                name = "[Unknown]";
            } else {
                if( link.empty() ) {
                    name = "[Private]";
                } else {
                    name = recIndividual::GetName( partners[i] );
                }
            }
            if( link.empty() ) {
                mnu += "<span class='dummy'>" + name + "</span>\n";
            } else {
                mnu += "<a href='" + link + "'>" + name + "</a>\n";
            }
        }
        mnu += "</div>\n</div>\n";
    }
    return mnu;
}

wxString worExport::WrXMenuClub( idt indID, idt blankIndID, idt blankFamID ) const
{
    wxString mnu;
    recFamilyVec fams = recIndividual::GetFamilyList( indID );
    recIdVec children;
    recIdVec famIDs;
    int priv = 0, options = 0;
    for( size_t i = 0 ; i < fams.size() ; i++ ) {
        idt famID = fams[i].FGetID();
        if( famID != blankFamID ) {
            recIdVec cIDs = recFamily::GetChildrenIds( famID );
            for( size_t j = 0 ; j < cIDs.size() ; j++ ) {
                children.push_back( cIDs[j] );
                famIDs.push_back( recIndividual::GetFamilyID( cIDs[j] ) );
            }
        }
    }
    if( children.size() > 0 ) {
        mnu << 
            "<div class='xmenu'>\n"
            "<img src='../sys/club.png' alt='Menu' />\n"
            "<div class='ops'>\n"
            "<div class='x2item x2title'>Children</div>\n"
        ;
        for( size_t i = 0 ; i < children.size() ; i++ ) {
            wxString link = GetFamFileName( famIDs[i] );
            if( link.empty() ) {
                priv++;
                continue;
            }
            if( children[i] == blankIndID ) {
                mnu <<
                    "<span class='dummy'>"
                    << recIndividual::GetName( children[i] ) << "</span>\n"
                ;
            } else {
                mnu <<
                    "<a href='" << GetFamFileName( famIDs[i] ) <<
                    "'>" << recIndividual::GetName( children[i] ) <<
                    "</a>\n"
                ;
                options++;
            }
        }
        if( options == 0 ) {
            return "";
        }
        if( priv > 0 ) {
            wxString name = priv == 1 ? "Individual" : "Individuals";
            mnu << "<span class='dummy'>+" << priv << " Private " << name << "</span>\n";
        }
        mnu << "</div>\n</div>\n";
    }
    return mnu;
}

wxString worExport::WrXMenuFamily( idt indID, unsigned flags, idt blankIndID, idt blankFamID ) const
{
    wxString diamond, spade, heart, club;
    int count = 0;
    if( flags & X2MF_IncDiamond ) {
        diamond = WrXMenuDiamond( indID );
        if( !diamond.empty() ) count++;
    }
    if( flags & X2MF_IncSpade ) {
        spade = WrXMenuSpade( indID );
        if( !spade.empty() ) count++;
    }
    if( flags & X2MF_IncHeart ) {
        heart = WrXMenuHeart( indID, 1 );
        if( !heart.empty() ) count++;
    } else if( flags & X2MF_IncHeart2 ) {
        heart = WrXMenuHeart( indID, 2 );
        if( !heart.empty() ) count++;
    }
    if( flags & X2MF_IncClub ) {
        club = WrXMenuClub( indID, blankIndID, blankFamID );
        if( !club.empty() ) count++;
    }
    if( count == 0 ) {
        return "";
    }
    if( count == 1 && ! (flags & X2MF_IncFamily) ) {
        return diamond + spade + heart + club;
    }

    return 
        "<div class='x2menu'>\n"
        "<a class='default' href='" +
        GetFamFileName( recIndividual::GetFamilyID( indID ) ) +
        "'><img src='../sys/family.png' alt='Menu' /></a>\n"
        "<div class='x2ops'>\n"
        "<div class='x2item x2title'>\n" +
        diamond + spade + heart + club +
        "Family\n"
        "</div>\n"
        "</div>\n"
        "</div>\n"
    ;
}

wxString worExport::WrIndNameCell( idt indID ) const
{
    if ( indID == 0 ) {
        return "<td class='core neut'>[Partner unknown]</td>\n";
    }
    return
        "<td class='core " + GetSexClass( indID ) +
        "'>\n" + WrXMenuFamily( indID, X2MF_All ) + 
        "<a href='" + GetIndFileName( indID ) +
        "'>"  + recIndividual::GetNameStr( indID ) +
        "</a> " + recIndividual::GetEpitaph( indID ) + "\n</td>\n"
    ;
}

wxString worExport::WrRefList( const recIdVec& refIDs ) const
{
    wxString htm;
    if( !refIDs.empty() ) {
        htm = "<sup>\n";
        for( size_t i = 0 ; i < refIDs.size() ; i++ ) {
            recReference ref( refIDs[i] );
            idt refID;
            if( ref.FGetUserRef().empty() ) {
                refID = refIDs[i];
            } else {
                refID = recGetID( ref.FGetUserRef().substr( 2 ) );
            }
            wxString link = GetRefFileName( refID );
            if( i > 0 ) {
                htm += ",";
            }
            htm +=
                "\n[<a href='" + link + 
                "'>" + recReference::GetIdStr( refID ) +
                "</a>]"
            ;
        }
        htm += "</sup>\n";
    }
    return htm;
}

void worExport::OutputHtmFile( const wxString& name, const wxString& content ) const
{
    wxFileName fname( name );
    fname.Normalize( wxPATH_NORM_ALL, m_outpath );
    fname.Mkdir( wxS_DIR_DEFAULT, wxPATH_NORM_ALL );
    wxFFile outfile( fname.GetFullPath(), "w" );
    if( !outfile.IsOpened() ) return;

    outfile.Write( content );

    outfile.Close();
}

void worExport::OutputSysFile( const wxString& name, const char* text ) const
{
    wxFileName fname( name );
    fname.Normalize( wxPATH_NORM_ALL, m_outpath );
    fname.Mkdir( wxS_DIR_DEFAULT, wxPATH_NORM_ALL );
    wxFFile outfile( fname.GetFullPath(), "w" );
    if( !outfile.IsOpened() ) return;

    outfile.Write( text );

    outfile.Close();
}

void worExport::OutputBinFile( const wxString& name, const unsigned char* data, size_t size ) const
{
    wxFileName fname( name );
    fname.Normalize( wxPATH_NORM_ALL, m_outpath );
    fname.Mkdir( wxS_DIR_DEFAULT, wxPATH_NORM_ALL );
    wxFFile outfile( fname.GetFullPath(), "wb" );
    if( !outfile.IsOpened() ) return;

    outfile.Write( data, size );

    outfile.Close();
}

void worExport::OutputHomePage() const
{
    wxString menuBar = GetMenuBar( 0, "Home" );

    wxString htm = 
        GetHeader( 0, "Home" ) +
        "<h1>Home Page</h1>\n"
        "<div id='topmenu'>\n" + menuBar + "</div>\n<br />\n"
        "<h2>Introduction</h2>"
        "<p>Under construction</p>"
        "<div id='botmenu'>\n" + menuBar + "</div>\n"
        + GetFooter( "Home" )
    ;
    OutputHtmFile( "index.htm", htm );
}

// End of src/wor/worExport.cpp Source
