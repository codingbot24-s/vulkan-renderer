//
// Created by saad on 15/09/26.
//

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

char *read_file(const char *path, const char *out_buffer, size_t *buffer_size) {
  FILE *file = fopen(path, "rb");
  if (file == NULL) {
    fprintf(stderr, "Cant open the file \n");
    return NULL;
  }

  fseek(file, 0, SEEK_END);
  size_t f_size = ftell(file);
  rewind(file);

  size_t alloctaion_size = f_size * sizeof(char) + 1;
  /// NOTE:MALLOC
  char *buff = malloc(alloctaion_size);
  if (!buff) {
    fprintf(stderr, "Cant allocate the buffer \n");
    return NULL;
  }

  size_t bytes_read = fread(buff, sizeof(char), f_size, file);
  if (bytes_read < f_size) {
    free(buff);
    fprintf(stderr, "cant read the file \n");
    return NULL;
  }

  buff[bytes_read + 1] = '\0';
  out_buffer = buff;
  *buffer_size = alloctaion_size;
  fclose(file);
  return buff;
}
