/*
 * ghwp-enums.c
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

#include "config.h"
#include "ghwp.h"

/**
 * ghwp_error_quark:
 *
 * Returns:  the #GQuark used to identify libghwp errors in #GError structures.
 *  Specific error codes come from the #GHWPError enumeration.
 *
 * Since: 0.2
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
 *
 * Since: 0.2
 **/
const char *ghwp_get_version (void)
{
    return ghwp_version;
}

/**
 * Since: TODO
 **/
const char *ghwp_get_tag_name (guint tag_id)
{
    GEnumClass *enum_class = (GEnumClass *) g_type_class_ref (GHWP_TYPE_TAG);
    GEnumValue *tag        = g_enum_get_value (enum_class, tag_id);
    g_type_class_unref (enum_class);

    if (tag == NULL)
        return g_strdup_printf ("unknown tag id: %d", tag_id);

    return tag->value_name;
}

/**
 * Since: TODO
 **/
const char *ghwp_get_ctrl_name (guint32 ctrl_id)
{
    GEnumClass *enum_class = (GEnumClass *) g_type_class_ref (GHWP_TYPE_CTRL_ID);
    GEnumValue *ctrl       = g_enum_get_value (enum_class, ctrl_id);
    g_type_class_unref (enum_class);

    if (ctrl == NULL)
        return g_strdup_printf ("unknown ctrl id: %d", ctrl_id);

    return ctrl->value_name;
}
