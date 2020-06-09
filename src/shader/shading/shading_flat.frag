#version 450 core

layout( binding = 0 ) uniform usampler2D gbColorNormal;
layout( binding = 2 ) uniform sampler2D gbAmbientOcclusion;

uniform vec3 uBackgroundColor;

out vec4 fragColor;

struct FragmentData
{
	vec3  color;
	vec3  normal;
	float ambientOcclusion;
};

void unpackGBuffers( ivec2 px, out FragmentData fd )
{
	const uvec4 colorNormal		 = texelFetch( gbColorNormal, px, 0 );
	const float ambientOcclusion = texelFetch( gbAmbientOcclusion, px, 0 ).x;

	const vec2 tmp = unpackHalf2x16( colorNormal.y );

	fd.color			= vec3( unpackHalf2x16( colorNormal.x ), tmp.x );
	fd.normal			= vec3( tmp.y, unpackHalf2x16( colorNormal.z ) );
	fd.ambientOcclusion = ambientOcclusion;
}

void main()
{
	FragmentData fd;
	unpackGBuffers( ivec2( gl_FragCoord.xy ), fd );
	
	if ( fd.normal.x == 0.f && fd.normal.y == 0.f && fd.normal.z == 0.f )
	{
		fragColor = vec4(uBackgroundColor, 1.f);
		return;
	}

	fragColor = vec4( fd.color * fd.ambientOcclusion, 1.f );
}
