#version 450 core

#include "layout_uniforms_camera.glsl";

// In.
layout( binding = 0 ) uniform sampler2D inTextureColor;

layout ( std140, binding = 1 ) uniform Uniforms
{
    float offsetRed;
    float offsetGreen;
    float offsetBlue;
} uniforms;

// Out.
layout( location = 0 ) out vec4 outFragColor;

void main() 
{
    const ivec2 texCoord = ivec2( gl_FragCoord.xy );
	const vec2 texSize = textureSize( inTextureColor, 0 );
    const vec2 uv =  texCoord / texSize;
    const vec2 direction = uv - uniformsCamera.mousePosition / texSize;

    outFragColor.r = texture( inTextureColor, uv + ( direction * vec2( uniforms.offsetRed ) ) ).r;
    outFragColor.g = texture( inTextureColor, uv + ( direction * vec2( uniforms.offsetGreen ) ) ).g;
    outFragColor.b = texture( inTextureColor, uv + ( direction * vec2( uniforms.offsetBlue ) ) ).b;
    outFragColor.a = 1.f;
}