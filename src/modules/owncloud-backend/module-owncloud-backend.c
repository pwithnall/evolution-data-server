/*
 * module-owncloud-backend.c
 *
 * This library is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "evolution-data-server-config.h"

#include <libebackend/libebackend.h>
#include <libedataserver/libedataserver.h>

/* Standard GObject macros */
#define E_TYPE_OWNCLOUD_BACKEND \
	(e_owncloud_backend_get_type ())
#define E_OWNCLOUD_BACKEND(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST \
	((obj), E_TYPE_OWNCLOUD_BACKEND, EOwncloudBackend))
#define E_IS_OWNCLOUD_BACKEND(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE \
	((obj), E_TYPE_OWNCLOUD_BACKEND))

typedef struct _EOwncloudBackend EOwncloudBackend;
typedef struct _EOwncloudBackendClass EOwncloudBackendClass;

typedef struct _EOwncloudBackendFactory EOwncloudBackendFactory;
typedef struct _EOwncloudBackendFactoryClass EOwncloudBackendFactoryClass;

struct _EOwncloudBackend {
	EWebDAVCollectionBackend parent;
};

struct _EOwncloudBackendClass {
	EWebDAVCollectionBackendClass parent_class;
};

struct _EOwncloudBackendFactory {
	ECollectionBackendFactory parent;
};

struct _EOwncloudBackendFactoryClass {
	ECollectionBackendFactoryClass parent_class;
};

/* Module Entry Points */
void e_module_load (GTypeModule *type_module);
void e_module_unload (GTypeModule *type_module);

/* Forward Declarations */
GType e_owncloud_backend_get_type (void);
GType e_owncloud_backend_factory_get_type (void);

G_DEFINE_DYNAMIC_TYPE (
	EOwncloudBackend,
	e_owncloud_backend,
	E_TYPE_WEBDAV_COLLECTION_BACKEND)

G_DEFINE_DYNAMIC_TYPE (
	EOwncloudBackendFactory,
	e_owncloud_backend_factory,
	E_TYPE_COLLECTION_BACKEND_FACTORY)

static ESourceAuthenticationResult
owncloud_backend_authenticate_sync (EBackend *backend,
				    const ENamedParameters *credentials,
				    gchar **out_certificate_pem,
				    GTlsCertificateFlags *out_certificate_errors,
				    GCancellable *cancellable,
				    GError **error)
{
	ESourceGoa *goa_extension;
	ESource *source;

	g_return_val_if_fail (E_IS_COLLECTION_BACKEND (backend), E_SOURCE_AUTHENTICATION_ERROR);

	source = e_backend_get_source (backend);

	/* Ignore the request for non-GOA ownCloud sources by pretending success */
	if (!e_source_has_extension (source, E_SOURCE_EXTENSION_GOA))
		return E_SOURCE_AUTHENTICATION_ACCEPTED;

	goa_extension = e_source_get_extension (source, E_SOURCE_EXTENSION_GOA);

	return e_webdav_collection_backend_discover_sync (E_WEBDAV_COLLECTION_BACKEND (backend),
		e_source_goa_get_calendar_url (goa_extension),
		e_source_goa_get_contacts_url (goa_extension),
		credentials, out_certificate_pem, out_certificate_errors, cancellable, error);
}

static void
e_owncloud_backend_class_init (EOwncloudBackendClass *class)
{
	EBackendClass *backend_class;

	backend_class = E_BACKEND_CLASS (class);
	backend_class->authenticate_sync = owncloud_backend_authenticate_sync;
}

static void
e_owncloud_backend_class_finalize (EOwncloudBackendClass *class)
{
}

static void
e_owncloud_backend_init (EOwncloudBackend *backend)
{
}

static void
e_owncloud_backend_factory_class_init (EOwncloudBackendFactoryClass *class)
{
	ECollectionBackendFactoryClass *factory_class;

	factory_class = E_COLLECTION_BACKEND_FACTORY_CLASS (class);
	factory_class->factory_name = "owncloud";
	factory_class->backend_type = E_TYPE_OWNCLOUD_BACKEND;
}

static void
e_owncloud_backend_factory_class_finalize (EOwncloudBackendFactoryClass *class)
{
}

static void
e_owncloud_backend_factory_init (EOwncloudBackendFactory *factory)
{
}

G_MODULE_EXPORT void
e_module_load (GTypeModule *type_module)
{
	e_owncloud_backend_register_type (type_module);
	e_owncloud_backend_factory_register_type (type_module);
}

G_MODULE_EXPORT void
e_module_unload (GTypeModule *type_module)
{
}