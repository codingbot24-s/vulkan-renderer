//
// Created by saad on 9/6/26.
//

#include "init.h"
#include "window.h"
#include <GLFW/glfw3.h>
#include <complex.h>
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

static const char **enabled_extensions;
static uint32_t total_extensions;

bool check_required_instance_extension_support() {

  static uint32_t required_instance_extension_count = 0;
  /// required_instance_extension by glfw
  static const char **required_extensions;
  required_extensions =
      glfwGetRequiredInstanceExtensions(&required_instance_extension_count);
  if (required_instance_extension_count == 0 || required_extensions == NULL) {
    return false;
  }
  total_extensions = required_instance_extension_count;
  if (enable_validation_layers) {
    total_extensions++;
  }
  /// using 64 as it is used in glfw demo vulkan triangle
  /// TODO: we would need to implement the vector aka dynamic
  /// array for pushing extension
  /// MALLOC: FREE THIS
  enabled_extensions = malloc(total_extensions * sizeof(char *));
  if (!enabled_extensions) {
    fprintf(stderr, "cant allocate for enabled extension \n");
    return false;
  }

  for (int i = 0; i < required_instance_extension_count; ++i) {
    enabled_extensions[i] = required_extensions[i];
  }

  /// NOTE: implementing vector will help us here this is bad code
  if (enable_validation_layers) {
    enabled_extensions[required_instance_extension_count] =
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
  }
  /// NOTE: add vulkan surface extension here
  // enabled_extensions[required_instance_extension_count + 2] =

  uint32_t instance_extension_count = 0;
  vkEnumerateInstanceExtensionProperties(NULL, &instance_extension_count, NULL);
  if (instance_extension_count == 0) {
    return false;
  }

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
      .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
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

void create_surface(renderer *renderer) {
  VkSurfaceKHR surface = VK_NULL_HANDLE;

  VkResult res = glfwCreateWindowSurface(renderer->my_vk_instance,
                                         renderer->window, NULL, &surface);
  if (res != VK_SUCCESS || surface == NULL) {
    fprintf(stderr, "cant create a vulkan surface \n");
    return;
  }

  renderer->my_surface = surface;
}

bool is_device_suitable(VkPhysicalDevice device) {
  VkPhysicalDeviceProperties physical_device_props;
  vkGetPhysicalDeviceProperties(device, &physical_device_props);
  if (physical_device_props.apiVersion < VK_API_VERSION_1_0) {
    return false;
  }

  uint32_t physical_device_queue_family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(
      device, &physical_device_queue_family_count, NULL);
  if (physical_device_queue_family_count == 0) {
    return false;
  }

  VkQueueFamilyProperties *physical_device_queue_family_properties = malloc(
      physical_device_queue_family_count * sizeof(VkQueueFamilyProperties));
  if (!physical_device_queue_family_properties) {
    return false;
  }
  vkGetPhysicalDeviceQueueFamilyProperties(
      device, &physical_device_queue_family_count,
      physical_device_queue_family_properties);

  bool graphics_property;
  /// check for the graphics property in queue family
  for (int i = 0; i < physical_device_queue_family_count; ++i) {
    graphics_property = false;
    VkQueueFamilyProperties qfp = physical_device_queue_family_properties[i];
    graphics_property = !!(qfp.queueFlags & VK_QUEUE_GRAPHICS_BIT);
  }

  if (!graphics_property) {
    return false;
  }

  /// we will continue from here
}

/// NOTE: we can also pick the device by score
void pick_physical_device(renderer *renderer) {
  VkPhysicalDevice my_pdevice = VK_NULL_HANDLE;

  uint32_t physical_devices_count = 0;
  vkEnumeratePhysicalDevices(renderer->my_vk_instance, &physical_devices_count,
                             NULL);

  if (physical_devices_count == 0) {
    fprintf(stderr, "failed to find GPUs with Vulkan support! \n");
    return;
  }
  /// MALLOC: FREE THIS
  VkPhysicalDevice *available_devices =
      malloc(physical_devices_count * sizeof(VkPhysicalDevice));
  if (!available_devices) {
    fprintf(stderr, "failed allocation for physical devices! \n");
    return;
  }

  if (vkEnumeratePhysicalDevices(renderer->my_vk_instance,
                                 &physical_devices_count,
                                 available_devices) != 0) {
    fprintf(stderr, "Error getting physical devices");
    return;
  }

  for (int i = 0; i < physical_devices_count; ++i) {
    if (is_device_suitable(available_devices[i])) {
      my_pdevice = available_devices[i];
      /// should we call free here also ?
      break;
    }
  }

  if (my_pdevice == VK_NULL_HANDLE) {
    fprintf(stderr, "failed to find a suitable gpu");
    free(available_devices);
    return;
  }
}

void init_vulkan(renderer *renderer) {
  renderer->my_vk_instance = create_instance();
  setup_debug_messenger(renderer->my_vk_instance);
  pick_physical_device(renderer);
  create_surface(renderer);
}

void run_app() {
  renderer renderer;
  create_window(&renderer);
  init_vulkan(&renderer);
  main_loop(renderer.window);
  clean_up(renderer.window);
}
