//
// Created by saad on 23/09/26.
//

#ifndef VULKAN_RENDERER_LOGER_H
#define VULKAN_RENDERER_LOGER_H

typedef enum LOG_LEVEL {
  LEVEL_FATAL,
  LEVEL_WARN,
} LOG_LEVEL;

void logg(const char *message, LOG_LEVEL log_level);

#define R_WARN(message) logg(message, LEVEL_WARN)

#define R_FATAL(message) logg(message, LEVEL_FATAL)

#endif // VULKAN_RENDERER_LOGER_H
