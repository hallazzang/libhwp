/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * ghwp-file-v5.c
 *
 * Copyright (C) 2012-2013 Hodong Kim <cogniti@gmail.com>
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
 * This software have been developed with reference to
 * the HWP file format open specification by Hancom, Inc.
 * http://www.hancom.co.kr/userofficedata.userofficedataList.do?menuFlag=3
 * 한글과컴퓨터의 한/글 문서 파일(.hwp) 공개 문서를 참고하여 개발하였습니다.
 */

#include <glib.h>
#include <glib-object.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <gsf/gsf-input-impl.h>
#include <gsf/gsf-input-memory.h>
#include <gsf/gsf-msole-utils.h>
#include <gsf/gsf-input-stdio.h>
#include <gsf/gsf-infile-impl.h>
#include <gsf/gsf-doc-meta-data.h>
#include <gsf/gsf-meta-names.h>
#include <gsf/gsf-timestamp.h>

#include "gsf-input-stream.h"
#include "ghwp-file-v5.h"
#include "config.h"
#include <pango/pango.h>
#include <pango/pangocairo.h>

G_DEFINE_TYPE (GHWPFileV5, ghwp_file_v5, GHWP_TYPE_FILE);

#define _g_object_unref0(var) ((var == NULL) ? NULL : (var = (g_object_unref (var), NULL)))
#define _g_array_free0(var) ((var == NULL) ? NULL : (var = (g_array_free (var, TRUE), NULL)))
#define _g_error_free0(var) ((var == NULL) ? NULL : (var = (g_error_free (var), NULL)))
#define _g_free0(var) (var = (g_free (var), NULL))

static gpointer _g_object_ref0 (gpointer obj)
{
    return obj ? g_object_ref (obj) : NULL;
}

/*typedef enum {*/
/*    ID_BINARY_DATA      = 0,*/
/*    ID_KOREAN_FONTS     = 1,*/
/*    ID_ENGLISH_FONTS    = 2,*/
/*    ID_HANJA_FONTS      = 3,*/
/*    ID_JAPANESE_FONTS   = 4,*/
/*    ID_OTHERS_FONTS     = 5,*/
/*    ID_SYMBOL_FONTS     = 6,*/
/*    ID_USER_FONTS       = 7,*/
/*    ID_BORDER_FILLS     = 8,*/
/*    ID_CHAR_SHAPES      = 9,*/
/*    ID_TAB_DEFS         = 10,*/
/*    ID_PARA_NUMBERINGS  = 11,*/
/*    ID_BULLETS          = 12,*/
/*    ID_PARA_SHAPES      = 13,*/
/*    ID_STYLES           = 14,*/
    /*
     * 메모 모양(MemoShape)는 한/글2007부터 추가되었다.
     * 한/글2007 이전 문서는 data_len <= 60,
     * v5.0.0.6 : ID_MAPPINGS data_len: 60
     * v5.0.1.7 : ID_MAPPINGS data_len: 64
     * v5.0.2.4 : ID_MAPPINGS data_len: 64
     */
/*    ID_MEMO_SHAPES      = 15,*/
    /* 한/글2010 에서 추가된 것으로 추정됨 */
    /* v5.0.3.4 : ID_MAPPINGS data_len: 72 */
/*    ID_KNOWN_16         = 16,*/
/*    ID_KNOWN_17         = 17,*/
/*} IDMappingsID;*/

static void _ghwp_file_v5_parse_doc_info (GHWPFileV5 *file, GError **error)
{
    g_return_if_fail (GHWP_IS_FILE_V5 (file));

/*    guint32 id_mappings[16] = {0}; */ /* 반드시 초기화 해야 한다. */
/*    int i;*/

/*    GInputStream *stream  = file->doc_info_stream;
    GHWPContext  *context = ghwp_context_new (stream);
    while (ghwp_context_pull (context, error)) {
        switch (context->tag_id) {
        case GHWP_TAG_DOCUMENT_PROPERTIES:*/
            /* TODO */
/*            break;
        case GHWP_TAG_ID_MAPPINGS:*/
/*            for (i = 0; i < sizeof(id_mappings); i = i + sizeof(guint32)) {*/
/*                memcpy(&id_mappings[i], &(context->data[i]), sizeof(guint32));*/
/*                id_mappings[i] = GUINT16_FROM_LE(id_mappings[i]);*/
/*                printf("%d\n", id_mappings[i]);*/
/*            }*/
/*            break;
        default:
            printf("%s:%d: %s not implemented\n", __FILE__, __LINE__,
                _ghwp_get_tag_name (context->tag_id));
            break;
        }
    }

    g_object_unref (context);*/
}

