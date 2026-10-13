//
// Created by saad on 10/13/26.
//

#ifndef VULKAN_RENDERER_VERTEX_H
#define VULKAN_RENDERER_VERTEX_H

/// NOTE: with this structer this is not just a vertex it is a whole face
/// containing the vertex and uvs for that face
typedef struct vertex {
    float pos[3];
    float color[3];
    float uvs[2];
} vertex;


#endif //VULKAN_RENDERER_VERTEX_H
