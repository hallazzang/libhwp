/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */
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

#include <gsf/gsf-input-impl.h>
#include <stdlib.h>
#include <string.h>
#include <gsf/gsf-input-stdio.h>
#include <gsf/gsf-infile-impl.h>
#include <stdio.h>

#include "gsf-input-stream.h"
#include "ghwp-file-v5.h"

G_DEFINE_TYPE (GHWPFileV5, ghwp_file_v5, G_TYPE_OBJECT);

#define _g_object_unref0(var) ((var == NULL) ? NULL : (var = (g_object_unref (var), NULL)))
#define _g_array_free0(var) ((var == NULL) ? NULL : (var = (g_array_free (var, TRUE), NULL)))
#define _g_error_free0(var) ((var == NULL) ? NULL : (var = (g_error_free (var), NULL)))
#define _g_free0(var) (var = (g_free (var), NULL))

#define GHWP_FILE_V5_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), GHWP_TYPE_FILE_V5, GHWPFileV5Private))

static void       _ghwp_file_v5_make_stream (GHWPFileV5* file);
static void        ghwp_file_v5_decode_file_header (GHWPFileV5 *file);
static void        ghwp_file_v5_finalize (GObject* obj);

static gpointer _g_object_ref0 (gpointer obj)
{
    return obj ? g_object_ref (obj) : NULL;
}


GHWPFileV5* ghwp_file_v5_new_from_uri (const gchar* uri, GError** error)
{
    g_return_val_if_fail (uri != NULL, NULL);

    gchar      *filename = g_filename_from_uri (uri, NULL, error);
    GHWPFileV5 *file     = ghwp_file_v5_new_from_filename (filename, error);
    _g_free0 (filename);
    return file;
}


GHWPFileV5* ghwp_file_v5_new_from_filename (const gchar* filename, GError** error)
{
    g_return_val_if_fail (filename != NULL, NULL);
    GFile *gfile = g_file_new_for_path (filename);

    GsfInputStdio* input;
    GsfInfileMSOle* olefile;

    gchar *path = g_file_get_path(gfile);
    _g_object_unref0 (gfile);
    input = (GsfInputStdio*) gsf_input_stdio_new (path, error);
    _g_free0 (path);

    if (input == NULL) {
        g_warning("%s:%d: %s\n", __FILE__, __LINE__, (*error)->message);
        return NULL;
    }

    olefile = (GsfInfileMSOle*) gsf_infile_msole_new ((GsfInput*) input,
                                                      error);

    if (olefile == NULL) {
        g_warning("%s:%d: %s\n", __FILE__, __LINE__, (*error)->message);
        _g_object_unref0 (input);
        return NULL;
    }

    GHWPFileV5 *file = (GHWPFileV5*) g_object_new (GHWP_TYPE_FILE, NULL);
    _g_object_unref0 (file->priv->olefile);
    file->priv->olefile = olefile;
    _g_object_unref0 (input);
    _ghwp_file_v5_make_stream (file);
    return file;
}

