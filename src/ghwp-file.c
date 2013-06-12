/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * ghwp-file.c
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
#include <string.h>
#include <math.h>
#include <gsf/gsf-meta-names.h>
#include <gsf/gsf-input-memory.h>
#include <gsf/gsf-msole-utils.h>
#include <gsf/gsf-timestamp.h>

#include "ghwp-file.h"
#include "ghwp-file-v5.h"
#include "ghwp-file-v3.h"

G_DEFINE_TYPE (GHWPFile, ghwp_file, G_TYPE_OBJECT);

#define _g_object_unref0(var) ((var == NULL) ? NULL : (var = (g_object_unref (var), NULL)))
#define _g_array_free0(var) ((var == NULL) ? NULL : (var = (g_array_free (var, TRUE), NULL)))
#define _g_error_free0(var) ((var == NULL) ? NULL : (var = (g_error_free (var), NULL)))
#define _g_free0(var) (var = (g_free (var), NULL))

static gpointer _g_object_ref0 (gpointer obj)
{
    return obj ? g_object_ref (obj) : NULL;
}

#define GHWP_FILE_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), GHWP_TYPE_FILE, GHWPFilePrivate))

static void _ghwp_file_parse                 (GHWPDocument *doc,
                                              GError      **error);
static void _ghwp_file_parse_doc_info        (GHWPDocument *doc,
                                              GError      **error);
static void _ghwp_file_parse_body_text       (GHWPDocument *doc,
                                              GError      **error);
static void _ghwp_file_parse_prv_text        (GHWPDocument *doc);
static void _ghwp_file_parse_summary_info    (GHWPDocument *doc);
static void  ghwp_file_finalize              (GObject      *obj);

/* NOTE: LE 저장 방식이 아닌 점에 유의, 설계 실수 같음 */
#define MAKE_CTRL_ID(a, b, c, d)      \
    (guint32)((((guint8)(a)) << 24) | \
              (((guint8)(b)) << 16) | \
              (((guint8)(c)) <<  8) | \
              (((guint8)(d)) <<  0))

/* enum의 최대값은 ?? */
typedef enum
{
    CTRL_ID_TABLE = GUINT32_FROM_LE(MAKE_CTRL_ID('t', 'b', 'l', ' '))
} CtrlID;

static gchar *
_ghwp_file_get_text_from_context (GHWPContext *context)
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

static void _ghwp_file_parse (GHWPDocument *doc, GError **error)
{
    g_return_if_fail (doc != NULL);

    _ghwp_file_parse_doc_info (doc, error);
    if (*error) return;
    _ghwp_file_parse_body_text (doc, error);
    if (*error) return;
    _ghwp_file_parse_prv_text (doc);
    _ghwp_file_parse_summary_info (doc);
}

/* TODO fsm parser, nautilus에서 파일 속성만 보는 경우가 있으므로 속도 문제
 * 때문에 get_n_pages 로 옮겨갈 필요가 있다. */