static gchar *_ghwp_file_get_text_from_context (GHWPContext *context)
{
    g_return_val_if_fail (context != NULL, NULL);
    gunichar2 ch; /* guint16 */
    GString  *text = g_string_new("");
    guint     i;

    for (i = 0; i < context->data_len; i = i + 2)
    {
        context_read_uint16 (context, &ch);
        switch (ch) {
        case 0:
            break;
        case 1:
        case 2:
        case 3:
        case 4: /* inline */
        case 5: /* inline */
        case 6: /* inline */
        case 7: /* inline */
        case 8: /* inline */
            i = i + 14;
            context_skip(context, 14);;
            break;
        case 9: /* inline */ /* tab */
            i = i + 14;
            context_skip(context, 14);;
            g_string_append_unichar(text, ch);
            break;
        case 10:
            break;
        case 11:
        case 12:
            i = i + 14;
            context_skip(context, 14);;
            break;
        case 13:
            break;
        case 14:
        case 15:
        case 16:
        case 17:
        case 18:
        case 19: /* inline */
        case 20: /* inline */
        case 21:
        case 22:
        case 23:
            i = i + 14;
            context_skip(context, 14);;
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
            g_string_append_unichar(text, ch);
            break;
        } /* switch */
    } /* for */

    if (context->data_count != context->data_len) {
        g_string_free(text, TRUE);
        return NULL;
    }

    return g_string_free(text, FALSE);
}

/* NOTE: LE 저장 방식이 아닌 점에 유의, 설계 실수 같음 */
#define MAKE_CTRL_ID(a, b, c, d)      \
    (guint32)((((guint8)(a)) << 24) | \
              (((guint8)(b)) << 16) | \
              (((guint8)(c)) <<  8) | \
              (((guint8)(d)) <<  0))

/* enum의 최대값은 ?? */
typedef enum
{
    CTRL_ID_TABLE = GUINT32_FROM_LE(MAKE_CTRL_ID('t', 'b', 'l', ' ')),
    CTRL_ID_SECTION_DEFINITION = GUINT32_FROM_LE(MAKE_CTRL_ID('s', 'e', 'c', 'd'))
} CtrlID;

    /*
          \  col 0   col 1
           +-------+-------+
    row 0  |  00   |   01  |
           +-------+-------+
    row 1  |  10   |   11  |
           +-------+-------+
    row 2  |  20   |   21  |
           +-------+-------+

    <table> ::= { <list-header> <para-header>+ }+

    para-header
        ...
        ctrl-header (id:tbl)
            table: row-count, col-count
            list-header (00)
            ...
            list-header (01)
            ...
            list-header (10)
            ...
            list-header (11)
            ...
            list-header (20)
            ...
            list-header (21)
    */

