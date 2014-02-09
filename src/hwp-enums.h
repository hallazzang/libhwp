/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * hwp-enums.h
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

/*
 * This software have been developed with reference to
 * the HWP file format open specification by Hancom, Inc.
 * http://www.hancom.co.kr/userofficedata.userofficedataList.do?menuFlag=3
 * 한글과컴퓨터의 한/글 문서 파일(.hwp) 공개 문서를 참고하여 개발하였습니다.
 */

#if !defined (__HWP_H_INSIDE__) && !defined (HWP_COMPILATION)
#error "Only <hwp/hwp.h> can be included directly."
#endif

#ifndef __HWP_ENUMS__
#define __HWP_ENUMS__

#define HWP_ERROR hwp_error_quark ()

G_BEGIN_DECLS

GQuark      hwp_error_quark   (void);
const char *hwp_get_version   (void);
const char *hwp_get_tag_name  (guint   tag_id);
const char *hwp_get_ctrl_name (guint32 ctrl_id);

G_END_DECLS

/**
 * HwpError:
 * @HWP_ERROR_INVALID: Generic error when a document opration fails
 * @HWP_ERROR_ENCRYPTED: Document is encrypted
 * @HWP_ERROR_OPEN_FILE: File could not be opened for writing when saving document
 * @HWP_ERROR_DAMAGED: Document is damaged
 *
 * Error codes returned by #HwpDocument
 *
 * Since: 0.2
 */
typedef enum
{
    HWP_ERROR_INVALID,
    HWP_ERROR_ENCRYPTED,
    HWP_ERROR_OPEN_FILE,
    HWP_ERROR_DAMAGED
} HwpError;

/**
 * HwpSelectionStyle:
 * @HWP_SELECTION_GLYPH: glyph is the minimum unit for selection
 * @HWP_SELECTION_WORD: word is the minimum unit for selection
 * @HWP_SELECTION_LINE: line is the minimum unit for selection
 *
 * Selection styles
 *
 * Since: 0.2
 */
typedef enum
{
    HWP_SELECTION_GLYPH,
    HWP_SELECTION_WORD,
    HWP_SELECTION_LINE
} HwpSelectionStyle;

/**
 * Since: 0.2
 */
#define HWP_TAG_BEGIN                    16
typedef enum
{
    HWP_TAG_DOCUMENT_PROPERTIES       =  16,
    HWP_TAG_ID_MAPPINGS               =  17,
    HWP_TAG_BIN_DATA                  =  18,
    HWP_TAG_FACE_NAME                 =  19,
    HWP_TAG_BORDER_FILL               =  20,
    HWP_TAG_CHAR_SHAPE                =  21,
    HWP_TAG_TAB_DEF                   =  22,
    HWP_TAG_NUMBERING                 =  23,
    HWP_TAG_BULLET                    =  24,
    HWP_TAG_PARA_SHAPE                =  25,
    HWP_TAG_STYLE                     =  26,
    HWP_TAG_DOC_DATA                  =  27,
    HWP_TAG_DISTRIBUTE_DOC_DATA       =  28,
    HWP_TAG_RESERVED_29               =  29,
    HWP_TAG_COMPATIBLE_DOCUMENT       =  30,
    HWP_TAG_LAYOUT_COMPATIBILITY      =  31,
    HWP_TAG_DOC_INFO_32               =  32,

    HWP_TAG_PARA_HEADER               =  66,
    HWP_TAG_PARA_TEXT                 =  67,
    HWP_TAG_PARA_CHAR_SHAPE           =  68,
    HWP_TAG_PARA_LINE_SEG             =  69,
    HWP_TAG_PARA_RANGE_TAG            =  70,
    HWP_TAG_CTRL_HEADER               =  71,
    HWP_TAG_LIST_HEADER               =  72,
    HWP_TAG_PAGE_DEF                  =  73,
    HWP_TAG_FOOTNOTE_SHAPE            =  74,
    HWP_TAG_PAGE_BORDER_FILL          =  75,
    HWP_TAG_SHAPE_COMPONENT           =  76,
    HWP_TAG_TABLE                     =  77,
    HWP_TAG_SHAPE_COMPONENT_LINE      =  78,
    HWP_TAG_SHAPE_COMPONENT_RECTANGLE =  79,
    HWP_TAG_SHAPE_COMPONENT_ELLIPSE   =  80,
    HWP_TAG_SHAPE_COMPONENT_ARC       =  81,
    HWP_TAG_SHAPE_COMPONENT_POLYGON   =  82,
    HWP_TAG_SHAPE_COMPONENT_CURVE     =  83,
    HWP_TAG_SHAPE_COMPONENT_OLE       =  84,
    HWP_TAG_SHAPE_COMPONENT_PICTURE   =  85,
    HWP_TAG_SHAPE_COMPONENT_CONTAINER =  86,
    HWP_TAG_CTRL_DATA                 =  87,
    HWP_TAG_EQEDIT                    =  88,
    HWP_TAG_RESERVED_89               =  89,
    HWP_TAG_SHAPE_COMPONENT_TEXTART   =  90,
    HWP_TAG_FORM_OBJECT               =  91,
    HWP_TAG_MEMO_SHAPE                =  92,
    HWP_TAG_MEMO_LIST                 =  93,
    HWP_TAG_FORBIDDEN_CHAR            =  94,
    HWP_TAG_CHART_DATA                =  95,

    HWP_TAG_SHAPE_COMPONENT_UNKNOWN   = 115
} HwpTag;

