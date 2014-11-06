/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * hwp-hwp5-file.c
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

#include <gsf/gsf-input.h>
#include <gsf/gsf-utils.h>
#include <gsf/gsf-infile.h>
#include <gsf/gsf-input-stdio.h>
#include <gsf/gsf-input-memory.h>
#include <openssl/evp.h>

#include "gsf-input-stream.h"
#include "hwp-hwp5-file.h"
#include "hwp-hwp5-parser.h"
#include "hwp-models.h"

G_DEFINE_TYPE (HwpHWP5File, hwp_hwp5_file, HWP_TYPE_FILE);

/**
 * hwp_hwp5_file_get_document:
 * @file: a #HwpFile
 * @error: location to store the error occurring, or %NULL to ignore
 *
 * Return value: (transfer full): A #HwpDocument, or %NULL
 *
 * Since: 0.0.1
 */
HwpDocument *hwp_hwp5_file_get_document (HwpFile *file, GError **error)
{
  g_return_val_if_fail (HWP_IS_HWP5_FILE (file), NULL);

  HwpDocument *document = hwp_document_new ();

  HwpHWP5Parser *parser;
  parser = hwp_hwp5_parser_new (HWP_LISTENER (document), document);
  hwp_hwp5_parser_parse (parser, HWP_HWP5_FILE (file), error);
  g_object_unref (parser);

  if (*error) {
    g_object_unref (document);
    document = NULL;
  }

  return document;
}

/**
 * hwp_hwp5_file_check_version:
 * @file: a #HwpHWP5File
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
 *   if (hwp_hwp5_file_check_version (doc, 5, 0, 0, 7))
 *     g_print ("HWP document version is 5.0.0.7 or above");
 * </programlisting>
 * </example>
 *
 * Returns: %TRUE if the version of the HWP document
 * is the same as or newer than the passed-in version.
 *
 * Since: 0.0.1
 */