static void _ghwp_file_v5_parse_body_text (GHWPFileV5 *file, GError **error)
{
    g_return_if_fail (GHWP_IS_FILE_V5 (file));

    gdouble y = 0.0, dy = 0.0;
    GHWPDocument *doc  = file->document;
    GHWPPage     *page = ghwp_page_new ();

    for (guint i = 0; i < file->section_streams->len; i++) {
        GInputStream  *stream       = g_array_index (file->section_streams,
                                                     GInputStream *, i);
        GHWPContext   *ghwp_context = ghwp_context_new (stream);
        ghwp_context->document      = file->document;
        PangoFontMap  *fontmap      = pango_cairo_font_map_get_default ();
        PangoContext  *context      = pango_font_map_create_context (fontmap);
        PangoLanguage *lang         = pango_language_from_string ("ko_KR");
        guint32        ctrl_id      = 0;
        guint16        ctrl_lv      = 0;
        guint16        curr_lv      = 0;
        PangoLayout   *layout       = NULL;
        GHWPTable     *table        = NULL;
        GHWPTableCell *cell         = NULL;
        pango_context_set_language (context, lang);

        while (ghwp_context_pull(ghwp_context, error)) {
            curr_lv = ghwp_context->level;
            /* 상태 변화 */
            if (curr_lv <= ctrl_lv)
                ghwp_context->status = STATE_NORMAL;

            switch (ghwp_context->tag_id) {
            case GHWP_TAG_PARA_TEXT:
                if (ghwp_context->status == STATE_NORMAL) {
                    layout = pango_layout_new (context);
                    pango_layout_set_width (layout, 595 * PANGO_SCALE);
                    pango_layout_set_wrap  (layout, PANGO_WRAP_WORD_CHAR);
                    gchar *text =_ghwp_file_get_text_from_context (ghwp_context);
                    if (text)
                        pango_layout_set_text (layout, text, -1);
                    g_free (text);

                    GHWPLayout *ghwp_layout   = g_new0 (GHWPLayout, 1);
                    ghwp_layout->pango_layout = layout;

                    ghwp_layout->x = 0;
                    ghwp_layout->y = y;

                    int height;
                    pango_layout_get_size (layout, NULL, &height);
                    dy = height / (gdouble) PANGO_SCALE;
                    y += dy;

                    /* pagination */
                    if (y >= 842.0) {
                        y = 0;
                        g_array_append_val (doc->pages, page);
                        page = ghwp_page_new ();
                        ghwp_layout->y = y;
                        g_array_append_val (page->layouts, ghwp_layout);
                        y = dy;
                    } else {
                        g_array_append_val (page->layouts, ghwp_layout);
                    }
                /* table cell text */
                } else if (ghwp_context->status == STATE_INSIDE_TABLE) {
                    layout = pango_layout_new (context);
                    pango_layout_set_width (layout, cell->width / 100.0 * PANGO_SCALE);
                    pango_layout_set_wrap  (layout, PANGO_WRAP_WORD_CHAR);
                    gchar *text =_ghwp_file_get_text_from_context (ghwp_context);
                    if (text)
                        pango_layout_set_text (layout, text, -1);
                    g_free (text);

                    GHWPLayout *ghwp_layout   = g_new0 (GHWPLayout, 1);
                    ghwp_layout->pango_layout = layout;
                    /* TODO x, y coordinate */
                    ghwp_layout->x = cell->width / 100.0 * cell->col_addr;
                    ghwp_layout->y = y;

                    int height;
                    pango_layout_get_size (layout, NULL, &height);
                    y += dy;

                    /* pagination */
                    if (y >= 842.0) {
                        y = 0;
                        g_array_append_val (doc->pages, page);
                        page = ghwp_page_new ();
                        ghwp_layout->y = y;
                        g_array_append_val (page->layouts, ghwp_layout);
                        y = dy;
                    } else {
                        g_array_append_val (page->layouts, ghwp_layout);
                    }
                } /* if */
                break;
            case GHWP_TAG_CTRL_HEADER:
                context_read_uint32 (ghwp_context, &ctrl_id);
                switch (ctrl_id) {
                case CTRL_ID_TABLE:
                    ghwp_context->status = STATE_INSIDE_TABLE;
                    break;
                default:
/*                    g_warning ("%s:%d:%s not implemented",
                               __FILE__, __LINE__, ghwp_get_tag_name (ctrl_id));*/
                    break;
                }
                break;
            case GHWP_TAG_TABLE:
                table = ghwp_table_new_from_context (ghwp_context);
                break;
            case GHWP_TAG_LIST_HEADER:
                if (ghwp_context->status == STATE_INSIDE_TABLE) {
                    cell = ghwp_table_cell_new_from_context (ghwp_context);
/*                    printf ("row_addr:%d, col_addr:%d, y = %f\n",*/
/*                            cell->row_addr,*/
/*                            cell->col_addr,*/
/*                            y);*/
/*                    if (cell->col_addr == 0) {
                        cell->_y = y;
                        printf ("%f\n", y);
                    } else {
                        y = cell->_y;
                    }*/
                }
                break;
            default:
/*                g_warning ("%s:%d:%s not implemented",
                           __FILE__, __LINE__, ghwp_get_tag_name (ghwp_context->tag_id));*/
                break;
            } /* switch */
        } /* while */
        /* add last page */
        /* FIXME: problem that a page is appended
         even if there is no page in document. */
        g_array_append_val (doc->pages, page);
        g_object_unref (ghwp_context);
    } /* for */
}

static void _ghwp_file_v5_parse_prv_text (GHWPFileV5 *file)
{
    g_return_if_fail (GHWP_IS_FILE_V5 (file));

    GsfInputStream *gis   = _g_object_ref0 (file->prv_text_stream);
    gssize          size  = gsf_input_stream_size (gis);
    guchar         *buf   = g_new (guchar, size);
    GError         *error = NULL;
    GHWPDocument   *doc   = file->document;

    g_input_stream_read ((GInputStream*) gis, buf, size, NULL, &error);

    if (error != NULL) {
        g_warning("%s:%d: %s\n", __FILE__, __LINE__, error->message);
        _g_free0 (file->document->prv_text);
        g_clear_error (&error);
        buf = (g_free (buf), NULL);
        _g_object_unref0 (gis);
        return;
    }

    /* g_convert() can be used to convert a byte buffer of UTF-16 data of
       ambiguous endianess. */
    doc->prv_text = g_convert ((const gchar*) buf, (gssize) size,
                               "UTF-8", "UTF-16LE", NULL, NULL, &error);

    if (error != NULL) {
        g_warning("%s:%d: %s\n", __FILE__, __LINE__, error->message);
        _g_free0 (doc->prv_text);
        g_clear_error (&error);
        buf = (g_free (buf), NULL);
        _g_object_unref0 (gis);
        return;
    }

    buf = (g_free (buf), NULL);
    _g_object_unref0 (gis);
}

