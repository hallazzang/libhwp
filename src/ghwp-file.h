/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * ghwp-file.h
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

#ifndef _GHWP_FILE_H_
#define _GHWP_FILE_H_

#include <glib-object.h>
#include <gio/gio.h>
#include <gsf/gsf-infile-msole.h>

#include "ghwp-document.h"

G_BEGIN_DECLS

#define GHWP_TYPE_FILE             (ghwp_file_get_type ())
#define GHWP_FILE(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GHWP_TYPE_FILE, GHWPFile))
#define GHWP_FILE_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GHWP_TYPE_FILE, GHWPFileClass))
#define GHWP_IS_FILE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GHWP_TYPE_FILE))
#define GHWP_IS_FILE_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GHWP_TYPE_FILE))
#define GHWP_FILE_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GHWP_TYPE_FILE, GHWPFileClass))

typedef struct _GHWPFile      GHWPFile;
typedef struct _GHWPFileClass GHWPFileClass;

struct _GHWPFile {
    GObject parent_instance;
};

struct _GHWPFileClass {
    GObjectClass    parent_class;
    GHWPDocument* (*get_document)           (GHWPFile *file, GError **error);
    gchar*        (*get_hwp_version_string) (GHWPFile* file);
    void          (*get_hwp_version)        (GHWPFile *file,
                                             guint8   *major_version,
                                             guint8   *minor_version,
                                             guint8   *micro_version,
                                             guint8   *extra_version);
};

/**
 * GHWP_FILE_ERROR:
 *
 * Error domain for #GHWPFile. Errors in this domain will be from
 * the #GHWPFileError enumeration.
 * See #GError for more information on error domains.
 *
 * Since: 0.2
 */
#define GHWP_FILE_ERROR          (ghwp_file_error_quark ())

/**
 * GHWPFileError:
 * @GHWP_FILE_ERROR_INVALID: The HWP is invalid.
 *
 * Error codes returned by #GHWPFile functions.
 *
 * Since: 0.2
 */
typedef enum {
    GHWP_FILE_ERROR_INVALID
} GHWPFileError;

GType         ghwp_file_get_type          (void) G_GNUC_CONST;
GQuark        ghwp_file_error_quark       (void) G_GNUC_CONST;
GHWPFile     *ghwp_file_new_from_uri      (const gchar* uri,
                                           GError     **error);
GHWPFile     *ghwp_file_new_from_filename (const gchar *filename,
                                           GError     **error);
GHWPDocument *ghwp_file_get_document      (GHWPFile    *file,
                                           GError     **error);
gchar *
ghwp_file_get_hwp_version_string          (GHWPFile*    self);
void          ghwp_file_get_hwp_version   (GHWPFile    *file,
                                           guint8      *major_version,
                                           guint8      *minor_version,
                                           guint8      *micro_version,
                                           guint8      *extra_version);

G_END_DECLS

#endif /* _GHWP_FILE_H_ */