gboolean hwp_hwp5_file_check_version (HwpHWP5File *file,
                                      guint8       major,
                                      guint8       minor,
                                      guint8       micro,
                                      guint8       extra)
{
    g_return_val_if_fail (HWP_IS_HWP5_FILE (file), FALSE);

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
 * hwp_hwp5_file_get_hwp_version:
 * @file: A #HwpFile
 * @major_version: (out) (allow-none): return location for the HWP major version number
 * @minor_version: (out) (allow-none): return location for the HWP minor version number
 * @micro_version: (out) (allow-none): return location for the HWP micro version number
 * @extra_version: (out) (allow-none): return location for the HWP extra version number
 *
 * Returns: the major, minor, micro and extra HWP version numbers
 *
 * Since: 0.0.1
 */
void hwp_hwp5_file_get_hwp_version (HwpFile *file,
                                    guint8  *major_version,
                                    guint8  *minor_version,
                                    guint8  *micro_version,
                                    guint8  *extra_version)
{
  g_return_if_fail (HWP_IS_HWP5_FILE (file));

  if (major_version) *major_version = HWP_HWP5_FILE(file)->major_version;
  if (minor_version) *minor_version = HWP_HWP5_FILE(file)->minor_version;
  if (micro_version) *micro_version = HWP_HWP5_FILE(file)->micro_version;
  if (extra_version) *extra_version = HWP_HWP5_FILE(file)->extra_version;
}

/**
 * hwp_hwp5_file_get_hwp_version_string:
 * @file: A #HwpFile
 *
 * Returns: the major, minor, micro and extra HWP version string
 *
 * Since: 0.0.1
 */
gchar *hwp_hwp5_file_get_hwp_version_string (HwpFile *file)
{
  g_return_val_if_fail (HWP_IS_HWP5_FILE (file), NULL);

  return g_strdup_printf ("%d.%d.%d.%d", HWP_HWP5_FILE(file)->major_version,
                                         HWP_HWP5_FILE(file)->minor_version,
                                         HWP_HWP5_FILE(file)->micro_version,
                                         HWP_HWP5_FILE(file)->extra_version);
}

static void parse_file_header (HwpHWP5File *file)
{
  size_t size = gsf_input_size (file->file_header_stream);
  const guint8 *buf = gsf_input_read (file->file_header_stream, size, NULL);
  guint32 prop = 0;

  if (buf == NULL)
    return;

  file->signature = g_strndup ((const gchar *) buf, 32); /* null로 끝남 */
  file->major_version = buf[35];
  file->minor_version = buf[34];
  file->micro_version = buf[33];
  file->extra_version = buf[32];

  prop = GSF_LE_GET_GUINT32(buf + 36);
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

static guint32 random_seed = 1;

static void msvc_srand (guint32 seed)
{
  random_seed = seed;
}

static int msvc_rand ()
{
  random_seed = (random_seed * 214013 + 2531011) & 0xffffffff;
  return ((random_seed >> 16) & 0x7fff);
}

static void make_stream (HwpHWP5File *file, GError **error)
{
  GsfInput  *input        = NULL;
  GsfInfile *ole          = GSF_INFILE (file->priv->olefile);
  gint       n_root_entry = gsf_infile_num_children (ole);

  if (n_root_entry < 1)
  {
    g_set_error_literal (error,
                         HWP_FILE_ERROR,
                         HWP_FILE_ERROR_INVALID,
                         "invalid hwp file");
    return;
  }

  /* 우선 순위에 따라 스트림을 만든다 */
  input = gsf_infile_child_by_name (ole, "FileHeader");
  if (input && gsf_infile_num_children (GSF_INFILE (input)) == -1)
  {
    file->file_header_stream = input;
    input = NULL;
    parse_file_header (file);
  }
  else
  {
    goto FAIL;
  }

  input = gsf_infile_child_by_name (ole, "DocInfo");
  if (input && gsf_infile_num_children (GSF_INFILE (input)) == -1)
  {
    if (file->is_compress)
    {
      GInputStream    *gis;
      GZlibDecompressor *zd;
      GInputStream      *cis;

      gis = (GInputStream *) gsf_input_stream_new (input);
      zd  = g_zlib_decompressor_new (G_ZLIB_COMPRESSOR_FORMAT_RAW);
      cis = g_converter_input_stream_new (gis, (GConverter *)   zd);
      g_filter_input_stream_set_close_base_stream (G_FILTER_INPUT_STREAM (cis), TRUE);
      file->doc_info_stream = cis;

      g_object_unref (zd);
      g_object_unref (gis);

      input = NULL;
    }
    else
    {
      file->doc_info_stream = (GInputStream *) gsf_input_stream_new (input);
    }
  }
  else
  {
    goto FAIL;
  }

  if (!file->is_distribute)
    input = gsf_infile_child_by_name (ole, "BodyText");
  else
    input = gsf_infile_child_by_name (ole, "ViewText");

  if (input)
  {
    for (gint i = 0; i < gsf_infile_num_children (GSF_INFILE (input)); i++)
    {
      GsfInput *section =
                  gsf_infile_child_by_name (GSF_INFILE (input),
                                            g_strdup_printf("Section%d", i));

      if (gsf_infile_num_children (GSF_INFILE (section)) != -1)
      {
        if (GSF_IS_INPUT (section))
          g_object_unref (section);

        g_set_error_literal (error,
                             HWP_FILE_ERROR,
                             HWP_FILE_ERROR_INVALID,
                             "invalid hwp file");
        return;
      }

      if (file->is_distribute)
      {
        g_set_error_literal (error,
                             HWP_FILE_ERROR,
                             HWP_FILE_ERROR_INVALID,
                             "The encrypted hwp document for distribution is not supported");

        return; /* FIXME */

        /* FIXME: decrypt */
        /* get sha1sum */
        guint8 *data = g_malloc0 (256);
        gsf_input_read (section, 4, NULL);
        gsf_input_read (section, 256, data);
        guint32 seed = GSF_LE_GET_GUINT32 (data);
        msvc_srand (seed);
        gint n = 0, val = 0, offset;

        for (guint i = 0; i < 256; i++)
        {
          if (n == 0)
          {
            val = msvc_rand() & 0xff;
            n = (msvc_rand() & 0xf) + 1;
          }

          data[i] ^= val;

          n--;
        }

        offset = 4 + (seed & 0xf);
        gchar *sha1sum = g_convert ((const gchar *) data + offset, 80,
                           "UTF-8", "UTF-16LE", NULL, NULL, error);
        g_free (data);
        gchar *key = g_strndup (sha1sum, 16);
#ifdef HWP_ENABLE_DEBUG
        printf ("sha1sum: %s\n", sha1sum);
        printf ("key: %s\n", key);
#endif
        g_free (sha1sum);

        EVP_CIPHER_CTX *ctx;
        ctx = EVP_CIPHER_CTX_new ();
        EVP_CIPHER_CTX_init (ctx);
        EVP_DecryptInit_ex (ctx, EVP_aes_128_ecb(), NULL, (unsigned char *) key, NULL);
        g_free (key);
        EVP_CIPHER_CTX_set_padding(ctx, 0); /* no padding */

        gsf_off_t encrypted_data_len = gsf_input_remaining (section);
        guint8 const *encrypted_data = gsf_input_read (section, encrypted_data_len, NULL);
        guint8 *decrypted_data = g_malloc (encrypted_data_len);
        int decrypted_data_len;
        int len;
        EVP_DecryptUpdate (ctx, decrypted_data, &len, encrypted_data, encrypted_data_len);
        decrypted_data_len = len;
        EVP_DecryptFinal_ex (ctx, decrypted_data + len, &len);
        decrypted_data_len += len;
        EVP_CIPHER_CTX_free (ctx);
        g_free (decrypted_data);
        g_object_unref (section);
        section = gsf_input_memory_new (decrypted_data, decrypted_data_len, TRUE);
      }

      if (file->is_compress)
      {
        GInputStream      *gis;
        GZlibDecompressor *zd;
        GInputStream      *cis;

        gis = (GInputStream *) gsf_input_stream_new (section);
        zd  = g_zlib_decompressor_new (G_ZLIB_COMPRESSOR_FORMAT_RAW);
        cis = g_converter_input_stream_new (gis, (GConverter *) zd);
        g_filter_input_stream_set_close_base_stream (G_FILTER_INPUT_STREAM (cis), TRUE);
        g_ptr_array_add (file->section_streams, cis);
        g_object_unref (zd);
        g_object_unref (gis);
      }
      else
      {
        GInputStream *stream = (GInputStream *) gsf_input_stream_new (section);
        g_ptr_array_add (file->section_streams, stream);
      }
    } /* for */
    g_object_unref (input);
    input = NULL;
  }
  else
  {
    goto FAIL;
  }

  input = gsf_infile_child_by_name (ole, "\005HwpSummaryInformation");
  if (input && gsf_infile_num_children (GSF_INFILE (input)) == -1)
  {
    file->summary_info_stream = input;
    input = NULL;
  }
  else
  {
    goto FAIL;
  }

  input = gsf_infile_child_by_name (ole, "BinData");

  if (input)
  {
    gint n_data = gsf_infile_num_children (GSF_INFILE (input));

    for (gint i = 0; i < n_data; i++)
    {
      GsfInput *bin_data_input =
                  gsf_infile_child_by_index (GSF_INFILE (input), i);

      if (gsf_infile_num_children (GSF_INFILE (bin_data_input)) != -1)
      {
        if (GSF_IS_INPUT (bin_data_input))
          g_object_unref (bin_data_input);

        g_set_error_literal (error,
                             HWP_FILE_ERROR,
                             HWP_FILE_ERROR_INVALID,
                             "invalid hwp file");
        return;
      }

      if (file->is_compress)
      {
        GInputStream      *gis;
        GZlibDecompressor *zd;
        GInputStream      *cis;

        gis = (GInputStream *) gsf_input_stream_new (bin_data_input);
        zd  = g_zlib_decompressor_new (G_ZLIB_COMPRESSOR_FORMAT_RAW);
        cis = g_converter_input_stream_new (gis, (GConverter *) zd);
        g_filter_input_stream_set_close_base_stream (G_FILTER_INPUT_STREAM (cis), TRUE);
        g_ptr_array_add (file->bin_data_streams, cis);
        g_object_unref (zd);
        g_object_unref (gis);
      }
      else
      {
        GInputStream *stream = (GInputStream *) gsf_input_stream_new (bin_data_input);
        g_ptr_array_add (file->bin_data_streams, stream);
      }
    }

    g_object_unref (input);
    input = NULL;
  }

  input = gsf_infile_child_by_name (ole, "PrvText");
  if (input && gsf_infile_num_children (GSF_INFILE (input)) == -1)
  {
    file->prv_text_stream = input;
    input = NULL;
  }
  else
  {
    goto FAIL;
  }

  input = gsf_infile_child_by_name (ole, "PrvImage");
  if (input && gsf_infile_num_children (GSF_INFILE (input)) == -1)
  {
    file->prv_image_stream = input;
    input = NULL;
  }
  else
  {
    goto FAIL;
  }

  return;

  FAIL:

  if (GSF_IS_INPUT (input))
    g_object_unref (input);

  g_set_error_literal (error,
                       HWP_FILE_ERROR,
                       HWP_FILE_ERROR_INVALID,
                       "invalid hwp file");
  return;
}

/**
 * hwp_hwp5_file_new_for_path:
 * @path: path of the file to load
 * @error: location to store the error occurring, or %NULL to ignore
 *
 * Creates a new #HwpHWP5File.  If %NULL is returned, then @error will be
 * set. Possible errors include those in the #HWP_ERROR and #HWP_FILE_ERROR
 * domains.
 *
 * Returns: A newly created #HwpHWP5File, or %NULL
 *
 * Since: 0.0.1
 */
HwpHWP5File *hwp_hwp5_file_new_for_path (const gchar *path, GError **error)
{
  g_return_val_if_fail (path != NULL, NULL);

  GsfInput  *input;
  GsfInfile *olefile;

  if ((input = gsf_input_stdio_new (path, error))) {
    if ((olefile = gsf_infile_msole_new (input, error))) {
      HwpHWP5File *file   = g_object_new (HWP_TYPE_HWP5_FILE, NULL);
      file->priv->olefile = olefile;
      g_object_unref (input);
      make_stream (file, error);
      return file;
    }
  }

  g_warning("%s:%d: %s\n", __FILE__, __LINE__, (*error)->message);

  if (input)
    g_object_unref (input);

  return NULL;
}

static void hwp_hwp5_file_finalize (GObject *object)
{
  HwpHWP5File *file = HWP_HWP5_FILE(object);

  if (file->priv->olefile)
    g_object_unref (file->priv->olefile);

  if (file->prv_text_stream)
    g_object_unref (file->prv_text_stream);

  if (file->prv_image_stream)
    g_object_unref (file->prv_image_stream);

  if (file->file_header_stream)
    g_object_unref (file->file_header_stream);

  if (file->doc_info_stream)
    g_object_unref (file->doc_info_stream);

  g_ptr_array_unref (file->section_streams);
  g_ptr_array_unref (file->bin_data_streams);

  if (file->summary_info_stream)
    g_object_unref (file->summary_info_stream);

  g_free (file->signature);

  G_OBJECT_CLASS (hwp_hwp5_file_parent_class)->finalize (object);
}

static void hwp_hwp5_file_class_init (HwpHWP5FileClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  g_type_class_add_private (klass, sizeof (HwpHWP5FilePrivate));
  HwpFileClass *file_class           = HWP_FILE_CLASS (klass);
  file_class->get_document           = hwp_hwp5_file_get_document;
  file_class->get_hwp_version_string = hwp_hwp5_file_get_hwp_version_string;
  file_class->get_hwp_version        = hwp_hwp5_file_get_hwp_version;
  object_class->finalize = hwp_hwp5_file_finalize;
}

static void hwp_hwp5_file_init (HwpHWP5File *file)
{
  file->section_streams  = g_ptr_array_new_with_free_func (g_object_unref);
  file->bin_data_streams = g_ptr_array_new_with_free_func (g_object_unref);

  file->priv = G_TYPE_INSTANCE_GET_PRIVATE (file,
                                            HWP_TYPE_HWP5_FILE,
                                            HwpHWP5FilePrivate);
}
