/// TODO: add memory arena
/// TODO: 1.currently we need darray and strings and

/*
  1. create a graphics pipeline
  2. record render command
  3. submit and present
  /// this all is complete
*/

/*
  TODO:
  1. Create Vertex and Index Buffers
  The triangle is rendered from GPU vertex and index buffers.

  2. Create a uniform buffer that stores per-frame shader data and make it
  accessible from the graphics pipeline.
  Create one uniform buffer for the current frame.
  Upload a simple value such as a transformation matrix.
  Verify the shader can read the uniform data.

  3.Create the Vulkan descriptor set layout, descriptor pool, and descriptor set
  required to bind your uniform buffer to the shaders.
  Bind the uniform buffer through a descriptor set.
  Bind the descriptor set during rendering.
  Verify the triangle still renders correctly.
*/

#include "init.h"
int main() { run_app(); }
