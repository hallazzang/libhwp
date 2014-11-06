/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * hwp-listenable.h
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

#if !defined (__HWP_H_INSIDE__) && !defined (HWP_COMPILATION)
#error "Only <hwp/hwp.h> can be included directly."
#endif

#ifndef __HWP_LISTENABLE_H__
#define __HWP_LISTENABLE_H__

#include <glib-object.h>

#include "hwp-enums.h"
#include "hwp-models.h"
#include "hwp-document.h"

G_BEGIN_DECLS

#define HWP_TYPE_LISTENABLE            (hwp_listenable_get_type ())
#define HWP_LISTENABLE(o)              (G_TYPE_CHECK_INSTANCE_CAST ((o), HWP_TYPE_LISTENABLE, HwpListenable))
#define HWP_IS_LISTENABLE(o)           (G_TYPE_CHECK_INSTANCE_TYPE ((o), HWP_TYPE_LISTENABLE))
#define HWP_LISTENABLE_GET_IFACE(inst) (G_TYPE_INSTANCE_GET_INTERFACE ((inst), HWP_TYPE_LISTENABLE, HwpListenableInterface))

/**
 * HwpListenable:
 *
 * A handle to an object implementing the #HwpListenableInterface interface.
 */
typedef struct _HwpListenable          HwpListenable; /* dummy typedef */
typedef struct _HwpListenableInterface HwpListenableInterface;

/**
 * HwpListenableInterface:
 * @base_iface: base interface
 * @document_version: Callback to invoke for document version
 * @face_name: Callback to invoke for #HwpFaceName
 * @char_shape: Callback to invoke for #HwpCharShape
 * @bin_data: Callback to invoke for #HwpBinData
 * @paragraph: Callback to invoke when #HwpParagraph instance has been built
 * @prv_text: Callback to invoke for prv text
 * @summary_info: Callback to invoke for #HwpSummaryInfo
 */
struct _HwpListenableInterface
{
  GTypeInterface             base_iface;

  /* file header */
  void (* document_version) (HwpListenable  *listenable,
                             guint8          major_version,
                             guint8          minor_version,
                             guint8          micro_version,
                             guint8          extra_version,
                             gpointer        user_data,
                             GError        **error);
  /* doc info */
  void (* face_name)        (HwpListenable  *listenable,
                             HwpFaceName    *face_name,
                             gpointer        user_data,
                             GError        **error);
  void (* char_shape)       (HwpListenable  *listenable,
                             HwpCharShape   *char_shape,
                             gpointer        user_data,
                             GError        **error);
  void (* bin_data)         (HwpListenable  *listenable,
                             HwpBinData     *bin_data,
                             gpointer        user_data,
                             GError        **error);
  /* paragraph */
  void (* paragraph)        (HwpListenable  *listenable,
                             HwpParagraph   *paragraph,
                             gpointer        user_data,
                             GError        **error);
  /* preview text */
  void (* prv_text)         (HwpListenable  *listenable,
                             gchar          *prv_text,
                             gpointer        user_data,
                             GError        **error);
  void (* summary_info)     (HwpListenable  *listenable,
                             HwpSummaryInfo *info,
                             gpointer        user_data,
                             GError        **error);
};

GType hwp_listenable_get_type  (void) G_GNUC_CONST;

G_END_DECLS

#endif /* __HWP_LISTENABLE_H__ */
