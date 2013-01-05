/*
 * ghwp.c
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

#include "config.h"
#include "ghwp.h"

/**
 * ghwp_error_quark:
 *
 * Returns:  the #GQuark used to identify libghwp errors in #GError structures.
 *  Specific error codes come from the #GHWPError enumeration.
 **/
GQuark ghwp_error_quark (void)
{
    static GQuark q = 0;

    if (q == 0)
        q = g_quark_from_static_string ("ghwp-error-quark");

    return q;
}

static const char ghwp_version[] = PACKAGE_VERSION;

/**
 * ghwp_get_version:
 * 
 * Returns the version of ghwp in use.  This result is not to be freed.
 * 
 * Return value: the version of ghwp.
 **/
const char *
ghwp_get_version (void)
{
    return ghwp_version;
}

const char *
_ghwp_get_tag_name (guint tag_id)
{
    GEnumClass *tag_class = (GEnumClass *) g_type_class_ref (GHWP_TYPE_TAG);
    GEnumValue *tag       = g_enum_get_value (tag_class, tag_id);

    if (tag == NULL) {
        g_type_class_unref (tag_class);
        return g_strdup_printf ("unknown tag: %d", tag_id);
    }
    g_type_class_unref (tag_class);
    return tag->value_name;
}

GType
ghwp_error_get_type (void)
{
    static volatile gsize g_define_type_id__volatile = 0;

    if (g_once_init_enter (&g_define_type_id__volatile)) {
        static const GEnumValue values[] = {
            { GHWP_ERROR_INVALID,   "GHWP_ERROR_INVALID",   "invalid"   },
            { GHWP_ERROR_ENCRYPTED, "GHWP_ERROR_ENCRYPTED", "encrypted" },
            { GHWP_ERROR_OPEN_FILE, "GHWP_ERROR_OPEN_FILE", "open-file" },
            { GHWP_ERROR_DAMAGED,   "GHWP_ERROR_DAMAGED",   "damaged"   },
            { 0, NULL, NULL }
        };
        GType g_define_type_id = 
            g_enum_register_static (g_intern_static_string ("GHWPError"),
                                    values);

        g_once_init_leave (&g_define_type_id__volatile, g_define_type_id);
    }

    return g_define_type_id__volatile;
}

GType
ghwp_selection_style_get_type (void)
{
    static volatile gsize g_define_type_id__volatile = 0;

    if (g_once_init_enter (&g_define_type_id__volatile)) {
        static const GEnumValue values[] = {
            { GHWP_SELECTION_GLYPH, "GHWP_SELECTION_GLYPH", "glyph" },
            { GHWP_SELECTION_WORD,  "GHWP_SELECTION_WORD",  "word"  },
            { GHWP_SELECTION_LINE,  "GHWP_SELECTION_LINE",  "line"  },
            { 0, NULL, NULL }
        };
        GType g_define_type_id = 
            g_enum_register_static (g_intern_static_string ("GHWPSelectionStyle"),
                                    values);

        g_once_init_leave (&g_define_type_id__volatile, g_define_type_id);
    }

    return g_define_type_id__volatile;
}