/*typedef enum {*/
/*    ID_BINARY_DATA      = 0,*/
/*    ID_KOREAN_FONTS     = 1,*/
/*    ID_ENGLISH_FONTS    = 2,*/
/*    ID_HANJA_FONTS      = 3,*/
/*    ID_JAPANESE_FONTS   = 4,*/
/*    ID_OTHERS_FONTS     = 5,*/
/*    ID_SYMBOL_FONTS     = 6,*/
/*    ID_USER_FONTS       = 7,*/
/*    ID_BORDER_FILLS     = 8,*/
/*    ID_CHAR_SHAPES      = 9,*/
/*    ID_TAB_DEFS         = 10,*/
/*    ID_PARA_NUMBERINGS  = 11,*/
/*    ID_BULLETS          = 12,*/
/*    ID_PARA_SHAPES      = 13,*/
/*    ID_STYLES           = 14,*/
    /*
     * 메모 모양(MemoShape)는 한/글2007부터 추가되었다.
     * 한/글2007 이전 문서는 data_len <= 60,
     * v5.0.0.6 : ID_MAPPINGS data_len: 60
     * v5.0.1.7 : ID_MAPPINGS data_len: 64
     * v5.0.2.4 : ID_MAPPINGS data_len: 64
     */
/*    ID_MEMO_SHAPES      = 15,*/
    /* 한/글2010 에서 추가된 것으로 추정됨 */
    /* v5.0.3.4 : ID_MAPPINGS data_len: 72 */
/*    ID_KNOWN_16         = 16,*/
/*    ID_KNOWN_17         = 17,*/
/*} IDMappingsID;*/

/**
 * MAKE_CTRL_ID:
 * @a: a #char value
 * @b: a #char value
 * @c: a #char value
 * @d: a #char value
 *
 * Makes @ctrl_id from #char values
 *
 * Returns: interpreted @ctrl_id
 *
 * Since: 0.2
 */
#define MAKE_CTRL_ID(a, b, c, d)      \
    (guint32)((((guint8)(a)) << 24) | \
              (((guint8)(b)) << 16) | \
              (((guint8)(c)) <<  8) | \
              (((guint8)(d)) <<  0))
/**
 * Since: TODO
 */
