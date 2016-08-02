/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * unhwp.c
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

/* TODO 할 일들
 * 압축된 경우 압축 비트 읽어서 압축 풀 것
 */

#include <string.h>
#include <gsf/gsf-utils.h>
#include <gsf/gsf-input-stdio.h>
#include <gsf/gsf-outfile-stdio.h>
#include <gsf/gsf-infile.h>
#include <gsf/gsf-infile-msole.h>
#include <gsf/gsf-structured-blob.h>
#include <gio/gio.h>

int main (int argc, char **argv)
{
  GError    *error = NULL;
  GsfInput  *input;
  GsfInfile *infile;

  if (argc < 2) {
    puts ("Usage: unhwp file.hwp");
    return -1;
  }

#if (!GLIB_CHECK_VERSION(2, 35, 0))
  g_type_init();
#endif

  input  = gsf_input_stdio_new (argv[1], &error);
  infile = gsf_infile_msole_new (input, &error);

  if (error) {
    fprintf (stderr, "Error: %s is a invalid hwp v5.0 file. %s\n", argv[1], error->message);
    return 1;
  }

  char *p = NULL;
  char *out_filename = NULL;
  /* basename 은 확장자를 포함합니다. */
  char *basename = g_path_get_basename (argv[1]);

  if ((p = rindex (basename, '.'))) {
    int len = strlen (basename) - strlen (p);
    /* filebase 는 확장자를 포함하지 않습니다. */
    char *filebase = g_strndup (basename, len);
    out_filename = g_strconcat (filebase, "_FILES", NULL);
    g_free (filebase);
  } else {
    out_filename = g_strconcat (basename, "_FILES", NULL);
  }
  g_free (basename);

  if (g_file_test (out_filename, G_FILE_TEST_EXISTS)) {
    fprintf (stderr, "Error: %s %s\n", out_filename, error->message);
    return 1;
  }

  GsfOutfile *folder = gsf_outfile_stdio_new (out_filename, &error);
  g_free (out_filename);

  if (error) {
    fprintf (stderr, "%s\n", error->message);
    return 1;
  }

  for (int i = 0; i < gsf_infile_num_children (infile); i++)
  {
    GsfInput *item = gsf_infile_child_by_index (infile, i);
    GsfStructuredBlob *itemfile = gsf_structured_blob_read (item);
    gsf_structured_blob_write (itemfile, folder);
  }

  gsf_shutdown ();
  return 0;
}