/* 알려지지 않은 것을 감지하기 위해 이렇게 작성함 */
static void
_ghwp_metadata_hash_func (gpointer k, gpointer v, gpointer user_data)
{
    gchar        *name  = (gchar        *) k;
    GsfDocProp   *prop  = (GsfDocProp   *) v;
    GHWPDocument *doc   = (GHWPDocument *) user_data;
    GValue const *value = gsf_doc_prop_get_val (prop);

    if ( g_str_equal(name, GSF_META_NAME_CREATOR) ) {
        doc->creator = g_value_get_string (value);
    } else if ( g_str_equal(name, GSF_META_NAME_DATE_MODIFIED) ) {
        GsfTimestamp *ts    = g_value_get_boxed (value);
        doc->mod_date = (GTime) ts->timet;
    } else if ( g_str_equal(name, GSF_META_NAME_DESCRIPTION) ) {
        doc->desc = g_value_get_string (value);
    } else if ( g_str_equal(name, GSF_META_NAME_KEYWORDS) ) {
        doc->keywords = g_value_get_string (value);
    } else if ( g_str_equal(name, GSF_META_NAME_SUBJECT) ) {
        doc->subject = g_value_get_string (value);
    } else if ( g_str_equal(name, GSF_META_NAME_TITLE) ) {
        doc->title = g_value_get_string (value);
    } else if ( g_str_equal(name, GSF_META_NAME_LAST_PRINTED) ) {
        GsfTimestamp *ts    = g_value_get_boxed (value);
        doc->last_printed   = (GTime) ts->timet;
    } else if ( g_str_equal(name, GSF_META_NAME_LAST_SAVED_BY) ) {
        doc->last_saved_by = g_value_get_string (value);
    } else if ( g_str_equal(name, GSF_META_NAME_DATE_CREATED) ) {
        GsfTimestamp *ts    = g_value_get_boxed (value);
        doc->creation_date  = (GTime) ts->timet;
    /* hwp 문서를 저장할 때 사용된 한컴 워드프로세서의 내부 버전 */
    } else if ( g_str_equal(name, GSF_META_NAME_REVISION_COUNT) ) {
        doc->hanword_version = g_value_get_string (value);
    } else if ( g_str_equal(name, GSF_META_NAME_PAGE_COUNT) ) {
        /* not correct n_pages == 0 ?? */
        /* g_value_get_int (value); */
    } else {
        g_warning("%s:%d:%s not implemented\n", __FILE__, __LINE__, name);
    }
}

/*
typedef enum {
    INFO_ID_CREATOR,
    INFO_ID_DATE_MODIFIED,
    INFO_ID_DESCRIPTION,
    INFO_ID_KEYWORDS,
    INFO_ID_SUBJECT,
    INFO_ID_TITLE,
    INFO_ID_LAST_PRINTED,
    INFO_ID_LAST_SAVED_BY,
    INFO_ID_DATE_CREATED,
    INFO_ID_REVISION_COUNT,
    INFO_ID_PAGE_COUNT,
} InfoID;

typedef struct {
    InfoID id;
    const  gchar *name;
} SummaryInfo;

SummaryInfo info[]   = {
        { INFO_ID_CREATOR,        GSF_META_NAME_CREATOR       },
        { INFO_ID_DATE_MODIFIED,  GSF_META_NAME_DATE_MODIFIED },
        { INFO_ID_DESCRIPTION,    GSF_META_NAME_DESCRIPTION   },
        { INFO_ID_KEYWORDS,       GSF_META_NAME_KEYWORDS      },
        { INFO_ID_SUBJECT,        GSF_META_NAME_SUBJECT       },
        { INFO_ID_TITLE,          GSF_META_NAME_TITLE         },
        { INFO_ID_LAST_PRINTED,   GSF_META_NAME_LAST_PRINTED  },
        { INFO_ID_LAST_SAVED_BY,  GSF_META_NAME_LAST_SAVED_BY },
        { INFO_ID_DATE_CREATED,   GSF_META_NAME_DATE_CREATED  },
        { INFO_ID_REVISION_COUNT, GSF_META_NAME_REVISION_COUNT},
        { INFO_ID_PAGE_COUNT,     GSF_META_NAME_PAGE_COUNT    }
};
*/

