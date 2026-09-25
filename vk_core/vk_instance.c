//
// Created by saad on 9/24/26.
//
#include "include/vk_instance.h"
#include "../log/loger.h"
#include "../memory/rmemory.h"
#include <stdbool.h>
#include <string.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef NDEBUG
static const bool enable_validation_layers = false;
#else
static const bool enable_validation_layers = true;
#endif

static const char *validation_layers[] = {
    "VK_LAYER_KHRONOS_validation",
};
static const uint32_t validation_layer_count = 1;

static const char **enabled_extensions;
static uint32_t total_extensions;

void check_validation_layer_support() {
  uint32_t layer_count;
  vkEnumerateInstanceLayerProperties(&layer_count, NULL);

  size_t size_for_available_layer = (layer_count * sizeof(VkLayerProperties));
  VkLayerProperties *available_layer =
      r_allocate(size_for_available_layer, MEM_TAG_RENDERER);
  if (available_layer == NULL) {
    R_FATAL("cant allocate the buffer for availaber layer \n");
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
      r_free(available_layer, size_for_available_layer, MEM_TAG_RENDERER);
      R_FATAL("required layer not found \n");
    }
  }
  r_free(available_layer, size_for_available_layer, MEM_TAG_RENDERER);

/// TODO: check why this is working in release mode
#ifndef NDEBUG
  R_INFO("validation layer check passed \n");
#endif
}

void check_required_instance_extension_support() {

  static uint32_t required_instance_extension_count = 0;
  /// required_instance_extension by glfw
  static const char **required_extensions;
  required_extensions =
      glfwGetRequiredInstanceExtensions(&required_instance_extension_count);
  if (required_instance_extension_count == 0 || required_extensions == NULL) {
    R_FATAL("required extension error glfw");
  }
  total_extensions = required_instance_extension_count;
  if (enable_validation_layers) {
    total_extensions++;
  }
  /// TODO: we would need to implement the vector aka
  /// array for pushing extension
  enabled_extensions = malloc(total_extensions * sizeof(char *));
  if (!enabled_extensions) {
    R_FATAL("cant allocate for enabled extension \n");
  }
  for (int i = 0; i < required_instance_extension_count; ++i) {
    enabled_extensions[i] = required_extensions[i];
  }

  /// NOTE: implementing vector will help us here this is bad code
  if (enable_validation_layers) {
    enabled_extensions[required_instance_extension_count] =
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
  }

  uint32_t instance_extension_count = 0;
  vkEnumerateInstanceExtensionProperties(NULL, &instance_extension_count, NULL);
  if (instance_extension_count == 0) {
    R_FATAL("error getting instance extension count");
  }

  VkExtensionProperties *availabe_extension =
      malloc(instance_extension_count * sizeof(VkExtensionProperties));
  if (!availabe_extension) {
    R_FATAL("error allocating for available extension \n");
  }

  VkResult result = vkEnumerateInstanceExtensionProperties(
      NULL, &instance_extension_count, availabe_extension);

  if (result != VK_SUCCESS) {
    free(availabe_extension);
    R_FATAL("vkEnumerateInstanceExtensionProperties failed: \n");
  }
  for (int i = 0; i < total_extensions; ++i) {
    bool found_extension = false;
    for (int j = 0; j < instance_extension_count; ++j) {
      if (strcmp(enabled_extensions[i], availabe_extension[j].extensionName) ==
          0) {
        found_extension = true;
        break;
      }
    }
    if (found_extension == false) {
      fprintf(stderr, "Required extension %s not found \n",
              enabled_extensions[i]);
      free(availabe_extension);
      free(enabled_extensions);

      R_FATAL("Required extension not found ");
    }
  }

  free(availabe_extension);
  /// do we also need to free the enabled extension check this ?

#ifndef NDEBUG
  R_INFO("required extension is supported \n");
#endif
}

VkInstance create_instance() {

  check_validation_layer_support();
  VkInstance instance;
  VkApplicationInfo app_info = {
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .pNext = NULL,
      .pApplicationName = "Renderer",
      .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
      .pEngineName = NULL,
      .engineVersion = 0,
      .apiVersion = VK_API_VERSION_1_3,
  };

  check_required_instance_extension_support();
  VkInstanceCreateInfo instance_creation_info = {
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pNext = NULL,
      .pApplicationInfo = &app_info,
      .enabledExtensionCount = total_extensions,
      .ppEnabledExtensionNames = enabled_extensions,
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
    R_FATAL("Cant create the vulkan instance \n");
  }


#ifndef NDEBUG
  R_INFO("vulkan instance created \n");
#endif
  return instance;
}

/// DEBUG  CALLBACK  CREATION
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
      .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
      .messageType = messagetype_flags,
      .messageSeverity = severity_flags,
      .pfnUserCallback = &debugCallback,
  };

  if (vkCreateDebugUtilsMessengerEXT(instance, &debug_messenger_create_info,
                                     NULL, &messenger) != VK_SUCCESS) {
    R_FATAL("error creating debug messenger \n");
  }
}
