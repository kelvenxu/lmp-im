/* vi: set sw=4 ts=4: */
/*
 * lmp-im-object.h
 *
 * This file is part of lmp-im.
 *
 * Copyright (C) 2010 - kelvenxu <kelvenxu@gmail.com>.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 * */

#ifndef __LMP_IM_OBJECT_H__
#define __LMP_IM_OBJECT_H__  1

#include <glib-object.h>
#include <gtk/gtk.h>
#include <gtk/gtkimmodule.h>

G_BEGIN_DECLS

#define LMP_IM_TYPE_OBJECT (lmp_im_object_get_type())
#define LMP_IM_OBJECT(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), LMP_IM_TYPE_OBJECT, LmpIMObject))
#define LMP_IM_OBJECT_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), LMP_IM_TYPE_OBJECT, LmpIMObjectClass))
#define LMP_IM_IS_OBJECT(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), LMP_IM_TYPE_OBJECT))
#define LMP_IM_IS_OBJECT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), LMP_IM_TYPE_OBJECT))
#define LMP_IM_OBJECT_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), LMP_IM_TYPE_OBJECT, LmpIMObjectClass))

typedef struct _LmpIMObjectPrivate LmpIMObjectPrivate;

typedef struct 
{
	GtkIMContext parent;

	LmpIMObjectPrivate *priv;
} LmpIMObject;

typedef struct
{
	GtkIMContextClass parent;
} LmpIMObjectClass;

GType lmp_im_object_get_type();
void lmp_im_object_register_type(GTypeModule *type_module);

G_END_DECLS
#endif /*__LMP_IM_OBJECT_H__ */