static void _ghwp_file_v5_parse_summary_info (GHWPFileV5 *file)
{
    g_return_if_fail (GHWP_IS_FILE_V5 (file));

    GsfInputStream *gis;
    gssize          size;
    guint8         *buf = NULL;
    GsfInputMemory *summary;
    GsfDocMetaData *meta;
    GError         *error = NULL;
    GHWPDocument   *doc   = file->document;

    gis  = _g_object_ref0 (file->summary_info_stream);
    size = gsf_input_stream_size (gis);
    buf  = g_malloc(size);

    g_input_stream_read ((GInputStream*) gis, buf, (gsize) size, NULL, &error);

    if (error != NULL) {
        buf = (g_free (buf), NULL);
        _g_object_unref0 (gis);
        g_warning("%s:%d: %s\n", __FILE__, __LINE__, error->message);
        g_clear_error (&error);
        return;
    }

    /* changwoo's solution, thanks to changwoo.
     * https://groups.google.com/forum/#!topic/libhwp/gFDD7UMCXBc
     * https://github.com/changwoo/gnome-hwp-support/blob/master/properties/props-data.c
     * Trick the libgsf's MSOLE property set parser, by changing
     * its GUID. The \005HwpSummaryInformation is compatible with
     * the summary property set.
     */
    guint8 component_guid [] = {
        0xe0, 0x85, 0x9f, 0xf2, 0xf9, 0x4f, 0x68, 0x10,
        0xab, 0x91, 0x08, 0x00, 0x2b, 0x27, 0xb3, 0xd9
    };

    if (size >= sizeof(component_guid) + 28) {
        memcpy (buf + 28, component_guid, (gsize) sizeof(component_guid));
    } else {
        buf = (g_free (buf), NULL);
        _g_object_unref0 (file->summary_info_stream);
        _g_object_unref0 (gis);
        g_warning("%s:%d: file corrupted\n", __FILE__, __LINE__);
        return;
    }
    summary = (GsfInputMemory*) gsf_input_memory_new (buf, size, FALSE);

    meta = gsf_doc_meta_data_new ();

#ifdef HAVE_GSF_DOC_META_DATA_READ_FROM_MSOLE
    /* since libgsf 1.14.24 */
    gsf_doc_meta_data_read_from_msole (meta, (GsfInput*) summary);
#else
    /* NOTE gsf_msole_metadata_read: deprecated since libgsf 1.14.24 */
    gsf_msole_metadata_read ((GsfInput*) summary, meta);
#endif

    gsf_doc_meta_data_foreach (meta, _ghwp_metadata_hash_func, doc);

    _g_object_unref0 (doc->summary_info);
    doc->summary_info = _g_object_ref0 (meta);
    _g_object_unref0 (meta);
    _g_object_unref0 (summary);
    buf = (g_free (buf), NULL);
    _g_object_unref0 (gis);
}

static void _ghwp_file_v5_parse (GHWPFileV5 *file, GError **error)
{
    g_return_if_fail (file != NULL);

    _ghwp_file_v5_parse_doc_info (file, error);
    if (*error) return;
    _ghwp_file_v5_parse_body_text (file, error);
    if (*error) return;
    _ghwp_file_v5_parse_prv_text (file);
    _ghwp_file_v5_parse_summary_info (file);
}

/**
 * Since: 0.2
 **/
GHWPDocument *ghwp_file_v5_get_document (GHWPFile *file, GError **error)
{
    g_return_val_if_fail (GHWP_IS_FILE_V5 (file), NULL);

    GHWPFileV5 *file_v5 = GHWP_FILE_V5 (file);
    file_v5->document = ghwp_document_new();
    file_v5->document->major_version = file_v5->major_version;
    file_v5->document->minor_version = file_v5->minor_version;
    file_v5->document->micro_version = file_v5->micro_version;
    file_v5->document->extra_version = file_v5->extra_version;

    _ghwp_file_v5_parse (file_v5, error);

    return file_v5->document;
}

/**
 * ghwp_file_v5_get_hwp_version:
 * @file: A #GHWPFile
 * @major_version: (out) (allow-none): return location for the HWP major version number
 * @minor_version: (out) (allow-none): return location for the HWP minor version number
 * @micro_version: (out) (allow-none): return location for the HWP micro version number
 * @extra_version: (out) (allow-none): return location for the HWP extra version number
 *
 * Returns: the major and minor and micro and extra HWP version numbers
 *
 * Since: 0.2
 **/
