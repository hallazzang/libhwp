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

public class GHWPFile : GLib.Object {
    private Gsf.InfileMSOle olefile;
    private GHWPDocument doc;
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

        parse_file_header();

        // prv_text 가져오기
        Gsf.Input prv_text = olefile.child_by_name("PrvText");
        Gsf.off_t _size    = prv_text.size();
        // TODO free _buf_in ?
        uchar [] _buf_in   = new uchar[_size];
        prv_text.read((size_t) _size, _buf_in);
        string result = GLib.convert( (string)  _buf_in,
                                      (ssize_t) _size,
                                      "UTF-8",  "UTF-16LE");

        // doc 생성
        doc = new GHWPDocument();
        doc.prv_text = result;
    }

    /**
     * This method is converted to
     * GHWPDocument *ghwp_file_get_document (GHWPFile *hwp,
     *                                       GError  **error);
     */
    public GHWPDocument get_document () throws Error {
        return doc;
    }

    void parse_file_header() {
        Gsf.Input _header = olefile.child_by_name("FileHeader");
        Gsf.off_t size = _header.size();
        uchar[] buf  = new uchar[size];
        _header.read((size_t) size, buf);

        header.signature = (string) buf[0:31]; // 32 bytes
        // 5 << 24 | 0 << 16 | 0 << 0 | 6 => 83886086
        // 83886086.to_s(16) => "5000006"
        header.version   = (buf[35] << 24) |
                           (buf[34] << 16) |
                           (buf[33] <<  8) |
                            buf[32];
        // type conversion
        uint32 _property = (buf[39] << 24) |
                           (buf[38] << 16) |
                           (buf[37] <<  8) |
                            buf[36];

        if((_property & (1 <<  0)) ==  1) header.is_compress            = true;
        if((_property & (1 <<  1)) ==  1) header.is_encrypt             = true;
        if((_property & (1 <<  2)) ==  1) header.is_distribute          = true;
        if((_property & (1 <<  3)) ==  1) header.is_script              = true;
        if((_property & (1 <<  4)) ==  1) header.is_drm                 = true;
        if((_property & (1 <<  5)) ==  1) header.is_xml_template        = true;
        if((_property & (1 <<  6)) ==  1) header.is_history             = true;
        if((_property & (1 <<  7)) ==  1) header.is_sign                = true;
        if((_property & (1 <<  8)) ==  1) header.is_certificate_encrypt = true;
        if((_property & (1 <<  9)) ==  1) header.is_sign_spare          = true;
        if((_property & (1 << 10)) ==  1) header.is_certificate_drm     = true;
        if((_property & (1 << 11)) ==  1) header.is_ccl                 = true;
    }
}
