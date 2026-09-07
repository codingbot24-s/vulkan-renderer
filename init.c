//
// Created by saad on 9/6/26.
//

#include "window.h"
#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

static const char *validation_layers[] = {
    "VK_LAYER_KHRONOS_validation",
};
static const uint32_t validation_layer_count = 1;

#ifdef NDEBUG
static const bool enable_validation_layers = false;
#else
static const bool enable_validation_layers = true;
#endif

/// required_instance_extension_count by glfw
static uint32_t required_instance_extension_count = 0;
static const char **required_extensions;

bool check_required_instance_extension_support() {
  uint32_t instance_extension_count = 0;

  required_extensions =
      glfwGetRequiredInstanceExtensions(&required_instance_extension_count);
  if (required_instance_extension_count == 0 || required_extensions) {
    return false;
  }

  vkEnumerateInstanceExtensionProperties(NULL, &instance_extension_count, NULL);
  if (instance_extension_count == 0) {
    return false;
  }

  // MALLOC: free this malloc
  VkExtensionProperties *availabe_extension =
      malloc(instance_extension_count * sizeof(VkExtensionProperties));
  if (!availabe_extension) {
    return false;
  }

  vkEnumerateInstanceExtensionProperties(NULL, &instance_extension_count,
                                         availabe_extension);

  // TODO: add instance extension check
  return true;
}

bool check_validation_layer_support() {
  uint32_t layer_count;
  vkEnumerateInstanceLayerProperties(&layer_count, NULL);

  VkLayerProperties *available_layer =
      malloc(layer_count * sizeof(VkLayerProperties));
  if (available_layer == NULL) {
    fprintf(stderr, "cant allocate the buffer for availaber layer \n");
    return false;
  }
  vkEnumerateInstanceLayerProperties(&layer_count, available_layer);

  for (int i = 0; i < validation_layer_count; ++i) {
    bool layer_found = false;

    for (int j = 0; j < layer_count; ++j) {
      if (strcmp(validation_layers[i], available_layer[j].layerName) == 0) {
        layer_found = true;
        break;
      }
    }

    if (!layer_found) {
      free(available_layer);
      return false;
    }
  }

  free(available_layer);
  return true;
}

void init_vulkan() {

  if (enable_validation_layers && !check_validation_layer_support()) {
    exit(EXIT_FAILURE);
  }
  VkInstance instance;
  VkApplicationInfo app_info = {
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .pNext = NULL,
      .pApplicationName = "Renderer",
      .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
      .pEngineName = NULL,
      .engineVersion = 0,
      .apiVersion = VK_API_VERSION_1_0,
  };

  VkInstanceCreateInfo instance_creation_info = {
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pNext = NULL,
      .pApplicationInfo = &app_info,
      .enabledExtensionCount = glfw_extension_count,
      .ppEnabledExtensionNames = glfw_extensions,
      .enabledLayerCount = 0,
  };
  if (enable_validation_layers) {
    instance_creation_info.enabledLayerCount = validation_layer_count;
    instance_creation_info.ppEnabledLayerNames = validation_layers;
  } else {
    instance_creation_info.enabledLayerCount = 0;
  }

  if (vkCreateInstance(&instance_creation_info, NULL, &instance) !=
      VK_SUCCESS) {
    fprintf(stderr, "Cant create the vulkan instance \n");
    exit(EXIT_FAILURE);
  }

  printf("instance created  \n");
}

void run_app() {

  GLFWwindow *win = create_window();
  if (win == NULL) {
    exit(EXIT_FAILURE);
  }
  init_vulkan();
  main_loop(win);
  clean_up(win);
}
