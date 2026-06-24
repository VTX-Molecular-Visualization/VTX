#version 460 core

#include "common.glsl"

const float PI					= 3.14159265359f;
const float TRIPLANAR_SHARPNESS = 4.f;
const float TRIPLANAR_SCALE_FACTOR    = 0.1f;

const uint MATERIAL_TEXTURE_ALBEDO			  = 1u << 0u;
const uint MATERIAL_TEXTURE_NORMAL			  = 1u << 1u;
const uint MATERIAL_TEXTURE_METALLIC		  = 1u << 2u;
const uint MATERIAL_TEXTURE_ROUGHNESS		  = 1u << 3u;
const uint MATERIAL_TEXTURE_AMBIENT_OCCLUSION = 1u << 4u;
const uint MATERIAL_TEXTURE_EMISSIVE		  = 1u << 5u;

// Triplanar UV.
struct TriplanarData
{
	vec2 uvX;
	vec2 uvY;
	vec2 uvZ;
	vec3 weights;
	vec3 normalWeights;
	vec3 signs;
};

// Sampled material properties.
struct SampledMaterial
{
	vec3  albedo;
	vec3  normal;
	float metallic;
	float roughness;
	float ambientOcclusion;
	vec3  emissive;
};

bool hasMaterialTexture( const uint p_texture ) { return ( uniforms.materialTextureMask & p_texture ) != 0u; }

// Triplanar projection for texture UV mapping.
TriplanarData buildTriplanarData( const vec3 p_worldPosition, const vec3 p_worldNormal )
{
	TriplanarData data;
	data.signs	 = step( vec3( 0.f ), p_worldNormal ) * 2.f - 1.f;
	data.uvX	 = vec2( -data.signs.x * p_worldPosition.z, p_worldPosition.y ) * uniforms.materialTextureScale  * TRIPLANAR_SCALE_FACTOR;
	data.uvY	 = vec2( -data.signs.y * p_worldPosition.x, p_worldPosition.z ) * uniforms.materialTextureScale * TRIPLANAR_SCALE_FACTOR;
	data.uvZ	 = vec2( data.signs.z * p_worldPosition.x, p_worldPosition.y ) * uniforms.materialTextureScale * TRIPLANAR_SCALE_FACTOR;
	data.weights = pow( abs( p_worldNormal ), vec3( TRIPLANAR_SHARPNESS ) );
	data.weights /= max( data.weights.x + data.weights.y + data.weights.z, 0.0001f );
	data.normalWeights = abs( p_worldNormal );
	data.normalWeights /= max( data.normalWeights.x + data.normalWeights.y + data.normalWeights.z, 0.0001f );

	return data;
}

// Sample a texture using triplanar data.
vec4 sampleTriplanar( sampler2D p_texture, const TriplanarData p_data )
{
	return texture( p_texture, p_data.uvX ) * p_data.weights.x + texture( p_texture, p_data.uvY ) * p_data.weights.y
		   + texture( p_texture, p_data.uvZ ) * p_data.weights.z;
}

// Sample normal texture.
vec3 sampleTriplanarNormal( sampler2D p_texture, const TriplanarData p_data )
{
	const vec3 normalX		= texture( p_texture, p_data.uvX ).xyz * 2.f - 1.f;
	const vec3 normalY		= texture( p_texture, p_data.uvY ).xyz * 2.f - 1.f;
	const vec3 normalZ		= texture( p_texture, p_data.uvZ ).xyz * 2.f - 1.f;
	const vec3 worldNormalX = vec3( p_data.signs.x * normalX.z, normalX.y, -p_data.signs.x * normalX.x );
	const vec3 worldNormalY = vec3( -p_data.signs.y * normalY.x, p_data.signs.y * normalY.z, normalY.y );
	const vec3 worldNormalZ = vec3( p_data.signs.z * normalZ.x, normalZ.y, p_data.signs.z * normalZ.z );

	return normalize(
		worldNormalX * p_data.normalWeights.x + worldNormalY * p_data.normalWeights.y
		+ worldNormalZ * p_data.normalWeights.z
	);
}

