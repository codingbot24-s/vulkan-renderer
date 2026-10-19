//
// Created by saad on 15/09/26.
//

#include "libs/cglm/include/cglm/affine.h"
#include "renderer.h"
#include <GLFW/glfw3.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/*
 *
 *  reading the shader file function
 *
 */

void read_file(const char *path, uint32_t **out_buffer, size_t *buffer_size) {
  FILE *file = fopen(path, "rb");
  if (file == NULL) {
    fprintf(stderr, "Cant open the file \n");
    return;
  }

  fseek(file, 0, SEEK_END);
  size_t f_size = ftell(file);
  rewind(file);

  *out_buffer = malloc(f_size);

  if (!out_buffer) {

    fprintf(stderr, "cant allocate the buffer for reading spirv \n");
    return;
  }

  size_t bytes_readed = fread(*out_buffer, 1, f_size, file);

  if (bytes_readed < f_size) {
    fclose(file);
    fprintf(stderr, "readed bytes not matched \n");
    return;
  }

  *buffer_size = f_size;
}

/*
 *
 *  keyboard input function
 *
 */

void handle_input(int key, int action, renderer *renderer) {

  if (action != GLFW_PRESS && action == GLFW_REPEAT) {
    return;
  }

  camera *cam = renderer->camera;

  vec3 forward;
  glm_vec3_sub(cam->target, cam->pos, forward);
  forward[2] = 0.0f;

  if (glm_vec3_norm(forward) > 0.0001f) {
    glm_vec3_normalize(forward);
  }

  vec3 right = {forward[1], -forward[0], 0.0f};

  const float speed = 0.1f;
  vec3 movement = {0.0f, 0.0f, 0.0f};

  if (key == GLFW_KEY_W) {
    glm_vec3_scale(forward, speed, movement);
  } else if (key == GLFW_KEY_S) {
    glm_vec3_scale(forward, -speed, movement);
  } else if (key == GLFW_KEY_A) {
    glm_vec3_scale(right, -speed, movement);
  } else if (key == GLFW_KEY_D) {
    glm_vec3_scale(right, speed, movement);
  } else {
    return;
  }

  glm_vec3_add(cam->pos, movement, cam->pos);
  glm_vec3_add(cam->target, movement, cam->target);
}
