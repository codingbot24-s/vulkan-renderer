//
// Created by saad on 9/6/26.
//

#include "window.h"
#include <GLFW/glfw3.h>
#include <ctype.h>
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

static uint32_t required_instance_extension_count = 0;
/// required_instance_extension by glfw
static const char **required_extensions;

bool check_required_instance_extension_support() {
  uint32_t instance_extension_count = 0;

  required_extensions =
      glfwGetRequiredInstanceExtensions(&required_instance_extension_count);
  if (required_instance_extension_count == 0 || required_extensions == NULL) {
    return false;
  }
  if (enable_validation_layers) {
  }
  vkEnumerateInstanceExtensionProperties(NULL, &instance_extension_count, NULL);
  if (instance_extension_count == 0) {
    return false;
  }

  uint32_t total_extensions = required_instance_extension_count + 1;
  /// MALLOC: FREE THIS
  char **enabled_extensions = malloc(total_extensions * sizeof(char *));
  if (!enabled_extensions) {
    fprintf(stderr, "cant allocate for enabled extension \n");
    return false;
  }
  for (int i = 0; i < required_instance_extension_count; ++i) {
    enabled_extensions[i] = required_extensions[i];
  }
  /// the error was here -1 is correct
  enabled_extensions[total_extensions - 1] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;

  VkExtensionProperties *availabe_extension =
      malloc(instance_extension_count * sizeof(VkExtensionProperties));
  if (!availabe_extension) {
    return false;
  }
  VkResult result = vkEnumerateInstanceExtensionProperties(
      NULL, &instance_extension_count, availabe_extension);

  if (result != VK_SUCCESS) {
    fprintf(stderr, "vkEnumerateInstanceExtensionProperties failed: %d\n",
            result);
    free(availabe_extension);
    return false;
  }

  for (int i = 0; i < required_instance_extension_count; ++i) {
    bool found_extension = false;
    for (int j = 0; j < instance_extension_count; ++j) {
      if (strcmp(required_extensions[i], availabe_extension[j].extensionName) ==
          0) {
        found_extension = true;
        break;
      }
    }
    if (found_extension == false) {
      fprintf(stderr, "Required extension %s not found \n",
              required_extensions[i]);
      free(availabe_extension);
      return false;
    }
  }

  free(availabe_extension);
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

  if (!check_required_instance_extension_support()) {
    fprintf(stderr, "Error in checking extension \n");
    exit(EXIT_FAILURE);
  }

  VkInstanceCreateInfo instance_creation_info = {
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pNext = NULL,
      .pApplicationInfo = &app_info,
      .enabledExtensionCount = required_instance_extension_count,
      .ppEnabledExtensionNames = required_extensions,
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

// static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
//     VkDebugUtilsMessageSeverityFlagBitsEXT severity,
//     VkDebugUtilsMessageTypeFlagsEXT type,
//     VkDebugUtilsMessengerCallbackDataEXT *p_callbackdata, void *p_user_data)
//     {
//   if (severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT ||
//       severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
//     /// TODO: Print the message
//   }
// }

void run_app() {

  GLFWwindow *win = create_window();
  if (win == NULL) {
    exit(EXIT_FAILURE);
  }
  init_vulkan();
  main_loop(win);
  clean_up(win);
}
