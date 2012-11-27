/*
 * ghwp.vala
 *
 * Copyright (C) 2012  Hodong Kim <cogniti@gmail.com>
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
 * 한글과컴퓨터의 한/글 문서 파일(.hwp) 공개 문서를 참고하여 개발하였습니다.
 */

namespace GHWP.Tag
{
    const uint16 BEGIN                = 16; // 0x010
    // docinfo
    const uint16 DOCUMENT_PROPERTIES  = 16;
    const uint16 ID_MAPPINGS          = 17;
    const uint16 BIN_DATA             = 18;
    const uint16 FACE_NAME            = 19;
    const uint16 BORDER_FILL          = 20;
    const uint16 CHAR_SHAPE           = 21;
    const uint16 TAB_DEF              = 22;
    const uint16 NUMBERING            = 23;
    const uint16 BULLET               = 24;
    const uint16 PARA_SHAPE           = 25;
    const uint16 STYLE                = 26;
    const uint16 DOC_DATA             = 27;
    const uint16 DISTRIBUTE_DOC_DATA  = 28;
    // 상수명 충돌을 피하기 위해 _29
    const uint16 RESERVED_29          = 29;
    const uint16 COMPATIBLE_DOCUMENT  = 30;
    const uint16 LAYOUT_COMPATIBILITY = 31;
    const uint16 DOC_INFO_32          = 32; // 레이아웃 관련 태그로 추정됨.
    const uint16 FORBIDDEN_CHAR       = 94;
    // section
    const uint16 PARA_HEADER               =  66;
    const uint16 PARA_TEXT                 =  67;
    const uint16 PARA_CHAR_SHAPE           =  68;
    const uint16 PARA_LINE_SEG             =  69;
    const uint16 PARA_RANGE_TAG            =  70;
    const uint16 CTRL_HEADER               =  71;
    const uint16 LIST_HEADER               =  72;
    const uint16 PAGE_DEF                  =  73;
    const uint16 FOOTNOTE_SHAPE            =  74;
    const uint16 PAGE_BORDER_FILL          =  75;
    const uint16 SHAPE_COMPONENT           =  76;
    const uint16 TABLE                     =  77;
    const uint16 SHAPE_COMPONENT_LINE      =  78;
    const uint16 SHAPE_COMPONENT_RECTANGLE =  79;
    const uint16 SHAPE_COMPONENT_ELLIPSE   =  80;
    const uint16 SHAPE_COMPONENT_ARC       =  81;
    const uint16 SHAPE_COMPONENT_POLYGON   =  82;
    const uint16 SHAPE_COMPONENT_CURVE     =  83;
    const uint16 SHAPE_COMPONENT_OLE       =  84;
    const uint16 SHAPE_COMPONENT_PICTURE   =  85;
    const uint16 SHAPE_COMPONENT_CONTAINER =  86;
    const uint16 CTRL_DATA                 =  87;
    const uint16 EQEDIT                    =  88;
    const uint16 RESERVED_89               =  89;
    const uint16 SHAPE_COMPONENT_TEXTART   =  90;
    const uint16 FORM_OBJECT               =  91;
    const uint16 MEMO_SHAPE                =  92;
    const uint16 MEMO_LIST                 =  93;
    const uint16 CHART_DATA                =  95;
    const uint16 SHAPE_COMPONENT_UNKNOWN   = 115;

    const string[] NAMES = {
        "0", "1",  "2",  "3",  "4",  "5",  "6",  "7",
        "8", "9", "10", "11", "12", "13", "14", "15",
        "DOCUMENT_PROPERTIES",  // 16
        "ID_MAPPINGS",          // 17
        "BIN_DATA",             // 18
        "FACE_NAME",            // 19
        "BORDER_FILL",          // 20
        "CHAR_SHAPE",           // 21
        "TAB_DEF",              // 22
        "NUMBERING",            // 23
        "BULLET",               // 24
        "PARA_SHAPE",           // 25
        "STYLE",                // 26
        "DOC_DATA",             // 27
        "DISTRIBUTE_DOC_DATA",  // 28
        "RESERVED_29",          // 29
        "COMPATIBLE_DOCUMENT",  // 30
        "LAYOUT_COMPATIBILITY", // 31
        "DOC_INFO_32",          // 32
        "33", "34", "35", "36", "37", "38", "39", "40", "41", "42", "43",
        "44", "45", "46", "47", "48", "49", "50", "51", "52", "53", "54",
        "55", "56", "57", "58", "59", "60", "61", "62", "63", "64", "65",
        "PARA_HEADER",               //  66
        "PARA_TEXT",                 //  67
        "PARA_CHAR_SHAPE",           //  68
        "PARA_LINE_SEG",             //  69
        "PARA_RANGE_TAG",            //  70
        "CTRL_HEADER",               //  71
        "LIST_HEADER",               //  72
        "PAGE_DEF",                  //  73
        "FOOTNOTE_SHAPE",            //  74
        "PAGE_BORDER_FILL",          //  75
        "SHAPE_COMPONENT",           //  76
        "TABLE",                     //  77
        "SHAPE_COMPONENT_LINE",      //  78
        "SHAPE_COMPONENT_RECTANGLE", //  79
        "SHAPE_COMPONENT_ELLIPSE",   //  80
        "SHAPE_COMPONENT_ARC",       //  81
        "SHAPE_COMPONENT_POLYGON",   //  82
        "SHAPE_COMPONENT_CURVE",     //  83
        "SHAPE_COMPONENT_OLE",       //  84
        "SHAPE_COMPONENT_PICTURE",   //  85
        "SHAPE_COMPONENT_CONTAINER", //  86
        "CTRL_DATA",                 //  87
        "EQEDIT",                    //  88
        "RESERVED_89",               //  89
        "SHAPE_COMPONENT_TEXTART",   //  90
        "FORM_OBJECT",               //  91
        "MEMO_SHAPE",                //  92
        "MEMO_LIST",                 //  93
        "FORBIDDEN_CHAR",            //  94
         "95",  "96",  "97",  "98",  "99", "100", "101", "102", "103", "104",
        "105", "106", "107", "108", "109", "110", "111", "112", "113", "114",
        "SHAPE_COMPONENT_UNKNOWN"    // 115
    };
}
