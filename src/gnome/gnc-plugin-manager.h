/* 
 * gnc-plugin-manager.h -- Manage gnucash plugins.
 *
 * Copyright (C) 2003 Jan Arne Petersen
 * Author: Jan Arne Petersen <jpetersen@uni-bonn.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, contact:
 *
 * Free Software Foundation           Voice:  +1-617-542-5942
 * 59 Temple Place - Suite 330        Fax:    +1-617-542-2652
 * Boston, MA  02111-1307,  USA       gnu@gnu.org
 */

#ifndef __GNC_PLUGIN_MANAGER_H
#define __GNC_PLUGIN_MANAGER_H

#include "gnc-plugin.h"

G_BEGIN_DECLS

/* type macros */
#define GNC_TYPE_PLUGIN_MANAGER            (gnc_plugin_manager_get_type ())
#define GNC_PLUGIN_MANAGER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GNC_TYPE_PLUGIN_MANAGER, GncPluginManager))
#define GNC_PLUGIN_MANAGER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GNC_TYPE_PLUGIN_MANAGER, GncPluginManagerClass))
#define GNC_IS_PLUGIN_MANAGER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GNC_TYPE_PLUGIN_MANAGER))
#define GNC_IS_PLUGIN_MANAGER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GNC_TYPE_PLUGIN_MANAGER))
#define GNC_PLUGIN_MANAGER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GNC_TYPE_PLUGIN_MANAGER, GncPluginManagerClass))

/* typedefs & structures */
typedef struct GncPluginManagerPrivate GncPluginManagerPrivate;

typedef struct {
	GObject parent;

	GncPluginManagerPrivate *priv;
} GncPluginManager;

typedef struct {
	GObjectClass parent;

	/* Signals */
	void (* plugin_added) (GncPluginManager *plugin_manager, GncPlugin *plugin);
	void (* plugin_removed) (GncPluginManager *plugin_manager, GncPlugin *plugin);
} GncPluginManagerClass;

/* function prototypes */
GType             gnc_plugin_manager_get_type      (void);

GncPluginManager *gnc_plugin_manager_get           (void);

void              gnc_plugin_manager_add_plugin    (GncPluginManager *manager,
						    GncPlugin *plugin);
void		  gnc_plugin_manager_remove_plugin (GncPluginManager *manager,
						    GncPlugin *plugin);

GList            *gnc_plugin_manager_get_plugins   (GncPluginManager *manager);
GncPlugin        *gnc_plugin_manager_get_plugin    (GncPluginManager *manager,
						    const gchar *name);

G_END_DECLS

#endif /* __GNC_PLUGIN_MANAGER_H */
