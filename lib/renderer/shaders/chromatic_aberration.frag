#version 450 core

#include "struct_data_packed.glsl"

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

    const ivec2 focusPoint = ivec2( 0, 0 );
    const vec2 direction = uv - focusPoint;

    outFragColor.r = texture( inTextureColor, uv + ( direction * vec2( uniforms.offsetRed ) ) ).r;
    outFragColor.g = texture( inTextureColor, uv + ( direction * vec2( uniforms.offsetGreen ) ) ).g;
    outFragColor.b = texture( inTextureColor, uv + ( direction * vec2( uniforms.offsetBlue ) ) ).b;
    outFragColor.a = 1.f;
}