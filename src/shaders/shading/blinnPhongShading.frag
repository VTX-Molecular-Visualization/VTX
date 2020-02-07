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
	float specular;
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
	fd.specular			= camPosition.w;
	fd.ambientOcclusion = ambientOcclusion;
}

void main()
{
	FragmentData fd;
	unpackGBuffers( ivec2( gl_FragCoord ), fd );

	const vec3	lightDir = normalize( -fd.camPosition );
	const float diffuse	 = dot( fd.normal, lightDir );
	const vec3	h		 = normalize( lightDir - fd.camPosition );
	const float spec	 = pow( max( dot( fd.normal, h ), 0.f ), fd.specular );

	// TODO compute well ^^
	const vec3 colorDiff = ( fd.color ) * diffuse;
	const vec3 colorSpec = ( vec3( 1.f ) - fd.color ) * spec;

	const vec3 color = colorDiff + colorSpec - fd.ambientOcclusion;

	fragColor = vec4( color, 1.f );
}