static void _ghwp_file_parse_body_text (GHWPDocument *doc, GError **error)
{
    g_return_if_fail (doc != NULL);
    guint32 ctrl_id = 0;
    guint16 ctrl_lv = 0;
    guint16 curr_lv = 0;
    guint   index;

    gdouble    y    = 0.0;
    guint      len  = 0;
    GHWPPage  *page = ghwp_page_new ();

    for (index = 0; index < doc->file->section_streams->len; index++) {
        GInputStream *section_stream;
        GHWPContext  *context;
        section_stream = g_array_index (doc->file->section_streams,
                                        GInputStream *,
                                        index);
        section_stream = _g_object_ref0 (section_stream);

        context = ghwp_context_new (section_stream);

        while (ghwp_context_pull(context, error)) {
            curr_lv = (guint) context->level;
            /* 상태 변화 */
            if (curr_lv <= ctrl_lv)
                context->status = STATE_NORMAL;

            switch (context->tag_id) {
            case GHWP_TAG_PARA_HEADER:
                if (context->status != STATE_INSIDE_TABLE) {
                    GHWPParagraph *paragraph = ghwp_paragraph_new ();
                    g_array_append_val (doc->paragraphs, paragraph);
                } else if (context->status == STATE_INSIDE_TABLE) {
                    GHWPParagraph *paragraph;
                    GHWPTable     *table;
                    GHWPTableCell *cell;
                    paragraph = g_array_index (doc->paragraphs,
                                               GHWPParagraph *,
                                               doc->paragraphs->len - 1);
                    table = ghwp_paragraph_get_table (paragraph);
                    cell  = ghwp_table_get_last_cell (table);
                    GHWPParagraph *c_paragraph = ghwp_paragraph_new ();
                    ghwp_table_cell_add_paragraph (cell, c_paragraph);
                }
                break;
            case GHWP_TAG_PARA_TEXT:
            {
                GHWPParagraph *paragraph;
                GHWPText      *ghwp_text;
                paragraph    = g_array_index (doc->paragraphs, GHWPParagraph *,
                                              doc->paragraphs->len - 1);
                gchar *text  = _ghwp_file_get_text_from_context (context);
                ghwp_text    = ghwp_text_new (text);
                g_free (text);

                if (context->status != STATE_INSIDE_TABLE) {
                    ghwp_paragraph_set_ghwp_text (paragraph, ghwp_text);
                    /* 높이 계산 */
                    len = g_utf8_strlen (ghwp_text->text, -1);
                    y += 18.0 * ceil (len / 33.0);

                    if (y > 842.0 - 80.0) {
                        g_array_append_val (doc->pages, page);
                        page = ghwp_page_new ();
                        g_array_append_val (page->paragraphs, paragraph);
                        y = 0.0;
                    } else {
                        g_array_append_val (page->paragraphs, paragraph);
                    } /* if */
                    paragraph = NULL;
                } else if (context->status == STATE_INSIDE_TABLE) {
                    GHWPTable     *table;
                    GHWPTableCell *cell;
                    GHWPParagraph *c_paragraph;
                    table       = ghwp_paragraph_get_table (paragraph);
                    cell        = ghwp_table_get_last_cell (table);
                    c_paragraph = ghwp_table_cell_get_last_paragraph (cell);
                    ghwp_paragraph_set_ghwp_text (c_paragraph, ghwp_text);
                }
            }
                break;
            case GHWP_TAG_CTRL_HEADER:
                context_read_uint32 (context, &ctrl_id);
                ctrl_lv = context->level;
                switch (ctrl_id) {
                case CTRL_ID_TABLE:
                    context->status = STATE_INSIDE_TABLE;
                    break;
                default:
                    context->status = STATE_NORMAL;
                    break;
                }
                break;
            case GHWP_TAG_TABLE:
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
            {
                GHWPTable     *table;
                GHWPParagraph *paragraph;
                table = ghwp_table_new_from_context (context);
                paragraph = g_array_index (doc->paragraphs, GHWPParagraph *,
                                           doc->paragraphs->len - 1);
                ghwp_paragraph_set_table (paragraph, table);
            }
                break;
            case GHWP_TAG_LIST_HEADER:
                /* TODO ctrl_id 에 따른 객체를 생성한다 */
                switch (context->status) {
                /* table에 cell을 추가한다 */
                case STATE_INSIDE_TABLE:
                {
                    GHWPParagraph *paragraph;
                    GHWPTable     *table;
                    GHWPTableCell *cell;

                    paragraph = g_array_index (doc->paragraphs, GHWPParagraph *,
                                               doc->paragraphs->len - 1);

                    table = ghwp_paragraph_get_table (paragraph);
                    cell  = ghwp_table_cell_new_from_context(context);
                    ghwp_table_add_cell (table, cell);
                    /* TODO 높이 계산 cell_spacing 고려할 것 FIXME 소수점 */
                    y += cell->height / 7200.0 * 25.4 *
                            cell->col_span / table->n_cols;

                    if (y > 842.0 - 80.0) {
                        g_array_append_val (doc->pages, page);
                        page = ghwp_page_new ();
                        /* FIXME 중복 저장 */
                        g_array_append_val (page->paragraphs, paragraph);
                        y = 0.0;
                    } else {
                        /* FIXME 중복 저장 */
                        g_array_append_val (page->paragraphs, paragraph);
                    }
                }
                    break;
                default:
                    break;
                }
                break;
            default:
/*                printf ("%s:%d: %s not implemented\n", __FILE__, __LINE__,*/
/*                    _ghwp_get_tag_name(context->tag_id));*/
                break;
            } /* switch */
        } /* while */
        /* add last page */
        g_array_append_val (doc->pages, page);
        _g_object_unref0 (context);
        _g_object_unref0 (section_stream);
    } /* for */
}

static void _ghwp_file_parse_prv_text (GHWPDocument *doc)
{
    g_return_if_fail (doc != NULL);

    GsfInputStream *gis   = _g_object_ref0 (doc->file->prv_text_stream);
    gssize          size  = gsf_input_stream_size (gis);
    guchar         *buf   = g_new (guchar, size);
    GError         *error = NULL;

    g_input_stream_read ((GInputStream*) gis, buf, size, NULL, &error);

    if (error != NULL) {
        g_warning("%s:%d: %s\n", __FILE__, __LINE__, error->message);
        _g_free0 (doc->prv_text);
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

    } else if ( g_str_equal(name, GSF_META_NAME_REVISION_COUNT) ) {
        doc->revision_count = g_value_get_string (value);

    } else if ( g_str_equal(name, GSF_META_NAME_PAGE_COUNT) ) {
        /* not correct n_pages == 0 ?? */
        doc->n_pages = g_value_get_int (value);

    } else {
        g_warning("%s:%d:%s not implemented\n", __FILE__, __LINE__, name);
    }
}

