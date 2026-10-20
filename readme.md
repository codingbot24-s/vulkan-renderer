# Vulkan Renderer From Scratch

A 3D renderer built from scratch using **Vulkan**, developed step by step while learning the fundamentals of modern graphics programming.

This project is being built **live on stream**, documenting the process of developing a Vulkan renderer from the ground up, from window creation and swapchain setup to 3D model rendering, textures, camera controls, and lighting.

The goal is to understand how a modern graphics renderer works internally by implementing its core systems rather than relying on a prebuilt rendering engine.

## About the Project

This project follows a milestone-based development approach inspired by hands-on programming challenges. Each milestone introduces a new graphics programming concept and builds on the previous implementation.

The renderer is being developed incrementally, with a focus on understanding Vulkan's explicit resource management, GPU execution model, graphics pipeline, and rendering workflow.

## Tech Stack

- **Graphics API:** Vulkan
- **Language:** C 
- **Development approach:** Built from scratch, one milestone at a time
- **Development format:** Live-streamed coding sessions

## Development Progress

### 1. Vulkan Initialization and Setup

- [x] Create a window
- [x] Create a Vulkan instance
- [x] Create a window surface
- [x] Select a physical device
- [x] Create a logical device
- [x] Create a graphics queue

### 2. Swapchain and Rendering Infrastructure

- [x] Create a swapchain
- [x] Retrieve swapchain images
- [x] Create image views
- [x] Create a command pool
- [x] Allocate command buffers
- [x] Create a render pass
- [x] Create framebuffers

### 3. Graphics Pipeline and First Draw Call

- [x] Create a graphics pipeline
- [x] Compile and use vertex and fragment shaders
- [x] Record rendering commands
- [x] Submit command buffers to the graphics queue
- [x] Present rendered images
- [x] Render the first triangle

### 4. Geometry and Shader Data

- [x] Create vertex buffers
- [x] Create index buffers
- [x] Implement uniform buffers
- [x] Create descriptor set layouts
- [x] Create descriptor pools and descriptor sets
- [x] Implement model, view, and projection transformations

### 5. 3D Model Rendering

- [x] Load external 3D models
- [x] Parse OBJ model geometry
- [x] Upload model data to GPU buffers
- [x] Render 3D models
- [x] Implement depth buffering

### 6. Textures and Camera

- [x] Load 2D image textures
- [x] Create Vulkan texture images and image views
- [x] Create texture samplers
- [x] Bind textures using descriptor sets
- [x] Apply textures to 3D models
- [x] Implement an interactive 3D camera
- [x] Add WASD camera movement
- [x] Add mouse-look camera controls

### 7. Lighting

- [ ] Implement directional lighting
- [ ] Add vertex normals to the lighting pipeline
- [ ] Implement Blinn-Phong shading
- [ ] Combine textures with lighting

_Status: Lighting is the next milestone in the current development sequence._

### 8. Upcoming Milestones

- [ ] Implement synchronization with semaphores, fences, and pipeline barriers
- [ ] Support multiple frames in flight
- [ ] Improve GPU memory and resource management
- [ ] Add GPU profiling and debugging tools
- [ ] Improve renderer robustness and resource cleanup

## Development Philosophy

The renderer is built incrementally, with each milestone focusing on one core feature.

The main objectives are to:

- Understand Vulkan's explicit graphics programming model.
- Learn how CPU-side code prepares and submits GPU work.
- Understand graphics pipelines, buffers, images, and descriptors.
- Explore the mathematics behind 3D transformations and rendering.
- Build a foundation for more advanced real-time rendering techniques.

## Built Live on Stream

This project is being developed **live on stream**, sharing the actual implementation process, problem-solving, debugging, and progress along the way.

Follow the journey as the renderer evolves from a blank window into a complete 3D rendering system.

**Livestream:** https://youtube.com/playlist?list=PLU064qurTQo0&si=6u4Y1QV_OoGidode

**GitHub Repository:** https://github.com/codingbot24-s/vulkan-renderer

## Project Status

The project has progressed from Vulkan initialization to rendering textured 3D models with interactive camera controls. The next phase focuses on lighting, synchronization, GPU resource management, and rendering performance.

This is a work in progress. The goal is not just to make things render, but to understand how they work under the hood.

---

_Built from scratch with Vulkan. Developed live, one milestone at a time._
