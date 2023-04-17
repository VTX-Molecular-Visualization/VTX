#version 450 core

layout( binding = 0 ) uniform usampler2D gbViewPositionNormal;
layout( binding = 1 ) uniform sampler2D gbColor;
layout( binding = 2 ) uniform sampler2D gbAmbientOcclusion;

uniform vec4 uBackgroundColor;

uniform float uFogNear;
uniform float uFogFar;
uniform float uFogDensity;
uniform vec3  uFogColor;
uniform vec3 uLightColor;
uniform bool uIsPerspective;

// TODO: let the user set it !
uniform float uSpecularFactor = 0.4f;

out vec4 fragColor;

struct UnpackedData
{
	vec3 viewPosition;
	vec3 normal;
};

void unpackGBuffers( ivec2 px, out UnpackedData data )
{
	const uvec4 viewPositionNormal = texelFetch( gbViewPositionNormal, px, 0 );

	const vec2 tmp	  = unpackHalf2x16( viewPositionNormal.y );
	data.viewPosition = vec3( unpackHalf2x16( viewPositionNormal.x ), tmp.x );
	data.normal		  = vec3( tmp.y, unpackHalf2x16( viewPositionNormal.z ) );
}

void main()
{
	const ivec2 texCoord = ivec2( gl_FragCoord.xy );

	UnpackedData data;
	unpackGBuffers( texCoord, data );

	if ( data.viewPosition.z == 0.f )
	{
		if ( uFogDensity != 0.f )
			fragColor = vec4( mix( vec3( uBackgroundColor ), uFogColor, uFogDensity ), uBackgroundColor.w );
		else
			fragColor = uBackgroundColor;
		return;
	}

	// Light on camera.
	const vec3 lightDir = uIsPerspective ?
		normalize( -data.viewPosition ) :
		vec3( 0.f, 0.f, 1.f );

	// Shader does not produce energy ! ;-)
	const float diffuse = 1.f - uSpecularFactor;

	const vec4 pixelColor = texelFetch( gbColor, texCoord, 0 );

	// Blinn-Phong.
	const vec3	viewDir = normalize( -data.viewPosition ); // == lightDir for the moment
	const vec3	h		= normalize( lightDir + viewDir );
	const float specular
		= uSpecularFactor * pow( max( dot( h, data.normal ), 0.f ), pixelColor.w ); // pixelColor.x is shininess.

	const float cosTheta = max( dot( data.normal, lightDir ), 0.f );
	const float lighting = ( diffuse + specular ) * cosTheta;

	const float ambientOcclusion = texelFetch( gbAmbientOcclusion, texCoord, 0 ).x;

	const float fogFactor = smoothstep( uFogNear, uFogFar, -data.viewPosition.z ) * uFogDensity;
	const vec3	color	  = pixelColor.xyz * lighting * ambientOcclusion;

	fragColor = vec4( mix( color, uFogColor, fogFactor ) * uLightColor, 1.f );
}
