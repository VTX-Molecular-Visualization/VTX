#version 460 core

#include "common.glsl"

const float PI = 3.14159265359f;

float distributionGGX( const vec3 p_normal, const vec3 p_halfway, const float p_roughness )
{
	const float alpha		 = p_roughness * p_roughness;
	const float alphaSquared = alpha * alpha;
	const float nDotH		 = max( dot( p_normal, p_halfway ), 0.f );
	const float nDotHSquared = nDotH * nDotH;
	const float denominator	 = nDotHSquared * ( alphaSquared - 1.f ) + 1.f;

	return alphaSquared / max( PI * denominator * denominator, 0.0001f );
}

float geometrySchlickGGX( const float p_nDotDirection, const float p_roughness )
{
	const float roughness = p_roughness + 1.f;
	const float k		  = roughness * roughness / 8.f;

	return p_nDotDirection / max( p_nDotDirection * ( 1.f - k ) + k, 0.0001f );
}

float geometrySmith(
	const vec3	p_normal,
	const vec3	p_viewDirection,
	const vec3	p_lightDirection,
	const float p_roughness
)
{
	const float nDotV = max( dot( p_normal, p_viewDirection ), 0.f );
	const float nDotL = max( dot( p_normal, p_lightDirection ), 0.f );

	return geometrySchlickGGX( nDotV, p_roughness ) * geometrySchlickGGX( nDotL, p_roughness );
}

vec3 fresnelSchlick( const float p_cosTheta, const vec3 p_f0 )
{ return p_f0 + ( vec3( 1.f ) - p_f0 ) * pow( clamp( 1.f - p_cosTheta, 0.f, 1.f ), 5.f ); }

vec3 computePBR( const UnpackedData p_data, const vec3 p_lightDirection, const vec3 p_albedo )
{
	const vec3	normal		  = normalize( p_data.normal );
	const vec3	viewDirection = normalize( -p_data.viewPosition );
	const vec3	halfway		  = normalize( viewDirection + p_lightDirection );
	const float roughness	  = clamp( uniforms.materialRoughness, 0.04f, 1.f );
	const float metallic	  = clamp( uniforms.materialMetallic, 0.f, 1.f );
	const float nDotV		  = max( dot( normal, viewDirection ), 0.f );
	const float nDotL		  = max( dot( normal, p_lightDirection ), 0.f );
	const vec3	f0			  = mix( vec3( 0.04f ), p_albedo, metallic );
	const vec3	fresnel		  = fresnelSchlick( max( dot( halfway, viewDirection ), 0.f ), f0 );
	const float distribution  = distributionGGX( normal, halfway, roughness );
	const float geometry	  = geometrySmith( normal, viewDirection, p_lightDirection, roughness );
	const vec3	specular	  = distribution * geometry * fresnel / max( 4.f * nDotV * nDotL, 0.0001f );
	const vec3	diffuseWeight = ( vec3( 1.f ) - fresnel ) * ( 1.f - metallic );

	return ( diffuseWeight * p_albedo / PI + specular ) * nDotL;
}

void main()
{
	const ivec2 texCoord = ivec2( gl_FragCoord.xy );

	UnpackedData data;
	unpackData( inTexturePackedData, data, texCoord );
	if ( shadeBackground( data ) )
	{
		return;
	}

	const vec3 lightDirection
		= uniformsCamera.isCameraPerspective == 1 ? normalize( -data.viewPosition ) : vec3( 0.f, 0.f, 1.f );
	const vec3	albedo			 = texelFetch( inTextureColor, texCoord, 0 ).rgb;
	const float ambientOcclusion = sampleAmbientOcclusion( texCoord, data.viewPosition.z );
	const vec3 lightRadiance = uniforms.colorLight.rgb * 2.f; // LIGHT INTENSITY
	const vec3 directLighting = computePBR( data, lightDirection, albedo ) * lightRadiance;
	const vec3 emissive		  = uniforms.materialEmissive.rgb * uniforms.materialEmissive.a;
	const float fogFactor = smoothstep( uniforms.fogNear, uniforms.fogFar, -data.viewPosition.z ) * uniforms.fogDensity;

	const vec3 ambientLighting = albedo * 0.01f; // AMBIENT INTENSITY

	outFragColor = vec4( mix( directLighting + ( ambientLighting * ambientOcclusion ) + emissive, uniforms.colorFog.rgb, fogFactor ), 1.f );
}
