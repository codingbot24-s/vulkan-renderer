//
// Created by saad on 23/09/26.
//

#ifndef VULKAN_RENDERER_LOGER_H
#define VULKAN_RENDERER_LOGER_H

typedef enum LOG_LEVEL {
  LEVEL_FATAL,
  LEVEL_WARN,
  LEVEL_INFO,
} LOG_LEVEL;

void logg(const char *message,LOG_LEVEL log_level, ...);
#define R_FATAL(message, ...) logg(message, LEVEL_FATAL, ##__VA_ARGS__)

#define R_WARN(message, ...) logg(message, LEVEL_WARN, ##__VA_ARGS__)

#define R_INFO(message, ...) logg(message, LEVEL_INFO, ##__VA_ARGS__)

#endif // VULKAN_RENDERER_LOGER_H
