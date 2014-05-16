/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * hwp-file.h
 *
 * Copyright (C) 2012-2014 Hodong Kim <hodong@cogno.org>
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

#if !defined (__HWP_H_INSIDE__) && !defined (HWP_COMPILATION)
#error "Only <hwp/hwp.h> can be included directly."
#endif

#ifndef __HWP_FILE_H__
#define __HWP_FILE_H__

#include <glib-object.h>
#include <gio/gio.h>
#include <gsf/gsf-infile-msole.h>

#include "hwp-document.h"

G_BEGIN_DECLS

#define HWP_TYPE_FILE             (hwp_file_get_type ())
#define HWP_FILE(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), HWP_TYPE_FILE, HwpFile))
#define HWP_FILE_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), HWP_TYPE_FILE, HwpFileClass))
#define HWP_IS_FILE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HWP_TYPE_FILE))
#define HWP_IS_FILE_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), HWP_TYPE_FILE))
#define HWP_FILE_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), HWP_TYPE_FILE, HwpFileClass))

typedef struct _HwpFile      HwpFile;
typedef struct _HwpFileClass HwpFileClass;

struct _HwpFile
{
  GObject parent_instance;
};

struct _HwpFileClass
{
  GObjectClass      parent_class;

  HwpDocument *  (* get_document)           (HwpFile *file, GError **error);
  gchar *        (* get_hwp_version_string) (HwpFile *file);
  void           (* get_hwp_version)        (HwpFile *file,
                                             guint8  *major_version,
                                             guint8  *minor_version,
                                             guint8  *micro_version,
                                             guint8  *extra_version);
};

/**
 * HWP_FILE_ERROR:
 *
 * Error domain for #HwpFile. Errors in this domain will be from
 * the #HwpFileError enumeration.
 * See #GError for more information on error domains.
 *
 * Since: 0.0.1
 */
#define HWP_FILE_ERROR          (hwp_file_error_quark ())

/**
 * HwpFileError:
 * @HWP_FILE_ERROR_FAILED: failed for unspecified reason
 * @HWP_FILE_ERROR_INVALID: The HWP is invalid.
 *
 * Error codes returned by #HwpFile functions.
 */
typedef enum
{
  HWP_FILE_ERROR_FAILED,
  HWP_FILE_ERROR_INVALID
} HwpFileError;

GType        hwp_file_get_type               (void) G_GNUC_CONST;
GQuark       hwp_file_error_quark            (void) G_GNUC_CONST;
HwpFile     *hwp_file_new_for_path           (const gchar *path,
                                              GError     **error);
HwpDocument *hwp_file_get_document           (HwpFile     *file,
                                              GError     **error);
gchar       *hwp_file_get_hwp_version_string (HwpFile     *file);
void         hwp_file_get_hwp_version        (HwpFile     *file,
                                              guint8      *major_version,
                                              guint8      *minor_version,
                                              guint8      *micro_version,
                                              guint8      *extra_version);

G_END_DECLS

#endif /* __HWP_FILE_H__ */
