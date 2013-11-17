/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * hwp-file.c
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

#include <glib-object.h>
#include <string.h>

#include "hwp-file.h"
#include "hwp-file-ml.h"
#include "hwp-file-v3.h"
#include "hwp-file-v5.h"

G_DEFINE_ABSTRACT_TYPE (GHWPFile, hwp_file, G_TYPE_OBJECT);

/**
 * hwp_file_error_quark:
 *
 * The error domain for GHWPFile
 *
 * Returns: The error domain
 *
 * Since: 0.2
 */
GQuark hwp_file_error_quark (void)
{
    return g_quark_from_string ("hwp-file-error-quark");
}

/**
 * Since: 0.2
 */
void hwp_file_get_hwp_version (GHWPFile *file,
                                guint8   *major_version,
                                guint8   *minor_version,
                                guint8   *micro_version,
                                guint8   *extra_version)
{
    g_return_if_fail (GHWP_IS_FILE (file));

    return GHWP_FILE_GET_CLASS (file)->get_hwp_version (file,
                                                        major_version,
                                                        minor_version,
                                                        micro_version,
                                                        extra_version);
}

/**
 * Since: 0.2
 */
GHWPDocument *hwp_file_get_document (GHWPFile *file, GError **error)
{
    g_return_val_if_fail (GHWP_IS_FILE (file), NULL);

    return GHWP_FILE_GET_CLASS (file)->get_document (file, error);
}

/**
 * Since: 0.2
 */
gchar *hwp_file_get_hwp_version_string (GHWPFile *file)
{
    g_return_val_if_fail (GHWP_IS_FILE (file), NULL);

    return GHWP_FILE_GET_CLASS (file)->get_hwp_version_string (file);
}

/**
 * hwp_file_new_from_uri:
 * @uri: uri of the file to load
 * @error: (allow-none): Return location for an error, or %NULL
 * 
 * Creates a new #GHWPFile.  If %NULL is returned, then @error will be
 * set. Possible errors include those in the #GHWP_ERROR and #G_FILE_ERROR
 * domains.
 * 
 * Return value: A newly created #GHWPFile, or %NULL
 *
 * Since: 0.1
 */
GHWPFile *hwp_file_new_from_uri (const gchar* uri, GError** error)
{
    g_return_val_if_fail (uri != NULL, NULL);

    gchar    *filename = g_filename_from_uri (uri, NULL, error);
    GHWPFile *file     = hwp_file_new_from_filename (filename, error);
    g_free (filename);

    return file;
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

/**
 * Since: 0.1
 */
GHWPFile *hwp_file_new_from_filename (const gchar *filename, GError **error)
{
    g_return_val_if_fail (filename != NULL, NULL);

    /* check signature */
    static const guint8 const signature_ole[] = {
        0xd0, 0xcf, 0x11, 0xe0, 0xa1, 0xb1, 0x1a, 0xe1
    };

    static const guint8 signature_v3[] = {
        /* HWP Document File V3.00 \x1a\1\2\3\4\5 */
        0x48, 0x57, 0x50, 0x20, 0x44, 0x6f, 0x63, 0x75,
        0x6d, 0x65, 0x6e, 0x74, 0x20, 0x46, 0x69, 0x6c,
        0x65, 0x20, 0x56, 0x33, 0x2e, 0x30, 0x30, 0x20,
        0x1a, 0x01, 0x02, 0x03, 0x04, 0x05
    };

    GFile            *file   = g_file_new_for_path (filename);
    GFileInputStream *stream = g_file_read (file, NULL, error);
    g_object_unref (file);

    if (*error)
        return NULL;

    gsize bytes_read = 0;
    guint8 *buffer = g_malloc0 (4096);
    g_input_stream_read_all (G_INPUT_STREAM(stream), buffer, 4096,
                             &bytes_read, NULL, error);
    g_object_unref(stream);

    GHWPFile *retval = NULL;

    if (memcmp(buffer, signature_ole, sizeof(signature_ole)) == 0) {
        /* hwp v5 */
        retval = GHWP_FILE (hwp_file_v5_new_from_filename (filename, error));
    } else if (memcmp(buffer, signature_v3, sizeof(signature_v3)) == 0) {
        /* hwp v3 */
        retval = GHWP_FILE (hwp_file_v3_new_from_filename (filename, error));
    } else if (is_hwpml((gchar *) buffer, bytes_read)) {
        /* hwp ml */
        retval = GHWP_FILE (hwp_file_ml_new_from_filename (filename, error));
    } else {
        /* invalid hwp file */
        g_set_error (error, GHWP_FILE_ERROR, GHWP_FILE_ERROR_INVALID,
                            "invalid hwp file");
        retval = NULL;
    }

    g_free(buffer);
    return retval;
}

static void hwp_file_finalize (GObject *obj)
{
    G_OBJECT_CLASS (hwp_file_parent_class)->finalize (obj);
}

static void hwp_file_class_init (GHWPFileClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->finalize = hwp_file_finalize;
}

static void hwp_file_init (GHWPFile *file)
{
}
