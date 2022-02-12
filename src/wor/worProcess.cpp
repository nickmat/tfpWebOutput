/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        src/wor/worPocess.cpp
 * Project:     wor: Web Output for The Family Pack Rec Library.
 * Purpose:     Proccess the creation of the Web Pages.
 * Author:      Nick Matthews
 * Website:     http://thefamilypack.org
 * Created:     15th December 2012
 * Copyright:   Copyright (c) 2012..2022, Nick Matthews.
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

#include <wx/filename.h>
#include <wx/ffile.h>
#include <wx/tokenzr.h>

#include <rec/recIndividual.h>
#include <rec/recEvent.h>
#include <rec/recPersona.h>

#include "worWr.h"
#include "worFiles.h"


/***************************************************************************
 **  GetTodayStr  Return the current date as a htm marked up string.
 **  ~~~~~~~~~~~  
 */

wxString GetTodayStr()
{
    static wxString month[] = { 
        "January", "Febuary", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December"
    };
    time_t now;
    time( &now );
    struct tm* tp = localtime( &now );
    wxString str;

    str << tp->tm_mday
        << "<sup>";

    switch( tp->tm_mday )
    {
    case 1: case 21: case 31:
        str << "st";
        break;
    case 2: case 22:
        str << "nd";
        break;
    case 3: case 23:
        str << "rd";
        break;
    default:
        str << "th";
    }
    str << "</sup> "
        << month[ tp->tm_mon ]
        << " " 
        << tp->tm_year + 1900;

   return str;
}

wxString GetSexClass( idt indID, Sex pref )
{
    Sex sex = recIndividual::GetSex( indID );
    if( sex == Sex::unstated ) {
        sex = pref;
    }
    switch( sex ) {
    case Sex::male:
        return "male";
    case Sex::female:
        return "fem";
    }
    return "neut";
}


// End of src/wor/worPocess.cpp Source
