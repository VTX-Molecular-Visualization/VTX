#version 450 core

#include "../layout_uniforms_camera.glsl"
// In.
layout (location = 0) in vec3 inPosition;

// Out.
out vec3 texCoord;

void main()
{
    texCoord = inPosition;
    gl_Position = uniformsCamera.matrixProjection * uniformsCamera.matrixView * vec4( inPosition * 1000.f, 1.0 );
}