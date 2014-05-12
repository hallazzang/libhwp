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

#include <string.h>
#include <math.h>
#include "hwp-hwpml-file.h"
#include "hwp-hwpml-parser.h"
#include "hwp-enums.h"

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
 * hwp_hwpml_file_get_hwp_version_string:
 * @file: a #HwpFile
 *
 * Since: 0.0.1
 */
gchar *hwp_hwpml_file_get_hwp_version_string (HwpFile *file)
{
  return NULL;
}

/**
 * hwp_hwpml_file_get_hwp_version:
 * @file:
 * @major_version:
 * @minor_version:
 * @micro_version:
 * @extra_version:
 *
 * Since: 0.0.1
 */
void hwp_hwpml_file_get_hwp_version (HwpFile *file,
                                     guint8  *major_version,
                                     guint8  *minor_version,
                                     guint8  *micro_version,
                                     guint8  *extra_version)
{

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

  HwpDocument *document = hwp_document_new ();
  HwpHWPMLParser *parser;
  parser = hwp_hwpml_parser_new (HWP_LISTENER (document), document);
  hwp_hwpml_parser_parse (parser, HWP_HWPML_FILE (file), error);

  g_object_unref (parser);

  if (*error) {
    g_object_unref (document);
    document = NULL;
  }

  return document;
}

static void hwp_hwpml_file_init (HwpHWPMLFile *file)
{
  file->priv = G_TYPE_INSTANCE_GET_PRIVATE (file, HWP_TYPE_HWPML_FILE,
                                                  HwpHWPMLFilePrivate);
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
