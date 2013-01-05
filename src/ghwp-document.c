/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * ghwp-document.c
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
 * This software have been developed with reference to
 * the HWP file format open specification by Hancom, Inc.
 * http://www.hancom.co.kr/userofficedata.userofficedataList.do?menuFlag=3
 * 한글과컴퓨터의 한/글 문서 파일(.hwp) 공개 문서를 참고하여 개발하였습니다.
 */

#include <glib.h>
#include <glib-object.h>
#include <stdlib.h>
#include <string.h>
#include <gio/gio.h>
#include <stdio.h>
#include <float.h>
#include <math.h>
#include <gsf/gsf-input-memory.h>
#include <gsf/gsf-msole-utils.h>
#include <gsf/gsf-input-impl.h>
#include <cairo.h>

#include "config.h"
#include "ghwp-document.h"
#include "gsf-input-stream.h"
#include "ghwp-parse.h"

#include <gsf/gsf-meta-names.h>
#include <gsf/gsf-timestamp.h>
#include <gsf/gsf-docprop-vector.h>

G_DEFINE_TYPE (GHWPDocument, ghwp_document, G_TYPE_OBJECT);

/* private function */
static void   _ghwp_document_parse                  (GHWPDocument  *self,
                                                     GError       **error);
static void   _ghwp_document_parse_doc_info         (GHWPDocument  *self,
                                                     GError       **error);
static void   _ghwp_document_parse_body_text        (GHWPDocument* self,
                                                     GError       **error);
static void   _ghwp_document_parse_prv_text         (GHWPDocument* self);
static void   _ghwp_document_parse_summary_info     (GHWPDocument* self);
static void   _ghwp_document_make_pages             (GHWPDocument* self);
static gchar *_ghwp_document_get_text_from_raw_data (GHWPDocument *self,
                                                     guchar       *raw,
                                                     int           raw_len);
static void ghwp_document_finalize   (GObject* obj);

#define _g_array_free0(var) ((var == NULL) ? NULL : (var = (g_array_free (var, TRUE), NULL)))
#define _g_free0(var) (var = (g_free (var), NULL))
#define _g_object_unref0(var) ((var == NULL) ? NULL : (var = (g_object_unref (var), NULL)))
#define _g_error_free0(var) ((var == NULL) ? NULL : (var = (g_error_free (var), NULL)))

static gpointer _g_object_ref0 (gpointer self)
{
    return self ? g_object_ref (self) : NULL;
}

GHWPDocument *ghwp_document_new_from_uri (const gchar* uri, GError** error)
{
    g_return_val_if_fail (uri != NULL, NULL);

    GHWPFile *file = ghwp_file_new_from_uri (uri, error);

    if (file == NULL) {
        return NULL;
    }

    GHWPDocument *doc = ghwp_document_new();
    doc->file = file;
    _ghwp_document_parse (doc, error);

    return doc;
}

GHWPDocument *
ghwp_document_new_from_filename (const gchar* filename, GError** error)
{
    g_return_val_if_fail (filename != NULL, NULL);

    GHWPFile *file = ghwp_file_new_from_filename (filename, error);

    if (file == NULL) {
        return NULL;
    }

    GHWPDocument *doc = ghwp_document_new();
    doc->file = file;
    _ghwp_document_parse (doc, error);

    if (*error) return NULL;

    return doc;
}


static void _ghwp_document_parse (GHWPDocument *self, GError **error)
{
    g_return_if_fail (self != NULL);

    _ghwp_document_parse_doc_info (self, error);
    if (*error) return;
    _ghwp_document_parse_body_text (self, error);
    if (*error) return;
    _ghwp_document_parse_prv_text (self);
    _ghwp_document_parse_summary_info (self);
}


guint ghwp_document_get_n_pages (GHWPDocument* self)
{
    g_return_val_if_fail (self != NULL, 0U);
    return self->pages->len;
}

/**
 * ghwp_document_get_page:
 * @doc: a #GHWPDocument
 * @n_page: the index of the page to get
 *
 * Returns a #GHWPPage representing the page at index
 *
 * Returns: (transfer none): a #GHWPPage
 *     DO NOT FREE the page.
 */