void ghwp_file_v5_get_hwp_version (GHWPFile *file,
                                   guint8   *major_version,
                                   guint8   *minor_version,
                                   guint8   *micro_version,
                                   guint8   *extra_version)
{
    g_return_if_fail (GHWP_IS_FILE_V5 (file));

    if (major_version) *major_version = GHWP_FILE_V5(file)->major_version;
    if (minor_version) *minor_version = GHWP_FILE_V5(file)->minor_version;
    if (micro_version) *micro_version = GHWP_FILE_V5(file)->micro_version;
    if (extra_version) *extra_version = GHWP_FILE_V5(file)->extra_version;
}

/**
 * Since: 0.2
 **/
gchar *ghwp_file_v5_get_hwp_version_string (GHWPFile *file)
{
    g_return_val_if_fail (GHWP_IS_FILE_V5 (file), NULL);

    return g_strdup_printf ("%d.%d.%d.%d", GHWP_FILE_V5(file)->major_version,
                                           GHWP_FILE_V5(file)->minor_version,
                                           GHWP_FILE_V5(file)->micro_version,
                                           GHWP_FILE_V5(file)->extra_version);
}

/**
 * ghwp_file_v5_new_from_uri:
 * @uri: uri of the file to load
 * @error: (allow-none): Return location for an error, or %NULL
 *
 * Creates a new #GHWPFileV5.  If %NULL is returned, then @error will be
 * set. Possible errors include those in the #GHWP_ERROR and #G_FILE_ERROR
 * domains.
 *
 * Return value: A newly created #GHWPFileV5, or %NULL
 *
 * Since: 0.2
 **/
GHWPFileV5* ghwp_file_v5_new_from_uri (const gchar* uri, GError** error)
{
    g_return_val_if_fail (uri != NULL, NULL);

    gchar      *filename = g_filename_from_uri (uri, NULL, error);
    GHWPFileV5 *file     = ghwp_file_v5_new_from_filename (filename, error);
    g_free (filename);
    return file;
}

/* TODO 에러 감지/전파 코드 있어야 한다. */
static void ghwp_file_v5_decode_file_header (GHWPFileV5 *file)
{
    g_return_if_fail (file != NULL);

    GsfInputStream *gis;
    gssize          size;
    gsize           bytes_read;
    guint8         *buf;
    guint32         prop = 0;

    gis  = (GsfInputStream *) g_object_ref (file->file_header_stream);
    size = gsf_input_stream_size (gis);
    buf  = g_malloc (size);

    g_input_stream_read_all ((GInputStream*) gis, buf, (gsize) size,
                             &bytes_read, NULL, NULL);
    g_object_unref (gis);

    if (bytes_read >= 40) {
        file->signature = g_strndup ((const gchar *)buf, 32); /* null로 끝남 */
        file->major_version = buf[35];
        file->minor_version = buf[34];
        file->micro_version = buf[33];
        file->extra_version = buf[32];

        memcpy (&prop, buf + 36, 4);
        prop = GUINT32_FROM_LE(prop);

        file->is_compress            = prop & (1 <<  0);
        file->is_encrypt             = prop & (1 <<  1);
        file->is_distribute          = prop & (1 <<  2);
        file->is_script              = prop & (1 <<  3);
        file->is_drm                 = prop & (1 <<  4);
        file->is_xml_template        = prop & (1 <<  5);
        file->is_history             = prop & (1 <<  6);
        file->is_sign                = prop & (1 <<  7);
        file->is_certificate_encrypt = prop & (1 <<  8);
        file->is_sign_spare          = prop & (1 <<  9);
        file->is_certificate_drm     = prop & (1 << 10);
        file->is_ccl                 = prop & (1 << 11);
    }

    buf = (g_free (buf), NULL);
}

static gint get_entry_order (char *a)
{
    if (g_str_equal (a, "FileHeader"))
        return 0;
    if (g_str_equal (a, "DocInfo"))
        return 1;
    if (g_str_equal (a, "BodyText"))
        return 2;
    if (g_str_equal (a, "ViewText"))
        return 3;
    if (g_str_equal (a, "\005HwpSummaryInformation"))
        return 4;
    if (g_str_equal (a, "BinData"))
        return 5;
    if (g_str_equal (a, "PrvText"))
        return 6;
    if (g_str_equal (a, "PrvImage"))
        return 7;
    if (g_str_equal (a, "DocOptions"))
        return 8;
    if (g_str_equal (a, "Scripts"))
        return 9;
    if (g_str_equal (a, "XMLTemplate"))
        return 10;
    if (g_str_equal (a, "DocHistory"))
        return 11;

    return G_MAXINT;
}

static gint compare_entry_names (gconstpointer a, gconstpointer b)
{
    gint i = get_entry_order (*(char **)a);
    gint j = get_entry_order (*(char **)b);
    return i - j;
}

