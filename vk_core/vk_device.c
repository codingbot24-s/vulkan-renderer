//
// Created by saad on 24/09/26.
//

#include "../log/loger.h"
#include "../memory/rmemory.h"
#include "../renderer.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan.h>

bool is_support_surface(renderer *renderer, uint32_t q_index) {
  VkBool32 is_supported = VK_FALSE;
  vkGetPhysicalDeviceSurfaceSupportKHR(renderer->my_physical_device, q_index,
                                       renderer->my_surface, &is_supported);
  if (!is_supported) {
    return false;
  }

  return true;
}

static const char *requiredDeviceExtension[] = {
    "VK_KHR_swapchain",
};

static uint32_t required_device_extension_count = 1;

bool check_physical_device_props(VkPhysicalDevice device) {
  VkPhysicalDeviceProperties physical_device_props;
  vkGetPhysicalDeviceProperties(device, &physical_device_props);
  if (physical_device_props.apiVersion < VK_API_VERSION_1_3) {
    return false;
  }

#ifndef NDEBUG
  R_INFO("device support api version\n");
#endif
  return true;
}

bool check_physical_queue_family(VkPhysicalDevice device) {
  uint32_t physical_device_queue_family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(
      device, &physical_device_queue_family_count, NULL);
  if (physical_device_queue_family_count == 0) {
    return false;
  }

  size_t queue_family_property_allocation_size =
      physical_device_queue_family_count * sizeof(VkQueueFamilyProperties);
  VkQueueFamilyProperties *physical_device_queue_family_properties =
      r_allocate(queue_family_property_allocation_size, MEM_TAG_RENDERER);
  if (!physical_device_queue_family_properties) {
    return false;
  }
  vkGetPhysicalDeviceQueueFamilyProperties(
      device, &physical_device_queue_family_count,
      physical_device_queue_family_properties);

  uint32_t iqf_support_graphics = -1;
  bool graphics_property = false;
  for (int i = 0; i < physical_device_queue_family_count; ++i) {
    if (physical_device_queue_family_properties[i].queueFlags &
        VK_QUEUE_GRAPHICS_BIT) {
      graphics_property = true;
      iqf_support_graphics = i;
      break;
    }
  }

  if (!graphics_property || iqf_support_graphics == -1) {
    r_free(physical_device_queue_family_properties,
           queue_family_property_allocation_size, MEM_TAG_RENDERER);
    return false;
  }

  r_free(physical_device_queue_family_properties,
         queue_family_property_allocation_size, MEM_TAG_RENDERER);

#ifndef NDEBUG
  R_INFO("device support graphics queue\n");
#endif
  return true;
}

bool check_physical_extension(VkPhysicalDevice device) {
  uint32_t device_extension_prop_count = 0;
  if (vkEnumerateDeviceExtensionProperties(
          device, NULL, &device_extension_prop_count, NULL) != VK_SUCCESS) {
    fprintf(stderr, "cant get the device extension count \n");
    return false;
  }

  size_t available_device_extension_size =
      device_extension_prop_count * sizeof(VkExtensionProperties);
  VkExtensionProperties *available_device_extension =
      r_allocate(available_device_extension_size, MEM_TAG_RENDERER);

  if (!available_device_extension) {
    fprintf(stderr, "cant allocate the extension properties buffer \n");
    return false;
  }

  if (vkEnumerateDeviceExtensionProperties(
          device, NULL, &device_extension_prop_count,
          available_device_extension) != VK_SUCCESS) {

    fprintf(stderr, "cant get the available device extension\n");
    r_free(available_device_extension, available_device_extension_size,
           MEM_TAG_RENDERER);
    return false;
  }
  for (int i = 0; i < required_device_extension_count; ++i) {
    bool required_extension_found = false;
    for (int j = 0; j < device_extension_prop_count; ++j) {
      if (strcmp(requiredDeviceExtension[i],
                 available_device_extension[j].extensionName) == 0) {
        required_extension_found = true;
        break;
      }
    }
    if (!required_extension_found) {
      r_free(available_device_extension, available_device_extension_size,
             MEM_TAG_RENDERER);
      fprintf(
          stderr,
          "cant find the required extension %s in the availabe extension \n",
          requiredDeviceExtension[i]);
      return false;
    }
  }

  r_free(available_device_extension, available_device_extension_size,
         MEM_TAG_RENDERER);

#ifndef NDEBUG
  R_INFO("device support required extension\n");
#endif
  return true;
}

bool check_physical_device_features(VkPhysicalDevice device) {
  VkPhysicalDeviceExtendedDynamicStateFeaturesEXT
      vk_physical_device_extended_dstate_features;
  vk_physical_device_extended_dstate_features.sType =
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT;
  vk_physical_device_extended_dstate_features.pNext = NULL;
  VkPhysicalDeviceVulkan13Features vk13_physical_device_features;
  vk13_physical_device_features.sType =
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
  vk13_physical_device_features.pNext =
      &vk_physical_device_extended_dstate_features;

  VkPhysicalDeviceVulkan11Features vk11_physical_device_features;
  vk11_physical_device_features.sType =
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
  vk11_physical_device_features.pNext = &vk13_physical_device_features;

  VkPhysicalDeviceFeatures2 vk_phsical_device_features2;
  vk_phsical_device_features2.sType =
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
  vk_phsical_device_features2.pNext = &vk11_physical_device_features;

  vkGetPhysicalDeviceFeatures2(device, &vk_phsical_device_features2);

  if (!vk11_physical_device_features.shaderDrawParameters) {
    return false;
  }
  if (!vk13_physical_device_features.dynamicRendering) {
    return false;
  }

  if (!vk_physical_device_extended_dstate_features.extendedDynamicState) {
    return false;
  }

#ifndef NDEBUG
  R_INFO("device support required features\n");
#endif
  return true;
}

