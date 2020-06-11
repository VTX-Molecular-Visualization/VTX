#version 450 core

layout( binding = 0 ) uniform usampler2D gbColorNormal;
layout( binding = 1 ) uniform sampler2D gbViewPosition;
layout( binding = 2 ) uniform sampler2D gbAmbientOcclusion;

uniform float uSpecularFactor = 0.4f;
uniform vec3  uBackgroundColor;

out vec4 fragColor;

struct UnpackedData
{
	vec3  color;
	vec3  normal;
	vec3  viewPosition;
	float shininess;
};

void unpackGBuffers( ivec2 px, out UnpackedData data )
{
	const uvec4 colorNormal	 = texelFetch( gbColorNormal, px, 0 );
	const vec4	viewPosition = texelFetch( gbViewPosition, px, 0 );

	const vec2 tmp = unpackHalf2x16( colorNormal.y );

	data.color		  = vec3( unpackHalf2x16( colorNormal.x ), tmp.x );
	data.normal		  = vec3( tmp.y, unpackHalf2x16( colorNormal.z ) );
	data.viewPosition = viewPosition.xyz;
	data.shininess	  = viewPosition.w;
}

void main()
{
	const ivec2 texCoord = ivec2( gl_FragCoord.xy );

	UnpackedData data;
	unpackGBuffers( texCoord, data );

	if ( data.normal.x == 0.f && data.normal.y == 0.f && data.normal.z == 0.f )
	{
		fragColor = vec4( uBackgroundColor, 1.f );
		return;
	}

	// Light on camera.
	const vec3 lightDir = normalize( -data.viewPosition );

	// Shader does not produce energy ! ;-)
	const float diffuse = 1.f - uSpecularFactor;

	// Blinn-Phong.
	const vec3	h		 = normalize( lightDir - data.viewPosition );
	const float specular = uSpecularFactor * pow( max( dot( h, data.normal ), 0.f ), data.shininess );

	const float cosTheta = max( dot( data.normal, lightDir ), 0.f );
	const float lighting = ( diffuse + specular ) * cosTheta;

	const float ambientOcclusion = texelFetch( gbAmbientOcclusion, texCoord, 0 ).x;

	fragColor = vec4( data.color * lighting * ambientOcclusion, 1.f );
}