static void _ghwp_file_parse_summary_info (GHWPDocument *doc)
{
    g_return_if_fail (doc != NULL);

    GsfInputStream *gis;
    gssize          size;
    guint8         *buf = NULL;
    GsfInputMemory *summary;
    GsfDocMetaData *meta;
    GError         *error = NULL;

    gis  = _g_object_ref0 (doc->file->summary_info_stream);
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
        _g_object_unref0 (doc->file->summary_info_stream);
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


static void _ghwp_file_parse_doc_info (GHWPDocument *doc, GError **error)
{
    g_return_if_fail (doc != NULL);

/*    guint32 id_mappings[16] = {0}; */ /* 반드시 초기화 해야 한다. */
/*    int i;*/

/*    GInputStream *stream  = doc->file->doc_info_stream;
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

GHWPFile* ghwp_file_new_from_uri (const gchar* uri, GError** error)
{
    g_return_val_if_fail (uri != NULL, NULL);

    gchar    *filename = g_filename_from_uri (uri, NULL, error);
    GHWPFile *file     = ghwp_file_new_from_filename (filename, error);
    _g_free0 (filename);

    return file;
}


GHWPFile* ghwp_file_new_from_filename (const gchar* filename, GError** error)
{
    g_return_val_if_fail (filename != NULL, NULL);

    /* check signature */
	static guint8 const signature_ole[] =
		{ 0xd0, 0xcf, 0x11, 0xe0, 0xa1, 0xb1, 0x1a, 0xe1 };

	static guint8 const signature_v3[] =
        { 0x48, 0x57, 0x50, 0x20, 0x44, 0x6f, 0x63, 0x75, 0x6d, 0x65,
          0x6e, 0x74, 0x20, 0x46, 0x69, 0x6c, 0x65, 0x20, 0x56, 0x33,
          0x2e, 0x30, 0x30, 0x20, 0x1a, 0x01, 0x02, 0x03, 0x04, 0x05 };

    GFile            *file   = g_file_new_for_path (filename);
    GFileInputStream *stream = g_file_read(file, NULL, error);

    if (!stream)
        return NULL;

    gsize bytes_read = 0;
    guint8 *buffer = g_malloc((gsize)30);
    g_input_stream_read_all (G_INPUT_STREAM(stream), buffer, (gsize) 30,
                             &bytes_read, NULL, error);

    if (bytes_read != 30) {
        g_free(buffer);
        return NULL;
    }

    if ( memcmp(buffer, signature_ole, sizeof(signature_ole)) == 0) {
        /* hwp v5 */
        g_free(buffer);
        g_object_unref(stream);
        return GHWP_FILE (ghwp_file_v5_new_from_filename (filename, error));
    } else if ( memcmp(buffer, signature_v3, sizeof(signature_v3)) == 0) {
        /* hwp v3 */
        g_free(buffer);
        g_object_unref(stream);
        return NULL;
/*        return (GHWP_FILE) ghwp_file_v3_new_from_filename (filename, error);*/
    } else {
        /* not a valid hwp file */
        *error = g_error_new (ghwp_error_quark(), 0, "not a valid hwp file");
        g_free(buffer);
        g_object_unref(stream);
        return NULL;
    }
}

GHWPDocument *ghwp_file_get_document (GHWPFile *file, GError **error)
{
    GHWPDocument *doc = ghwp_document_new();
    doc->file = file;
    _ghwp_file_parse (doc, error);

    return doc;
}

static void ghwp_file_class_init (GHWPFileClass * klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    g_type_class_add_private (klass, sizeof (GHWPFilePrivate));
    object_class->finalize = ghwp_file_finalize;
}


static void ghwp_file_init (GHWPFile * file)
{
    file->priv = GHWP_FILE_GET_PRIVATE (file);
}

static void ghwp_file_finalize (GObject* obj)
{
    GHWPFile *file = GHWP_FILE(obj);
    _g_object_unref0 (file->priv->olefile);
    _g_object_unref0 (file->prv_text_stream);
    _g_object_unref0 (file->prv_image_stream);
    _g_object_unref0 (file->file_header_stream);
    _g_object_unref0 (file->doc_info_stream);
    _g_array_free0 (file->section_streams);
    _g_object_unref0 (file->priv->section_stream);
    _g_object_unref0 (file->summary_info_stream);
    g_free (file->signature);
    G_OBJECT_CLASS (ghwp_file_parent_class)->finalize (obj);
}