bool is_device_suitable(VkPhysicalDevice device) {
  if (!check_physical_device_props(device)) {
    return false;
  }

  if (!check_physical_queue_family(device)) {
    return false;
  }

  if (!check_physical_extension(device)) {
    return false;
  }
  if (!check_physical_device_features(device)) {
    fprintf(stderr, "cant get the available device extension\n");
    return false;
  }

  return true;
}

void pick_physical_device(renderer *renderer) {
  renderer->my_physical_device = VK_NULL_HANDLE;
  uint32_t physical_devices_count = 0;
  vkEnumeratePhysicalDevices(renderer->my_vk_instance, &physical_devices_count,
                             NULL);

  if (physical_devices_count == 0) {
    R_FATAL("failed to find GPUs with Vulkan support!");
  }

  size_t physical_device_allocation_size =
      physical_devices_count * sizeof(VkPhysicalDevice);
  VkPhysicalDevice *available_devices =
      r_allocate(physical_device_allocation_size, MEM_TAG_RENDERER);
  if (!available_devices) {
    R_FATAL("failed allocation for physical devices! \n");
  }

  if (vkEnumeratePhysicalDevices(renderer->my_vk_instance,
                                 &physical_devices_count,
                                 available_devices) != 0) {
    r_free(available_devices, physical_device_allocation_size,
           MEM_TAG_RENDERER);
    R_FATAL("Error getting physical devices");
  }

  for (int i = 0; i < physical_devices_count; ++i) {
    if (is_device_suitable(available_devices[i])) {
      renderer->my_physical_device = available_devices[i];
      r_free(available_devices, physical_device_allocation_size,
             MEM_TAG_RENDERER);
      break;
    }
  }

  if (renderer->my_physical_device == VK_NULL_HANDLE) {
    R_FATAL("failed to find a suitable gpu");
    r_free(available_devices, physical_device_allocation_size,
           MEM_TAG_RENDERER);
  }
}
void create_logical_device(renderer *renderer) {
  /// NOTE: quering devcie is repeated change this
  uint32_t physical_device_queue_family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(
      renderer->my_physical_device, &physical_device_queue_family_count, NULL);

  VkQueueFamilyProperties *physical_device_queue_family_props = malloc(
      physical_device_queue_family_count * sizeof(VkQueueFamilyProperties));
  if (!physical_device_queue_family_props) {
    fprintf(stderr, "cant allocate for physical device properties \n");
    return;
  }

  vkGetPhysicalDeviceQueueFamilyProperties(renderer->my_physical_device,
                                           &physical_device_queue_family_count,
                                           physical_device_queue_family_props);

  /// queue family index that support graphics
  uint32_t queue_family_index = -1;
  for (int i = 0; i < physical_device_queue_family_count; ++i) {
    if ((physical_device_queue_family_props[i].queueFlags &
         VK_QUEUE_GRAPHICS_BIT) != 0 &&
        is_support_surface(renderer, i)) {
      queue_family_index = i;
      break;
    }
  }
  if (queue_family_index == -1) {
    return;
  }

  VkDeviceQueueCreateInfo device_queue_create_info = {0};
  device_queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  device_queue_create_info.queueCount = 1;
  device_queue_create_info.queueFamilyIndex = queue_family_index;
  float queue_priority = 0.5f;
  device_queue_create_info.pQueuePriorities = &queue_priority;

  VkPhysicalDeviceExtendedDynamicStateFeaturesEXT
      vk_physical_device_extended_dstate_features = {0};
  vk_physical_device_extended_dstate_features.sType =
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT;
  vk_physical_device_extended_dstate_features.pNext = NULL;
  vk_physical_device_extended_dstate_features.extendedDynamicState = true;

  VkPhysicalDeviceVulkan13Features vk13_physical_device_features = {0};
  vk13_physical_device_features.sType =
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
  vk13_physical_device_features.pNext =
      &vk_physical_device_extended_dstate_features;
  vk13_physical_device_features.dynamicRendering = true;
  vk13_physical_device_features.synchronization2 = true;

  VkPhysicalDeviceVulkan11Features vk11_physical_device_features = {0};
  vk11_physical_device_features.sType =
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
  vk11_physical_device_features.pNext = &vk13_physical_device_features;
  vk11_physical_device_features.shaderDrawParameters = true;

  VkPhysicalDeviceFeatures2 physical_device_features_2 = {0};
  physical_device_features_2.sType =
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
  physical_device_features_2.pNext = &vk11_physical_device_features;

  VkDeviceCreateInfo device_create_info = {
      .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
      .pNext = &physical_device_features_2,
      .queueCreateInfoCount = 1,
      .pQueueCreateInfos = &device_queue_create_info,
      .enabledExtensionCount = required_device_extension_count,
      .ppEnabledExtensionNames = requiredDeviceExtension,
  };

  VkDevice mydevice = renderer->my_device;
  mydevice = VK_NULL_HANDLE;
  if (vkCreateDevice(renderer->my_physical_device, &device_create_info, NULL,
                     &mydevice) != VK_SUCCESS) {
    fprintf(stderr, "cant create the logical device \n");
    return;
  }

  renderer->my_device = mydevice;
  VkQueue graphics_queue = {0};
  vkGetDeviceQueue(renderer->my_device, queue_family_index, 0, &graphics_queue);
  renderer->graphics_queue_index = queue_family_index;
  renderer->my_queue = graphics_queue;
}
