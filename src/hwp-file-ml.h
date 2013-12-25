/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * hwp-file-ml.h
 *
 * Copyright (C) 2013 Hodong Kim <hodong@cogno.org>
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

#ifndef __HWP_FILE_ML_H__
#define __HWP_FILE_ML_H__

#include <glib-object.h>

#include "hwp-file.h"
#include "hwp-models.h"

G_BEGIN_DECLS

#define HWP_TYPE_FILE_ML             (hwp_file_ml_get_type ())
#define HWP_FILE_ML(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), HWP_TYPE_FILE_ML, HWPFileML))
#define HWP_FILE_ML_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), HWP_TYPE_FILE_ML, HWPFileMLClass))
#define HWP_IS_FILE_ML(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HWP_TYPE_FILE_ML))
#define HWP_IS_FILE_ML_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), HWP_TYPE_FILE_ML))
#define HWP_FILE_ML_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), HWP_TYPE_FILE_ML, HWPFileMLClass))

typedef struct _HWPFileML        HWPFileML;
typedef struct _HWPFileMLClass   HWPFileMLClass;
typedef struct _HWPFileMLPrivate HWPFileMLPrivate;

struct _HWPFileMLClass {
  HWPFileClass parent_class;
};

struct _HWPFileML {
  HWPFile           parent_instance;
  HWPFileMLPrivate *priv;

  HWPDocument      *document;
  HWPPage          *page;
};

struct _HWPFileMLPrivate {
  gchar *uri;
};

GType        hwp_file_ml_get_type               (void) G_GNUC_CONST;
HWPFileML   *hwp_file_ml_new_for_path           (const gchar *path,
                                                 GError     **error);
gchar       *hwp_file_ml_get_hwp_version_string (HWPFile     *file);
void         hwp_file_ml_get_hwp_version        (HWPFile     *file,
                                                 guint8      *major_version,
                                                 guint8      *minor_version,
                                                 guint8      *micro_version,
                                                 guint8      *extra_version);
HWPDocument *hwp_file_ml_get_document           (HWPFile     *file,
                                                 GError     **error);

G_END_DECLS

#endif /* __HWP_FILE_ML_H__ */