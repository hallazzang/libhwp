/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * hwp-file.h
 *
 * Copyright (C) 2012-2013 Hodong Kim <hodong@cogno.org>
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

#ifndef __HWP_FILE_H__
#define __HWP_FILE_H__

#include <glib-object.h>
#include <gio/gio.h>
#include <gsf/gsf-infile-msole.h>

#include "hwp-document.h"

G_BEGIN_DECLS

#define HWP_TYPE_FILE             (hwp_file_get_type ())
#define HWP_FILE(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), HWP_TYPE_FILE, HWPFile))
#define HWP_FILE_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), HWP_TYPE_FILE, HWPFileClass))
#define HWP_IS_FILE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HWP_TYPE_FILE))
#define HWP_IS_FILE_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), HWP_TYPE_FILE))
#define HWP_FILE_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), HWP_TYPE_FILE, HWPFileClass))

typedef struct _HWPFile      HWPFile;
typedef struct _HWPFileClass HWPFileClass;

struct _HWPFile {
  GObject parent_instance;
};

struct _HWPFileClass {
  GObjectClass   parent_class;
  HWPDocument* (*get_document)           (HWPFile *file, GError **error);
  gchar*       (*get_hwp_version_string) (HWPFile *file);
  void         (*get_hwp_version)        (HWPFile *file,
                                          guint8  *major_version,
                                          guint8  *minor_version,
                                          guint8  *micro_version,
                                          guint8  *extra_version);
};

/**
 * HWP_FILE_ERROR:
 *
 * Error domain for #HWPFile. Errors in this domain will be from
 * the #HWPFileError enumeration.
 * See #GError for more information on error domains.
 *
 * Since: 0.2
 */
#define HWP_FILE_ERROR          (hwp_file_error_quark ())

/**
 * HWPFileError:
 * @HWP_FILE_ERROR_INVALID: The HWP is invalid.
 *
 * Error codes returned by #HWPFile functions.
 *
 * Since: 0.2
 */
typedef enum {
  HWP_FILE_ERROR_INVALID
} HWPFileError;

GType        hwp_file_get_type               (void) G_GNUC_CONST;
GQuark       hwp_file_error_quark            (void) G_GNUC_CONST;
HWPFile     *hwp_file_new_for_path           (const gchar *filename,
                                              GError     **error);
HWPDocument *hwp_file_get_document           (HWPFile     *file,
                                              GError     **error);
gchar       *hwp_file_get_hwp_version_string (HWPFile*     self);
void         hwp_file_get_hwp_version        (HWPFile     *file,
                                              guint8      *major_version,
                                              guint8      *minor_version,
                                              guint8      *micro_version,
                                              guint8      *extra_version);

G_END_DECLS

#endif /* __HWP_FILE_H__ */