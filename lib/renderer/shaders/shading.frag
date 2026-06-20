#version 460 core

#include "layout_uniforms_camera.glsl"
#include "struct_data_packed.glsl"

// In.
in vec3 worldDirection;
layout( binding = 0 ) uniform usampler2D inTexturePackedData;
layout( binding = 1 ) uniform sampler2D inTextureColor;
layout( binding = 2 ) uniform sampler2D inTextureAmbientOcclusion;
layout( binding = 3 ) uniform sampler2D inTextureDepth;
layout( binding = 4 ) uniform samplerCube inTextureEnvironment;

layout( std140, binding = 5 ) uniform Uniforms
{
	vec4  colorBackground;
	vec4 colorLight;	
	vec4 colorFog;	
	int shadingMode;
	float specularFactor;
	float shininess;
	uint toonSteps;
	float fogNear;
	float fogFar;
	float fogDensity;
	float ssaoScale;
	uint environmentEnabled;
	float environmentExposure;
	float environmentRotation;
}

uniforms;

// Out.
layout( location = 0 ) out vec4 outFragColor;

const uint DIFFUSE = 0;
const uint GLOSSY = 1;
const uint TOON = 2;
const uint FLAT_COLOR = 3;
const uint GGX = 4;

vec3 linearToSrgb(vec3 c)
{
return c;
    c = max(c, vec3(0.0));
    bvec3 lo = lessThanEqual(c, vec3(0.0031308));
    vec3  low  = 12.92 * c;
    vec3  high = 1.055 * pow(c, vec3(1.0/2.4)) - 0.055;
    return mix(high, low, vec3(lo));
}

float sampleAmbientOcclusion( const ivec2 p_texCoord, const float p_viewDepth )
{
	const ivec2 aoTextureSize = textureSize( inTextureAmbientOcclusion, 0 );
	// No reconstruction is needed when AO and shading have the same resolution.
	if ( uniforms.ssaoScale <= 1.f )
	{
		return texelFetch( inTextureAmbientOcclusion, clamp( p_texCoord, ivec2( 0 ), aoTextureSize - 1 ), 0 ).x;
	}

	// Locate the full-resolution pixel within the four surrounding low-resolution AO texels.
	const vec2	aoPosition		 = ( vec2( p_texCoord ) + 0.5f ) / uniforms.ssaoScale - 0.5f;
	const ivec2 aoBase			 = ivec2( floor( aoPosition ) );
	const vec2	fraction		 = fract( aoPosition );
	const ivec2 depthTextureSize = textureSize( inTextureDepth, 0 );
	const float sharpness		 = max( 1.f, 64.f / max( -p_viewDepth, 0.0001f ) );

	float weightedAO  = 0.f;
	float totalWeight = 0.f;
	for ( int y = 0; y < 2; ++y )
	{
		for ( int x = 0; x < 2; ++x )
		{
			const ivec2 offset	= ivec2( x, y );
			const ivec2 aoCoord = clamp( aoBase + offset, ivec2( 0 ), aoTextureSize - 1 );
			// Preserve bilinear interpolation away from depth discontinuities.
			const vec2	axisWeight	  = mix( vec2( 1.f ) - fraction, fraction, vec2( offset ) );
			const float spatialWeight = axisWeight.x * axisWeight.y;

			// Match the representative full-resolution depth used to compute this AO texel.
			const ivec2 depthCoord
				= clamp( ivec2( ( vec2( aoCoord ) + 0.5f ) * uniforms.ssaoScale ), ivec2( 0 ), depthTextureSize - 1 );
			const float sampleViewDepth = -texelFetch( inTextureDepth, depthCoord, 0 ).x;
			// Reject AO from another surface while smoothly blending nearby depths.
			const float depthWeight = exp2( -abs( sampleViewDepth - p_viewDepth ) * sharpness );
			const float weight		= spatialWeight * depthWeight;
			weightedAO += texelFetch( inTextureAmbientOcclusion, aoCoord, 0 ).x * weight;
			totalWeight += weight;
		}
	}

	if ( totalWeight > 0.0001f )
	{
		return weightedAO / totalWeight;
	}

	// Thin geometry may have no matching low-resolution depth sample.
	const ivec2 nearestCoord = clamp( ivec2( round( aoPosition ) ), ivec2( 0 ), aoTextureSize - 1 );
	return texelFetch( inTextureAmbientOcclusion, nearestCoord, 0 ).x;
}

void main()
{
	const ivec2 texCoord = ivec2( gl_FragCoord.xy );

	UnpackedData data;
	unpackData( inTexturePackedData, data, texCoord );

	if ( data.viewPosition.z == 0.f )
	{
		if ( uniforms.environmentEnabled != 0u )
		{
			vec3 direction = normalize( worldDirection );
			const float cosine = cos( uniforms.environmentRotation );
			const float sine = sin( uniforms.environmentRotation );
			direction.xz = mat2( cosine, -sine, sine, cosine ) * direction.xz;
			outFragColor = vec4( texture( inTextureEnvironment, direction ).rgb * uniforms.environmentExposure, 1.f );
		}
		else if ( uniforms.fogDensity != 0.f )
		{
			outFragColor = vec4( linearToSrgb(mix( vec3( uniforms.colorBackground ),  vec3( uniforms.colorFog ), uniforms.fogDensity ) *  vec3( uniforms.colorLight )), uniforms.colorBackground.w );
		}
		else
		{
			outFragColor = vec4(linearToSrgb(uniforms.colorBackground.xyz), 1.f);
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

	const float ambientOcclusion = sampleAmbientOcclusion( texCoord, data.viewPosition.z );

	const float fogFactor = smoothstep( uniforms.fogNear, uniforms.fogFar, -data.viewPosition.z ) * uniforms.fogDensity;
	const vec3	color	  = texelFetch( inTextureColor, texCoord, 0 ).xyz * ambientOcclusion * lighting;

	outFragColor = vec4( linearToSrgb(mix( color, vec3( uniforms.colorFog ), fogFactor ) * vec3( uniforms.colorLight )), 1.f );
}
