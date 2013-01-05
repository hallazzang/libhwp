/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * ghwp-table.c
 *
 * Copyright (C) 2013  Hodong Kim <cogniti@gmail.com>
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

#ifndef __GHWP_MODELS_H__
#define __GHWP_MODELS_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define TEXT_TYPE_SPAN             (text_span_get_type ())
#define TEXT_SPAN(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), TEXT_TYPE_SPAN, TextSpan))
#define TEXT_SPAN_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), TEXT_TYPE_SPAN, TextSpanClass))
#define TEXT_IS_SPAN(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TEXT_TYPE_SPAN))
#define TEXT_IS_SPAN_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), TEXT_TYPE_SPAN))
#define TEXT_SPAN_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), TEXT_TYPE_SPAN, TextSpanClass))

typedef struct _TextSpan        TextSpan;
typedef struct _TextSpanClass   TextSpanClass;
typedef struct _TextSpanPrivate TextSpanPrivate;

struct _TextSpan {
    GObject          parent_instance;
    TextSpanPrivate *priv;
    gchar           *text;
};

struct _TextSpanClass {
    GObjectClass parent_class;
};

GType     text_span_get_type (void) G_GNUC_CONST;
TextSpan* text_span_new      (const gchar* text);

/*****************************************************************************/

#define GHWP_TYPE_PARAGRAPH             (ghwp_paragraph_get_type ())
#define GHWP_PARAGRAPH(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GHWP_TYPE_PARAGRAPH, GHWPParagraph))
#define GHWP_PARAGRAPH_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GHWP_TYPE_PARAGRAPH, GHWPParagraphClass))
#define TEXT_IS_P(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GHWP_TYPE_PARAGRAPH))
#define TEXT_IS_P_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GHWP_TYPE_PARAGRAPH))
#define GHWP_PARAGRAPH_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GHWP_TYPE_PARAGRAPH, GHWPParagraphClass))

typedef struct _GHWPParagraph        GHWPParagraph;
typedef struct _GHWPParagraphClass   GHWPParagraphClass;
typedef struct _GHWPParagraphPrivate GHWPParagraphPrivate;

struct _GHWPParagraph {
    GObject               parent_instance;
    GHWPParagraphPrivate *priv;
    GArray               *textspans;
};

struct _GHWPParagraphClass {
    GObjectClass parent_class;
};

GType          ghwp_paragraph_get_type     (void) G_GNUC_CONST;
void           ghwp_paragraph_add_textspan (GHWPParagraph *self,
                                            TextSpan      *textspan);
GHWPParagraph *ghwp_paragraph_new          (void);

/** GHWPTable ****************************************************************/

#define GHWP_TYPE_TABLE             (ghwp_table_get_type ())
#define GHWP_TABLE(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GHWP_TYPE_TABLE, GHWPTable))
#define GHWP_TABLE_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GHWP_TYPE_TABLE, GHWPTableClass))
#define GHWP_IS_TABLE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GHWP_TYPE_TABLE))
#define GHWP_IS_TABLE_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GHWP_TYPE_TABLE))
#define GHWP_TABLE_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GHWP_TYPE_TABLE, GHWPTableClass))

typedef struct _GHWPTable      GHWPTable;
typedef struct _GHWPTableClass GHWPTableClass;

struct _GHWPTableClass
{
    GObjectClass parent_class;
};

struct _GHWPTable
{
    GObject parent_instance;
};

GType      ghwp_table_get_type (void) G_GNUC_CONST;
GHWPTable *ghwp_table_new      (void);

G_END_DECLS

#endif /* __GHWP_MODELS_H__ */