// Sample material properties from textures and uniforms.
SampledMaterial sampleMaterial( const UnpackedData p_data, const vec3 p_baseAlbedo )
{
	SampledMaterial material;
	material.albedo			  = p_baseAlbedo;
	material.normal			  = normalize( p_data.normal );
	material.metallic		  = clamp( uniforms.materialMetallic, 0.f, 1.f );
	material.roughness		  = clamp( uniforms.materialRoughness, 0.04f, 1.f );
	material.ambientOcclusion = 1.f;
	material.emissive		  = vec3( 0.f );

	if ( uniforms.materialTextureMask == 0u )
	{
		material.emissive = material.albedo * uniforms.materialEmissive.a;
		return material;
	}

	const vec3			worldPosition = ( uniformsCamera.matrixViewInv * vec4( p_data.viewPosition, 1.f ) ).xyz;
	const vec3			worldNormal	  = normalize( mat3( uniformsCamera.matrixViewInv ) * p_data.normal );
	const TriplanarData triplanar	  = buildTriplanarData( worldPosition, worldNormal );

	if ( hasMaterialTexture( MATERIAL_TEXTURE_ALBEDO ) )
	{
		material.albedo *= sampleTriplanar( inTextureMaterialAlbedo, triplanar ).rgb;
	}
	if ( hasMaterialTexture( MATERIAL_TEXTURE_NORMAL ) )
	{
		material.normal = normalize(
			mat3( uniformsCamera.matrixView ) * sampleTriplanarNormal( inTextureMaterialNormal, triplanar )
		);
	}
	if ( hasMaterialTexture( MATERIAL_TEXTURE_METALLIC ) )
	{
		material.metallic *= sampleTriplanar( inTextureMaterialMetallic, triplanar ).r;
	}
	if ( hasMaterialTexture( MATERIAL_TEXTURE_ROUGHNESS ) )
	{
		material.roughness *= sampleTriplanar( inTextureMaterialRoughness, triplanar ).r;
	}
	if ( hasMaterialTexture( MATERIAL_TEXTURE_AMBIENT_OCCLUSION ) )
	{
		material.ambientOcclusion = sampleTriplanar( inTextureMaterialAmbientOcclusion, triplanar ).r;
	}

	material.emissive = material.albedo * uniforms.materialEmissive.a;
	if ( hasMaterialTexture( MATERIAL_TEXTURE_EMISSIVE ) )
	{
		material.emissive = sampleTriplanar( inTextureMaterialEmissive, triplanar ).rgb * uniforms.materialEmissive.a;
	}

	// Clamp.
	material.metallic		  = clamp( material.metallic, 0.f, 1.f );
	material.roughness		  = clamp( material.roughness, 0.04f, 1.f );
	material.ambientOcclusion = clamp( material.ambientOcclusion, 0.f, 1.f );

	return material;
}

// Distribution function based on GGX.
float distributionGGX( const vec3 p_normal, const vec3 p_halfway, const float p_roughness )
{
	const float alpha		 = p_roughness * p_roughness;
	const float alphaSquared = alpha * alpha;
	const float nDotH		 = max( dot( p_normal, p_halfway ), 0.f );
	const float nDotHSquared = nDotH * nDotH;
	const float denominator	 = nDotHSquared * ( alphaSquared - 1.f ) + 1.f;

	return alphaSquared / max( PI * denominator * denominator, 0.0001f );
}

// Geometry function based on Schlick-GGX approximation.
float geometrySchlickGGX( const float p_nDotDirection, const float p_roughness )
{
	const float roughness = p_roughness + 1.f;
	const float k		  = roughness * roughness / 8.f;

	return p_nDotDirection / max( p_nDotDirection * ( 1.f - k ) + k, 0.0001f );
}

// Geometry function based on Smith's method.
float geometrySmith( const float p_nDotV, const float p_nDotL, const float p_roughness )
{
	return geometrySchlickGGX( p_nDotV, p_roughness ) * geometrySchlickGGX( p_nDotL, p_roughness );
}

// Fresnel function based on Schlick's approximation.
vec3 fresnelSchlick( const float p_cosTheta, const vec3 p_f0 )
{
	return p_f0 + ( vec3( 1.f ) - p_f0 ) * pow( clamp( 1.f - p_cosTheta, 0.f, 1.f ), 5.f );
}