/* FIXME streams 배열과 enum을 이용하여 코드 재적성 바람 */
static void _ghwp_file_v5_make_stream (GHWPFileV5 *file)
{
    g_return_if_fail (GHWP_IS_FILE_V5 (file));

    const gchar *name       = NULL;
    GsfInfile   *oleinfile  = GSF_INFILE (file->priv->olefile);
    gint         n_children = gsf_infile_num_children (oleinfile);

    if (n_children < 1) {
        fprintf (stderr, "invalid hwp file\n");
        return;
    }

    /* 스펙이 명확하지 않고, 추후 예고없이 스펙이 변할 수 있기 때문에
     * 이를 감지하고자 코드를 이렇게 작성하였다. */
    GArray *entry_names = g_array_new (TRUE, TRUE, sizeof(char *));

    for (gint i = 0; i < n_children; i++) {
        name = gsf_infile_name_by_index (oleinfile, i);
        g_array_append_val (entry_names, name);
    }
    g_array_sort(entry_names, compare_entry_names);

    for (gint i = 0; i < n_children; i++) {
        char     *entry = g_array_index (entry_names, char *, i);
        GsfInput* input;
        gint      n_children = 0;

        if (g_str_equal (entry, "FileHeader")) {
            input = gsf_infile_child_by_name (oleinfile, entry);
            n_children = gsf_infile_num_children ((GsfInfile*) input);

            if (n_children > 0) {
                fprintf (stderr, "invalid\n");
            }

            file->file_header_stream = G_INPUT_STREAM (gsf_input_stream_new (input));
            ghwp_file_v5_decode_file_header (file);
        } else if (g_str_equal (entry, "DocInfo")) {
            input = gsf_infile_child_by_name (oleinfile, entry);
            input = _g_object_ref0 (input);
            n_children = gsf_infile_num_children ((GsfInfile*) input);

            if (n_children > 0) {
                fprintf (stderr, "invalid\n");
            }

            if (file->is_compress) {
                GsfInputStream    *gis;
                GZlibDecompressor *zd;
                GInputStream      *cis;

                gis = gsf_input_stream_new (input);
                zd  = g_zlib_decompressor_new (G_ZLIB_COMPRESSOR_FORMAT_RAW);
                cis = g_converter_input_stream_new ((GInputStream*) gis,
                                                    (GConverter*) zd);
                _g_object_unref0 (file->doc_info_stream);
                file->doc_info_stream = cis;

                _g_object_unref0 (zd);
                _g_object_unref0 (gis);
            } else {
                _g_object_unref0 (file->doc_info_stream);
                file->doc_info_stream = (GInputStream*) gsf_input_stream_new (input);
            }
            _g_object_unref0 (input);
        } else if (g_str_equal(entry, "BodyText") ||
                   g_str_equal(entry, "VeiwText")) {
            GsfInfile* infile;

            _g_array_free0 (file->section_streams);
            file->section_streams = g_array_new (TRUE, TRUE,
                                                 sizeof (GInputStream*));

            infile = (GsfInfile*) gsf_infile_child_by_name (oleinfile, entry);
            infile = _g_object_ref0 (infile);

            n_children = gsf_infile_num_children (infile);

            if (n_children == 0) {
                fprintf (stderr, "nothing in %s\n", entry);
            }

            gint j;
            for (j = 0; j < n_children; j++) {
                input = gsf_infile_child_by_index (infile, j);
                GsfInfile *section = _g_object_ref0 (input);
                n_children = gsf_infile_num_children (section);

                if (n_children > 0) {
                    fprintf (stderr, "invalid section\n");
                }

                if (file->is_compress) {
                    GsfInputStream* gis;
                    GZlibDecompressor* zd;
                    GInputStream* cis;

                    gis = gsf_input_stream_new ((GsfInput*) section);
                    zd  = g_zlib_decompressor_new (G_ZLIB_COMPRESSOR_FORMAT_RAW);
                    cis = g_converter_input_stream_new ((GInputStream *) gis,
                                                        (GConverter *) zd);

                    _g_object_unref0 (file->priv->section_stream);
                    file->priv->section_stream = G_INPUT_STREAM (cis);
                    _g_object_unref0 (zd);
                    _g_object_unref0 (gis);
                } else {
                    GsfInputStream* stream;
                    stream = gsf_input_stream_new ((GsfInput*) section);
                    _g_object_unref0 (file->priv->section_stream);
                    file->priv->section_stream = G_INPUT_STREAM (stream);
                }

                GInputStream *_stream_ = file->priv->section_stream;
                _stream_ = _g_object_ref0 (_stream_);
                g_array_append_val (file->section_streams, _stream_);
                _g_object_unref0 (section);
            } /* for */
            _g_object_unref0 (infile);
        } else if (g_str_equal (entry, "\005HwpSummaryInformation")) {
            input = gsf_infile_child_by_name (oleinfile, entry);
            input = _g_object_ref0 (input);
            n_children = gsf_infile_num_children ((GsfInfile*) input);

            if (n_children > 0) {
                fprintf (stderr, "invalid\n");
            }

            _g_object_unref0 (file->summary_info_stream);
            file->summary_info_stream = (GInputStream*) gsf_input_stream_new (input);
            _g_object_unref0 (input);
        } else if (g_str_equal (entry, "PrvText")) {
            input = gsf_infile_child_by_name (oleinfile, entry);
            input = _g_object_ref0 (input);
            n_children = gsf_infile_num_children ((GsfInfile*) input);

            if (n_children > 0) {
                fprintf (stderr, "invalid\n");
            }

            _g_object_unref0 (file->prv_text_stream);
            file->prv_text_stream = (GInputStream *) gsf_input_stream_new (input);
            _g_object_unref0 (input);
        } else if (g_str_equal (entry, "PrvImage")) {
            input = gsf_infile_child_by_name (oleinfile, entry);
            input = _g_object_ref0 (input);
            n_children = gsf_infile_num_children ((GsfInfile*) input);

            if (n_children > 0) {
                fprintf (stderr, "invalid\n");
            }

            _g_object_unref0 (file->prv_image_stream);
            file->prv_image_stream = (GInputStream*) gsf_input_stream_new (input);
            _g_object_unref0 (input);
        } else {
            g_warning("%s:%d: %s not implemented\n", __FILE__, __LINE__, entry);
        } /* if */
    } /* for */
    g_array_free (entry_names, TRUE);
    g_array_unref (entry_names);
}

