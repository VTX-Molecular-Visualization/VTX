#include "../layout_uniforms_camera.glsl"
#include "../struct_data_packed.glsl"

// In.
in vec3 worldDirection;
layout( binding = 0 ) uniform usampler2D inTexturePackedData;
layout( binding = 1 ) uniform sampler2D inTextureColor;
layout( binding = 2 ) uniform sampler2D inTextureAmbientOcclusion;
layout( binding = 3 ) uniform sampler2D inTextureDepth;
layout( binding = 4 ) uniform samplerCube inTextureEnvironment;
layout( binding = 5 ) uniform sampler2D inTextureMaterialAlbedo;
layout( binding = 6 ) uniform sampler2D inTextureMaterialNormal;
layout( binding = 7 ) uniform sampler2D inTextureMaterialMetallic;
layout( binding = 8 ) uniform sampler2D inTextureMaterialRoughness;
layout( binding = 9 ) uniform sampler2D inTextureMaterialAmbientOcclusion;
layout( binding = 10 ) uniform sampler2D inTextureMaterialEmissive;

layout( std140, binding = 11 ) uniform Uniforms
{
	vec4  colorBackground;
	vec4  colorLight;
	vec4  colorFog;
	float specularFactor;
	float shininess;
	uint  toonSteps;
	float fogNear;
	float fogFar;
	float fogDensity;
	float ssaoScale;
	uint  environmentEnabled;
	float skyboxIntensity;
	float iblIntensity;
	float environmentRotation;
	float lightIntensity;
	float ambientIntensity;
	uint  materialTextureMask;
	vec4  materialEmissive;
	float materialMetallic;
	float materialRoughness;
	float materialOpacity;
	float materialTextureScale;
}

uniforms;

// Out.
layout( location = 0 ) out vec4 outFragColor;

// Joint bilateral upsampling guided by depth.
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

	// Return the weighted average of the four surrounding AO samples.
	if ( totalWeight > 0.0001f )
	{
		return weightedAO / totalWeight;
	}

	// Thin geometry may have no matching low-resolution depth sample.
	const ivec2 nearestCoord = clamp( ivec2( round( aoPosition ) ), ivec2( 0 ), aoTextureSize - 1 );
	return texelFetch( inTextureAmbientOcclusion, nearestCoord, 0 ).x;
}

// Rotate env.
vec3 rotateEnvironmentDirection( vec3 p_direction )
{
	const float cosine = cos( uniforms.environmentRotation );
	const float sine   = sin( uniforms.environmentRotation );
	p_direction.xz	   = mat2( cosine, -sine, sine, cosine ) * p_direction.xz;

	return p_direction;
}

float computeFogFactor( const float p_viewDepth )
{
	const float fogFar = max( uniforms.fogFar, uniforms.fogNear + 0.0001f );
	return smoothstep( uniforms.fogNear, fogFar, p_viewDepth ) * uniforms.fogDensity;
}

// Shade the background with either the environment map, fog, or a solid color.
bool shadeBackground( const UnpackedData p_data )
{
	// Hit something, no background.
	if ( p_data.viewPosition.z != 0.f )
	{
		return false;
	}

	// Cubemap.
	if ( uniforms.environmentEnabled != 0u )
	{
		const vec3 direction = rotateEnvironmentDirection( normalize( worldDirection ) );
		outFragColor = vec4( texture( inTextureEnvironment, direction ).rgb * uniforms.skyboxIntensity, 1.f );
	}

	// Fog.
	else if ( uniforms.fogDensity != 0.f )
	{
		outFragColor = vec4(
			mix( uniforms.colorBackground.rgb, uniforms.colorFog.rgb, uniforms.fogDensity ),
			uniforms.colorBackground.a
		);
	}

	// Background.
	else
	{
		outFragColor = uniforms.colorBackground;
	}

	return true;
}
