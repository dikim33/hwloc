/*
 * Copyright © 2009-2012 Inria.  All rights reserved.
 * See COPYING in top-level directory.
 */

#ifndef PRIVATE_XML_H
#define PRIVATE_XML_H 1

#include <hwloc.h>
#include <private/private.h>

#include <sys/types.h>

HWLOC_DECLSPEC int hwloc__xml_verbose(void);

typedef struct hwloc__xml_import_state_s {
  struct hwloc__xml_import_state_s *parent;

  int (*next_attr)(struct hwloc__xml_import_state_s * state, char **namep, char **valuep);
  int (*find_child)(struct hwloc__xml_import_state_s * state, struct hwloc__xml_import_state_s * childstate, char **tagp);
  int (*close_tag)(struct hwloc__xml_import_state_s * state); /* look for an explicit closing tag </name> */
  void (*close_child)(struct hwloc__xml_import_state_s * state);
  int (*get_content)(struct hwloc__xml_import_state_s * state, char **beginp, size_t expected_length);

  /* opaque data used to store backend-specific data.
   * statically allocated to allow stack-allocation by the common code without knowing actual backend needs.
   */
  char data[32];
} * hwloc__xml_import_state_t;

struct hwloc_xml_backend_data_s {
  /* xml backend parameters */
  int (*look)(struct hwloc_topology *topology, struct hwloc__xml_import_state_s *state);
  void (*look_failed)(struct hwloc_topology *topology);
  void (*backend_exit)(struct hwloc_topology *topology);
  void *data; /* libxml2 doc, or nolibxml buffer */
  struct hwloc_xml_imported_distances_s {
    hwloc_obj_t root;
    struct hwloc_distances_s distances;
    struct hwloc_xml_imported_distances_s *prev, *next;
  } *first_distances, *last_distances;
};

typedef struct hwloc__xml_export_output_s {
  void (*new_child)(struct hwloc__xml_export_output_s *output, const char *name);
  void (*new_prop)(struct hwloc__xml_export_output_s *output, const char *name, const char *value);
  void (*end_props)(struct hwloc__xml_export_output_s *output, unsigned nr_children, int has_content);
  void (*add_content)(struct hwloc__xml_export_output_s *output, const char *buffer, size_t length);
  void (*end_object)(struct hwloc__xml_export_output_s *output, const char *name, unsigned nr_children, int has_content);

  /* allocated by the backend (a single output is needed for the entire export) */
  void *data;
} * hwloc__xml_export_output_t;

HWLOC_DECLSPEC void hwloc__xml_export_object (hwloc__xml_export_output_t output, struct hwloc_topology *topology, struct hwloc_obj *obj);

struct hwloc_xml_callbacks {
  int (*backend_init)(struct hwloc_topology *topology, struct hwloc_backend *backend, const char *xmlpath, const char *xmlbuffer, int xmlbuflen);
  int (*export_file)(struct hwloc_topology *topology, const char *filename);
  int (*export_buffer)(struct hwloc_topology *topology, char **xmlbuffer, int *buflen);
  void (*free_buffer)(void *xmlbuffer);
};

HWLOC_DECLSPEC void hwloc_xml_callbacks_register(struct hwloc_xml_callbacks *nolibxml, struct hwloc_xml_callbacks *libxml);
HWLOC_DECLSPEC void hwloc_xml_callbacks_reset(void);

#endif /* PRIVATE_XML_H */
