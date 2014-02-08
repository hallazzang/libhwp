/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * hwp-hwpml-file.c
 *
 * Copyright (C) 2013-2014 Hodong Kim <hodong@cogno.org>
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

#include <libxml/xmlreader.h>
#include <string.h>
#include "hwp-hwpml-file.h"
#include <math.h>

G_DEFINE_TYPE (HwpHWPMLFile, hwp_hwpml_file, HWP_TYPE_FILE);

/**
 * hwp_hwpml_file_new_for_path:
 * @path: path of the file to load
 * @error: location to store the error occurring, or %NULL to ignore
 * 
 * Creates a new #HwpHWPMLFile.  If %NULL is returned, then @error will be
 * set. Possible errors include those in the #HWP_ERROR and #G_FILE_ERROR
 * domains.
 * 
 * Return value: A newly created #HwpHWPMLFile, or %NULL
 *
 * Since: 0.0.1
 */
HwpHWPMLFile *hwp_hwpml_file_new_for_path (const gchar *path, GError **error)
{
  g_return_val_if_fail (path != NULL, NULL);

  HwpHWPMLFile *file  = g_object_new (HWP_TYPE_HWPML_FILE, NULL);
  GFile     *gfile = g_file_new_for_path (path);
  file->priv->uri  = g_file_get_uri (gfile);
  g_object_unref (gfile);

  return file;
}

/**
 * Since: TODO
 */
gchar *hwp_hwpml_file_get_hwp_version_string (HwpFile *file)
{
    return NULL;
}

/**
 * Since: TODO
 */
void hwp_hwpml_file_get_hwp_version (HwpFile *file,
                                    guint8   *major_version,
                                    guint8   *minor_version,
                                    guint8   *micro_version,
                                    guint8   *extra_version)
{

}

/**
 * Since: 0.2
 */
enum HWPParseStateFlags {
    HWP_PARSE_NORMAL = 0,
    HWP_PARSE_P      = 1 << 0,
    HWP_PARSE_TEXT   = 1 << 1,
    HWP_PARSE_CHAR   = 1 << 2
};

static int   hwp_parse_state = HWP_PARSE_NORMAL;
static guint tag_p_count = 0;

static void _hwp_hwpml_file_parse_node(HwpHWPMLFile *file, xmlTextReaderPtr reader)
{
    g_return_if_fail (HWP_IS_HWPML_FILE (file));

    xmlChar *name, *value;
    int node_type = 0;

    name = xmlTextReaderName(reader);
    value = xmlTextReaderValue(reader);
    node_type = xmlTextReaderNodeType(reader);

    gchar *tag_name = g_utf8_casefold ((const char*) name,
                                       strlen ((const char*) name));
    gchar *tag_p    = g_utf8_casefold ("P", strlen("P"));
    gchar *tag_text = g_utf8_casefold ("TEXT", strlen("TEXT"));
    gchar *tag_char = g_utf8_casefold ("CHAR", strlen("CHAR"));

    switch (node_type) {
        case XML_READER_TYPE_ELEMENT:
            /* paragraph */
            if (g_utf8_collate (tag_name, tag_p) == 0) {
                hwp_parse_state |= HWP_PARSE_P;
                tag_p_count++;
                if (tag_p_count > 1) {
                    HwpParagraph *paragraph = hwp_paragraph_new ();
                    GString *string = g_string_new ("");
                    hwp_paragraph_set_string (paragraph, string);
                    g_array_append_val (file->document->paragraphs, paragraph);
                }
            /* char */
            } else if (g_utf8_collate (tag_name, tag_char) == 0) {
                hwp_parse_state |= HWP_PARSE_CHAR;
            }
            break;
        case XML_READER_TYPE_TEXT:
            if ((hwp_parse_state & HWP_PARSE_CHAR) == HWP_PARSE_CHAR) {
                HwpParagraph *paragraph = g_array_index (file->document->paragraphs,
                                                          HwpParagraph *,
                                                          file->document->paragraphs->len - 1);
                g_string_append (paragraph->string, (const gchar *) value);
            }
            break;
        case XML_READER_TYPE_END_ELEMENT:
            if ((g_utf8_collate (tag_name, tag_p) == 0) && (tag_p_count > 1)) {
                HwpParagraph *paragraph = g_array_index (file->document->paragraphs,
                                                          HwpParagraph *,
                                                          file->document->paragraphs->len - 1);

                /* 높이 계산 */
                static gdouble y   = 0.0;
                static guint   len = 0;
                len = g_utf8_strlen (paragraph->string->str, -1);
                y += 18.0 * ceil (len / 33.0);

                if (y > 842.0 - 80.0) {
                    g_array_append_val (file->document->pages, file->page);
                    file->page = hwp_page_new ();
                    g_array_append_val (file->page->paragraphs, paragraph);
                    y = 0.0;
                } else {
                    g_array_append_val (file->page->paragraphs, paragraph);
                } /* if */
            } else if (g_utf8_collate (tag_name, tag_char) == 0) {
                hwp_parse_state &= ~HWP_PARSE_CHAR;
            }
            break;
        default:
            break;
    }

    g_free (tag_name);
    g_free (tag_p);
    g_free (tag_text);
    g_free (tag_char);

    xmlFree(name);
    xmlFree(value);
}

