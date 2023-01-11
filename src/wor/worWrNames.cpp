/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/wor/worWrNames.h
 * Project:     wor: Web Output for The Family Pack Rec Library.
 * Purpose:     Create the Web Page text for the Name Indexes and Lists.
 * Author:      Nick Matthews
 * Website:     http://thefamilypack.org
 * Created:     11th November 2017
 * Copyright:   Copyright (c) 2017, Nick Matthews.
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

#include "worWr.h"

#include <rec/recDb.h>

#include <cctype>
#include <set>

struct NameListItem
{
    idt m_indID;
    idt m_famID;
    wxString m_surname;
    wxString m_full;
    wxString m_epitaph;
    idt m_style_id;

    bool operator<( const NameListItem& x ) const
    {
        if ( m_surname != x.m_surname ) {
            return m_surname < x.m_surname;
        }
        if ( m_full != x.m_full ) {
            return m_full < x.m_full;
        }
        if ( m_epitaph != x.m_epitaph ) {
            return m_epitaph < x.m_epitaph;
        }
        return m_indID < x.m_indID;
    }
};

class NameList
{
public:
    void ReadNameList( int privacy );
    wxString GetSurnameIndexTable( const worExport& exp );
    wxString GetNextLetter() const {
        wxString letter;
        if ( m_it != m_list.end() ) {
            letter = ( *m_it ).m_surname.substr( 0, 1 );
        }
        return letter;
    }
    wxString GetSurnameLetterTable( const worExport& exp );

    std::set<NameListItem> m_list;
    std::set<NameListItem>::const_iterator m_it;
    StringVec m_letters;
};

void NameList::ReadNameList( int privacy )
{
    recNameVec names = recName::GetNameList( "", recSG_Individual );
    for ( auto name : names ) {
        idt indID = name.FGetIndID();
        recIndividual ind( indID );
        if ( ind.FGetPrivacy() > privacy ) {
            continue;
        }
        NameListItem item;
        item.m_indID = indID;
        item.m_epitaph = ind.FGetEpitaph();
        item.m_famID = ind.FGetFamID();
        item.m_surname = name.GetSurname();
        item.m_full = name.GetNameStr();
        item.m_style_id = name.FGetTypeID();
        if ( !item.m_surname.empty() ) {
            m_list.insert( item );
        }
    }
    m_it = m_list.begin();
}

wxString NameList::GetSurnameIndexTable( const worExport& exp )
{

    if ( m_list.empty() ) {
        return "<p>No Names found!</p>\n";
    }
    wxString letter, surname, filename, rowclass;
    int count = 1, row = 0;
    bool row1st = true;

    wxString prev_surname, prev_letter;
    wxString htm = "<table class='list'>\n";
    for ( auto item : m_list ) {
        surname = item.m_surname;
        if ( surname == prev_surname ) {
            continue;
        }
        prev_surname = surname;
        letter = surname.substr( 0, 1 );
        if ( letter != prev_letter ) {
            prev_letter = letter;
            rowclass = GetEvenOddClass( ++row );
            filename = exp.GetNListFileName( letter );
            if ( row1st == true ) {
                row1st = false;
            } else {
                // End prevous line
                htm << "\n</td>\n</tr>\n";
            }
            // Start new line
            htm <<
                "<tr class='" << rowclass << "'>\n"
                "<td><b><a href='" << filename << "'>"
                << letter << "</a></b></td>\n"
                "<td>"
                ;
            count = 1;
            m_letters.push_back( letter );
        }
        if ( count != 1 ) {
            htm << ", ";
        }
        htm <<
            "\n<a href='" << filename << "#N" << count <<
            "'><b>" << surname <<
            "</b></a>"
            ;
        count++;
    }
    htm += "\n</td>\n</tr>\n</table>\n";
    return htm;
}

wxString NameList::GetSurnameLetterTable( const worExport & exp )
{
    wxString rowclass;
    wxString letter = m_it->m_surname.substr( 0, 1 );
    wxString prev_surname;
    wxString htm = "<table class='list'>\n";
    int count = 1, row = 0;
    for ( ;;) {
        rowclass = GetEvenOddClass( ++row );
        if ( prev_surname != m_it->m_surname ) {
            htm +=
                "<tr class='" + rowclass + "' id='N" + recGetStr( count ) + "'>\n"
                "<th class='sublist' colspan='2'>" + m_it->m_surname + "</th>\n"
                "</tr>\n"
            ;
            count++;
            prev_surname = m_it->m_surname;
            rowclass = GetEvenOddClass( ++row );
        }
        htm +=
            "<tr class='" + rowclass + "'>\n<td><b><a href='" +
            exp.GetFamFileName( m_it->m_famID ) + "'>" +
            m_it->m_full + "</a></b> <span class='epitaph'>" +
            m_it->m_epitaph + "</span></td>\n<td>" +
            recNameStyle::GetStyleStr( m_it->m_style_id ) +
            "</td></tr>\n"
        ;
        m_it++;
        if ( m_it == m_list.end()  ) {
            break;
        }
        wxString next_letter = m_it->m_surname.substr( 0, 1 );
        if ( next_letter != letter ) {
            break;
        }
        letter = next_letter;
    };
    htm += "</table>";
    return htm;
}

void worExport::OutputSurnameIndex() const
{
    StringVec css;
    css.push_back( "tlist" );
    wxString menuBar = GetMenuBar( 1, "Names" );
    wxString title = "Surname Index";

    NameList list;
    list.ReadNameList( m_privacy );
    wxString htm =
        GetHeader( 1, title, css ) +
        "<h1>" + title + "</h1>\n"
        "<div id='topmenu'>\n" + menuBar + "</div>\n<br />\n" +
        list.GetSurnameIndexTable( *this ) +
        "<div id='botmenu'>\n" + menuBar + "</div>\n" +
        GetFooter( "N-Index" )
    ;
    OutputHtmFile( "ind/n_index.htm", htm );

    menuBar = GetMenuBar( 1 );
    wxString letter = list.GetNextLetter();
    while ( !letter.empty() ) {
        // Create letter menu.
        wxString letter_menu = "<div>\n<span class='hmenu'\n >";
        for ( auto let : list.m_letters ) {
            if ( let == letter ) {
                letter_menu +=
                    "<a class='dummy' href='javascript:void(0)'>" + let + "</a\n >"
                ;
            } else {
                letter_menu +=
                    "<a href='" + GetNListFileName( let ) + "'>" + let + "</a\n >"
                ;
            }
        }
        letter_menu += "\n</span>\n</div>\n";
        // Create page.
        title = letter + ": Name Index";
        htm =
            GetHeader( 1, title, css ) +
            "<h1>" + title + "</h1>\n"
            "<div id='topmenu'>\n" + menuBar + "</div>\n" +
            letter_menu + "<br />\n" +
            list.GetSurnameLetterTable( *this ) +
            letter_menu +
            "<div id='botmenu'>\n" + menuBar + "</div>\n" +
            GetFooter( "N-" + letter )
        ;
        wxString filename = GetNListFileName( letter, 0 );
        OutputHtmFile( filename, htm );
        letter = list.GetNextLetter();
    }
}

// End of src/wor/worWrNames.cpp Source
