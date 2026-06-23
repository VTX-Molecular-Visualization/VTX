#version 460 core

// In.
layout( binding = 0 ) uniform sampler2D inTextureColor;

layout( std140, binding = 1 ) uniform Uniforms { uint mode; }

uniforms;

// Out.
layout( location = 0 ) out vec4 outFragColor;

const uint TONE_MAPPING_NONE	 = 0u;
const uint TONE_MAPPING_REINHARD = 1u;
const uint TONE_MAPPING_ACES_FILM = 2u;

// Tone mapping functions.
vec3 reinhard( const vec3 p_color )
{
	return p_color / ( vec3( 1.f ) + p_color );
}

vec3 ACESFilm(vec3 p_x)
{
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;

    return clamp( ( p_x * ( a * p_x + b ) ) / ( p_x * ( c * p_x + d ) + e ), 0.0, 1.0 );
}

// Apply.
vec3 toneMap( const vec3 p_color )
{
	if ( uniforms.mode == TONE_MAPPING_NONE )
	{
		return p_color;
	}

	const vec3 color = max( p_color, vec3( 0.f ) );
	if ( uniforms.mode == TONE_MAPPING_REINHARD )
	{
		return reinhard( color );
	}
	else if( uniforms.mode == TONE_MAPPING_ACES_FILM )
	{
		return ACESFilm( color );
	}

	return color;
}

void main()
{
	const ivec2 texCoord   = ivec2( gl_FragCoord.xy );
	vec3 color = texelFetch( inTextureColor, texCoord, 0 ).rgb * 2.f; //EXPOSURE
	color = toneMap( color );

	outFragColor = vec4( color, 1.f );
}