static void _hwp_hwpml_file_parse (HwpHWPMLFile *file, GError **error)
{
    g_return_if_fail (HWP_IS_HWPML_FILE (file));

    gchar *uri = file->priv->uri;
    
    xmlTextReaderPtr reader;
    int              ret;

    reader = xmlNewTextReaderFilename (uri);

    if (reader != NULL) {
        while ((ret = xmlTextReaderRead(reader)) == 1) {
            _hwp_hwpml_file_parse_node (file, reader);
        }
        /* 마지막 페이지 더하기 */
        g_array_append_val (file->document->pages, file->page);
        xmlFreeTextReader(reader);
        if (ret != 0) {
            g_warning ("%s : failed to parse\n", uri);
        }
    } else {
        g_warning ("Unable to open %s\n", uri);
    }
}

/**
 * hwp_hwpml_file_get_document:
 * @file: a #HwpFile
 * @error: location to store the error occurring, or %NULL to ignore
 *
 * Return value: (transfer none): A #HwpDocument, or %NULL
 *
 * Since: 0.0.1
 */
HwpDocument *hwp_hwpml_file_get_document (HwpFile *file, GError **error)
{
    g_return_val_if_fail (HWP_IS_HWPML_FILE (file), NULL);
    HWP_HWPML_FILE (file)->document = hwp_document_new();
    _hwp_hwpml_file_parse (HWP_HWPML_FILE (file), error);
    return HWP_HWPML_FILE (file)->document;
}

static void hwp_hwpml_file_init (HwpHWPMLFile *file)
{
    file->priv = G_TYPE_INSTANCE_GET_PRIVATE (file, HWP_TYPE_HWPML_FILE,
                                                    HwpHWPMLFilePrivate);
    file->page = hwp_page_new ();
}

static void hwp_hwpml_file_finalize (GObject *object)
{
    HwpHWPMLFile *file = HWP_HWPML_FILE(object);
    g_free (file->priv->uri);
    G_OBJECT_CLASS (hwp_hwpml_file_parent_class)->finalize (object);
}

static void hwp_hwpml_file_class_init (HwpHWPMLFileClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    g_type_class_add_private (klass, sizeof (HwpHWPMLFilePrivate));
    HwpFileClass *hwp_file_class = HWP_FILE_CLASS (klass);
    hwp_file_class->get_document = hwp_hwpml_file_get_document;
    hwp_file_class->get_hwp_version_string = hwp_hwpml_file_get_hwp_version_string;
    hwp_file_class->get_hwp_version = hwp_hwpml_file_get_hwp_version;

    object_class->finalize = hwp_hwpml_file_finalize;
}
