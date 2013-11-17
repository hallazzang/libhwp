/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * hwp-document.h
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

#ifndef __GHWP_DOCUMENT_H__
#define __GHWP_DOCUMENT_H__

#include <glib-object.h>
#include <gsf/gsf-doc-meta-data.h>

#include "hwp-page.h"

G_BEGIN_DECLS

#define GHWP_TYPE_DOCUMENT             (hwp_document_get_type ())
#define GHWP_DOCUMENT(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GHWP_TYPE_DOCUMENT, GHWPDocument))
#define GHWP_DOCUMENT_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GHWP_TYPE_DOCUMENT, GHWPDocumentClass))
#define GHWP_IS_DOCUMENT(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GHWP_TYPE_DOCUMENT))
#define GHWP_IS_DOCUMENT_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GHWP_TYPE_DOCUMENT))
#define GHWP_DOCUMENT_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GHWP_TYPE_DOCUMENT, GHWPDocumentClass))

typedef struct _GHWPDocument      GHWPDocument;
typedef struct _GHWPDocumentClass GHWPDocumentClass;

struct _GHWPDocument {
    GObject              parent_instance;
    gchar               *prv_text;
    GArray              *paragraphs;
    GArray              *pages;
    GsfDocMetaData      *summary_info;
    /* hwp version */
    guint8               major_version;
    guint8               minor_version;
    guint8               micro_version;
    guint8               extra_version;
    /* ev info */
    const gchar         *title;
    gchar               *format;
    const gchar         *author;
    const gchar         *subject;
    const gchar         *keywords;
    gchar               *layout;
    gchar               *start_mode;
    gchar               *permissions;
    gchar               *ui_hints;
    const gchar         *creator;
    gchar               *producer;
    GTime                creation_date;
    GTime                mod_date;
    gchar               *linearized;
    gchar               *security;
    gchar               *paper_size;
    gchar               *license;
    /* hwp info */
    const gchar         *desc;
    GTime                last_printed;
    const gchar         *last_saved_by;
    /* version of hanword */
    const gchar         *hanword_version;
};

struct _GHWPDocumentClass {
    GObjectClass parent_class;
};

GType         hwp_document_get_type           (void) G_GNUC_CONST;
GHWPDocument *hwp_document_new                (void);
GHWPDocument *hwp_document_new_from_uri       (const gchar  *uri,
                                                GError      **error);
GHWPDocument *hwp_document_new_from_filename  (const gchar  *filename,
                                                GError      **error);
guint         hwp_document_get_n_pages        (GHWPDocument *doc);
GHWPPage     *hwp_document_get_page           (GHWPDocument *doc, gint n_page);
/* meta data */
gchar    *hwp_document_get_title              (GHWPDocument *document);
gchar    *hwp_document_get_keywords           (GHWPDocument *document);
gchar    *hwp_document_get_subject            (GHWPDocument *document);
gchar    *hwp_document_get_creator            (GHWPDocument *document);
GTime     hwp_document_get_creation_date      (GHWPDocument *document);
GTime     hwp_document_get_modification_date  (GHWPDocument *document);
gchar    *hwp_document_get_format             (GHWPDocument *document);
gchar    *hwp_document_get_hwp_version_string (GHWPDocument *document);
void      hwp_document_get_hwp_version        (GHWPDocument *document,
                                                guint8       *major_version,
                                                guint8       *minor_version,
                                                guint8       *micro_version,
                                                guint8       *extra_version);

G_END_DECLS

#endif /* __GHWP_DOCUMENT_H__ */