GHWPPage* ghwp_document_get_page (GHWPDocument* doc, gint n_page)
{
    g_return_val_if_fail (doc != NULL, NULL);
    GHWPPage* page = g_array_index (doc->pages, GHWPPage*, (guint) n_page);
    return _g_object_ref0 (page);
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


static void _ghwp_document_parse_doc_info (GHWPDocument* self, GError **error)
{
    g_return_if_fail (self != NULL);

/*    guint32 id_mappings[16] = {0}; */ /* 반드시 초기화 해야 한다. */
/*    int i;*/

    GInputStream* stream  = self->file->doc_info_stream;
    GHWPContext*  context = ghwp_context_new (stream);
    while (ghwp_context_pull (context, error)) {
        switch (context->tag_id) {
        case GHWP_TAG_DOCUMENT_PROPERTIES:
            /* TODO */
            break;
        case GHWP_TAG_ID_MAPPINGS:
/*            for (i = 0; i < sizeof(id_mappings); i = i + sizeof(guint32)) {*/
/*                memcpy(&id_mappings[i], &(context->data[i]), sizeof(guint32));*/
/*                id_mappings[i] = GUINT16_FROM_LE(id_mappings[i]);*/
/*                printf("%d\n", id_mappings[i]);*/
/*            }*/
            break;
        default:
            printf("%s:%d: %s not implemented\n", __FILE__, __LINE__,
                _ghwp_get_tag_name (context->tag_id));
            break;
        }
    }

    g_object_unref (context);
}


static gchar*
_ghwp_document_get_text_from_raw_data (GHWPDocument *self,
                                       guchar       *raw,
                                       int           raw_len)
{
    g_return_val_if_fail (self != NULL, NULL);
    gunichar2 ch; /* guint16 */
    GString  *text = g_string_new("");
    guint     i;

    for (i = 0; i < raw_len; i = i + 2)
    {
        memcpy (&ch, raw + i, 2);
        ch = GUINT_FROM_LE(ch);
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
            break;
        case 9: /* inline */ /* tab */
            i = i + 14;
            g_string_append_unichar(text, ch);
            break;
        case 10:
            break;
        case 11:
        case 12:
            i = i + 14;
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

    return g_string_free(text, FALSE);
}

typedef enum
{
    GHWP_PARSE_ERROR_UNKNOWN_TAG
} GHWPParseError;

typedef enum
{
    STATE_NOT_IMPLEMENTED,
    STATE_INSIDE_TABLE
} GHWPParseState;

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

/* TODO fsm parser */
static void _ghwp_document_parse_body_text (GHWPDocument* self, GError **error)
{
    g_return_if_fail (self != NULL);
    guint16    curr_lv  = 0;
    guint16    prev_lv  = 0;
    guint      index;
    guint32    ctrl_id = 0;
/*    int        i = 0;*/
    GHWPTable *table;

    for (index = 0; index < self->file->section_streams->len; index++) {
        GInputStream *section_stream;
        GHWPContext  *context;
        section_stream = g_array_index (self->file->section_streams,
                                        GInputStream*,
                                        index);
        section_stream = _g_object_ref0 (section_stream);

        context = ghwp_context_new (section_stream);

        while (ghwp_context_pull(context, error)) {
            curr_lv = (guint) context->level;
            /* TODO ctrl_id 설정에 따른 context->status 갱신이 필요함 */

            switch (context->tag_id) {
            case GHWP_TAG_PARA_HEADER:
                if (context->status == STATE_INSIDE_TABLE) {
                    GHWPParagraph *_paragraph_ = ghwp_paragraph_new();
                    g_array_append_val (self->office_text, _paragraph_);
                    /* TODO 테이블 객체에 포함 문단을 포함시킬 것 */
                } else {
                    GHWPParagraph *_paragraph_ = ghwp_paragraph_new();
                    g_array_append_val (self->office_text, _paragraph_);
                }
                break;
            case GHWP_TAG_PARA_TEXT:
                if (curr_lv > prev_lv) {
                    GHWPParagraph *paragraph;
                    paragraph = g_array_index (self->office_text,
                                           GHWPParagraph*,
                                           self->office_text->len - 1);
                    paragraph = _g_object_ref0 (paragraph);

                    gchar *text;
                    text = _ghwp_document_get_text_from_raw_data (self,
                                                         context->data,
                                                         context->data_len);
                    TextSpan *_textspan_ = text_span_new (text);
                    ghwp_paragraph_add_textspan (paragraph, _textspan_);
                    _g_object_unref0 (_textspan_);
                    _g_free0 (text);
                    _g_object_unref0 (paragraph);
                }
                break;
            case GHWP_TAG_PARA_CHAR_SHAPE:
                break;
            case GHWP_TAG_PARA_LINE_SEG:
                break;
            case GHWP_TAG_CTRL_HEADER:
                memcpy(&ctrl_id, context->data, 4);
                ctrl_id = GUINT32_FROM_LE(ctrl_id);
                switch (ctrl_id) {
                case CTRL_ID_TABLE:
                    context->status = STATE_INSIDE_TABLE;
                    break;
                default:
                    context->status = STATE_NOT_IMPLEMENTED;
                    break;
                }
                break;
            case GHWP_TAG_PAGE_DEF:
                break;
            case GHWP_TAG_FOOTNOTE_SHAPE:
                break;
            case GHWP_TAG_PAGE_BORDER_FILL:
                break;
            case GHWP_TAG_LIST_HEADER:
                /* TODO ctrl_id 에 따른 객체를 생성한다 */
                switch (context->status) {
                case STATE_INSIDE_TABLE:
/*                    cell = cell_new();*/
/*                    table_add_cell (tabel, cell);*/
                    break;
                default:
                    break;
                }
                break;
            case GHWP_TAG_EQEDIT:
                break;
            case GHWP_TAG_TABLE:
            /*
                     col 0   col 1
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
                    table: row-span, col-span
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
                /* TODO 테이블 객체 생성한다. with row-span, col-span */
                table = ghwp_table_new();
/*                for (i = 0; i < context->data_len; i++) {*/
/*                    printf("%02x ", context->data[i]);*/
/*                }*/
/*                printf("\n");*/
                g_object_unref(table);
                break;
            default:
                printf ("%s:%d: %s not implemented\n", __FILE__, __LINE__,
                    _ghwp_get_tag_name(context->tag_id));
                break;
            } /* switch */

            prev_lv = curr_lv;
        } /* while */
        _g_object_unref0 (context);
        _g_object_unref0 (section_stream);
    } /* for */

    _ghwp_document_make_pages (self);
}


static void _ghwp_document_make_pages (GHWPDocument* self)
{
    g_return_if_fail (self != NULL);
    gdouble   y    = 0.0;
    guint     len  = 0;
    GHWPPage *page = ghwp_page_new ();
    guint     i, j;

    for (i = 0; i < self->office_text->len; i++) {
        GHWPParagraph* paragraph;
        paragraph = g_array_index (self->office_text, GHWPParagraph*, i);
        paragraph = _g_object_ref0 (paragraph);

        for(j = 0; j < paragraph->textspans->len; j++) {

            TextSpan* textspan;
            textspan = g_array_index (paragraph->textspans, TextSpan*, j);
            textspan = _g_object_ref0 (textspan);

            len = g_utf8_strlen (textspan->text, -1);

            y = y + (18.0 * ceil (len / 33.0));


            if (y > 842.0 - 80.0) {
                page = _g_object_ref0 (page);
                g_array_append_val (self->pages, page);
                _g_object_unref0 (page);
                page = ghwp_page_new ();
                textspan = _g_object_ref0 (textspan);
                g_array_append_val (page->elements, textspan);
                y = 0.0;
            } else {
                textspan = _g_object_ref0 (textspan);
                g_array_append_val (page->elements, textspan);
            } /* if */
            _g_object_unref0 (textspan);
        } /* for */
        _g_object_unref0 (paragraph);
    } /* for */
    /* add last page */
    page = _g_object_ref0 (page);
    g_array_append_val (self->pages, page);
    _g_object_unref0 (page);
}

static void _ghwp_document_parse_prv_text (GHWPDocument* doc)
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
        printf("%s:%d:%s not implemented\n", __FILE__, __LINE__, name);
    }
}

