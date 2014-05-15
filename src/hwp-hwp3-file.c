/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * hwp-hwp3-file.c
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

/*
 * This software have been developed with reference to
 * the HWP file format open specification by Hancom, Inc.
 * http://www.hancom.co.kr/userofficedata.userofficedataList.do?menuFlag=3
 * 한글과컴퓨터의 한/글 문서 파일(.hwp) 공개 문서를 참고하여 개발하였습니다.
 */

#include "hwp-hwp3-file.h"
#include "hwp-hwp3-parser.h"

G_DEFINE_TYPE (HwpHWP3File, hwp_hwp3_file, HWP_TYPE_FILE);

/**
 * hwp_hwp3_file_new_for_path:
 * @path: path of the file to load
 * @error: location to store the error occurring, or %NULL to ignore
 *
 * Creates a new #HwpHWP3File.  If %NULL is returned, then @error will be
 * set. Possible errors include those in the #HWP_ERROR and #G_FILE_ERROR
 * domains.
 *
 * Return value: A newly created #HwpHWP3File, or %NULL
 *
 * Since: 0.0.1
 */
HwpHWP3File *hwp_hwp3_file_new_for_path (const gchar *path,
                                         GError     **error)
{
  g_return_val_if_fail (path != NULL, NULL);
  GFile *gfile = g_file_new_for_path (path);
  GFileInputStream *stream = g_file_read (gfile, NULL, error);
  g_object_unref (gfile);
  HwpHWP3File *file = g_object_new (HWP_TYPE_HWP3_FILE, NULL);
  file->priv->stream = G_INPUT_STREAM (stream);
  return file;
}

/**
 * hwp_hwp3_file_get_hwp_version_string:
 * @file: a #HwpFile
 *
 * Returns:
 *
 * Since: 0.0.1
 **/
gchar *hwp_hwp3_file_get_hwp_version_string (HwpFile *file)
{
  g_return_val_if_fail (HWP_IS_HWP3_FILE (file), NULL);

  return g_strdup_printf ("3.0.0.%d", HWP_HWP3_FILE (file)->rev);
}

/**
 * hwp_hwp3_file_get_hwp_version:
 * @file: a #HwpFile
 * @major_version:
 * @minor_version:
 * @micro_version:
 * @extra_version:
 *
 * Returns:
 *
 * Since: 0.0.1
 **/
void hwp_hwp3_file_get_hwp_version (HwpFile *file,
                                    guint8  *major_version,
                                    guint8  *minor_version,
                                    guint8  *micro_version,
                                    guint8  *extra_version)
{
  g_return_if_fail (HWP_IS_HWP3_FILE (file));

  if (major_version) *major_version = 3;
  if (minor_version) *minor_version = 0;
  if (micro_version) *micro_version = 0;
  if (extra_version) *extra_version = HWP_HWP3_FILE (file)->rev;
}

/**
 * hwp_hwp3_file_get_document:
 * @file: a #HwpFile
 * @error: location to store the error occurring, or %NULL to ignore
 *
 * Return value: (transfer full): A #HwpDocument, or %NULL
 *
 * Since: 0.0.1
 */
HwpDocument *hwp_hwp3_file_get_document (HwpFile *file, GError **error)
{
  g_return_val_if_fail (HWP_IS_HWP3_FILE (file), NULL);

  HwpDocument *document = hwp_document_new ();

  HwpHWP3Parser *parser;
  parser = hwp_hwp3_parser_new (HWP_LISTENER (document), document);
  hwp_hwp3_parser_parse (parser, HWP_HWP3_FILE (file), error);
  g_object_unref (parser);

  if (*error) {
    g_object_unref (document);
    document = NULL;
  }

  return document;
}

static void hwp_hwp3_file_init (HwpHWP3File *file)
{
  file->priv = G_TYPE_INSTANCE_GET_PRIVATE (file, HWP_TYPE_HWP3_FILE,
                                                  HwpHWP3FilePrivate);
}

static void hwp_hwp3_file_finalize (GObject *object)
{
  HwpHWP3File *file = HWP_HWP3_FILE(object);
  g_object_unref (file->priv->stream);
  G_OBJECT_CLASS (hwp_hwp3_file_parent_class)->finalize (object);
}

static void hwp_hwp3_file_class_init (HwpHWP3FileClass *klass)
{
  GObjectClass* object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (HwpHWP3FilePrivate));
  HWP_FILE_CLASS (klass)->get_document = hwp_hwp3_file_get_document;
  HWP_FILE_CLASS (klass)->get_hwp_version_string = hwp_hwp3_file_get_hwp_version_string;
  HWP_FILE_CLASS (klass)->get_hwp_version = hwp_hwp3_file_get_hwp_version;
  object_class->finalize = hwp_hwp3_file_finalize;
}
