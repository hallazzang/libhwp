/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * hwp-file.c
 * This file is part of the libhwp project.
 *
 * Copyright (C) 2012-2016 Hodong Kim <cogniti@gmail.com>
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

#include <stdio.h>
#include <string.h>
#include "hwp-file.h"
#include "hwp-hwp3-file.h"
#include "hwp-hwp5-file.h"
#include "hwp-hwpml-file.h"

#include "config.h"
#include <glib/gi18n-lib.h>

G_DEFINE_ABSTRACT_TYPE (HwpFile, hwp_file, G_TYPE_OBJECT);

/**
 * hwp_file_error_quark:
 *
 * The error domain for HwpFile
 *
 * Returns: The error domain
 *
 * Since: 0.0.1
 */
GQuark hwp_file_error_quark (void)
{
  return g_quark_from_string ("hwp-file-error-quark");
}

/**
 * hwp_file_get_hwp_version:
 * @file:
 * @major_version:
 * @minor_version:
 * @micro_version:
 * @extra_version:
 *
 * Since: 0.0.1
 */
void hwp_file_get_hwp_version (HwpFile *file,
                               guint8  *major_version,
                               guint8  *minor_version,
                               guint8  *micro_version,
                               guint8  *extra_version)
{
  g_return_if_fail (HWP_IS_FILE (file));

  return HWP_FILE_GET_CLASS (file)->get_hwp_version (file,
                                                     major_version,
                                                     minor_version,
                                                     micro_version,
                                                     extra_version);
}

/**
 * hwp_file_get_hwp_version_string:
 * @file:
 *
 * Since: 0.0.1
 */
gchar *hwp_file_get_hwp_version_string (HwpFile *file)
{
  g_return_val_if_fail (HWP_IS_FILE (file), NULL);

  return HWP_FILE_GET_CLASS (file)->get_hwp_version_string (file);
}

static gboolean is_hwpml (gchar *haystack, gsize haystack_len)
{
  gchar *ptr1;
  gchar *ptr2;
  gchar *lowercase       = g_utf8_strdown (haystack, haystack_len);
  gchar *signature_xml   = g_utf8_strdown ("<?xml version=\"", 15);
  gchar *signature_hwpml = g_utf8_strdown ("<HWPML Version=\"", 16);

  ptr1 = g_strstr_len (lowercase, haystack_len, signature_xml);
  ptr2 = g_strstr_len (lowercase, haystack_len, signature_hwpml);

  g_free (lowercase);
  g_free (signature_xml);
  g_free (signature_hwpml);

  if (ptr1 && ptr2 && (ptr1 < ptr2))
    return TRUE;
  else
    return FALSE;
}

static const guint8 signature_ole[] =
{
  0xd0, 0xcf, 0x11, 0xe0, 0xa1, 0xb1, 0x1a, 0xe1
};

static const guint8 signature_v3[] =
{
  /* HWP Document File V3.00 \x1a\1\2\3\4\5 */
  0x48, 0x57, 0x50, 0x20, 0x44, 0x6f, 0x63, 0x75,
  0x6d, 0x65, 0x6e, 0x74, 0x20, 0x46, 0x69, 0x6c,
  0x65, 0x20, 0x56, 0x33, 0x2e, 0x30, 0x30, 0x20,
  0x1a, 0x01, 0x02, 0x03, 0x04, 0x05
};

/**
 * hwp_file_new_for_path:
 * @path: path of the file to load
 * @error: location to store the error occurring, or %NULL to ignore
 *
 * Creates a new #HwpFile.  If %NULL is returned, then @error will be
 * set. Possible errors include those in the #HWP_ERROR and #HWP_FILE_ERROR
 * domains.
 *
 * Return value: A newly created #HwpFile, or %NULL
 *
 * Since: 0.0.1
 */
HwpFile *hwp_file_new_for_path (const gchar *path, GError **error)
{
  g_return_val_if_fail (path != NULL, NULL);

  GFile            *file   = g_file_new_for_path (path);
  GFileInputStream *stream = g_file_read (file, NULL, error);
  g_object_unref (file);

  if (*error)
    return NULL;

  gsize bytes_read = 0;
  guint8 *buffer = g_malloc0 (4096);
  g_input_stream_read_all (G_INPUT_STREAM(stream), buffer, 4096,
                           &bytes_read, NULL, error);
  g_object_unref (stream);

  HwpFile *retval = NULL;

  if (memcmp(buffer, signature_ole, sizeof(signature_ole)) == 0) {
    /* hwp v5 */
    retval = HWP_FILE (hwp_hwp5_file_new_for_path (path, error));
  } else if (memcmp(buffer, signature_v3, sizeof(signature_v3)) == 0) {
    /* hwp v3 */
    retval = HWP_FILE (hwp_hwp3_file_new_for_path (path, error));
  } else if (is_hwpml((gchar *) buffer, bytes_read)) {
    /* hwp ml */
    retval = HWP_FILE (hwp_hwpml_file_new_for_path (path, error));
  } else {
    /* invalid hwp file */
    g_set_error (error, HWP_FILE_ERROR, HWP_FILE_ERROR_INVALID,
                        "invalid hwp file");
  }

  g_free(buffer);
  return retval;
}

/**
 * hwp_file_new_for_uri:
 * @uri: a UTF-8 string containing a URI
 * @error: location to store the error occurring, or %NULL to ignore
 *
 * Creates a new #HwpFile.  If %NULL is returned, then @error will be
 * set. Possible errors include those in the #HWP_ERROR and #HWP_FILE_ERROR
 * domains.
 *
 * Return value: A newly created #HwpFile, or %NULL
 *
 * Since: 2016.05.12
 */
HwpFile *hwp_file_new_for_uri (const gchar *uri, GError **error)
{
  g_return_val_if_fail (uri != NULL, NULL);

  GFile            *file   = g_file_new_for_uri (uri);
  GFileInputStream *stream = g_file_read (file, NULL, error);
  g_object_unref (file);

  if (*error)
    return NULL;

  gsize bytes_read = 0;
  guint8 *buffer = g_malloc0 (4096);
  g_input_stream_read_all (G_INPUT_STREAM(stream), buffer, 4096,
                           &bytes_read, NULL, error);
  g_object_unref (stream);

  HwpFile *retval = NULL;

  if (memcmp(buffer, signature_ole, sizeof(signature_ole)) == 0)
    retval = HWP_FILE (hwp_hwp5_file_new_for_uri (uri, error));
  else if (memcmp(buffer, signature_v3, sizeof(signature_v3)) == 0)
    retval = HWP_FILE (hwp_hwp3_file_new_for_uri (uri, error));
  else if (is_hwpml((gchar *) buffer, bytes_read))
    retval = HWP_FILE (hwp_hwpml_file_new_for_uri (uri, error));
  else
    g_set_error (error, HWP_FILE_ERROR, HWP_FILE_ERROR_INVALID,
                        "invalid hwp file");

  g_free(buffer);

  return retval;
}

static void hwp_file_finalize (GObject *object)
{
  G_OBJECT_CLASS (hwp_file_parent_class)->finalize (object);
}

static void hwp_file_class_init (HwpFileClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  object_class->finalize = hwp_file_finalize;
}

static void hwp_file_init (HwpFile *file)
{
}
