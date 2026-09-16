//
// Created by saad on 15/09/26.
//

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

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
