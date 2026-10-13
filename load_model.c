

#include "log/loger.h"
#include <stdint.h>
#define FAST_OBJ_IMPLEMENTATION
#include "./libs/fast_obj.h"
#include "renderer.h"
#include <stdio.h>
#include <stdlib.h>

void load_model(const char *filename, renderer *renderer) {

  fastObjMesh *mesh = fast_obj_read(filename);

  if (mesh == NULL) {
    fprintf(stderr, "Failed to load model: %s\n", filename);
    return;
  }

  /*
   *
        mesh->positions this is original vertices like x , y , z for 1 vertex
   and x , y , z for second vertex and it goes like this

   *
   *
   */

  /*
   this is how many positions (vertices we have)
  mesh->position_count;
   *
   *
   *
   */

  /// check if this mesh is triangles only we dont support yet things that
  /// are not triangles for now
  for (int i = 0; i < mesh->face_count; ++i) {
    if (mesh->face_vertices[i] > 3) {
      R_FATAL("Error we dont support mesh that with face count of > 3");
    }
  }

  /*
   *  a face is like this f 1/1/1 2/2/2 3/3/3
      1/1/1 this is one corner of the face
      mesh->indices[0] will give us this 1/1/1 and it goes on like
      mesh->inices[1 ] will give the second the second 2/2/2
      and mesh->index_count is how many of 1/1/1 and 2/2/2 3/3/3
      */
  uint32_t corner_count = mesh->index_count;
  // the vertex is not only x , y, z but is also the corner
  vertex *vertices = malloc(corner_count * sizeof(*vertices));
  uint32_t *indices = malloc(corner_count * sizeof(*indices));

  for (uint32_t i = 0; i < corner_count; ++i) {
    //// one corner
    fastObjIndex index = mesh->indices[i];

    vertices[i].pos[0] = mesh->positions[index.p * 3 + 0];
    vertices[i].pos[1] = mesh->positions[index.p * 3 + 1];
    vertices[i].pos[2] = mesh->positions[index.p * 3 + 2];

    vertices[i].uvs[0] = mesh->texcoords[index.t * 2 + 0];
    vertices[i].uvs[1] = 1.0f - mesh->texcoords[index.t * 2 + 1];

    vertices[i].color[0] = 1.0f;
    vertices[i].color[1] = 1.0f;
    vertices[i].color[2] = 1.0f;

    /// this is the index of the vertex that is currently generated from looping
    /// over the corner count
    indices[i] = i;
  }

  renderer->vertices = vertices;
  renderer->vertices_count = corner_count;
}
