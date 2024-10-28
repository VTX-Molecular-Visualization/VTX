#version 450 core

#include "constant.glsl"

// In.
layout( binding = 0 ) uniform sampler2D inTextureColor;

layout ( std140, binding = 1 ) uniform Uniforms
{
	vec2 curvature;
    float ratio;
    float opacityX;
    float opacityY;
    float roundness;
    float opacityVignette;
    float brightness;
} uniforms;

// Out.
layout( location = 0 ) out vec4 outFragColor;
 
vec2 curveRemapUV( const vec2 p_uv )
{
    vec2 uv = p_uv * 2.f - 1.f;
    vec2 offset = abs( uv.yx ) / vec2( uniforms.curvature.x, uniforms.curvature.y );
    uv = uv + uv * offset * offset;
    uv = uv * 0.5f + 0.5f;
    return uv;
}

vec4 intensityScanLine( float p_uv, float p_resolution, float p_opacity )
 {
     float intensity = sin( p_uv * p_resolution * PI * 2.f );
     intensity = ( ( 0.5f * intensity ) + 0.5f) * 0.9f + 0.1f;

     return vec4( vec3( pow( intensity, p_opacity ) ), 1.f );
 }

 vec4 intensityVignette( float p_uv, float p_resolution )
 {
	 float intensity = clamp( pow( ( p_uv * ( 1.f - p_uv ) * p_resolution / uniforms.roundness ),  uniforms.opacityVignette ), 0.f, 1.f );
	 return vec4( vec3( intensity ), 1.f );
 }

void main() 
{
    const ivec2 texCoord = ivec2( gl_FragCoord.xy );
	const vec2 texSize = textureSize( inTextureColor, 0 );
    const vec2 uvRemapped = curveRemapUV( texCoord / texSize );

    if ( uvRemapped.x < 0.f || uvRemapped.y < 0.f || uvRemapped.x > 1.f || uvRemapped.y > 1.f )
    {
        outFragColor = vec4( 0.f, 0.f, 0.f, 0.f );
    }
    else
    {
        const uint size = uint( 1 / uniforms.ratio );
        const vec2 xy = floor( size / 2.f ) - ( texCoord % size ) + texCoord;

        vec4 color = texture( inTextureColor,  curveRemapUV( xy / texSize ) );

        color *= intensityScanLine( uvRemapped.x, texSize.y, uniforms.opacityY );
        color *= intensityScanLine( uvRemapped.y, texSize.x, uniforms.opacityX );

        color *= intensityVignette( uvRemapped.x, texSize.y );
		color *= intensityVignette( uvRemapped.y, texSize.x );

        color *= vec4( vec3( uniforms.brightness ), 1.f );
        
        outFragColor = color;
    }
}