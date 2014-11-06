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

/**
 * hwp_hwpml_parser_parse:
 * @parser: a #HwpHWPMLParser
 * @file: a #HwpHWPMLFile
 * @error: a #GError
 *
 * Since: 0.0.1
 */
void hwp_hwpml_parser_parse (HwpHWPMLParser *parser,
                             HwpHWPMLFile   *file,
                             GError        **error)
{
  g_return_if_fail (HWP_IS_HWPML_PARSER (parser));

  gchar *uri = file->priv->uri;

  int ret;

  xmlTextReaderPtr reader = xmlNewTextReaderFilename (uri);

  if (reader == NULL)
  {
    g_warning ("Unable to open %s\n", uri);
    return;
  }

  HwpParseState parse_state = HWP_PARSE_STATE_NORMAL;
  guint tag_p_count = 0;
  HwpParagraph *paragraph = NULL;
  HwpListenableInterface *iface;
  iface = HWP_LISTENABLE_GET_IFACE (parser->listenable);

  gchar *tag_docsummary = g_utf8_casefold ("DOCSUMMARY", strlen("DOCSUMMARY"));
  gchar *tag_p          = g_utf8_casefold ("P",          strlen("P"));
  gchar *tag_text       = g_utf8_casefold ("TEXT",       strlen("TEXT"));
  gchar *tag_char       = g_utf8_casefold ("CHAR",       strlen("CHAR"));

  while ((ret = xmlTextReaderRead (reader)) == 1)
  {
    xmlChar *name  = xmlTextReaderName (reader);
    xmlChar *value = xmlTextReaderValue (reader);
    int node_type  = xmlTextReaderNodeType (reader);

    gchar *tag_name = g_utf8_casefold ((const char *) name,
                                       strlen ((const char *) name));

    switch (node_type)
    {
      case XML_READER_TYPE_ELEMENT:
        /* document summary */
        if (g_utf8_collate (tag_name, tag_docsummary) == 0)
        {
          parse_state = HWP_PARSE_STATE_DOCSUMMARY;
          parser->priv->info = hwp_summary_info_new ();
        /* paragraph */
        }
        else if (g_utf8_collate (tag_name, tag_p) == 0)
        {
          parse_state = HWP_PARSE_STATE_P;
          tag_p_count++;
          if (tag_p_count > 1)
            paragraph = hwp_paragraph_new ();
        /* char */
        }
        else if (g_utf8_collate (tag_name, tag_char) == 0)
        {
          parse_state = HWP_PARSE_STATE_CHAR;
        }
        break;
      case XML_READER_TYPE_TEXT:
        if (parse_state == HWP_PARSE_STATE_CHAR)
        {
          if (paragraph)
            paragraph->text = g_strdup ((const char *) value);
        }
        break;
      case XML_READER_TYPE_END_ELEMENT:
        if (g_utf8_collate (tag_name, tag_docsummary) == 0)
        {
          parse_state = HWP_PARSE_STATE_NORMAL;

          if (iface->summary_info)
            iface->summary_info (parser->listenable,
                                 g_object_ref (parser->priv->info),
                                 parser->user_data,
                                 error);
        }
        else if ((g_utf8_collate (tag_name, tag_p) == 0) && (tag_p_count > 1))
        {
          if (iface->paragraph)
          {
            iface->paragraph (parser->listenable,
                              paragraph,
                              parser->user_data,
                              error);
          }
          else
          {
            g_object_unref (paragraph);
            paragraph = NULL;
          }
        }
        else if (g_utf8_collate (tag_name, tag_char) == 0)
        {
          parse_state = HWP_PARSE_STATE_NORMAL;
        }
        break;
      case -1:
        /* TODO: error handling */
      default:
        break;
    } /* switch */

    g_free  (tag_name);
    xmlFree (name);
    xmlFree (value);
  }

  g_free (tag_docsummary);
  g_free (tag_p);
  g_free (tag_text);
  g_free (tag_char);

  xmlFreeTextReader (reader);

  if (ret != 0)
    g_warning ("%s : failed to parse\n", uri);
}

static void hwp_hwpml_parser_finalize (GObject *object)
{
  HwpHWPMLParser *parser = HWP_HWPML_PARSER (object);

  if (parser->priv->info)
    g_object_unref (parser->priv->info);

  G_OBJECT_CLASS (hwp_hwpml_parser_parent_class)->finalize (object);
}

static void hwp_hwpml_parser_class_init (HwpHWPMLParserClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  g_type_class_add_private (klass, sizeof (HwpHWPMLParserPrivate));
  object_class->finalize = hwp_hwpml_parser_finalize;
}

static void hwp_hwpml_parser_init (HwpHWPMLParser *parser)
{
  parser->priv = G_TYPE_INSTANCE_GET_PRIVATE (parser,
                                              HWP_TYPE_HWPML_PARSER,
                                              HwpHWPMLParserPrivate);
}

/**
 * hwp_hwpml_parser_new:
 * @listenable: a #HwpListenable
 * @user_data: a #gpointer
 *
 * Returns:
 *
 * Since: 0.0.1
 */
HwpHWPMLParser *hwp_hwpml_parser_new (HwpListenable *listenable,
                                      gpointer       user_data)
{
  g_return_val_if_fail (HWP_IS_LISTENABLE (listenable), NULL);

  HwpHWPMLParser *parser = g_object_new (HWP_TYPE_HWPML_PARSER, NULL);
  parser->listenable     = listenable;
  parser->user_data      = user_data;

  return parser;
}
