/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * hwp-hwpml-parser.c
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

#include "hwp-hwpml-parser.h"
#include <libxml/xmlreader.h>
#include "hwp-enums.h"
#include <string.h>

G_DEFINE_TYPE (HwpHWPMLParser, hwp_hwpml_parser, G_TYPE_OBJECT)

static guint hwp_parse_state;
static guint tag_p_count;

static void _hwp_hwpml_file_parse_node (HwpHWPMLParser  *parser,
                                        HwpHWPMLFile    *file,
                                        xmlTextReaderPtr reader,
                                        GError         **error)
{
  g_return_if_fail (HWP_IS_HWPML_PARSER (parser));

  xmlChar *name, *value;
  int node_type = 0;
  HwpListenerInterface *iface = HWP_LISTENER_GET_IFACE (parser->listener);

  name = xmlTextReaderName (reader);
  value = xmlTextReaderValue (reader);
  node_type = xmlTextReaderNodeType (reader);

  gchar *tag_name = g_utf8_casefold ((const char*) name,
                                     strlen ((const char*) name));
  gchar *tag_p    = g_utf8_casefold ("P", strlen("P"));
  gchar *tag_text = g_utf8_casefold ("TEXT", strlen("TEXT"));
  gchar *tag_char = g_utf8_casefold ("CHAR", strlen("CHAR"));

  switch (node_type) {
  case XML_READER_TYPE_ELEMENT:
    /* paragraph */
    if (g_utf8_collate (tag_name, tag_p) == 0) {
      hwp_parse_state |= HWP_PARSE_STATE_P;
      tag_p_count++;
      if (tag_p_count > 1) {
        HwpParagraph *paragraph = hwp_paragraph_new ();
        GString *string = g_string_new ("");
        hwp_paragraph_set_string (paragraph, string);
        g_array_append_val (HWP_DOCUMENT (parser->listener)->paragraphs,
                            paragraph);
      }
    /* char */
    } else if (g_utf8_collate (tag_name, tag_char) == 0) {
      hwp_parse_state |= HWP_PARSE_STATE_CHAR;
    }
    break;
  case XML_READER_TYPE_TEXT:
    if ((hwp_parse_state & HWP_PARSE_STATE_CHAR) == HWP_PARSE_STATE_CHAR) {
      HwpParagraph *paragraph;
      paragraph = g_array_index (HWP_DOCUMENT (parser->listener)->paragraphs,
                                 HwpParagraph *,
                                 HWP_DOCUMENT (parser->listener)->paragraphs->len - 1);
      g_string_append (paragraph->string, (const gchar *) value);
    }
    break;
  case XML_READER_TYPE_END_ELEMENT:
    if ((g_utf8_collate (tag_name, tag_p) == 0) && (tag_p_count > 1)) {
      HwpParagraph *paragraph;
      paragraph = g_array_index (HWP_DOCUMENT (parser->listener)->paragraphs,
                                 HwpParagraph *,
                                 HWP_DOCUMENT (parser->listener)->paragraphs->len - 1);

      if (iface->paragraph)
        iface->paragraph (parser->listener,
                          paragraph,
                          parser->user_data,
                          error);
    } else if (g_utf8_collate (tag_name, tag_char) == 0) {
      hwp_parse_state &= ~HWP_PARSE_STATE_CHAR;
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

void hwp_hwpml_parser_parse (HwpHWPMLParser *parser,
                             HwpHWPMLFile   *file,
                             GError        **error)
{
  g_return_if_fail (HWP_IS_HWPML_PARSER (parser));

  gchar *uri = file->priv->uri;
  
  xmlTextReaderPtr reader;
  int              ret;

  reader = xmlNewTextReaderFilename (uri);

  if (reader != NULL) {
    while ((ret = xmlTextReaderRead (reader)) == 1) {
      _hwp_hwpml_file_parse_node (parser, file, reader, error);
    }

    xmlFreeTextReader (reader);
    if (ret != 0) {
      g_warning ("%s : failed to parse\n", uri);
    }
  } else {
    g_warning ("Unable to open %s\n", uri);
  }
}

static void hwp_hwpml_parser_class_init (HwpHWPMLParserClass *klass)
{
}

static void hwp_hwpml_parser_init (HwpHWPMLParser *parser)
{
}

/**
 * hwp_hwpml_parser_new:
 * @listener:
 * @user_data
 *
 * Returns:
 *
 * Since: 0.0.1
 */
HwpHWPMLParser *hwp_hwpml_parser_new (HwpListener *listener,
                                      gpointer     user_data)
{
  g_return_val_if_fail (HWP_IS_LISTENER (listener), NULL);

  HwpHWPMLParser *parser = g_object_new (HWP_TYPE_HWPML_PARSER, NULL);
  parser->listener       = listener;
  parser->user_data      = user_data;

  return parser;
}