/* by glib-mkenums - C language enum description generation utility */
GType
ghwp_tag_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;
 
  if (g_once_init_enter (&g_define_type_id__volatile)) {
    static const GEnumValue values[] = {
      { GHWP_TAG_DOCUMENT_PROPERTIES, "GHWP_TAG_DOCUMENT_PROPERTIES", "document-properties" },
      { GHWP_TAG_ID_MAPPINGS, "GHWP_TAG_ID_MAPPINGS", "id-mappings" },
      { GHWP_TAG_BIN_DATA, "GHWP_TAG_BIN_DATA", "bin-data" },
      { GHWP_TAG_FACE_NAME, "GHWP_TAG_FACE_NAME", "face-name" },
      { GHWP_TAG_BORDER_FILL, "GHWP_TAG_BORDER_FILL", "border-fill" },
      { GHWP_TAG_CHAR_SHAPE, "GHWP_TAG_CHAR_SHAPE", "char-shape" },
      { GHWP_TAG_TAB_DEF, "GHWP_TAG_TAB_DEF", "tab-def" },
      { GHWP_TAG_NUMBERING, "GHWP_TAG_NUMBERING", "numbering" },
      { GHWP_TAG_BULLET, "GHWP_TAG_BULLET", "bullet" },
      { GHWP_TAG_PARA_SHAPE, "GHWP_TAG_PARA_SHAPE", "para-shape" },
      { GHWP_TAG_STYLE, "GHWP_TAG_STYLE", "style" },
      { GHWP_TAG_DOC_DATA, "GHWP_TAG_DOC_DATA", "doc-data" },
      { GHWP_TAG_DISTRIBUTE_DOC_DATA, "GHWP_TAG_DISTRIBUTE_DOC_DATA", "distribute-doc-data" },
      { GHWP_TAG_RESERVED_29, "GHWP_TAG_RESERVED_29", "reserved-29" },
      { GHWP_TAG_COMPATIBLE_DOCUMENT, "GHWP_TAG_COMPATIBLE_DOCUMENT", "compatible-document" },
      { GHWP_TAG_LAYOUT_COMPATIBILITY, "GHWP_TAG_LAYOUT_COMPATIBILITY", "layout-compatibility" },
      { GHWP_TAG_DOC_INFO_32, "GHWP_TAG_DOC_INFO_32", "doc-info-32" },
      { GHWP_TAG_PARA_HEADER, "GHWP_TAG_PARA_HEADER", "para-header" },
      { GHWP_TAG_PARA_TEXT, "GHWP_TAG_PARA_TEXT", "para-text" },
      { GHWP_TAG_PARA_CHAR_SHAPE, "GHWP_TAG_PARA_CHAR_SHAPE", "para-char-shape" },
      { GHWP_TAG_PARA_LINE_SEG, "GHWP_TAG_PARA_LINE_SEG", "para-line-seg" },
      { GHWP_TAG_PARA_RANGE_TAG, "GHWP_TAG_PARA_RANGE_TAG", "para-range-tag" },
      { GHWP_TAG_CTRL_HEADER, "GHWP_TAG_CTRL_HEADER", "ctrl-header" },
      { GHWP_TAG_LIST_HEADER, "GHWP_TAG_LIST_HEADER", "list-header" },
      { GHWP_TAG_PAGE_DEF, "GHWP_TAG_PAGE_DEF", "page-def" },
      { GHWP_TAG_FOOTNOTE_SHAPE, "GHWP_TAG_FOOTNOTE_SHAPE", "footnote-shape" },
      { GHWP_TAG_PAGE_BORDER_FILL, "GHWP_TAG_PAGE_BORDER_FILL", "page-border-fill" },
      { GHWP_TAG_SHAPE_COMPONENT, "GHWP_TAG_SHAPE_COMPONENT", "shape-component" },
      { GHWP_TAG_TABLE, "GHWP_TAG_TABLE", "table" },
      { GHWP_TAG_SHAPE_COMPONENT_LINE, "GHWP_TAG_SHAPE_COMPONENT_LINE", "shape-component-line" },
      { GHWP_TAG_SHAPE_COMPONENT_RECTANGLE, "GHWP_TAG_SHAPE_COMPONENT_RECTANGLE", "shape-component-rectangle" },
      { GHWP_TAG_SHAPE_COMPONENT_ELLIPSE, "GHWP_TAG_SHAPE_COMPONENT_ELLIPSE", "shape-component-ellipse" },
      { GHWP_TAG_SHAPE_COMPONENT_ARC, "GHWP_TAG_SHAPE_COMPONENT_ARC", "shape-component-arc" },
      { GHWP_TAG_SHAPE_COMPONENT_POLYGON, "GHWP_TAG_SHAPE_COMPONENT_POLYGON", "shape-component-polygon" },
      { GHWP_TAG_SHAPE_COMPONENT_CURVE, "GHWP_TAG_SHAPE_COMPONENT_CURVE", "shape-component-curve" },
      { GHWP_TAG_SHAPE_COMPONENT_OLE, "GHWP_TAG_SHAPE_COMPONENT_OLE", "shape-component-ole" },
      { GHWP_TAG_SHAPE_COMPONENT_PICTURE, "GHWP_TAG_SHAPE_COMPONENT_PICTURE", "shape-component-picture" },
      { GHWP_TAG_SHAPE_COMPONENT_CONTAINER, "GHWP_TAG_SHAPE_COMPONENT_CONTAINER", "shape-component-container" },
      { GHWP_TAG_CTRL_DATA, "GHWP_TAG_CTRL_DATA", "ctrl-data" },
      { GHWP_TAG_EQEDIT, "GHWP_TAG_EQEDIT", "eqedit" },
      { GHWP_TAG_RESERVED_89, "GHWP_TAG_RESERVED_89", "reserved-89" },
      { GHWP_TAG_SHAPE_COMPONENT_TEXTART, "GHWP_TAG_SHAPE_COMPONENT_TEXTART", "shape-component-textart" },
      { GHWP_TAG_FORM_OBJECT, "GHWP_TAG_FORM_OBJECT", "form-object" },
      { GHWP_TAG_MEMO_SHAPE, "GHWP_TAG_MEMO_SHAPE", "memo-shape" },
      { GHWP_TAG_MEMO_LIST, "GHWP_TAG_MEMO_LIST", "memo-list" },
      { GHWP_TAG_FORBIDDEN_CHAR, "GHWP_TAG_FORBIDDEN_CHAR", "forbidden-char" },
      { GHWP_TAG_CHART_DATA, "GHWP_TAG_CHART_DATA", "chart-data" },
      { GHWP_TAG_SHAPE_COMPONENT_UNKNOWN, "GHWP_TAG_SHAPE_COMPONENT_UNKNOWN", "shape-component-unknown" },
      { 0, NULL, NULL }
    };
    GType g_define_type_id = 
       g_enum_register_static (g_intern_static_string ("GHWPTag"), values);
      
    g_once_init_leave (&g_define_type_id__volatile, g_define_type_id);
  }
    
  return g_define_type_id__volatile;
}
