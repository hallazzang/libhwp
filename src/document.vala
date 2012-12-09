/*
 * ghwp-document.vala
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

public class GHWP.Document : Object {
    public GHWP.GHWPFile ghwp_file;
    public string        prv_text;
    public Gee.ArrayList<GHWP.Page> pages = new Gee.ArrayList<GHWP.Page>();
    public Gsf.DocMetaData summary_info;

    public Document.from_uri (string uri) throws Error
    {
        ghwp_file = new GHWPFile.from_uri (uri);
        init();
    }

    public Document.from_filename (string filename) throws Error
    {
        ghwp_file = new GHWPFile.from_filename (filename);
        init();
    }

    private void init()
    {
        parse_doc_info();
        parse_body_text();
        parse_prv_text();
        parse_summary_info();
    }

    public uint get_n_pages ()
    {
        return pages.size;
    }

    public GHWP.Page get_page (int n_page)
    {
        return pages[(int)n_page];
    }

    void parse_doc_info()
    {
        var context = new GHWP.Context(ghwp_file.doc_info_stream);
        context.parse();
    }

    void parse_body_text()
    {
        foreach (var section_stream in ghwp_file.section_streams) {
            var context = new GHWP.Context(section_stream);
            context.parse();
        }
    }

    void parse_prv_text()
    {
        var gis  = (GsfInputStream)ghwp_file.prv_text_stream;
        var size = gis.size();
        var buf  = new uchar[size];
        try {
            gis.read(buf);
            prv_text = GLib.convert( (string) buf, (ssize_t) size,
                                     "UTF-8",      "UTF-16LE");
        }
        catch (Error e) {
            error("%s", e.message);
        }
    }

    void parse_summary_info()
    {
        var gis  = (GsfInputStream)ghwp_file.summary_info_stream;
        var size = gis.size();
        var buf  = new uchar[size];
        try {
            gis.read(buf);
        }
        catch (Error e) {
            error("%s", e.message);
        }

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
        summary_info = meta;
    }
}

public class GHWP.Page : Object
{
    public void get_size (double *width, double *height)
    {
        // TODO
        *width = 595.0;
        *height = 842.0;
    }

    public bool render (Cairo.Context cr)
    {
        return render_static (cr);
    }
    
    static bool render_static (Cairo.Context cr)
    {
//        cr.select_font_face("Sans", Cairo.FontSlant.NORMAL,
//                                    Cairo.FontWeight.NORMAL);
//        cr.set_font_size(10.0);
//        cr.set_source_rgba(0.0, 0.0, 0.0, 1.0); // black
//        cr.move_to(100, 100);
//        cr.show_text("This is test string.");
        return true;
    }
}
