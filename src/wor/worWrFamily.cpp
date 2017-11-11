/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/wor/worWrFamily.cpp
 * Project:     wor: Web Output for The Family Pack Rec Library.
 * Purpose:     Create the Family Web Page text.
 * Author:      Nick Matthews
 * Website:     http://thefamilypack.org
 * Created:     15th December 2012
 * Copyright:   Copyright (c) 2012 - 2015, Nick Matthews.
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

void worExport::OutputFamilyPage( idt famID ) const
{
    wxString filename = GetFamFileName( famID, 0 );
    if( filename.empty() ) {
        return;
    }
    wxString thispage, link, name;
    if( filename == m_homeFamPath ) {
        thispage = "Family";
    }
    wxString menuBar = GetMenuBar( 1, thispage );
    recFamily fam(famID);
    recIndividual husb( fam.FGetHusbID() );
    recIndividual wife( fam.FGetWifeID() );
    recFamilyVec husbFams = husb.GetParentList();
    recFamilyVec wifeFams = wife.GetParentList();

    idt hIndID = husb.FGetID();
    idt wIndID = wife.FGetID();

    recIndividualList kids = fam.GetChildren();
    recFamilyVec husbWives = recIndividual::GetFamilyList( hIndID );
    recFamilyVec wifeHusbs = recIndividual::GetFamilyList( wIndID );

    StringVec css;
    css.push_back( "xmenu" );
    css.push_back( "fam" );
    wxString heading = recIndividual::GetName( hIndID );
    if( hIndID && wIndID ) {
        heading += " and ";
    }
    heading += recIndividual::GetName( wIndID );

    wxString htm = 
        GetHeader( 1, "Family " + recFamily::GetIdStr( famID ), css ) +
        "<h1>" + heading + "</h1>\n"
        "<div id='topmenu'>\n" + menuBar + "</div>\n<br />\n"
        // Start contents
        "<table class='core'>\n"
        // Parents
        "<tr>\n<td class='frame frame-top'>\n"
        "<table class='parent'>\n"
    ;

    // Husband's Parents
    for( size_t i = 0 ; i < husbFams.size() ; i++ ) {
        if( i > 0 ) {
            htm += "<tr>\n<td class='status'>\nAdditional Parents:\n</td>\n</tr>\n";
        }
        // Hubands Father
        if( husbFams[i].FGetHusbID() ) {
            link = GetFamFileName( husbFams[i].FGetID() );
            if( link.empty() ) {
                name = "<scan class='dummy'>" +
                    recIndividual::GetName( husbFams[i].FGetHusbID() ) +
                    "</scan>\n";
            } else {
                name = "<a href='" + link + "'>" +
                    recIndividual::GetName( husbFams[i].FGetHusbID() ) + "</a>\n";
            }
            htm += "<tr>\n<td class='" +
                GetSexClass( husbFams[i].FGetHusbID(), SEX_Male ) + "'>\n" +
                WrXMenuFamily( husbFams[i].FGetHusbID(), X2MF_Default, fam.FGetHusbID() ) +
                name + "<br />\n" +
                recIndividual::GetEpitaph( husbFams[i].FGetHusbID() ) +
                "\n</td>\n</tr>\n";
        } else {
            htm += "<tr>\n<td class='male'>&nbsp;</td>\n</tr>\n";
        }
        // Hubands Mother
        if( husbFams[i].FGetWifeID() ) {
            link = GetFamFileName( husbFams[i].FGetID() );
            if( link.empty() ) {
                name = "<scan class='dummy'>" +
                    recIndividual::GetName( husbFams[i].FGetWifeID() ) +
                    "</scan>\n";
            } else {
                name = "<a href='" + link + "'>" +
                    recIndividual::GetName( husbFams[i].FGetWifeID() ) + "</a>\n";
            }
            htm += "<tr>\n<td class='" +
                GetSexClass( husbFams[i].FGetWifeID(), SEX_Female ) + "'>\n" +
                WrXMenuFamily( husbFams[i].FGetWifeID(), X2MF_Default, fam.FGetHusbID() ) +
                name + "<br />\n" +
                recIndividual::GetEpitaph( husbFams[i].FGetWifeID() ) +
                "\n</td>\n</tr>\n";
        } else {
            htm += "<tr>\n<td class='fem'>&nbsp;</td>\n</tr>\n";
        }
    }
    if( husbFams.empty() ) {
        htm << "<tr>\n<td class='male'>&nbsp;</td>\n</tr>\n"
               "<tr>\n<td class='fem'>&nbsp;</td>\n</tr>\n"
        ;
    }
    htm << "</table>\n"
           "</td>\n<td class='frame frame-top'>\n"
           "<table class='parent'>\n"
    ;
    // Wife's Parents
    for( size_t i = 0 ; i < wifeFams.size() ; i++ ) {
        if( i > 0 ) {
            htm += "<tr>\n<td class='status'>\nAdditional Parents:\n</td>\n</tr>\n";
        }
        // Wifes Father
        if( wifeFams[i].FGetHusbID() ) {
            link = GetFamFileName( wifeFams[i].FGetID() );

            if( link.empty() ) {
                name = "<scan class='dummy'>" +
                    recIndividual::GetName( wifeFams[i].FGetHusbID() ) +
                    "</scan>\n";
            } else {
                name = "<a href='" + link + "'>" +
                    recIndividual::GetName( wifeFams[i].FGetHusbID() ) + "</a>\n";
            }
            htm += "<tr>\n<td class='" +
                GetSexClass( wifeFams[i].FGetHusbID(), SEX_Male ) + "'>\n" +
                WrXMenuFamily( wifeFams[i].FGetHusbID(), X2MF_Default, fam.FGetWifeID() ) +
                name + "<br />\n" +
                recIndividual::GetEpitaph( wifeFams[i].FGetHusbID() ) +
                "\n</td>\n</tr>\n";
        } else {
            htm += "<tr>\n<td class='male'>&nbsp;</td>\n</tr>\n";
        }
        // Wifes Mother
        if( wifeFams[i].FGetWifeID() ) {
            link = GetFamFileName( wifeFams[i].FGetID() );
            if( link.empty() ) {
                name = "<scan class='dummy'>" +
                    recIndividual::GetName( wifeFams[i].FGetWifeID() ) +
                    "</scan>\n";
            } else {
                name = "<a href='" + link + "'>" +
                    recIndividual::GetName( wifeFams[i].FGetWifeID() ) + "</a>\n";
            }
            htm += "<tr>\n<td class='" +
                GetSexClass( wifeFams[i].FGetWifeID(), SEX_Female ) + "'>\n" +
                WrXMenuFamily( wifeFams[i].FGetWifeID(), X2MF_Default, fam.FGetWifeID() ) +
                name + "<br />\n" +
                recIndividual::GetEpitaph( wifeFams[i].FGetWifeID() ) +
                "\n</td>\n</tr>\n";
        } else {
            htm += "<tr>\n<td class='fem'>&nbsp;</td>\n</tr>\n";
        }
    }
    if( wifeFams.empty() ) {
        htm << "<tr>\n<td class='male'>&nbsp;</td>\n</tr>\n"
               "<tr>\n<td class='fem'>&nbsp;</td>\n</tr>\n"
        ;
    }

    htm << "</table>\n</td>\n</tr>\n";

    // Marriage event
    if( m_dataPrivacy >= husb.FGetPrivacy() && m_dataPrivacy >= wife.FGetPrivacy() ) {
        idt unionID = fam.GetUnionEvent();
        if( unionID ) {
            htm << "<tr>\n<td colspan='2' class='both'>\n<b>Marriage: </b>"
                << recEvent::GetDetailStr( unionID )
                << "\n</td>\n</tr>\n"
            ;
        }
    }
    // The happy couple
    htm << "<tr>\n<td class='couple "
        << GetSexClass( fam.FGetHusbID(), SEX_Male )
        << "'>";
    if( fam.FGetHusbID() == 0 ) {
        htm << "&nbsp;";
    } else {
        htm << "\n" 
            << WrXMenuFamily( fam.FGetHusbID(), X2MF_IncSpade|X2MF_IncClub, 0, famID );
        if( m_includeInd ) {
            htm << "<a href='" << GetIndFileName( fam.FGetHusbID() )
                << "' class='subject'>" 
                << recIndividual::GetName( fam.FGetHusbID() )
                << "</a>";
        } else {
            htm << "<scan class='subject'>" 
                << recIndividual::GetName( fam.FGetHusbID() )
                << "</scan>";
        }
    }
    htm << "</td>\n<td class='couple "
        << GetSexClass( fam.FGetWifeID(), SEX_Female )
        << "'>";
    if( fam.FGetWifeID() == 0 ) {
        htm << "&nbsp;";
    } else {
        htm << "\n"
            << WrXMenuFamily( fam.FGetWifeID(), X2MF_IncSpade|X2MF_IncClub, 0, famID );
        if( m_includeInd ) {
            htm << "<a href='" << GetIndFileName( fam.FGetWifeID() )
                << "' class='subject'>" 
                << recIndividual::GetName( fam.FGetWifeID() )
                << "</a>";
        } else {
            htm << "<scan class='subject'>" 
                << recIndividual::GetName( fam.FGetWifeID() )
                << "</scan>";
        }
    }
    htm << "</td>\n</tr>\n";

    // Standard events
    wxString hBirth, hNBirth, hDeath, hNDeath, hOcc;
    if( m_dataPrivacy >= husb.FGetPrivacy() ) {
        hBirth = recEvent::GetDetailStr( husb.GetBirthEvent() );
        hNBirth = recEvent::GetDetailStr( husb.GetNrBirthEvent() );
        hDeath = recEvent::GetDetailStr( husb.GetDeathEvent() );
        hNDeath = recEvent::GetDetailStr( husb.GetNrDeathEvent() );
        hOcc = recIndividualEvent::GetRoleStr( hIndID, recEventType::ET_Occupation );
    }
    wxString wBirth, wNBirth, wDeath, wNDeath, wOcc;
    if( m_dataPrivacy >= wife.FGetPrivacy() ) {
        wBirth = recEvent::GetDetailStr( wife.GetBirthEvent() );
        wNBirth = recEvent::GetDetailStr( wife.GetNrBirthEvent() );
        wDeath = recEvent::GetDetailStr( wife.GetDeathEvent() );
        wNDeath = recEvent::GetDetailStr( wife.GetNrDeathEvent() );
        wOcc = recIndividualEvent::GetRoleStr( wIndID, recEventType::ET_Occupation );
    }
    htm += 
        "<tr>\n<td>\n<b>Birth: </b>" + hBirth +
        "\n</td>\n<td>\n<b>Birth: </b>" + wBirth +
        "\n</td>\n</tr>\n"

        "<tr>\n<td>\n<b>Baptism: </b>" + hNBirth +
        "\n</td>\n<td>\n<b>Baptism: </b>" + wNBirth +
        "\n</td>\n</tr>\n"

        "<tr>\n<td>\n<b>Death: </b>" + hDeath +
        "\n</td>\n<td>\n<b>Death: </b>" + wDeath +
        "\n</td>\n</tr>\n"

        "<tr>\n<td>\n<b>Burial: </b>" + hNDeath +
        "\n</td>\n<td>\n<b>Burial: </b>" + wNDeath +
        "\n</td>\n</tr>\n"

        "<tr>\n<td>\n<b>Occ: </b>" + hOcc +
        "\n</td>\n<td>\n<b>Occ: </b>" + wOcc +
        "\n</td>\n</tr>\n"
    ;

    // Add Children
    if( kids.size() > 0 ) {
        htm += "<tr>\n<td colspan='2' class='frame frame-bot'>\n"
               "<table class='parent'>\n";
        int priv = 0;
        for( size_t i = 0 ; i < kids.size() ; i++ ) {
            link = GetFamFileName( kids[i].FGetFamID() );
            if( link.empty() ) {
                priv++;
                continue;
            }
            htm << "<tr>\n<td class='"
                << GetSexClass( kids[i].f_id )
                << " kids'>\n"
                << WrXMenuFamily( kids[i].FGetID(), X2MF_IncHeart2|X2MF_IncClub )
                << "<b><a href='" << link << "'>"
                << kids[i].FGetName()
                << "</a></b>&nbsp;&nbsp;"
                << kids[i].f_epitaph
                << "\n</td>\n</tr>\n";
        }
        if( priv > 0 ) {
            name = priv == 1 ? " Child" : " Children";
            htm << "<tr>\n<td class='neut kids'>\n"
                   "<b><span class='dummy'>" << priv << name
                << "</b></span>\n</td>\n</tr>\n";
        }
        htm << "</table>\n</td>\n</tr>\n";
    }

    // Add additional Spouses
    if( husbWives.size() > 1 || wifeHusbs.size() > 1 ) {
        htm << "<tr>\n<td class='frame frame-bot'>\n";

        // Display additional Husbands Spouses
        if( husbWives.size() > 1 ) {
            htm << "<table class='parent'>\n"
                << "<tr>\n<td class='status'>\nAdditional Spouses:\n</td>\n</tr>\n";
            int priv = 0;
            for( size_t i = 0 ; i < husbWives.size() ; i++ ) {
                if( husbWives[i].f_wife_id == wife.f_id ) {
                    continue;
                }
                link = GetFamFileName( husbWives[i].FGetID() );
                if( link.empty() ) {
                    priv++;
                    continue;
                }
                htm << "<tr>\n<td class='"
                    << GetSexClass( husbWives[i].f_wife_id, SEX_Female ) << "'>\n"
                    << WrXMenuFamily( husbWives[i].f_wife_id, X2MF_Default )
                    << "<a href='" << link << "'>";
                if( husbWives[i].f_wife_id == 0 ) {
                    htm << "[Unknown]";
                } else {
                    htm << recIndividual::GetName( husbWives[i].f_wife_id );   //  Name
                }
                htm << "</a>&nbsp;&nbsp;"
                    << recIndividual::GetEpitaph( husbWives[i].f_wife_id )
                    << "\n</td>\n</tr>\n";
            }
            if( priv > 0 ) {
                name = priv == 1 ? "Individual" : "Individuals";
                htm << "<tr>\n<td class='neut'>\n"
                    << "<span class='dummy'>" << priv << " Private " << name
                    << "</span>\n</td>\n</tr>\n";
            }
            htm << "</table>\n";
        }

        htm << "</td>\n<td class='frame frame-bot'>\n";

        // Display additional Wifes Spouses
        if( wifeHusbs.size() > 1 ) {
            htm << "<table class='parent'>\n"
                << "<tr>\n<td class='status'>\nAdditional Spouses:\n</td>\n</tr>\n";
            int priv = 0;
            for( size_t i = 0 ; i < wifeHusbs.size() ; i++ ) {
                if( wifeHusbs[i].f_husb_id == husb.f_id ) {
                    continue;
                }
                link = GetFamFileName( wifeHusbs[i].FGetID() );
                if( link.empty() ) {
                    priv++;
                    continue;
                }
                htm << "<tr>\n<td class='"
                    << GetSexClass( wifeHusbs[i].f_husb_id, SEX_Male ) << "'>\n"
                    << WrXMenuFamily( wifeHusbs[i].f_husb_id, X2MF_Default )
                    << "<a href='" << link << "'>"
                ;
                if( wifeHusbs[i].f_husb_id == 0 ) {
                    htm << "[Unknown]";
                } else {
                    htm << recIndividual::GetName( wifeHusbs[i].f_husb_id );  //  Name
                }
                htm << "</a>&nbsp;&nbsp;"
                    << recIndividual::GetEpitaph( wifeHusbs[i].f_husb_id )
                    << "\n</td>\n</tr>\n";
            }
            if( priv > 0 ) {
                name = priv == 1 ? "Individual" : "Individuals";
                htm << "<tr>\n<td class='neut'>\n"
                    << "<span class='dummy'>" << priv << " Private " << name
                    << "</span>\n</td>\n</tr>\n";
            }
            htm << "</table>\n";
        }

        htm << "</td>\n</tr>\n";
    }

    // core done
    htm +=
        "</table>\n<br />\n"
        // Ends contents
        "<div id='botmenu'>\n" + menuBar + "</div>\n"
        + GetFooter( recFamily::GetIdStr( famID ) )
    ;
    OutputHtmFile( filename, htm );
}

// End of src/wor/worWrFamily.cpp Source
