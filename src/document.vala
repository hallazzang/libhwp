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

public class TextP : Object
{
    public Gee.ArrayList<TextSpan> textspans = new Gee.ArrayList<TextSpan>();

    public void add_textspan(TextSpan textspan)
    {
        textspans.add(textspan);
    }
}

public class TextSpan : Object
{
    public string text;

    public TextSpan(string text)
    {
        this.text = text;
    }
}

public class GHWP.Document : Object
{
    public GHWP.GHWPFile ghwp_file;
    public string        prv_text;
    public Gee.ArrayList<TextP> office_text = new Gee.ArrayList<TextP>();
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
        while ( context.pull() ) {
            // TODO
        }
    }

    string get_text_from_raw_data(uchar[] raw)
    {
        unichar ch;
        string text = "";
        for (int i = 0; i < raw.length; i = i + 2) {
            ch = (raw[i+1] <<  8) | raw[i];
            switch (ch) {
            case 0:
                break;
            case 1:
            case 2:
            case 3:
            case 4: // inline
            case 5: // inline
            case 6: // inline
            case 7: // inline
            case 8: // inline
                i += 14;
                break;
            case 9: // tab // inline
                i += 14;
                text += ch.to_string();
                break;
            case 10:
                break;
            case 11:
            case 12:
                i += 14;
                break;
            case 13:
                break;
            case 14:
            case 15:
            case 16:
            case 17:
            case 18:
            case 19: // inline
            case 20: // inline
            case 21:
            case 22:
            case 23:
                i += 14;
                break;
            case 24:
            case 25:
            case 26:
            case 27:
            case 28:
            case 29:
            case 30:
            case 31:
                break;
            default:
                text += ch.to_string();
                break;
            }
        }
        return text;
    }

    void parse_body_text()
    {
        uint curr_lv = 0;
        uint prev_lv = 0;

        foreach (var section_stream in ghwp_file.section_streams) {
            var context = new GHWP.Context(section_stream);
            while ( context.pull() ) {
                for (int i = 0; i < context.level; i++) {
                    stdout.printf("  ");
                }
                curr_lv = context.level;
                stdout.printf("%s\n", GHWP.Tag.NAMES[context.tag_id]);
                switch (context.tag_id) {
                case GHWP.Tag.PARA_HEADER:
                    if (curr_lv > prev_lv ) {
                        // TODO
                        office_text.add(new TextP());
                    }
                    else if (curr_lv < prev_lv) {
                        office_text.add(new TextP());
                    }
                    else if (curr_lv == prev_lv) {
                        office_text.add(new TextP());
                    }
                    break;
                case GHWP.Tag.PARA_TEXT:
                    if (curr_lv > prev_lv) {
                        var textp = office_text.get(office_text.size - 1);
                        var text = get_text_from_raw_data(context.data);
                        ((TextP)textp).add_textspan(new TextSpan(text));
                    }
                    else if (curr_lv < prev_lv) {
                    }
                    else if (curr_lv == prev_lv) {
                    }
                    break;
                case GHWP.Tag.PARA_CHAR_SHAPE:
                    // TODO
                    break;
                case GHWP.Tag.PARA_LINE_SEG:
                    // TODO
                    break;
                case GHWP.Tag.CTRL_HEADER:
                    // TODO
                    break;
                case GHWP.Tag.PAGE_DEF:
                    // TODO
                    break;
                case GHWP.Tag.FOOTNOTE_SHAPE:
                    // TODO
                    break;
                case GHWP.Tag.PAGE_BORDER_FILL:
                    // TODO
                    break;
                case GHWP.Tag.LIST_HEADER:
                    // TODO
                    break;
                case GHWP.Tag.EQEDIT:
                    // TODO
                    break;
                default:
                    stderr.printf("%s: not implemented\n",
                                   GHWP.Tag.NAMES[context.tag_id]);
                    // Process.exit(1);
                    break;
                } // switch
                prev_lv = curr_lv;
            } // while
        } // foreach

        make_pages();
    } // parse_body_text()

    void make_pages()
    {
        var desc = Pango.FontDescription.from_string("Sans 12");
        var width  = 595 * Pango.SCALE;
        var height = 842 * Pango.SCALE;
        var fm = Pango.CairoFontMap.new();
        var pc = new Pango.Context();
        pc.set_font_map(fm);
        pc.load_font(desc);

        Pango.Rectangle ink_rect, logical_rect;
        double y_pos = 0;
        var page = new GHWP.Page();
        foreach (var textp in office_text) {
            foreach (var textspan in textp.textspans) {
                var layout = new Pango.Layout(pc);
                layout.set_font_description(desc);
                layout.set_width (width * Pango.SCALE);
                layout.set_wrap (Pango.WrapMode.WORD_CHAR);
                layout.set_text(textspan.text, -1);
                layout.get_extents(out ink_rect, out logical_rect);
                y_pos += logical_rect.height;

                if (y_pos >= width) {
                    pages.add(page);
                    page = new GHWP.Page();
                    y_pos = logical_rect.height;
                    stdout.printf("n_page = %d\n", pages.size);
                }
                page.elements.add (textspan);
                stdout.printf("%s\n", textspan.text);
            }
        }
    }

    void parse_prv_text()
    {
        var gis  = (GsfInputStream) ghwp_file.prv_text_stream;
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
    public Gee.ArrayList<Object> elements = new Gee.ArrayList<Object>();

    public void get_size (out double width, out double height)
    {
        // TODO
        width = 595.0;
        height = 842.0;
    }

    public bool render (Cairo.Context cr)
    {
        var desc = Pango.FontDescription.from_string("Sans 12");
        var width = 595;
        Pango.Rectangle ink_rect, logical_rect;
        double dy = 0;

        foreach (var element in elements) {
            var textspan = element as TextSpan;
            var layout = Pango.cairo_create_layout (cr);
            layout.set_font_description(desc);
            layout.set_width (width * Pango.SCALE);
            layout.set_wrap (Pango.WrapMode.WORD_CHAR);
            layout.set_text(textspan.text, -1);
            layout.get_extents(out ink_rect, out logical_rect);
            dy += (logical_rect.height / Pango.SCALE);
            cr.move_to (0, dy);
            cr.set_source_rgba(0.0, 0.0, 0.0, 1.0); // black
            Pango.cairo_show_layout(cr, layout);
        }
        return true;
    }
}
