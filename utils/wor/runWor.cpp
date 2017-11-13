/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        utils/wor/runWor.cpp
 * Project:     wor: Web Output for The Family Pack Rec Library.
 * Purpose:     Command line utility to create Web Pages from *.tfpd file
 * Author:      Nick Matthews
 * Website:     http://thefamilypack.org
 * Created:     3 May 2012
 * Copyright:   Copyright (c) 2012 - 2017, Nick Matthews.
 * Licence:     GNU GPLv3
 *
 *  runScript is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  runScript is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with runScript.  If not, see <http://www.gnu.org/licenses/>.
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


#include <wx/cmdline.h>
#include <wx/fileconf.h>
#include <wx/filename.h>
#include <wx/wfstream.h>

#include <rec/recGedExport.h>
#include <rec/recVersion.h>

#include <wor/worExport.h>


#define PROGNAME         "runWor"
#define VERSION_NUM      "WOR 0.1.3"
#define COPYRIGHT_DATE   "2012 ~ 2017"

#define VERSION_STATUS   " private"

#ifdef _DEBUG
#define VERSION_CONFIG   " debug"
#else
#define VERSION_CONFIG   ""
#endif


#define VERSION   VERSION_NUM VERSION_STATUS VERSION_CONFIG

const char* g_title = PROGNAME " - Version " VERSION "\n"
                         "Copyright (c) " COPYRIGHT_DATE " Nick Matthews\n\n";


bool verbose = false;
bool quiet   = false;


bool ExportGedcom( wxString& path )
{
    wxFileName fname( path );
    wxFFileOutputStream outfile( fname.GetFullPath() );
    if( !outfile.IsOk() ) return false;
    try {
        recGedExport ged( outfile );
        if( !ged.Export() ) {
            recMessage( _("Error Writing GEDCOM File"), _("Export") );
            return true;
        }

    } catch( wxSQLite3Exception& e ) {
        recDb::ErrorMessage( e );
        recDb::Rollback();
    }
    return true;
}

/*#*************************************************************************
 **  main
 **  ~~~~
 */

int main( int argc, char** argv )
{
    static const wxCmdLineEntryDesc desc[] = {
        { wxCMD_LINE_SWITCH, "h", "help", "show this help message",
            wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
        { wxCMD_LINE_SWITCH, "v", "verbose", "be verbose" },
        { wxCMD_LINE_SWITCH, "q", "quiet",   "be quiet" },
        { wxCMD_LINE_PARAM,  NULL, NULL, "configuation file",
            wxCMD_LINE_VAL_STRING, wxCMD_LINE_OPTION_MANDATORY },
        { wxCMD_LINE_NONE }
    };

    wxInitializer initializer;
    if( !initializer )
    {
        fprintf( stderr, "Failed to initialize the wxWidgets library, aborting." );
        return EXIT_FAILURE;
    }

    wxCmdLineParser parser( desc, argc, argv );
    parser.SetLogo( g_title );

    int cmd = parser.Parse();
    if( cmd < 0 ) {
        return EXIT_SUCCESS;
    }
    if( cmd > 0 ) {
        return EXIT_FAILURE;
    }

    wxString inFileName = parser.GetParam();

    if( parser.Found( "q" ) ) quiet = true;
    if( parser.Found( "v" ) ) verbose = true;

    if( ! quiet ) wxPrintf( g_title );


    recDb::SetDb( new wxSQLite3Database() );
    if( verbose ) {
        wxPrintf( "SQLite3 version: %s\n", wxSQLite3Database::GetVersion() );
        wxPrintf( "TFPD version: %s\n", recVerStr );
    }

    wxFileName configName( parser.GetParam() );
    configName.MakeAbsolute();

    if( !configName.FileExists() ) {
        wxPrintf( "Input file \"%s\" not found.\n", configName.GetFullPath() );
        return EXIT_FAILURE;
    }
    wxPrintf( "Config file = \"%s\"\n", configName.GetFullPath() );

    wxFileConfig conf( "", "", configName.GetFullPath(), "", wxCONFIG_USE_LOCAL_FILE );

    wxString dbName = conf.Read( "/Input/Database", "Error" );
    wxString outpath = conf.Read( "/Output/OutPath", "Error" );
    wxString outged = conf.Read( "/Output/Gedcom", "" );
    if( dbName == "Error" || outpath == "Error" ) {
        wxPrintf( "Error reading configeration file.\n" );
        return EXIT_FAILURE;
    }
    if( verbose ) {
        wxPrintf( "Database file = \"%s\"\n", dbName );
        wxPrintf( "Output Path = \"%s\"\n", outpath );
        wxPrintf( "Output Gedcom file = \"%s\"\n", outged );
    }

    wxFileName inName( dbName );
    if( !inName.FileExists() ) {
        wxPrintf( "Input file \"%s\" not found.\n", inName.GetFullPath() );
        return EXIT_FAILURE;
    }
    if( recDb::OpenDb( inName.GetFullPath() ) ) {
        if( verbose ) {
            wxPrintf( "Opened Database file \"%s\".\n", inName.GetFullPath() );
        }
    } else {
        wxPrintf( "ERROR: Unable to open file [%s]\n", inName.GetFullPath() );
        delete recDb::GetDb();
        return EXIT_FAILURE;
    }

    worExport wor( outpath, VERSION_NUM );
    wor.SetImagePath( conf.Read( "/Input/Image-Text-Path", "" ) );
    wor.SetIncludeHome( conf.ReadBool( "/Output/Home-Page", false ) );
    wor.SetIncludeInd( conf.ReadBool( "/Output/Individuals", true ) );
    wor.SetIncludeNames( conf.ReadBool( "/Output/Names", true ) );
    wor.SetIncludeRef( conf.ReadBool( "/Output/References", true ) );
    long privacy = conf.ReadLong( "/Output/Privacy", 100 );
    wor.SetPrivacy( privacy );
    wor.SetDataPrivacy( conf.ReadLong( "/Output/Data-Privacy", privacy ) );
    wor.InitMenuBar( conf );
    wor.ExportPages();

    // Until we can output html charts, we output a GEDCOM file
    // and let tfp02 do it for us. 
    if( !outged.empty() ) {
        ExportGedcom( outged );
    }

    delete recDb::GetDb();
    return EXIT_SUCCESS;
}

/* End of utils/wor/runWor.cpp file */
