/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * hwp-hwp3-file.c
 * This file is part of the libhwp project.
 *
 * Copyright (C) 2013-2016 Hodong Kim <cogniti@gmail.com>
 *
 * The libhwp is dual licensed under the LGPL v3+ or Apache License 2.0
 *
 * The libhwp is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * The libhwp is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program;  If not, see <http://www.gnu.org/licenses/>.
 *
 * Or,
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * This software has been developed with reference to
 * the HWP file format open specification by Hancom, Inc.
 * http://www.hancom.co.kr/userofficedata.userofficedataList.do?menuFlag=3
 * 한글과컴퓨터의 한/글 문서 파일(.hwp) 공개 문서를 참고하여 개발하였습니다.
 */

#include "hwp-hwp3-file.h"
#include "hwp-hwp3-parser.h"

G_DEFINE_TYPE (HwpHWP3File, hwp_hwp3_file, HWP_TYPE_FILE);

static HwpHWP3File *hwp_hwp3_file_new_for_gfile (GFile   *gfile,
                                                 GError **error)
{
  GFileInputStream *stream = g_file_read (gfile, NULL, error);
  HwpHWP3File *file = g_object_new (HWP_TYPE_HWP3_FILE, NULL);
  file->priv->stream = G_INPUT_STREAM (stream);

  return file;
}

/**
 * hwp_hwp3_file_new_for_path:
 * @path: path of the file to load
 * @error: location to store the error occurring, or %NULL to ignore
 *
 * Creates a new #HwpHWP3File.  If %NULL is returned, then @error will be
 * set. Possible errors include those in the #HWP_ERROR and #HWP_FILE_ERROR
 * domains.
 *
 * Return value: A newly created #HwpHWP3File, or %NULL
 *
 * Since: 0.0.1
 */
HwpHWP3File *hwp_hwp3_file_new_for_path (const gchar *path,
                                         GError     **error)
{
  GFile *gfile = g_file_new_for_path (path);
  HwpHWP3File *file = hwp_hwp3_file_new_for_gfile (gfile, error);
  g_object_unref (gfile);

  return file;
}

/**
 * hwp_hwp3_file_new_for_uri:
 * @uri: a UTF-8 string containing a URI
 * @error: location to store the error occurring, or %NULL to ignore
 *
 * Creates a new #HwpHWP3File.  If %NULL is returned, then @error will be
 * set. Possible errors include those in the #HWP_ERROR and #HWP_FILE_ERROR
 * domains.
 *
 * Return value: A newly created #HwpHWP3File, or %NULL
 *
 * Since: 2016.05.12
 */
HwpHWP3File *hwp_hwp3_file_new_for_uri (const gchar  *uri,
                                         GError     **error)
{
  g_return_val_if_fail (uri != NULL, NULL);

  GFile *gfile = g_file_new_for_uri (uri);
  HwpHWP3File *file = hwp_hwp3_file_new_for_gfile (gfile, error);
  g_object_unref (gfile);

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
 * @major_version: (out) (allow-none): return location for the HWP major version number
 * @minor_version: (out) (allow-none): return location for the HWP minor version number
 * @micro_version: (out) (allow-none): return location for the HWP micro version number
 * @extra_version: (out) (allow-none): return location for the HWP extra version number
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
  HWP_FILE_CLASS (klass)->get_hwp_version_string = hwp_hwp3_file_get_hwp_version_string;
  HWP_FILE_CLASS (klass)->get_hwp_version = hwp_hwp3_file_get_hwp_version;
  object_class->finalize = hwp_hwp3_file_finalize;
}
