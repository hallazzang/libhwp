/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * ghwp-file-v5.c
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
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <gsf/gsf-input-impl.h>
#include <gsf/gsf-input-memory.h>
#include <gsf/gsf-msole-utils.h>
#include <gsf/gsf-input-stdio.h>
#include <gsf/gsf-infile-impl.h>
#include <gsf/gsf-doc-meta-data.h>
#include <gsf/gsf-meta-names.h>
#include <gsf/gsf-timestamp.h>
#include <pango/pango.h>
#include <pango/pangocairo.h>

#include "gsf-input-stream.h"
#include "ghwp-file-v5.h"
#include "ghwp-parser.h"
#include "ghwp-models.h"

G_DEFINE_TYPE (GHWPFileV5, ghwp_file_v5, GHWP_TYPE_FILE);

/**
 * Since: 0.2
 */
GHWPDocument *ghwp_file_v5_get_document (GHWPFile *file, GError **error)
{
  g_return_val_if_fail (GHWP_IS_FILE_V5 (file), NULL);

  GHWPDocument *document = ghwp_document_new ();

  GHWPParseContext *context;
  context = ghwp_parse_context_new (GHWP_PARSER (document), document);
  ghwp_parse_context_parse (context, GHWP_FILE_V5 (file), error);
  g_object_unref (context);

  if (*error) {
    g_object_unref (document);
    document = NULL;
  }

  return document;
}

/**
 * ghwp_file_v5_check_version:
 * @file: a #GHWPFileV5
 * @major: the major version to check for
 * @minor: the minor version to check for
 * @micro: the micro version to check for
 * @extra: the extra version to check for
 *
 * Checks the version of the HWP document
 *
 * <example>
 * <title>Checking the version of the HWP document</title>
 * <programlisting>
 *   if (ghwp_file_v5_check_version (doc, 5, 0, 0, 7))
 *     g_print ("HWP document version is 5.0.0.7 or above");
 * </programlisting>
 * </example>
 *
 * Returns: %TRUE if the version of the HWP document
 * is the same as or newer than the passed-in version.
 *
 * Since: TODO
 */
gboolean ghwp_file_v5_check_version (GHWPFileV5 *file,
                                     guint8      major,
                                     guint8      minor,
                                     guint8      micro,
                                     guint8      extra)
{
    g_return_val_if_fail (GHWP_IS_FILE_V5 (file), FALSE);

    return (file->major_version >  major)   ||
           (file->major_version == major &&
            file->minor_version >  minor)   ||
           (file->major_version == major &&
            file->minor_version == minor &&
            file->micro_version >  micro)   ||
           (file->major_version == major &&
            file->minor_version == minor &&
            file->micro_version == micro &&
            file->extra_version >= extra);
}

/**
 * ghwp_file_v5_get_hwp_version:
 * @file: A #GHWPFile
 * @major_version: (out) (allow-none): return location for the HWP major version number
 * @minor_version: (out) (allow-none): return location for the HWP minor version number
 * @micro_version: (out) (allow-none): return location for the HWP micro version number
 * @extra_version: (out) (allow-none): return location for the HWP extra version number
 *
 * Returns: the major and minor and micro and extra HWP version numbers
 *
 * Since: 0.2
 */
void ghwp_file_v5_get_hwp_version (GHWPFile *file,
                                   guint8   *major_version,
                                   guint8   *minor_version,
                                   guint8   *micro_version,
                                   guint8   *extra_version)
{
    g_return_if_fail (GHWP_IS_FILE_V5 (file));

    if (major_version) *major_version = GHWP_FILE_V5(file)->major_version;
    if (minor_version) *minor_version = GHWP_FILE_V5(file)->minor_version;
    if (micro_version) *micro_version = GHWP_FILE_V5(file)->micro_version;
    if (extra_version) *extra_version = GHWP_FILE_V5(file)->extra_version;
}

/**
 * Since: 0.2
 */