static void _ghwp_document_parse_summary_info (GHWPDocument* doc)
{
    g_return_if_fail (doc != NULL);

    GsfInputStream *gis;
    gssize          size;
    guint8         *buf;
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

    memcpy (buf + 28, component_guid, (gsize) sizeof(component_guid));
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


GHWPDocument* ghwp_document_new (void)
{
    return (GHWPDocument*) g_object_new (GHWP_TYPE_DOCUMENT, NULL);
}


static void ghwp_document_class_init (GHWPDocumentClass * klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->finalize     = ghwp_document_finalize;
}


static void ghwp_document_init (GHWPDocument * doc) {
    doc->office_text = g_array_new (TRUE, TRUE, sizeof (GHWPParagraph*));
    doc->pages       = g_array_new (TRUE, TRUE, sizeof (GHWPPage*));
}


static void ghwp_document_finalize (GObject* obj) {
    GHWPDocument * doc;
    doc = G_TYPE_CHECK_INSTANCE_CAST (obj, GHWP_TYPE_DOCUMENT, GHWPDocument);
    _g_object_unref0 (doc->file);
    _g_free0 (doc->prv_text);
    _g_array_free0 (doc->office_text);
    _g_array_free0 (doc->pages);
    _g_object_unref0 (doc->summary_info);
    G_OBJECT_CLASS (ghwp_document_parent_class)->finalize (obj);
}

gchar *
ghwp_document_get_title (GHWPDocument *doc)
{
    return g_strdup (doc->title);
}

gchar *
ghwp_document_get_keywords (GHWPDocument *doc)
{
    return g_strdup (doc->keywords);
}

gchar *
ghwp_document_get_subject (GHWPDocument *doc)
{
    return g_strdup (doc->subject);
}

gchar *
ghwp_document_get_creator (GHWPDocument *doc)
{
    return g_strdup (doc->creator);
}

GTime
ghwp_document_get_creation_date (GHWPDocument *doc)
{
    return doc->creation_date;
}

GTime
ghwp_document_get_modification_date (GHWPDocument *doc)
{
    return doc->mod_date;
}

/**
 * ghwp_document_get_hwp_format:
 * @document: A #GHWPDocument
 *
 * Returns the HWP format of @document as a string (e.g. HWP v5.0.0.6)
 *
 * Return value: a new allocated string containing the HWP format
 *               of @document, or %NULL
 *
 * Since: 0.1.2
 **/
gchar *
ghwp_document_get_format (GHWPDocument *document)
{
    gchar *format;

    g_return_val_if_fail (GHWP_IS_DOCUMENT (document), NULL);

    format = g_strdup_printf ("HWP v%s",
        ghwp_document_get_hwp_version_string (document));
  return format;
}

/**
 * ghwp_document_get_hwp_version_string:
 * @document: A #GHWPDocument
 *
 * Returns the HWP version of @document as a string (e.g. 5.0.0.6)
 *
 * Return value: a new allocated string containing the HWP version
 *               of @document, or %NULL
 *
 * Since: 0.1.2
 **/
gchar *
ghwp_document_get_hwp_version_string (GHWPDocument *document)
{
    gchar *version_string;

    g_return_val_if_fail (GHWP_IS_DOCUMENT (document), NULL);

    version_string = g_strdup_printf ("%d.%d.%d.%d",
        document->file->major_version,
        document->file->minor_version,
        document->file->micro_version,
        document->file->extra_version);
  return version_string;
}

/**
 * ghwp_document_get_hwp_version:
 * @document: A #GHWPDocument
 * @major_version: (out) (allow-none): return location for the HWP major version number
 * @minor_version: (out) (allow-none): return location for the HWP minor version number
 * @micro_version: (out) (allow-none): return location for the HWP micro version number
 * @extra_version: (out) (allow-none): return location for the HWP extra version number
 *
 * Returns: the major and minor and micro and extra HWP version numbers
 *
 * Since: 0.1.2
 **/
void
ghwp_document_get_hwp_version (GHWPDocument *document,
                               guint8       *major_version,
                               guint8       *minor_version,
                               guint8       *micro_version,
                               guint8       *extra_version)
{
    g_return_if_fail (GHWP_IS_DOCUMENT (document));

    if (major_version)
       *major_version = document->file->major_version;
    if (minor_version)
       *minor_version = document->file->minor_version;
    if (micro_version)
       *micro_version = document->file->micro_version;
    if (extra_version)
       *extra_version = document->file->extra_version;
}