// Compute PBR lighting using the Cook-Torrance model.
vec3 computePBR( const vec3 p_viewPosition, const vec3 p_lightDirection, const SampledMaterial p_material )
{
	const vec3	normal		  = normalize( p_material.normal );
	const vec3	viewDirection = normalize( -p_viewPosition );
	const vec3	halfway		  = normalize( viewDirection + p_lightDirection );

	const float roughness	  = p_material.roughness;
	const float metallic	  = p_material.metallic;

	const float nDotV		  = max( dot( normal, viewDirection ), 0.f );
	const float nDotL		  = max( dot( normal, p_lightDirection ), 0.f );

	const vec3	f0			  = mix( vec3( 0.04f ), p_material.albedo, metallic );
	const vec3	fresnel		  = fresnelSchlick( max( dot( halfway, viewDirection ), 0.f ), f0 );

	const float distribution  = distributionGGX( normal, halfway, roughness );
	const float geometry	  = geometrySmith( nDotV, nDotL, roughness );

	const vec3	specular	  = distribution * geometry * fresnel / max( 4.f * nDotV * nDotL, 0.0001f );
	const vec3	diffuseWeight = ( vec3( 1.f ) - fresnel ) * ( 1.f - metallic );

	return ( diffuseWeight * p_material.albedo / PI + specular ) * nDotL;
}

// Compute environment lighting using IBL.
vec3 computeEnvironmentLighting( const vec3 p_viewPosition, const SampledMaterial p_material )
{
	// Use ambient if no env map.
	if ( uniforms.environmentEnabled == 0u )
	{
		return p_material.albedo * uniforms.ambientIntensity;
	}

	const vec3 normal			   = normalize( p_material.normal );
	const vec3 viewDirection	   = normalize( -p_viewPosition );
	const vec3 reflectionDirection = rotateEnvironmentDirection(
		normalize( mat3( uniformsCamera.matrixViewInv ) * reflect( -viewDirection, normal ) )
	);
	const float roughness		 = p_material.roughness;
	const float metallic		 = p_material.metallic;
	const vec3	f0				 = mix( vec3( 0.04f ), p_material.albedo, metallic );
	const vec3	fresnel			 = fresnelSchlick( max( dot( normal, viewDirection ), 0.f ), f0 );
	const float maxMipLevel		 = float( max( textureQueryLevels( inTextureEnvironment ) - 1, 0 ) );
	const vec3	environmentColor = textureLod( inTextureEnvironment, reflectionDirection, roughness * maxMipLevel ).rgb;

	return environmentColor * uniforms.iblIntensity * fresnel;
}

void main()
{
	const ivec2 texCoord = ivec2( gl_FragCoord.xy );

	UnpackedData data;
	unpackData( inTexturePackedData, data, texCoord );

	// Return if background.
	if ( shadeBackground( data ) )
	{
		return;
	}

	// Use camera as light.
	const vec3 lightDirection
		= uniformsCamera.isCameraPerspective == 1 ? normalize( -data.viewPosition ) : vec3( 0.f, 0.f, 1.f );

	// Albedo from previous pass.
	const vec3 baseAlbedo = texelFetch( inTextureColor, texCoord, 0 ).rgb;

	// Sample material.
	const SampledMaterial material = sampleMaterial( data, baseAlbedo );

	// Sample AO and apply texture AO.
	const float ambientOcclusion = sampleAmbientOcclusion( texCoord, data.viewPosition.z ) * material.ambientOcclusion;

	// Light.
	const vec3	lightRadiance = uniforms.colorLight.rgb * uniforms.lightIntensity;

	// Compute direct lighting.
	const vec3	directLighting = computePBR( data.viewPosition, lightDirection, material ) * lightRadiance;

	// Compute env lighting.
	const vec3	environmentLighting = computeEnvironmentLighting( data.viewPosition, material );

	// Fog.
	const float fogFactor = computeFogFactor( -data.viewPosition.z );

	// Indirect lighting.
	const vec3 indirectLighting = environmentLighting * ambientOcclusion;
	
	// Combine direct and indirect lighting with emissive.
	vec3 sceneColor = directLighting + indirectLighting + material.emissive;
	
	// Apply fog.
	sceneColor = mix(sceneColor, uniforms.colorFog.rgb, fogFactor);

	outFragColor = vec4( sceneColor, 1.0 );
}