typedef enum
{
  CTRL_ID_SECTION_DEF          = MAKE_CTRL_ID('s', 'e', 'c', 'd'), /* 구역 */
  CTRL_ID_COLUMN_DEF           = MAKE_CTRL_ID('c', 'o', 'l', 'd'), /* 단 */
  CTRL_ID_HEADEDR              = MAKE_CTRL_ID('h', 'e', 'a', 'd'), /* 머리말 */
  CTRL_ID_FOOTER               = MAKE_CTRL_ID('f', 'o', 'o', 't'), /* 꼬리말 */
  CTRL_ID_FOOTNOTE             = MAKE_CTRL_ID('f', 'n', ' ', ' '), /* 각주 */
  CTRL_ID_ENDNOTE              = MAKE_CTRL_ID('e', 'n', ' ', ' '), /* 미주 */
  /* 자동 번호*/
  CTRL_ID_AUTO_NUM             = MAKE_CTRL_ID('a', 't', 'n', 'o'),
  /* 새 번호 지정 */
  CTRL_ID_NEW_NUM              = MAKE_CTRL_ID('n', 'w', 'n', 'o'),
  CTRL_ID_PAGE_HIDE            = MAKE_CTRL_ID('p', 'g', 'h', 'd'), /* 감추기 */
  /* 페이지 번호 제어(97의 홀수쪽에서 시작) */
  CTRL_ID_PAGE_NUM_CTRL        = MAKE_CTRL_ID('p', 'g', 'c', 't'),
  /* 쪽번호 위치 */
  CTRL_ID_PAGE_NUM_POS         = MAKE_CTRL_ID('p', 'g', 'n', 'p'),
  /* 찾아보기 표식 */
  CTRL_ID_INDEX_MARK           = MAKE_CTRL_ID('i', 'd', 'x', 'm'),
  CTRL_ID_BOKM                 = MAKE_CTRL_ID('b', 'o', 'k', 'm'), /* 책갈피 */
  /* 글자 겹침 */
  CTRL_ID_TCPS                 = MAKE_CTRL_ID('t', 'c', 'p', 's'),
  CTRL_ID_DUTMAL               = MAKE_CTRL_ID('t', 'd', 'u', 't'), /* 덧말 */
  /* 숨은 설명 */
  CTRL_ID_TCMT                 = MAKE_CTRL_ID('t', 'c', 'm', 't'),

  CTRL_ID_TABLE                = MAKE_CTRL_ID('t', 'b', 'l', ' '), /* 표 */
  CTRL_ID_LINE                 = MAKE_CTRL_ID('$', 'l', 'i', 'n'), /* 선 */
  CTRL_ID_RECT                 = MAKE_CTRL_ID('$', 'r', 'e', 'c'), /* 사각형 */
  CTRL_ID_ELL                  = MAKE_CTRL_ID('$', 'e', 'l', 'l'), /* 타원 */
  CTRL_ID_ARC                  = MAKE_CTRL_ID('$', 'a', 'r', 'c'), /* 호 */
  CTRL_ID_POLY                 = MAKE_CTRL_ID('$', 'p', 'o', 'l'), /* 다각형 */
  CTRL_ID_CURV                 = MAKE_CTRL_ID('$', 'c', 'u', 'r'), /* 곡선 */
  /* 한글97 수식 */
  CTRL_ID_EQEDID               = MAKE_CTRL_ID('e', 'q', 'e', 'd'),
  CTRL_ID_PIC                  = MAKE_CTRL_ID('$', 'p', 'i', 'c'), /* 그림 */
  CTRL_ID_OLE                  = MAKE_CTRL_ID('$', 'o', 'l', 'e'), /* OLE */
  /* 묶음 개체 */
  CTRL_ID_CON                  = MAKE_CTRL_ID('$', 'c', 'o', 'n'),

  CTRL_ID_DRAWING_SHAPE_OBJECT = MAKE_CTRL_ID('g', 's', 'o', ' '),

  FIELD_UNKNOWN                = MAKE_CTRL_ID('%', 'u', 'n', 'k'),
  /* 현재의 날짜/시간 필드 */
  FIELD_DATE                   = MAKE_CTRL_ID('%', 'd', 't', 'e'),
  /*파일 작성 날짜/시간 필드 */
  FIELD_DOCDATE                = MAKE_CTRL_ID('%', 'd', 'd', 't'),
  /* 문서 경로 필드 */
  FIELD_PATH                   = MAKE_CTRL_ID('%', 'p', 'a', 't'),
  /*블럭 책갈피 */
  FIELD_BOOKMARK               = MAKE_CTRL_ID('%', 'b', 'm', 'k'),
  /* 메일 머지 */
  FIELD_MAILMERGE              = MAKE_CTRL_ID('%', 'm', 'm', 'g'),
  /* 상호 참조 */
  FIELD_CROSSREF               = MAKE_CTRL_ID('%', 'x', 'r', 'f'),
  FIELD_FORMULA                = MAKE_CTRL_ID('%', 'f', 'm', 'u'), /* 계산식 */
  FIELD_CLICKHERE              = MAKE_CTRL_ID('%', 'c', 'l', 'k'), /* 누름틀 */
  /* 문서 요약 정보 필드 */
  FIELD_SUMMARY                 = MAKE_CTRL_ID('%', 's', 'm', 'r'),
  /* 사용자 정보 필드 */
  FIELD_USERINFO                = MAKE_CTRL_ID('%', 'u', 's', 'r'),
  /* 하이퍼링크 */
  FIELD_HYPERLINK              = MAKE_CTRL_ID('%', 'h', 'l', 'k'),
  FIELD_REVISION_SIGN          = MAKE_CTRL_ID('%', 's', 'i', 'g'),
  FIELD_REVISION_DELETE        = MAKE_CTRL_ID('%', '%', '*', 'd'),
  FIELD_REVISION_ATTACH        = MAKE_CTRL_ID('%', '%', '*', 'a'),
  FIELD_REVISION_CLIPPING      = MAKE_CTRL_ID('%', '%', '*', 'C'),
  FIELD_REVISION_SAWTOOTH      = MAKE_CTRL_ID('%', '%', '*', 'S'),
  FIELD_REVISION_THINKING      = MAKE_CTRL_ID('%', '%', '*', 'T'),
  FIELD_REVISION_PRAISE        = MAKE_CTRL_ID('%', '%', '*', 'P'),
  FIELD_REVISION_LINE          = MAKE_CTRL_ID('%', '%', '*', 'L'),
  FIELD_REVISION_SIMPLECHANGE  = MAKE_CTRL_ID('%', '%', '*', 'c'),
  FIELD_REVISION_HYPERLINK     = MAKE_CTRL_ID('%', '%', '*', 'h'),
  FIELD_REVISION_LINEATTACH    = MAKE_CTRL_ID('%', '%', '*', 'A'),
  FIELD_REVISION_LINELINK      = MAKE_CTRL_ID('%', '%', '*', 'i'),
  FIELD_REVISION_LINETRANSFER  = MAKE_CTRL_ID('%', '%', '*', 't'),
  FIELD_REVISION_RIGHTMOVE     = MAKE_CTRL_ID('%', '%', '*', 'r'),
  FIELD_REVISION_LEFTMOVE      = MAKE_CTRL_ID('%', '%', '*', 'l'),
  FIELD_REVISION_TRANSFER      = MAKE_CTRL_ID('%', '%', '*', 'n'),
  FIELD_REVISION_SIMPLEINSERT  = MAKE_CTRL_ID('%', '%', '*', 'e'),
  FIELD_REVISION_SPLIT         = MAKE_CTRL_ID('%', 's', 'p', 'l'),
  FIELD_REVISION_CHANGE        = MAKE_CTRL_ID('%', '%', 'm', 'r'),
  FIELD_MEMO                   = MAKE_CTRL_ID('%', '%', 'm', 'e'),
  FIELD_PRIVATE_INFO_SECURITY  = MAKE_CTRL_ID('%', 'c', 'p', 'r')
} HwpCtrlID;

#endif /* __HWP_ENUMS__ */
