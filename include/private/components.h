/*
 * Copyright © 2012 Inria.  All rights reserved.
 * See COPYING in top-level directory.
 */

#ifndef PRIVATE_COMPONENTS_H
#define PRIVATE_COMPONENTS_H 1

struct hwloc_backend;

/*******************
 * Core components *
 *******************/

/* Core components are components taking take of the discovery.
 * They are registered by generic components, either static or plugins.
 */

typedef enum hwloc_core_component_type_e {
  HWLOC_CORE_COMPONENT_TYPE_OS, /* OS backend, and no-OS support.
		       * that's where we take hooks from when is_this_system=1.
		       */
  HWLOC_CORE_COMPONENT_TYPE_GLOBAL, /* xml, synthetic or custom.
			   * no additional backend is used.
			   */
  HWLOC_CORE_COMPONENT_TYPE_ADDITIONAL, /* pci, etc.
			       */
  /* This value is only here so that we can end the enum list without
     a comma (thereby preventing compiler warnings) */
  HWLOC_CORE_COMPONENT_TYPE_MAX
} hwloc_core_component_type_t;

struct hwloc_core_component {
  hwloc_core_component_type_t type;
  const char *name;
  int (*instantiate)(struct hwloc_topology *topology, struct hwloc_core_component *component, const void *data1, const void *data2, const void *data3);
  void (*set_hooks)(struct hwloc_topology *topology); /* only used if HWLOC_COMPONENT_TYPE_OS */

  unsigned priority; /* used to sort topology->components and topology->additional_backends, higher priority first */
  struct hwloc_core_component * next; /* used internally to list components by priority on topology->components */
};

HWLOC_DECLSPEC int hwloc_core_component_register(struct hwloc_core_component *component);
extern struct hwloc_core_component * hwloc_core_component_find(int type, const char *name);
extern struct hwloc_core_component * hwloc_core_component_find_next(int type, const char *name, struct hwloc_core_component *prev);

/************
 * Backends *
 ************/

/* A backend is the instantiation of a core component.
 * When a component gets enabled for a topology,
 * its instantiate() callback creates a backend.
 */

struct hwloc_backend {
  struct hwloc_core_component * component;

  /* main discovery callback.
   * returns > 0 if it modified the topology tree, -1 on error, 0 otherwise.
   * maybe NULL if type is HWLOC_CORE_COMPONENT_TYPE_ADDITIONAL. */
  int (*discover)(struct hwloc_topology *topology);

  /* used by the libpci backend to retrieve pci device locality from the OS backend */
  int (*get_obj_cpuset)(struct hwloc_topology *topology, struct hwloc_obj *obj, hwloc_bitmap_t cpuset); /* may be NULL */

  /* used by additional backends to notify other backend when new objects are added.
   * returns > 0 if it modified the topology tree, 0 otherwise. */
  int (*notify_new_object)(struct hwloc_topology *topology, struct hwloc_obj *obj); /* may be NULL */

  void (*disable)(struct hwloc_topology *topology, struct hwloc_backend *backend); /* may be NULL */
  void * private_data;
  int is_custom; /* shortcut on !strcmp(..->component->name, "custom") */

  struct hwloc_backend * next; /* used internally to list additional backends by priority on topology->additional_backends.
				* unused (NULL) for other backends (on topology->backend).
				*/
};

HWLOC_DECLSPEC struct hwloc_backend * hwloc_backend_alloc(struct hwloc_topology *topology, struct hwloc_core_component *component);
HWLOC_DECLSPEC void hwloc_backend_enable(struct hwloc_topology *topology, struct hwloc_backend *backend);
extern void hwloc_backends_disable_all(struct hwloc_topology *topology);
HWLOC_DECLSPEC int hwloc_backends_notify_new_object(struct hwloc_topology *topology, struct hwloc_obj *obj);

/**********************
 * Generic components *
 **********************/

/* Generic components structure, either static listed by configure in static-components.h
 * or dynamically loaded as a plugin.
 */

extern void hwloc_components_init(struct hwloc_topology *topology); /* increases components refcount, should be called exactly once per topology (during init) */
extern void hwloc_components_destroy_all(struct hwloc_topology *topology); /* decreases components refcount, should be called exactly once per topology (during destroy) */

#define HWLOC_COMPONENT_ABI 1

typedef enum hwloc_component_type_e {
  HWLOC_COMPONENT_TYPE_CORE,	/* The data field must point to a struct hwloc_core_component. */
  HWLOC_COMPONENT_TYPE_XML,	/* The data field must point to a struct hwloc_xml_component. */
  HWLOC_COMPONENT_TYPE_MAX
} hwloc_component_type_t;

struct hwloc_component {
  unsigned abi;
  hwloc_component_type_t type;
  void * data;
};

/****************************************
 * Misc component registration routines *
 ****************************************/

#if defined(HWLOC_LINUX_SYS)
extern struct hwloc_component hwloc_core_linux_component;
#endif /* HWLOC_LINUX_SYS */

extern struct hwloc_component hwloc_core_xml_component;

#ifdef HWLOC_SOLARIS_SYS
extern struct hwloc_component hwloc_core_solaris_component;
#endif /* HWLOC_SOLARIS_SYS */

#ifdef HWLOC_AIX_SYS
extern struct hwloc_component hwloc_core_aix_component;
#endif /* HWLOC_AIX_SYS */

#ifdef HWLOC_OSF_SYS
extern struct hwloc_component hwloc_core_osf_component;
#endif /* HWLOC_OSF_SYS */

#ifdef HWLOC_WIN_SYS
extern struct hwloc_component hwloc_core_windows_component;
#endif /* HWLOC_WIN_SYS */

#ifdef HWLOC_DARWIN_SYS
extern struct hwloc_component hwloc_core_darwin_component;
#endif /* HWLOC_DARWIN_SYS */

#ifdef HWLOC_FREEBSD_SYS
extern struct hwloc_component hwloc_core_freebsd_component;
#endif /* HWLOC_FREEBSD_SYS */

#ifdef HWLOC_HPUX_SYS
extern struct hwloc_component hwloc_core_hpux_component;
#endif /* HWLOC_HPUX_SYS */

#ifdef HWLOC_HAVE_LIBPCI
extern struct hwloc_component hwloc_core_libpci_component;
#endif /* HWLOC_HAVE_LIBPCI */

extern struct hwloc_component hwloc_core_synthetic_component;

extern struct hwloc_component hwloc_core_noos_component;

extern struct hwloc_component hwloc_core_custom_component;


extern struct hwloc_component hwloc_xml_nolibxml_component;
#ifdef HWLOC_HAVE_LIBXML2
extern struct hwloc_component hwloc_xml_libxml_component;
#endif

#endif /* PRIVATE_COMPONENTS_H */

