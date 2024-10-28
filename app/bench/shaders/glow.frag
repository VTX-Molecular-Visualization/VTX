#version 450 core

// In.
layout( binding = 0 ) uniform sampler2D inTextureColor;
layout( binding = 1 ) uniform sampler2D inTextureDepth;

layout ( std140, binding = 2 ) uniform Uniforms
{	
	vec4 color;
	float sensivity;
	uint size;	
} uniforms;

// Out.
layout( location = 0 ) out vec4 outFragColor;

void main()
{
	const vec2 textSize = vec2( textureSize( inTextureColor, 0 ) );
	const vec2 texCoord = gl_FragCoord.xy / textSize;

	// Get current pixel color.
	vec4 colorCenter = texture( inTextureColor, texCoord, 0 );	
	
	// Glow with neightbors colors.
	const int size = int( uniforms.size );
	const int samples = 3; // 9
	const int radius = size * samples;
	
	for( int i = -radius; i <= radius; i += size )
	{
		for( int j = -radius; j <= radius; j += size )
		{			
			const vec2 offset = vec2( i, j ) / textSize;
			const vec4 colorNeightbor = texture( inTextureColor, texCoord + offset, 0 );
			
			colorCenter += colorNeightbor / float( samples * samples );
		}
	}

	outFragColor = colorCenter;
}
