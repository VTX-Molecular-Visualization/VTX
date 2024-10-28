#version 450 core

#include "layout_uniforms_camera.glsl"
#include "struct_data_packed.glsl"

// In.
layout( binding = 0 ) uniform usampler2D inTexturePackedData;
layout( binding = 1 ) uniform sampler2D inTextureColor;
layout( binding = 2 ) uniform sampler2D inTextureAmbientOcclusion;

layout ( std140, binding = 3 ) uniform Uniforms
{
	vec4 colorBackground;
	vec4 colorLight;	
	vec4 colorFog;	
	int shadingMode;
	float specularFactor;	
	float shininess;
	uint toonSteps;
	float fogNear;
	float fogFar;
	float fogDensity;
} uniforms;

// Out.
layout( location = 0 ) out vec4 outFragColor;

const uint DIFFUSE = 0;
const uint GLOSSY = 1;
const uint TOON = 2;
const uint FLAT_COLOR = 3;
const uint GGX = 4;

void main()
{
	const ivec2 texCoord = ivec2( gl_FragCoord.xy );

	UnpackedData data;
	unpackData( inTexturePackedData, data, texCoord );

	if ( data.viewPosition.z == 0.f )
	{
		if ( uniforms.fogDensity != 0.f )
		{
			outFragColor = vec4( mix( vec3( uniforms.colorBackground ),  vec3( uniforms.colorFog ), uniforms.fogDensity ) *  vec3( uniforms.colorLight ), uniforms.colorBackground.w );
		}
		else
		{
			outFragColor = uniforms.colorBackground;
		}
		return;
	}

	// Lighting (on camera).
	const vec3 lightDir = uniformsCamera.isCameraPerspective == 1 ? normalize( -data.viewPosition ) : vec3( 0.f, 0.f, 1.f );

	// FLAT_COLOR.
	float lighting = 1.f;

	// DIFFUSE.
	if( uniforms.shadingMode == DIFFUSE )
	{		
		lighting = max( dot( data.normal, lightDir ), 0.f );
	}
	// GLOSSY.
	else if( uniforms.shadingMode == GLOSSY )
	{		
		const float diffuse = 1.f - uniforms.specularFactor;
		const vec3	viewDir = normalize( -data.viewPosition );
		const vec3	h		= normalize( lightDir + viewDir );
		const float specular = uniforms.specularFactor * pow( max( dot( h, data.normal ), 0.f ), uniforms.shininess );
		const float cosTheta = max( dot( data.normal, lightDir ), 0.f );
		lighting = ( diffuse + specular ) * cosTheta;
	}
	// TOON.
	else if( uniforms.shadingMode == TOON ) 
	{
		const float intensity = dot( data.normal, lightDir );		

		// TODO: move to CPU.
		// Set ligthing base on intensity and toon steps.
		for( uint i = 0; i < uniforms.toonSteps; ++i )
		{
			const float range = float( i + 1 ) / float( uniforms.toonSteps );
			if ( intensity < range )
			{
				lighting = range;
				break;
			}
		}
	}
	// GGX.
	else if( uniforms.shadingMode == GGX )
	{
		const float diffuse = 1.f - uniforms.specularFactor;
		const vec3	viewDir = normalize( -data.viewPosition );
		const vec3	h		= normalize( lightDir + viewDir );
		const float specular = uniforms.specularFactor * pow( max( dot( h, data.normal ), 0.f ), uniforms.shininess );
		const float cosTheta = max( dot( data.normal, lightDir ), 0.f );
		lighting = ( diffuse + specular ) * cosTheta;
	}

	const ivec2 ambientOcclusionTextureSize = textureSize( inTextureAmbientOcclusion, 0 );
	const vec2 normalizedTexCoord = gl_FragCoord.xy / vec2( ambientOcclusionTextureSize );
	const float ambientOcclusion = texture( inTextureAmbientOcclusion, normalizedTexCoord ).x;
	//const float ambientOcclusion = texelFetch( inTextureAmbientOcclusion, texCoord, 0 ).x;

	const float fogFactor = smoothstep( uniforms.fogNear, uniforms.fogFar, -data.viewPosition.z ) * uniforms.fogDensity;
	const vec3	color	  = texelFetch( inTextureColor, texCoord, 0 ).xyz * ambientOcclusion * lighting;

	outFragColor = vec4( mix( color, vec3( uniforms.colorFog ), fogFactor ) * vec3( uniforms.colorLight ), 1.f );
}