/* FIXME streams 배열과 enum을 이용하여 코드 재적성 바람 */
static void _ghwp_file_v5_make_stream (GHWPFileV5 *file)
{
    g_return_if_fail (file != NULL);

    const gchar* name = NULL;
    gint n_children;
    n_children = gsf_infile_num_children ((GsfInfile*) file->priv->olefile);

    if (n_children < 1) {
        fprintf (stderr, "invalid hwp file\n");
        return;
    }

    gint i;
    for (i = 0; i < n_children; i++) {
        /* do not free the name string */
        name = gsf_infile_name_by_index ((GsfInfile*) file->priv->olefile, i);
        GsfInput* input;
        gint      num_children = 0;

        if (g_str_equal (name, "PrvText")) {
            input = gsf_infile_child_by_name ((GsfInfile*) file->priv->olefile,
                                              name);
            input = _g_object_ref0 (input);
            num_children = gsf_infile_num_children ((GsfInfile*) input);

            if (num_children > 0) {
                fprintf (stderr, "invalid\n");
            }

            _g_object_unref0 (file->prv_text_stream);
            file->prv_text_stream = (GInputStream *) gsf_input_stream_new (input);
            _g_object_unref0 (input);
        }
        else if (g_str_equal (name, "PrvImage")) {
            input = gsf_infile_child_by_name ((GsfInfile*) file->priv->olefile,
                                              name);
            input = _g_object_ref0 (input);
            num_children = gsf_infile_num_children ((GsfInfile*) input);

            if (num_children > 0) {
                fprintf (stderr, "invalid\n");
            }

            _g_object_unref0 (file->prv_image_stream);
            file->prv_image_stream = (GInputStream*) gsf_input_stream_new (input);
            _g_object_unref0 (input);
        }
        else if (g_str_equal (name, "FileHeader")) {
            input = gsf_infile_child_by_name ((GsfInfile*) file->priv->olefile,
                                              name);
            input = _g_object_ref0 (input);
            num_children = gsf_infile_num_children ((GsfInfile*) input);

            if (num_children > 0) {
                fprintf (stderr, "invalid\n");
            }

            _g_object_unref0 (file->file_header_stream);
            file->file_header_stream = (GInputStream*) gsf_input_stream_new (input);
            _g_object_unref0 (input);
            ghwp_file_v5_decode_file_header (file);
        }
        else if (g_str_equal (name, "DocInfo")) {
            input = gsf_infile_child_by_name ((GsfInfile*) file->priv->olefile,
                                              name);
            input = _g_object_ref0 (input);
            num_children = gsf_infile_num_children ((GsfInfile*) input);

            if (num_children > 0) {
                fprintf (stderr, "invalid\n");
            }

            if (file->is_compress) {
                GsfInputStream    *gis;
                GZlibDecompressor *zd;
                GInputStream      *cis;

                gis = gsf_input_stream_new (input);
                zd  = g_zlib_decompressor_new (G_ZLIB_COMPRESSOR_FORMAT_RAW);
                cis = g_converter_input_stream_new ((GInputStream*) gis,
                                                    (GConverter*) zd);
                _g_object_unref0 (file->doc_info_stream);
                file->doc_info_stream = cis;

                _g_object_unref0 (zd);
                _g_object_unref0 (gis);
            } else {
                _g_object_unref0 (file->doc_info_stream);
                file->doc_info_stream = (GInputStream*) gsf_input_stream_new (input);
            }
            _g_object_unref0 (input);
        }
        else if ((g_str_equal (name, "BodyText")) |
                 (g_str_equal (name, "VeiwText"))) {

            GsfInfile* infile;

            _g_array_free0 (file->section_streams);
            file->section_streams = g_array_new (TRUE, TRUE,
                                                 sizeof (GInputStream*));

            infile = (GsfInfile*) gsf_infile_child_by_index (
                                         (GsfInfile*) file->priv->olefile, i);
            infile = _g_object_ref0 (infile);

            num_children = gsf_infile_num_children (infile);

            if (num_children == 0) {
                fprintf (stderr, "nothing in BodyText\n");
            }

            gint j;
            for (j = 0; j < num_children; j++) {
                input = gsf_infile_child_by_index (infile, j);
                GsfInfile *section = _g_object_ref0 (input);
                num_children = gsf_infile_num_children (section);

                if (num_children > 0) {
                    fprintf (stderr, "invalid section\n");
                }

                if (file->is_compress) {
                    GsfInputStream* gis;
                    GZlibDecompressor* zd;
                    GConverterInputStream* cis;

                    gis = gsf_input_stream_new ((GsfInput*) section);
                    zd = g_zlib_decompressor_new (G_ZLIB_COMPRESSOR_FORMAT_RAW);
                    cis = (GConverterInputStream*) g_converter_input_stream_new ((GInputStream*) gis, (GConverter*) zd);

                    _g_object_unref0 (file->priv->section_stream);
                    file->priv->section_stream = (GInputStream*) cis;
                    _g_object_unref0 (zd);
                    _g_object_unref0 (gis);
                } else {
                    GsfInputStream* stream;
                    stream = gsf_input_stream_new ((GsfInput*) section);
                    _g_object_unref0 (file->priv->section_stream);
                    file->priv->section_stream = (GInputStream*) stream;
                }

                GInputStream *_stream_ = file->priv->section_stream;
                _stream_ = _g_object_ref0 (_stream_);
                g_array_append_val (file->section_streams, _stream_);
                _g_object_unref0 (section);
            } /* for */
            _g_object_unref0 (infile);
        }
        else if (g_str_equal (name, "\005HwpSummaryInformation")) {
            input = gsf_infile_child_by_name ((GsfInfile*) file->priv->olefile,
                                              name);
            input = _g_object_ref0 (input);
            num_children = gsf_infile_num_children ((GsfInfile*) input);

            if (num_children > 0) {
                fprintf (stderr, "invalid\n");
            }

            _g_object_unref0 (file->summary_info_stream);
            file->summary_info_stream = (GInputStream*) gsf_input_stream_new (input);
            _g_object_unref0 (input);
        }
        else {
            g_warning("%s:%d: %s not implemented\n", __FILE__, __LINE__, name);
        } /* if */
    } /* for */
}

/* TODO 에러 감지/전파 코드 있어야 한다. */
static void ghwp_file_v5_decode_file_header (GHWPFileV5 *file)
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

        if ((prop & (1 <<  0)) == 1) file->is_compress            = TRUE;
        if ((prop & (1 <<  1)) == 1) file->is_encrypt             = TRUE;
        if ((prop & (1 <<  2)) == 1) file->is_distribute          = TRUE;
        if ((prop & (1 <<  3)) == 1) file->is_script              = TRUE;
        if ((prop & (1 <<  4)) == 1) file->is_drm                 = TRUE;
        if ((prop & (1 <<  5)) == 1) file->is_xml_template        = TRUE;
        if ((prop & (1 <<  6)) == 1) file->is_history             = TRUE;
        if ((prop & (1 <<  7)) == 1) file->is_sign                = TRUE;
        if ((prop & (1 <<  8)) == 1) file->is_certificate_encrypt = TRUE;
        if ((prop & (1 <<  9)) == 1) file->is_sign_spare          = TRUE;
        if ((prop & (1 << 10)) == 1) file->is_certificate_drm     = TRUE;
        if ((prop & (1 << 11)) == 1) file->is_ccl                 = TRUE;
    }

    buf = (g_free (buf), NULL);
}

static void ghwp_file_v5_class_init (GHWPFileV5Class * klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    g_type_class_add_private (klass, sizeof (GHWPFileV5Private));
    object_class->finalize = ghwp_file_v5_finalize;
}

static void ghwp_file_v5_init (GHWPFileV5 *file)
{
    file->priv = GHWP_FILE_V5_GET_PRIVATE (file);
}

static void ghwp_file_v5_finalize (GObject* obj)
{
    GHWPFileV5 *file = GHWP_FILE_V5(obj);
    _g_object_unref0 (file->priv->olefile);
    _g_object_unref0 (file->prv_text_stream);
    _g_object_unref0 (file->prv_image_stream);
    _g_object_unref0 (file->file_header_stream);
    _g_object_unref0 (file->doc_info_stream);
    _g_array_free0 (file->section_streams);
    _g_object_unref0 (file->priv->section_stream);
    _g_object_unref0 (file->summary_info_stream);
    g_free (file->signature);
    G_OBJECT_CLASS (ghwp_file_v5_parent_class)->finalize (obj);
}