gchar *ghwp_file_v5_get_hwp_version_string (GHWPFile *file)
{
  g_return_val_if_fail (GHWP_IS_FILE_V5 (file), NULL);

  return g_strdup_printf ("%d.%d.%d.%d", GHWP_FILE_V5(file)->major_version,
                                         GHWP_FILE_V5(file)->minor_version,
                                         GHWP_FILE_V5(file)->micro_version,
                                         GHWP_FILE_V5(file)->extra_version);
}

/**
 * ghwp_file_v5_new_from_uri:
 * @uri: uri of the file to load
 * @error: (allow-none): Return location for an error, or %NULL
 *
 * Creates a new #GHWPFileV5.  If %NULL is returned, then @error will be
 * set. Possible errors include those in the #GHWP_ERROR and #G_FILE_ERROR
 * domains.
 *
 * Return value: A newly created #GHWPFileV5, or %NULL
 *
 * Since: 0.2
 */
GHWPFileV5* ghwp_file_v5_new_from_uri (const gchar* uri, GError** error)
{
    g_return_val_if_fail (uri != NULL, NULL);

    gchar      *filename = g_filename_from_uri (uri, NULL, error);
    GHWPFileV5 *file     = ghwp_file_v5_new_from_filename (filename, error);
    g_free (filename);
    return file;
}

/* TODO 에러 감지/전파 코드 있어야 한다. */
static void parse_file_header (GHWPFileV5 *file)
{
    g_return_if_fail (file != NULL);

    GsfInputStream *gis;
    gssize          size;
    gsize           bytes_read;
    guint8         *buf;
    guint32         prop = 0;

    gis  = (GsfInputStream *) g_object_ref (file->file_header_stream);
    size = gsf_input_stream_size (gis);
    buf  = g_malloc (size);

    g_input_stream_read_all ((GInputStream*) gis, buf, (gsize) size,
                             &bytes_read, NULL, NULL);
    g_object_unref (gis);

    if (bytes_read >= 40) {
        file->signature = g_strndup ((const gchar *)buf, 32); /* null로 끝남 */
        file->major_version = buf[35];
        file->minor_version = buf[34];
        file->micro_version = buf[33];
        file->extra_version = buf[32];

        memcpy (&prop, buf + 36, 4);
        prop = GUINT32_FROM_LE(prop);

        file->is_compress            = prop & (1 <<  0);
        file->is_encrypt             = prop & (1 <<  1);
        file->is_distribute          = prop & (1 <<  2);
        file->is_script              = prop & (1 <<  3);
        file->is_drm                 = prop & (1 <<  4);
        file->is_xml_template        = prop & (1 <<  5);
        file->is_history             = prop & (1 <<  6);
        file->is_sign                = prop & (1 <<  7);
        file->is_certificate_encrypt = prop & (1 <<  8);
        file->is_sign_spare          = prop & (1 <<  9);
        file->is_certificate_drm     = prop & (1 << 10);
        file->is_ccl                 = prop & (1 << 11);
    }

    buf = (g_free (buf), NULL);
}

static gint get_entry_order (char *a)
{
    if (g_str_equal (a, "FileHeader"))
        return 0;
    if (g_str_equal (a, "DocInfo"))
        return 1;
    if (g_str_equal (a, "BodyText"))
        return 2;
    if (g_str_equal (a, "ViewText"))
        return 3;
    if (g_str_equal (a, "\005HwpSummaryInformation"))
        return 4;
    if (g_str_equal (a, "BinData"))
        return 5;
    if (g_str_equal (a, "PrvText"))
        return 6;
    if (g_str_equal (a, "PrvImage"))
        return 7;
    if (g_str_equal (a, "DocOptions"))
        return 8;
    if (g_str_equal (a, "Scripts"))
        return 9;
    if (g_str_equal (a, "XMLTemplate"))
        return 10;
    if (g_str_equal (a, "DocHistory"))
        return 11;

    return G_MAXINT;
}

static gint compare_entry_names (gconstpointer a, gconstpointer b)
{
    gint i = get_entry_order (*(char **)a);
    gint j = get_entry_order (*(char **)b);
    return i - j;
}

