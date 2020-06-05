#version 450 core

layout( binding = 0 ) uniform usampler2D gbColorNormal;
layout( binding = 1 ) uniform sampler2D gbCamPosition;
layout( binding = 2 ) uniform sampler2D gbAmbientOcclusion;

out vec4 fragColor;

struct FragmentData
{
	vec3  color;
	vec3  normal;
	vec3  camPosition;
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
	fd.ambientOcclusion = ambientOcclusion;
}

void main()
{
	FragmentData fd;
	unpackGBuffers( ivec2( gl_FragCoord.xy ), fd );
	
	// Light on camera.
	const vec3 lightDir = normalize( -fd.camPosition );

	const float intensity = dot( fd.normal, lightDir );
	float		lighting  = 1.f;

	if ( intensity < 0.25f )
		lighting = 0.2f;
	else if ( intensity < 0.5f )
		lighting = 0.4f;
	else if ( intensity < 0.75f )
		lighting = 0.55f;
	else if ( intensity < 0.95f )
		lighting = 0.7f;

	fragColor = vec4( fd.color * lighting * fd.ambientOcclusion, 1.f );
}
