/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * ghwp-models.c
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

#include "ghwp-models.h"

G_DEFINE_TYPE (GHWPParagraph, ghwp_paragraph, G_TYPE_OBJECT);
G_DEFINE_TYPE (GHWPTable,     ghwp_table, G_TYPE_OBJECT);
G_DEFINE_TYPE (TextSpan,      text_span,  G_TYPE_OBJECT);

#define _g_array_free0(var) ((var == NULL) ? NULL : (var = (g_array_free (var, TRUE), NULL)))
#define _g_free0(var) (var = (g_free (var), NULL))

void ghwp_paragraph_add_textspan (GHWPParagraph* self, TextSpan* textspan)
{
    g_return_if_fail (self != NULL);
    g_return_if_fail (textspan != NULL);
    textspan = g_object_ref (textspan);
    g_array_append_val (self->textspans, textspan);
}

GHWPParagraph* ghwp_paragraph_new (void)
{
    return (GHWPParagraph *) g_object_new (GHWP_TYPE_PARAGRAPH, NULL);
}

static void ghwp_paragraph_finalize (GObject* obj)
{
    GHWPParagraph *self = G_TYPE_CHECK_INSTANCE_CAST (obj, GHWP_TYPE_PARAGRAPH, GHWPParagraph);
    _g_array_free0 (self->textspans);
    G_OBJECT_CLASS (ghwp_paragraph_parent_class)->finalize (obj);
}

static void ghwp_paragraph_class_init (GHWPParagraphClass * klass)
{
    ghwp_paragraph_parent_class = g_type_class_peek_parent (klass);
    G_OBJECT_CLASS (klass)->finalize = ghwp_paragraph_finalize;
}

static void ghwp_paragraph_init (GHWPParagraph * self)
{
    self->textspans = g_array_new (TRUE, TRUE, sizeof (TextSpan*));
}

/*****************************************************************************/

GHWPTable *ghwp_table_new (void)
{
    /* The object has a reference count of 1 after g_object_new. */
    return (GHWPTable *) g_object_new (GHWP_TYPE_TABLE, NULL);
}

static void
ghwp_table_init (GHWPTable *ghwp_table)
{
}

static void
ghwp_table_finalize (GObject *object)
{
    G_OBJECT_CLASS (ghwp_table_parent_class)->finalize (object);
}

static void
ghwp_table_class_init (GHWPTableClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->finalize     = ghwp_table_finalize;
}

/*****************************************************************************/

TextSpan* text_span_new (const gchar* text)
{
    g_return_val_if_fail (text != NULL, NULL);
    TextSpan * self = (TextSpan*) g_object_new (TEXT_TYPE_SPAN, NULL);
    _g_free0 (self->text);
    self->text = g_strdup (text);
    return self;
}

static void text_span_finalize (GObject* obj)
{
    TextSpan * self;
    self = G_TYPE_CHECK_INSTANCE_CAST (obj, TEXT_TYPE_SPAN, TextSpan);
    _g_free0 (self->text);
    G_OBJECT_CLASS (text_span_parent_class)->finalize (obj);
}

static void text_span_class_init (TextSpanClass * klass) {
    text_span_parent_class = g_type_class_peek_parent (klass);
    G_OBJECT_CLASS (klass)->finalize = text_span_finalize;
}

static void text_span_init (TextSpan * self)
{
}