/**
 * Since: 0.2
 **/
GHWPFileV5* ghwp_file_v5_new_from_filename (const gchar* filename, GError** error)
{
    g_return_val_if_fail (filename != NULL, NULL);
    GFile *gfile = g_file_new_for_path (filename);

    GsfInputStdio  *input;
    GsfInfileMSOle *olefile;

    gchar *path = g_file_get_path(gfile);
    _g_object_unref0 (gfile);
    input = (GsfInputStdio*) gsf_input_stdio_new (path, error);
    _g_free0 (path);

    if (input == NULL) {
        g_warning("%s:%d: %s\n", __FILE__, __LINE__, (*error)->message);
        return NULL;
    }

    olefile = (GsfInfileMSOle*) gsf_infile_msole_new ((GsfInput*) input,
                                                      error);

    if (olefile == NULL) {
        g_warning("%s:%d: %s\n", __FILE__, __LINE__, (*error)->message);
        _g_object_unref0 (input);
        return NULL;
    }

    GHWPFileV5 *file = g_object_new (GHWP_TYPE_FILE_V5, NULL);
    file->priv->olefile = olefile;
    _g_object_unref0 (input);
    _ghwp_file_v5_make_stream (file);

    return file;
}

static void ghwp_file_v5_finalize (GObject* obj)
{
    GHWPFileV5 *file = GHWP_FILE_V5(obj);
    _g_object_unref0 (file->priv->olefile);
    _g_object_unref0 (file->prv_text_stream);
    _g_object_unref0 (file->prv_image_stream);
    _g_object_unref0 (file->file_header_stream);
    _g_object_unref0 (file->doc_info_stream);
    _g_array_free0 (file->section_streams);
    _g_object_unref0 (file->priv->section_stream);
    _g_object_unref0 (file->summary_info_stream);
    g_free (file->signature);
    G_OBJECT_CLASS (ghwp_file_v5_parent_class)->finalize (obj);
}

static void ghwp_file_v5_class_init (GHWPFileV5Class * klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    g_type_class_add_private (klass, sizeof (GHWPFileV5Private));
    GHWPFileClass *file_class          = GHWP_FILE_CLASS (klass);
    file_class->get_document           = ghwp_file_v5_get_document;
    file_class->get_hwp_version_string = ghwp_file_v5_get_hwp_version_string;
    file_class->get_hwp_version        = ghwp_file_v5_get_hwp_version;
    object_class->finalize = ghwp_file_v5_finalize;
}

static void ghwp_file_v5_init (GHWPFileV5 *file)
{
    file->priv = G_TYPE_INSTANCE_GET_PRIVATE (file, GHWP_TYPE_FILE_V5,
                                                    GHWPFileV5Private);
}