static void make_stream (GHWPFileV5 *file)
{
    g_return_if_fail (GHWP_IS_FILE_V5 (file));

    const gchar *name       = NULL;
    GsfInfile   *oleinfile  = GSF_INFILE (file->priv->olefile);
    gint         n_children = gsf_infile_num_children (oleinfile);

    if (n_children < 1) {
        g_error ("invalid hwp file\n");
        return;
    }

    /* 스펙이 명확하지 않고, 추후 예고없이 스펙이 변할 수 있기 때문에
     * 이를 감지하고자 코드를 이렇게 작성하였다. */
    GArray *entry_names = g_array_new (TRUE, TRUE, sizeof(char *));

    for (gint i = 0; i < n_children; i++) {
        name = gsf_infile_name_by_index (oleinfile, i);
        g_array_append_val (entry_names, name);
    }
    g_array_sort(entry_names, compare_entry_names);

    for (gint i = 0; i < n_children; i++) {
        char     *entry = g_array_index (entry_names, char *, i);
        GsfInput* input;
        gint      n_children = 0;

        if (g_str_equal (entry, "FileHeader")) {
            input = gsf_infile_child_by_name (oleinfile, entry);
            n_children = gsf_infile_num_children ((GsfInfile*) input);

            if (n_children > 0) {
                g_error ("invalid\n");
            }

            file->file_header_stream = G_INPUT_STREAM (gsf_input_stream_new (input));
            parse_file_header (file);
        } else if (g_str_equal (entry, "DocInfo")) {
            input = gsf_infile_child_by_name (oleinfile, entry);
            input = g_object_ref (input);
            n_children = gsf_infile_num_children ((GsfInfile*) input);

            if (n_children > 0) {
                g_error ("invalid\n");
            }

            if (file->is_compress) {
                GsfInputStream    *gis;
                GZlibDecompressor *zd;
                GInputStream      *cis;

                gis = gsf_input_stream_new (input);
                zd  = g_zlib_decompressor_new (G_ZLIB_COMPRESSOR_FORMAT_RAW);
                cis = g_converter_input_stream_new ((GInputStream*) gis,
                                                    (GConverter*) zd);
                file->doc_info_stream = cis;

                g_object_unref (zd);
                g_object_unref (gis);
            } else {
                file->doc_info_stream = (GInputStream*) gsf_input_stream_new (input);
            }
            g_object_unref (input);
        } else if (g_str_equal(entry, "BodyText") ||
                   g_str_equal(entry, "VeiwText")) {
            GsfInfile* infile;

            file->section_streams = g_array_new (TRUE, TRUE,
                                                 sizeof (GInputStream*));

            infile = (GsfInfile*) gsf_infile_child_by_name (oleinfile, entry);
            infile = g_object_ref (infile);

            n_children = gsf_infile_num_children (infile);

            if (n_children == 0) {
                g_error ("nothing in %s\n", entry);
            }

            for (gint j = 0; j < n_children; j++) {
                input = gsf_infile_child_by_index (infile, j);
                GsfInfile *section = g_object_ref (input);

                if (gsf_infile_num_children (section) > 0) {
                    g_error ("invalid section");
                }

                if (file->is_compress) {
                    GsfInputStream    *gis;
                    GZlibDecompressor *zd;
                    GInputStream      *cis;

                    gis = gsf_input_stream_new ((GsfInput*) section);
                    zd  = g_zlib_decompressor_new (G_ZLIB_COMPRESSOR_FORMAT_RAW);
                    cis = g_converter_input_stream_new ((GInputStream *) gis,
                                                        (GConverter *) zd);

                    file->priv->section_stream = G_INPUT_STREAM (cis);
                    g_object_unref (zd);
                    g_object_unref (gis);
                } else {
                    GsfInputStream *stream;
                    stream = gsf_input_stream_new ((GsfInput*) section);
                    file->priv->section_stream = G_INPUT_STREAM (stream);
                }

                GInputStream *_stream_ = file->priv->section_stream;
                _stream_ = g_object_ref (_stream_);
                g_array_append_val (file->section_streams, _stream_);
                g_object_unref (section);
            } /* for */
            g_object_unref (infile);
        } else if (g_str_equal (entry, "\005HwpSummaryInformation")) {
            input = gsf_infile_child_by_name (oleinfile, entry);
            input = g_object_ref (input);
            n_children = gsf_infile_num_children ((GsfInfile*) input);

            if (n_children > 0) {
                g_error ("invalid\n");
            }

            file->summary_info_stream = (GInputStream*) gsf_input_stream_new (input);
            g_object_unref (input);
        } else if (g_str_equal (entry, "PrvText")) {
            input = gsf_infile_child_by_name (oleinfile, entry);
            input = g_object_ref (input);
            n_children = gsf_infile_num_children ((GsfInfile*) input);

            if (n_children > 0) {
                g_error ("invalid\n");
            }

            file->prv_text_stream = (GInputStream *) gsf_input_stream_new (input);
            g_object_unref (input);
        } else if (g_str_equal (entry, "PrvImage")) {
            input = gsf_infile_child_by_name (oleinfile, entry);
            input = g_object_ref (input);
            n_children = gsf_infile_num_children ((GsfInfile*) input);

            if (n_children > 0) {
                g_error ("invalid\n");
            }

            file->prv_image_stream = (GInputStream*) gsf_input_stream_new (input);
            g_object_unref (input);
        } else {
            g_warning("%s:%d: %s not implemented\n", __FILE__, __LINE__, entry);
        } /* if */
    } /* for */
    g_array_free (entry_names, TRUE);
    g_array_unref (entry_names);
}

