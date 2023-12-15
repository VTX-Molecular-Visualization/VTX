#version 450 core

#include "constant.glsl"

// In.
layout( binding = 0 ) uniform sampler2D inTextureColor;

layout ( std140, binding = 1 ) uniform Uniforms
{
	vec2 curvature;
    float ratio;
    float opacity;
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

vec4 scanLineIntensity( float p_uv, float p_resolution, float p_opacity )
 {
     float intensity = sin( p_uv * p_resolution * PI * 2.f );
     intensity = ( ( 0.5f * intensity ) + 0.5f) * 0.9f + 0.1f;
     return vec4( vec3( pow( intensity, p_opacity ) ), 1.f );
 }

void main(void) 
{
    const ivec2 texCoord = ivec2( gl_FragCoord.xy );
	const vec2 texSize = textureSize( inTextureColor, 0 );
    const vec2 uvRemapped = curveRemapUV( texCoord / texSize );

    //vec4 baseColor = texture2D( inTextureColor, uvRemapped );

    if ( uvRemapped.x < 0.f || uvRemapped.y < 0.f || uvRemapped.x > 1.f || uvRemapped.y > 1.f )
    {
        outFragColor = vec4( 0.f, 0.f, 0.f, 0.f );
    }
    else
    {
        const uint size = uint( 1 / uniforms.ratio );
        const vec2 xy = floor( size / 2.f ) - ( texCoord % size ) + texCoord;

        vec4 color = texture( inTextureColor,  curveRemapUV( xy / texSize ) );
        color *= scanLineIntensity( uvRemapped.x, texSize.y, uniforms.opacity );
        color *= scanLineIntensity( uvRemapped.y, texSize.x, uniforms.opacity );
        
        outFragColor = color;
    }
}