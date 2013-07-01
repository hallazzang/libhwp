/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * ghwp-file-ml.h
 *
 * Copyright (C) 2013 Hodong Kim <cogniti@gmail.com>
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

#ifndef _GHWP_FILE_ML_H_
#define _GHWP_FILE_ML_H_

#include <glib-object.h>
#include "ghwp.h"

G_BEGIN_DECLS

#define GHWP_TYPE_FILE_ML             (ghwp_file_ml_get_type ())
#define GHWP_FILE_ML(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GHWP_TYPE_FILE_ML, GHWPFileML))
#define GHWP_FILE_ML_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GHWP_TYPE_FILE_ML, GHWPFileMLClass))
#define GHWP_IS_FILE_ML(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GHWP_TYPE_FILE_ML))
#define GHWP_IS_FILE_ML_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GHWP_TYPE_FILE_ML))
#define GHWP_FILE_ML_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GHWP_TYPE_FILE_ML, GHWPFileMLClass))

typedef struct _GHWPFileMLClass   GHWPFileMLClass;
typedef struct _GHWPFileMLPrivate GHWPFileMLPrivate;

struct _GHWPFileMLClass
{
    GHWPFileClass parent_class;
};

struct _GHWPFileML
{
    GHWPFile           parent_instance;
    GHWPFileMLPrivate *priv;
    GHWPPage          *page;
};

struct _GHWPFileMLPrivate
{
    gchar *uri;
};

GType         ghwp_file_ml_get_type               (void) G_GNUC_CONST;
GHWPFileML   *ghwp_file_ml_new_from_uri           (const gchar *uri,
                                                   GError     **error);
GHWPFileML   *ghwp_file_ml_new_from_filename      (const gchar *filename,
                                                   GError     **error);
gchar        *ghwp_file_ml_get_hwp_version_string (GHWPFile    *file);
void          ghwp_file_ml_get_hwp_version        (GHWPFile    *file,
                                                   guint8      *major_version,
                                                   guint8      *minor_version,
                                                   guint8      *micro_version,
                                                   guint8      *extra_version);
GHWPDocument *ghwp_file_ml_get_document           (GHWPFile    *file,
                                                   GError     **error);

G_END_DECLS

#endif /* _GHWP_FILE_ML_H_ */
