#version 450 core

// In.
layout( binding = 0 ) uniform sampler2D inTextureColor;

layout ( std140, binding = 1 ) uniform Uniforms
{
    vec4 color;
} uniforms;

// Out.
layout( location = 0 ) out vec4 outFragColor;

void main() 
{
    const ivec2 texCoord = ivec2( gl_FragCoord.xy );
	const vec2 texSize = textureSize( inTextureColor, 0 );
    const vec2 uv =  texCoord / texSize;

    vec4 fragColor = texture( inTextureColor, texCoord / texSize );
    float greyscale = max( fragColor.r, max( fragColor.g, fragColor.b ) );

    outFragColor = vec4( greyscale, greyscale, greyscale, 1.f ) * uniforms.color;
}