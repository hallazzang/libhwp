/*
 * ghwp-file.vala
 *
 * Copyright (C) 2012  Hodong Kim <cogniti@gmail.com>
 * 
 * This library is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * 한글과컴퓨터의 한/글 문서 파일(.hwp) 공개 문서를 참고하여 개발하였습니다.
 */

public class GHWP.GHWPFile : GLib.Object {
    private Gsf.InfileMSOle olefile;
    private GHWP.Document doc;
    public struct Header {
        string signature;
        uint32 version;
        bool   is_compress;
        bool   is_encrypt;
        bool   is_distribute;
        bool   is_script;
        bool   is_drm;
        bool   is_xml_template;
        bool   is_history;
        bool   is_sign;
        bool   is_certificate_encrypt;
        bool   is_sign_spare;
        bool   is_certificate_drm;
        bool   is_ccl;
    }
    public Header header = Header();
    /**
     * This method is converted to
     * 
     * GHWPFile *ghwp_file_new (GFile *filename, GError **error);
     */
    public GHWPFile (File filename) throws Error {

        try {
            var input   = new Gsf.InputStdio (filename.get_path());
            olefile     = new Gsf.InfileMSOle (input);
        }
        catch (Error e) {
            error ("%s", e.message);
        }

        doc = new GHWP.Document();

        parse_file_header();
        parse_prv_text();
        parse_summary_info();
    }

    /**
     * This method is converted to
     * GHWPDocument *ghwp_file_get_document (GHWPFile *hwp,
     *                                       GError  **error);
     */
    public GHWP.Document get_document () throws Error {
        return doc;
    }

    void parse_file_header() {
        var _input =  olefile.child_by_name("FileHeader");
        var _size  = _input.size();
        var _buf   =  new uchar[_size];
        _input.read((size_t) _size, _buf);

        header.signature = (string) _buf[0:31]; // 32 bytes
        // 5 << 24 | 0 << 16 | 0 << 0 | 6 => 83886086
        // 83886086.to_s(16) => "5000006"
        header.version = (_buf[35] << 24) |
                         (_buf[34] << 16) |
                         (_buf[33] <<  8) |
                          _buf[32];
        // type conversion
        uint32 _prop   = (_buf[39] << 24) |
                         (_buf[38] << 16) |
                         (_buf[37] <<  8) |
                          _buf[36];

        if((_prop & (1 <<  0)) ==  1) header.is_compress            = true;
        if((_prop & (1 <<  1)) ==  1) header.is_encrypt             = true;
        if((_prop & (1 <<  2)) ==  1) header.is_distribute          = true;
        if((_prop & (1 <<  3)) ==  1) header.is_script              = true;
        if((_prop & (1 <<  4)) ==  1) header.is_drm                 = true;
        if((_prop & (1 <<  5)) ==  1) header.is_xml_template        = true;
        if((_prop & (1 <<  6)) ==  1) header.is_history             = true;
        if((_prop & (1 <<  7)) ==  1) header.is_sign                = true;
        if((_prop & (1 <<  8)) ==  1) header.is_certificate_encrypt = true;
        if((_prop & (1 <<  9)) ==  1) header.is_sign_spare          = true;
        if((_prop & (1 << 10)) ==  1) header.is_certificate_drm     = true;
        if((_prop & (1 << 11)) ==  1) header.is_ccl                 = true;
    }

    void parse_prv_text()
    {
        var input   = olefile.child_by_name("PrvText");
        var size    = input.size();
        var buf     = new uchar[size];
        input.read((size_t) size, buf);
        try {
            doc.prv_text = GLib.convert( (string) buf, (ssize_t) size,
                                         "UTF-8",      "UTF-16LE");
        }
        catch (Error e) {
            error("%s", e.message);
        }
    }

    void parse_summary_info()
    {
        var input = olefile.child_by_name("\005HwpSummaryInformation");
        var size  = input.size();
        var buf   = new uchar[size];
        input.read((size_t) size, buf);

        uint8[] component_guid = {
            0xe0, 0x85, 0x9f, 0xf2, 0xf9, 0x4f, 0x68, 0x10,
            0xab, 0x91, 0x08, 0x00, 0x2b, 0x27, 0xb3, 0xd9
        };

        // changwoo's solution, thanks to changwoo.
        // https://groups.google.com/forum/#!topic/libhwp/gFDD7UMCXBc
        // https://github.com/changwoo/gnome-hwp-support/blob/master/properties/props-data.c
        Memory.copy(buf + 28, component_guid, component_guid.length);
        var summary = new Gsf.InputMemory(buf, false);
        var meta = new Gsf.DocMetaData();
        Gsf.msole_metadata_read(summary, meta);
        // FIXME
        doc.summary_info = meta;
    }
}
