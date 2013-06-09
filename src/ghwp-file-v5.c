/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * ghwp-file-v5.c
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

#include "ghwp-file-v5.h"



G_DEFINE_TYPE (GHWPFileV5, ghwp_file_v5, G_TYPE_OBJECT);

static void
ghwp_file_v5_init (GHWPFileV5 *ghwp_file_v5)
{


	/* TODO: Add initialization code here */
}

static void
ghwp_file_v5_finalize (GObject *object)
{
	/* TODO: Add deinitalization code here */

	G_OBJECT_CLASS (ghwp_file_v5_parent_class)->finalize (object);
}

static void
ghwp_file_v5_class_init (GHWPFileV5Class *klass)
{
	GObjectClass* object_class = G_OBJECT_CLASS (klass);
	GObjectClass* parent_class = G_OBJECT_CLASS (klass);

	object_class->finalize = ghwp_file_v5_finalize;
}

