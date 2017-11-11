/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        include/wor/worWr.h
 * Project:     Web Output for The Family Pack Rec Library.
 * Purpose:     The wor library function header.
 * Author:      Nick Matthews
 * Website:     http://thefamilypack.org
 * Created:     15 December 2012
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
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with The Family Pack.  If not, see <http://www.gnu.org/licenses/>.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*/

#ifndef WORWR_H
#define WORWR_H

#include <rec/recDatabase.h>


// See src/wor/worProcess.cpp
extern wxString GetTodayStr();
extern wxString GetSexClass( idt indID, Sex pref = SEX_Unknown );

// See src/wor/worWrFamily.cpp

#endif // WORWR_H
