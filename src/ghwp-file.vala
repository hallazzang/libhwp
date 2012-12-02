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

public class GsfInputStream : InputStream
{
    private Gsf.Input input;

    public GsfInputStream(Gsf.Input input)
    {
        this.input = input;
    }

    public override ssize_t
    read (uint8[] buffer, Cancellable? cancellable = null) throws IOError
    {
        var stamp = input.remaining();

        if (input.remaining() < buffer.length) {
            input.read ((size_t)input.remaining(), buffer);
        }
        else {
            input.read (buffer.length, buffer);
        }

        return (ssize_t) (stamp - input.remaining());
    }

    public override bool
    close (Cancellable? cancellable = null) throws IOError
    {
        // pseudo true
        return true;
    }

    public ssize_t size()
    {
        return (ssize_t) input.size();
    }
}

public class GHWP.GHWPFile : GLib.Object {
    private Gsf.InfileMSOle olefile;

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

    public InputStream prv_text_stream;
    public InputStream prv_image_stream;
    public InputStream file_header_stream;
    public InputStream doc_info_stream;
    public Gee.ArrayList<InputStream> section_streams;
    private InputStream section_stream;
    public InputStream summary_info_stream;

    public GHWPFile.from_uri (string uri) throws Error
    {
        var filename = Filename.from_uri(uri);
        try {
            var input = new Gsf.InputStdio (filename);
            olefile   = new Gsf.InfileMSOle (input);
        }
        catch (Error e) {
            error ("%s", e.message);
        }

        init();
    }

    public GHWPFile.from_filename (string filename) throws Error
    {
        var file = File.new_for_path(filename);
        try {
            var input = new Gsf.InputStdio (file.get_path());
            olefile   = new Gsf.InfileMSOle (input);
        }
        catch (Error e) {
            error ("%s", e.message);
        }

        init();
    }

    // hwp 파일의 오류를 검사하고 stream을 만든다.
    void init()
    {
        var n_children = olefile.num_children();

        if (n_children < 1) {
            stderr.printf("invalid hwp file\n");
            return;
        }

        for(int i = 0; i < n_children; i++) {
            string name = olefile.name_by_index(i);
            switch (name) {
            case "PrvText":
                var input = olefile.child_by_name("PrvText");
                if (((Gsf.Infile)input).num_children() > 0)
                    stderr.printf("invalid\n");
                prv_text_stream = new GsfInputStream(input);
                break;
            case "PrvImage":
                var input = olefile.child_by_name("PrvImage");
                if (((Gsf.Infile)input).num_children() > 0)
                    stderr.printf("invalid\n");
                prv_image_stream = new GsfInputStream(input);
                break;
            case "FileHeader":
                var input = olefile.child_by_name("FileHeader");
                if (((Gsf.Infile)input).num_children() > 0)
                    stderr.printf("invalid\n");
                file_header_stream = new GsfInputStream(input);
                decode_file_header();
                break;
            case "DocInfo":
                var input = olefile.child_by_name("DocInfo");
                if (((Gsf.Infile)input).num_children() > 0)
                    stderr.printf("invalid\n");
                if (header.is_compress) {
                    var gis = new GsfInputStream(input);
                    var zd  = new ZlibDecompressor (ZlibCompressorFormat.RAW);
                    doc_info_stream = new ConverterInputStream(gis, zd);
                }
                else {
                    doc_info_stream = new GsfInputStream(input);
                }
                break;
            case "BodyText":
            case "VeiwText":
                section_streams = new Gee.ArrayList<InputStream>();
                var infile = (Gsf.Infile) olefile.child_by_index(i);
                if (infile.num_children() == 0)
                    stderr.printf("nothing in BodyText\n");

                for (int j = 0; j < infile.num_children(); j++) {
                    var section = (Gsf.Infile) infile.child_by_index(j);
                    if (section.num_children() > 0)
                        stderr.printf("invalid section\n");

                    if (header.is_compress) {
                        var gis = new GsfInputStream(section);
                        var zd  = new ZlibDecompressor (ZlibCompressorFormat.RAW);
                        section_stream = new ConverterInputStream(gis, zd);
                    }
                    else {
                        section_stream = new GsfInputStream(section);
                    }
                    section_streams.add(section_stream);
                    stdout.printf("%s\n", infile.name_by_index(j));
                }
                break;
            case "\005HwpSummaryInformation":
                var input = olefile.child_by_name("\005HwpSummaryInformation");
                if (((Gsf.Infile)input).num_children() > 0)
                    stderr.printf("invalid\n");
                summary_info_stream = new GsfInputStream(input);
                break;
            default:
                stderr.printf("not implemented error: %s\n", name);
                break;
            }
        }
    }

    void decode_file_header()
    {
        var gis  = (GsfInputStream)file_header_stream;
        var size = gis.size();
        var buf  = new uchar[size];
        try {
            gis.read(buf);
        }
        catch (Error e) {
            error("%s", e.message);
        }

        header.signature = (string) buf[0:31]; // 32 bytes
        // 5 << 24 | 0 << 16 | 0 << 0 | 6 => 83886086
        // 83886086.to_s(16) => "5000006"
        header.version = (buf[35] << 24) |
                         (buf[34] << 16) |
                         (buf[33] <<  8) |
                          buf[32];
        // type conversion
        uint32 prop    = (buf[39] << 24) |
                         (buf[38] << 16) |
                         (buf[37] <<  8) |
                          buf[36];

        if((prop & (1 <<  0)) ==  1) header.is_compress            = true;
        if((prop & (1 <<  1)) ==  1) header.is_encrypt             = true;
        if((prop & (1 <<  2)) ==  1) header.is_distribute          = true;
        if((prop & (1 <<  3)) ==  1) header.is_script              = true;
        if((prop & (1 <<  4)) ==  1) header.is_drm                 = true;
        if((prop & (1 <<  5)) ==  1) header.is_xml_template        = true;
        if((prop & (1 <<  6)) ==  1) header.is_history             = true;
        if((prop & (1 <<  7)) ==  1) header.is_sign                = true;
        if((prop & (1 <<  8)) ==  1) header.is_certificate_encrypt = true;
        if((prop & (1 <<  9)) ==  1) header.is_sign_spare          = true;
        if((prop & (1 << 10)) ==  1) header.is_certificate_drm     = true;
        if((prop & (1 << 11)) ==  1) header.is_ccl                 = true;
    }
}
