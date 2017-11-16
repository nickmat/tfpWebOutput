/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        include/wor/worExport.h
 * Project:     wor: Web Output for The Family Pack Rec Library.
 * Purpose:     Manage the files to be exported, class header.
 * Author:      Nick Matthews
 * Website:     http://thefamilypack.org
 * Created:     30th October 2015
 * Copyright:   Copyright (c) 2015, Nick Matthews.
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

#ifndef WOREXPORT_H_GUARD
#define WOREXPORT_H_GUARD

#include <rec/recDatabase.h>

class wxFileConfig;

class worExport
{
public:
    worExport( const wxString& outpath, const wxString& version );

    void SetImagePath( const wxString& path ) { m_imageTextPath = path; }
    bool InitMenuBar( const wxFileConfig& config );
    void SetIncludeHome( bool incHome ) { m_includeHome = incHome; }
    void SetIncludeInd( bool incInd ) { m_includeInd = incInd; }
    void SetIncludeNames( bool incNames ) { m_includeNames = incNames; }
    void SetIncludeRef( bool incRef ) { m_includeRef = incRef; }
    void SetPrivacy( int privacy ) { m_privacy = privacy; }
    void SetDataPrivacy( int privacy ) { m_dataPrivacy = privacy; }

    bool ExportPages();

    enum {
        MB_Home, MB_Family, MB_Name, MB_Gallery, MB_Chart, MB_Source, MB_Research,
        MB_MAX
    };

    wxString GetBasePath( unsigned level ) const;
    wxString GetIndFileName( idt indID, unsigned level = 1 ) const;
    wxString GetFamFileName( idt famID, unsigned level = 1 ) const;
    wxString GetFileName( idt id, const wxString& code, const wxString& ext, unsigned level ) const;
    wxString GetRefFileName( idt refID, unsigned level = 1 ) const
        { return GetFileName( refID, "rd", "htm", level ); }
    wxString GetImgFileName( idt imgID, unsigned level = 1 ) const
        { return GetFileName( imgID, "im", "jpg", level ); }
    wxString GetGalFileName( idt galID, unsigned level = 1 ) const
        { return GetFileName( galID, "ga", "htm", level ); }
    wxString GetNListFileName( const wxString& letter, unsigned level = 1 ) const;

    wxString GetHeader(
        unsigned level, const wxString& title,
        const StringVec& css = StringVec(), const StringVec& js = StringVec()
        ) const;
    wxString GetFooter( const wxString& pageref ) const;
    wxString GetMenuBar( unsigned level, const wxString& thispage = "" ) const;

    wxString WrXMenuDiamond( idt indID ) const;
    wxString WrXMenuSpade( idt indID ) const;
    wxString WrXMenuHeart( idt indID, size_t min_size = 2 ) const;
    wxString WrXMenuClub( idt indID, idt blankIndID = 0, idt blankFamID = 0 ) const;
    enum X2MENU_FLAGS {
        X2MF_IncDiamond = 0x0001, // Parents
        X2MF_IncSpade   = 0x0002, // Siblings
        X2MF_IncHeart   = 0x0004, // Partners
        X2MF_IncHeart2  = 0x0008, // Partners if more than one
        X2MF_IncClub    = 0x0010, // Children
        X2MF_IncFamily  = 0x0020, // Default Family
        X2MF_Default    = X2MF_IncDiamond | X2MF_IncSpade | X2MF_IncHeart2 | X2MF_IncClub,
        X2MF_All        = X2MF_IncFamily | X2MF_IncDiamond | X2MF_IncSpade | X2MF_IncHeart | X2MF_IncClub
    };
    wxString WrXMenuFamily( idt indID, unsigned flags = X2MF_Default, idt blankIndID = 0, idt blankFamID = 0 ) const;
    wxString WrIndNameCell( idt indID ) const;
    wxString WrRefList( const recIdVec& refIDs ) const;

    void OutputHtmFile( const wxString& filename, const wxString& content ) const;
    void OutputSysFile( const wxString& name, const char* text ) const;
    void OutputBinFile( const wxString& name, const unsigned char* data, size_t size ) const;

    void OutputHomePage() const;
    // See src/wor/worWrFamily.cpp
    void OutputFamilyPage( idt famID ) const;
    // See src/wor/worWrIndividual.cpp
    wxString OutputEventRow( idt eveID ) const;
    wxString HtmlifyNotes( const wxString& note ) const;
    wxString GetPhotoPanel( int image, const wxString& title ) const;
    wxString GetImageText( int image ) const;
    wxString HtmlifyPhotos( const wxString& photos ) const;
    void OutputIndividualPage( idt indID ) const;

    void OutputSurnameIndex() const;

private:
    wxString m_imageTextPath; // TODO: Temp, until images are included in database.
    wxString m_outpath;
    wxString m_menuBar[MB_MAX];
    wxString m_homeFamPath;
    bool     m_includeHome;  // Include Home page.
    bool     m_includeInd;   // Include Individual pages.
    bool     m_includeNames; // Include Name index and lists.
    bool     m_includeRef;   // Include Reference pages.
    int      m_privacy;      // 0 = only if ind.FGetPrivacy() = 0, 100 = everything.
    int      m_dataPrivacy;  // As m_privacy but allows name and relationships.
    wxString m_version;
};

#endif // WOREXPORT_H_GUARD