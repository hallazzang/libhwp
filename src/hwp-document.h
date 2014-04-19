/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * hwp-document.h
 *
 * Copyright (C) 2012-2014 Hodong Kim <hodong@cogno.org>
 * 
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * This software have been developed with reference to
 * the HWP file format open specification by Hancom, Inc.
 * http://www.hancom.co.kr/userofficedata.userofficedataList.do?menuFlag=3
 * 한글과컴퓨터의 한/글 문서 파일(.hwp) 공개 문서를 참고하여 개발하였습니다.
 */

#if !defined (__HWP_H_INSIDE__) && !defined (HWP_COMPILATION)
#error "Only <hwp/hwp.h> can be included directly."
#endif

#ifndef __HWP_DOCUMENT_H__
#define __HWP_DOCUMENT_H__

#include <glib-object.h>
#include <gsf/gsf-doc-meta-data.h>

#include "hwp-page.h"
#include "hwp-models.h"
#include <poppler.h>

G_BEGIN_DECLS

#define HWP_TYPE_DOCUMENT             (hwp_document_get_type ())
#define HWP_DOCUMENT(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), HWP_TYPE_DOCUMENT, HwpDocument))
#define HWP_DOCUMENT_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), HWP_TYPE_DOCUMENT, HwpDocumentClass))
#define HWP_IS_DOCUMENT(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HWP_TYPE_DOCUMENT))
#define HWP_IS_DOCUMENT_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), HWP_TYPE_DOCUMENT))
#define HWP_DOCUMENT_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), HWP_TYPE_DOCUMENT, HwpDocumentClass))

typedef struct _HwpDocument      HwpDocument;
typedef struct _HwpDocumentClass HwpDocumentClass;

struct _HwpDocument
{
  GObject      parent_instance;

  GPtrArray   *char_shapes;
  GPtrArray   *face_names;

  GPtrArray   *paragraphs;
  GPtrArray   *pages;
  const gchar *prv_text;
  /* hwp version */
  guint8       major_version;
  guint8       minor_version;
  guint8       micro_version;
  guint8       extra_version;

  HwpSummaryInfo *info;

  GByteArray      *pdf_data;
  PopplerDocument *poppler_document;
};

struct _HwpDocumentClass
{
  GObjectClass parent_class;
};

GType        hwp_document_get_type               (void) G_GNUC_CONST;
void         hwp_document_add_char_shape         (HwpDocument  *document,
                                                  HwpCharShape *char_shape);
void         hwp_document_add_face_name          (HwpDocument  *document,
                                                  HwpFaceName  *face_name);
void         hwp_document_add_paragraph          (HwpDocument  *document,
                                                  HwpParagraph *paragraph);
GTime        hwp_document_get_creation_date      (HwpDocument  *document);
gchar       *hwp_document_get_creator            (HwpDocument  *document);
gchar       *hwp_document_get_format             (HwpDocument  *document);
void         hwp_document_get_hwp_version        (HwpDocument  *document,
                                                  guint8       *major_version,
                                                  guint8       *minor_version,
                                                  guint8       *micro_version,
                                                  guint8       *extra_version);
gchar       *hwp_document_get_hwp_version_string (HwpDocument  *document);
gchar       *hwp_document_get_keywords           (HwpDocument  *document);
GTime        hwp_document_get_modification_date  (HwpDocument  *document);
guint        hwp_document_get_n_pages            (HwpDocument  *document);
HwpPage     *hwp_document_get_page               (HwpDocument  *document,
                                                  gint          n_page);
gchar       *hwp_document_get_subject            (HwpDocument  *document);
gchar       *hwp_document_get_title              (HwpDocument  *document);
HwpDocument *hwp_document_new                    (void);
HwpDocument *hwp_document_new_from_file          (const gchar  *filename,
                                                  GError      **error);

G_END_DECLS

#endif /* __HWP_DOCUMENT_H__ */
