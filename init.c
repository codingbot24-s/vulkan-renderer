//
// Created by saad on 9/6/26.
//

#include "init.h"
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

  enabled_extensions[total_extensions - 1] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
  /// also we need to check if enabled extensions are availabe in the instance
  /// // ok so we will fix this in next stream extension or offline
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

VkInstance create_instance() {
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

  return instance;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL
debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
              VkDebugUtilsMessageTypeFlagsEXT messageType,
              const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
              void *pUserData) {

  if (severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT ||
      severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
    fprintf(stderr, "msg %s", pCallbackData->pMessage);
  }

  return VK_FALSE;
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugUtilsMessengerEXT *pMessenger) {
  PFN_vkCreateDebugUtilsMessengerEXT func =
      (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
          instance, "vkCreateDebugUtilsMessengerEXT");
  if (func != NULL) {
    return func(instance, pCreateInfo, pAllocator, pMessenger);
  }
  return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void setup_debug_messenger(VkInstance instance) {
  if (!enable_validation_layers) {
    return;
  }

  VkDebugUtilsMessengerEXT messenger = NULL;
  VkDebugUtilsMessageSeverityFlagBitsEXT severity_flags =
      (VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
       VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT);
  VkDebugUtilsMessageTypeFlagBitsEXT messagetype_flags =
      (VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
       VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
       VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT);
  VkDebugUtilsMessengerCreateInfoEXT debug_messenger_create_info = {
      .messageType = messagetype_flags,
      .messageSeverity = severity_flags,
      .pfnUserCallback = &debugCallback,
  };
  if (vkCreateDebugUtilsMessengerEXT(instance, &debug_messenger_create_info,
                                     NULL, &messenger) != VK_SUCCESS) {
    fprintf(stderr, "Cant create a debug messenger \n");
    return;
  }
}

void init_vulkan() {
  VkInstance instance = create_instance();
  setup_debug_messenger(instance);
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
