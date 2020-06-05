#version 450 core

layout( binding = 0 ) uniform usampler2D gbColorNormal;
layout( binding = 2 ) uniform sampler2D gbAmbientOcclusion;

uniform float uAoFactor;

out vec4 fragColor;

struct FragmentData
{
	vec3  color;
	float ambientOcclusion;
};

void unpackGBuffers( ivec2 px, out FragmentData fd )
{
	const uvec4 colorNormal		 = texelFetch( gbColorNormal, px, 0 );
	const float ambientOcclusion = texelFetch( gbAmbientOcclusion, px, 0 ).x;

	const vec2 tmp = unpackHalf2x16( colorNormal.y );

	fd.color			= vec3( unpackHalf2x16( colorNormal.x ), tmp.x );
	fd.ambientOcclusion = ambientOcclusion;
}

void main()
{
	FragmentData fd;
	unpackGBuffers( ivec2( gl_FragCoord ), fd );

	const float colorFactor = 1.f - uAoFactor;

	fragColor = vec4( fd.color * ( colorFactor + uAoFactor * fd.ambientOcclusion ), 1.f );
}
