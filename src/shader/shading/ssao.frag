#version 450

// Crytek (Crysis) like SSAO

layout( binding = 0 ) uniform usampler2D gbColorNormal;
layout( binding = 1 ) uniform sampler2D gbViewPosition;
layout( binding = 2 ) uniform sampler2D noise;

layout( location = 0 ) out float ambientOcclusion;

const float BIAS	= 0.025f;

uniform mat4  uProjMatrix;
uniform vec3  uAoKernel[ 512 ]; // TODO: better use texture no ? ;-)
uniform int	  uAoIntensity;
uniform int	  uKernelSize;

struct FragmentData
{
	vec3 normal;
	vec3 viewPosition;
};

void unpackGBuffers( const in vec2 texPos, out FragmentData fd )
{
	const uvec4 colorNormal = texture( gbColorNormal, texPos, 0 );
	const vec4	viewPosition = texture( gbViewPosition, texPos, 0 );

	const vec2 tmp = unpackHalf2x16( colorNormal.y );

	fd.normal	   = vec3( tmp.y, unpackHalf2x16( colorNormal.z ) );
	fd.viewPosition = viewPosition.xyz;
}

void main()
{
	const vec2 texSize		= textureSize( gbViewPosition, 0 );
	const vec2 noisTextSize = textureSize( noise, 0 );
	const vec2 noiseScale	= texSize / noisTextSize;
	const vec2 texPos		= gl_FragCoord.xy / texSize;

	FragmentData fd;
	unpackGBuffers( texPos, fd );
	const vec3 pos = fd.viewPosition;

	// Adapt sharpness wrt depth: the deeper the fragment is, the larger the radius is.
	const float rad = -pos.z;

	const vec3 randomVec = normalize( texture( noise, texPos * noiseScale ).xyz );
	// Gram-Schmidt process.
	const vec3 tangent   = normalize( randomVec - fd.normal * dot( randomVec, fd.normal ) );
	const vec3 bitangent = cross( fd.normal, tangent );
	const mat3 TBN	   = mat3( tangent, bitangent, fd.normal );

	float ao = 0.f;

	for ( int i = 0; i < uKernelSize; ++i )
	{
		// Compute sample position.
		const vec3 samplePos = TBN * uAoKernel[ i ] * rad + pos;

		// Project sample position.
		vec4 offset = uProjMatrix * vec4( samplePos, 1.f );
		offset.xy /= offset.w;
		offset.xy = offset.xy * 0.5f + 0.5f;

		// Get sample depth.
		float sampleDepth = texture( gbViewPosition, offset.xy ).z;

		// Range check: ignore background.
		const float rangeCheck = sampleDepth == 0.f ? 0.f : smoothstep( 0.f, 1.f, rad / abs( pos.z - sampleDepth ) );
		ao += ( sampleDepth >= samplePos.z + BIAS ? 1.f : 0.f ) * rangeCheck;
	}

	ao				 = 1.f - ( ao / uKernelSize );
	ambientOcclusion = pow( ao, uAoIntensity );
}
