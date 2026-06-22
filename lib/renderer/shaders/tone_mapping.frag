#version 450 core

// In.
layout( binding = 0 ) uniform sampler2D inTextureColor;

// Out.
layout( location = 0 ) out vec4 outFragColor;

float linearToSrgb( const float color )
{
	const float clamped = max( color, 0.f );
	return clamped <= 0.0031308f ? 12.92f * clamped : 1.055f * pow( clamped, 1.f / 2.4f ) - 0.055f;
}

void main()
{
	const ivec2 texCoord   = ivec2( gl_FragCoord.xy );
	const vec3	color	   = texelFetch( inTextureColor, texCoord, 0 ).rgb;
	const float linearLuma = dot( color, vec3( 0.2126f, 0.7152f, 0.0722f ) );

	// TODO: Apply the selected tone mapping operator to RGB.
	outFragColor = vec4( color, linearToSrgb( linearLuma ) );
}
