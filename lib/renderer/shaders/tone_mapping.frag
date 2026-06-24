#version 460 core

// In.
layout( binding = 0 ) uniform sampler2D inTextureColor;

layout( std140, binding = 1 ) uniform Uniforms
{
	uint  mode;
	float exposure;
}

uniforms;

// Out.
layout( location = 0 ) out vec4 outFragColor;

const uint TONE_MAPPING_NONE	   = 0u;
const uint TONE_MAPPING_LDR        = 1u;
const uint TONE_MAPPING_REINHARD   = 2u;
const uint TONE_MAPPING_ACES_FILM  = 3u;
const uint TONE_MAPPING_UNCHARTED2 = 4u;

// Tone mapping functions.
vec3 reinhard( const vec3 p_color )
{
	return p_color / ( vec3( 1.f ) + p_color );
}

vec3 ACESFilm( const vec3 p_x )
{
	const float a = 2.51;
	const float b = 0.03;
	const float c = 2.43;
	const float d = 0.59;
	const float e = 0.14;

	return ( p_x * ( a * p_x + b ) ) / ( p_x * ( c * p_x + d ) + e );
}

vec3 uncharted2Tonemap( const vec3 p_x )
{
	const float A = 0.15;
    const float B = 0.50;
    const float C = 0.10;
    const float D = 0.20;
    const float E = 0.02;
    const float F = 0.30;

    return ( ( p_x * ( A * p_x + C * B ) + D * E ) / ( p_x * ( A * p_x + B ) + D * F ) ) - E / F;
}

// Apply.
vec3 toneMap( const vec3 p_color )
{
	vec3 color = p_color;

	if( uniforms.mode == TONE_MAPPING_LDR )
	{		
	}
	else if ( uniforms.mode == TONE_MAPPING_REINHARD )
	{
		color = reinhard( color );
	}
	else if ( uniforms.mode == TONE_MAPPING_ACES_FILM )
	{
		color = ACESFilm( color );
	}
	else if( uniforms.mode == TONE_MAPPING_UNCHARTED2 )
	{
		const float WHITE_SCALE = 1.37906424665;
		color = uncharted2Tonemap( color ) * WHITE_SCALE;
	}	

	return clamp( color, 0.0, 1.0 );
}

void main()
{
	const ivec2 texCoord = ivec2( gl_FragCoord.xy );
	vec3		color	 = texelFetch( inTextureColor, texCoord, 0 ).rgb;
	
	
	if ( uniforms.mode != TONE_MAPPING_NONE )
	{
		color = max( color, vec3( 0.f ) );
		color *= uniforms.exposure;
		color = toneMap( color );
	}

	outFragColor = vec4( color, 1.f );
}
