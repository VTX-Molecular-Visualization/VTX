#version 450 core

#include "../global_uniforms.glsl"
// In.
layout (location = 0) in vec3 inPosition;

// Out.
out vec3 texCoord;

void main()
{
    texCoord = inPosition;
    gl_Position = uniforms.matrixProjection * uniforms.matrixView * vec4( inPosition * 1000.f, 1.0 );
}