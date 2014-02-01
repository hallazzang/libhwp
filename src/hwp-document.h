/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * hwp-document.h
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

#ifndef __HWP_DOCUMENT_H__
#define __HWP_DOCUMENT_H__

#include <glib-object.h>
#include <gsf/gsf-doc-meta-data.h>

#include "hwp-page.h"

G_BEGIN_DECLS

#define HWP_TYPE_DOCUMENT             (hwp_document_get_type ())
#define HWP_DOCUMENT(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), HWP_TYPE_DOCUMENT, HwpDocument))
#define HWP_DOCUMENT_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), HWP_TYPE_DOCUMENT, HwpDocumentClass))
#define HWP_IS_DOCUMENT(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HWP_TYPE_DOCUMENT))
#define HWP_IS_DOCUMENT_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), HWP_TYPE_DOCUMENT))
#define HWP_DOCUMENT_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), HWP_TYPE_DOCUMENT, HwpDocumentClass))

typedef struct _HwpDocument      HwpDocument;
typedef struct _HwpDocumentClass HwpDocumentClass;

struct _HwpDocument {
  GObject         parent_instance;
  gchar          *prv_text;
  GArray         *paragraphs;
  GArray         *pages;
  /* hwp version */
  guint8          major_version;
  guint8          minor_version;
  guint8          micro_version;
  guint8          extra_version;
  /* ev info */
  const gchar    *title;
  gchar          *format;
  const gchar    *author;
  const gchar    *subject;
  const gchar    *keywords;
  gchar          *layout;
  gchar          *start_mode;
  gchar          *permissions;
  gchar          *ui_hints;
  const gchar    *creator;
  gchar          *producer;
  GTime           creation_date;
  GTime           mod_date;
  gchar          *linearized;
  gchar          *security;
  gchar          *paper_size;
  gchar          *license;
  /* hwp info */
  const gchar    *desc;
  GTime           last_printed;
  const gchar    *last_saved_by;
  /* version of hanword */
  const gchar    *hanword_version;
};

struct _HwpDocumentClass {
  GObjectClass parent_class;
};

GType        hwp_document_get_type               (void) G_GNUC_CONST;
HwpDocument *hwp_document_new                    (void);
HwpDocument *hwp_document_new_from_file          (const gchar *filename,
                                                  GError     **error);
guint        hwp_document_get_n_pages            (HwpDocument *document);
HwpPage     *hwp_document_get_page               (HwpDocument *document, gint n_page);
/* meta data */
gchar       *hwp_document_get_title              (HwpDocument *document);
gchar       *hwp_document_get_keywords           (HwpDocument *document);
gchar       *hwp_document_get_subject            (HwpDocument *document);
gchar       *hwp_document_get_creator            (HwpDocument *document);
GTime        hwp_document_get_creation_date      (HwpDocument *document);
GTime        hwp_document_get_modification_date  (HwpDocument *document);
gchar       *hwp_document_get_format             (HwpDocument *document);
gchar       *hwp_document_get_hwp_version_string (HwpDocument *document);
void         hwp_document_get_hwp_version        (HwpDocument *document,
                                                  guint8      *major_version,
                                                  guint8      *minor_version,
                                                  guint8      *micro_version,
                                                  guint8      *extra_version);

G_END_DECLS

#endif /* __HWP_DOCUMENT_H__ */