/**
 * Since: 0.2
 */
GHWPFileV5* ghwp_file_v5_new_from_filename (const gchar* filename, GError** error)
{
    g_return_val_if_fail (filename != NULL, NULL);
    GFile *gfile = g_file_new_for_path (filename);

    GsfInputStdio  *input;
    GsfInfileMSOle *olefile;

    gchar *path = g_file_get_path(gfile);
    g_object_unref (gfile);
    input = (GsfInputStdio*) gsf_input_stdio_new (path, error);
    g_free (path);

    if (input == NULL) {
        g_warning("%s:%d: %s\n", __FILE__, __LINE__, (*error)->message);
        return NULL;
    }

    olefile = (GsfInfileMSOle*) gsf_infile_msole_new ((GsfInput*) input,
                                                      error);

    if (olefile == NULL) {
        g_warning("%s:%d: %s\n", __FILE__, __LINE__, (*error)->message);
        g_object_unref (input);
        return NULL;
    }

    GHWPFileV5 *file = g_object_new (GHWP_TYPE_FILE_V5, NULL);
    file->priv->olefile = olefile;
    g_object_unref (input);
    make_stream (file);

    return file;
}

static void ghwp_file_v5_finalize (GObject *object)
{
    GHWPFileV5 *file = GHWP_FILE_V5(object);
    g_object_unref (file->priv->olefile);
    g_object_unref (file->prv_text_stream);
    g_object_unref (file->prv_image_stream);
    g_object_unref (file->file_header_stream);
    g_object_unref (file->doc_info_stream);
    g_array_free   (file->section_streams, TRUE);
    g_object_unref (file->priv->section_stream);
    g_object_unref (file->summary_info_stream);
    g_free         (file->signature);
    G_OBJECT_CLASS (ghwp_file_v5_parent_class)->finalize (object);
}

static void ghwp_file_v5_class_init (GHWPFileV5Class *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  g_type_class_add_private (klass, sizeof (GHWPFileV5Private));
  GHWPFileClass *file_class          = GHWP_FILE_CLASS (klass);
  file_class->get_document           = ghwp_file_v5_get_document;
  file_class->get_hwp_version_string = ghwp_file_v5_get_hwp_version_string;
  file_class->get_hwp_version        = ghwp_file_v5_get_hwp_version;
  object_class->finalize = ghwp_file_v5_finalize;
}

static void ghwp_file_v5_init (GHWPFileV5 *file)
{
    file->priv = G_TYPE_INSTANCE_GET_PRIVATE (file, GHWP_TYPE_FILE_V5,
                                                    GHWPFileV5Private);
}
