#version 450 core

layout( binding = 0 ) uniform usampler2D gbColorNormal;
layout( binding = 1 ) uniform sampler2D gbCamPosition;
layout( binding = 2 ) uniform sampler2D gbAmbientOcclusion;

uniform float uAoFactor;
uniform float uSpecularFactor = 0.6f;

out vec4 fragColor;

struct FragmentData
{
	vec3  color;
	vec3  normal;
	vec3  camPosition;
	float shininess;
	float ambientOcclusion;
};

void unpackGBuffers( ivec2 px, out FragmentData fd )
{
	const uvec4 colorNormal		 = texelFetch( gbColorNormal, px, 0 );
	const vec4	camPosition		 = texelFetch( gbCamPosition, px, 0 );
	const float ambientOcclusion = texelFetch( gbAmbientOcclusion, px, 0 ).x;

	const vec2 tmp = unpackHalf2x16( colorNormal.y );

	fd.color			= vec3( unpackHalf2x16( colorNormal.x ), tmp.x );
	fd.normal			= normalize( vec3( tmp.y, unpackHalf2x16( colorNormal.z ) ) );
	fd.camPosition		= camPosition.xyz;
	fd.shininess		= camPosition.w;
	fd.ambientOcclusion = ambientOcclusion;
}

void main()
{
	FragmentData fd;
	unpackGBuffers( ivec2( gl_FragCoord ), fd );

	const vec3	lightDir	= normalize( -fd.camPosition );
	const float lightFactor = 1.f - uAoFactor;

	const float diffuse = 1.f - uSpecularFactor;

	const vec3	h		 = normalize( lightDir - fd.camPosition );
	const float specular = uSpecularFactor * pow( max( dot( h, fd.normal ), 0.f ), fd.shininess );

	const float ao = uAoFactor * fd.ambientOcclusion;

	const float cosTheta = max( dot( fd.normal, lightDir ), 0.f );
	const float lighting = lightFactor * ( diffuse + specular ) * cosTheta;

	fragColor = vec4( fd.color * ( lighting + ao ), 1.f );
}
