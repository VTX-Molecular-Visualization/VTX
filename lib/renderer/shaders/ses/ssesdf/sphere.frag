#version 450

//#define SHOW_IMPOSTORS

layout (depth_greater) out float gl_FragDepth;

layout(std140, binding = 0) uniform SsesdfSettings
{
	mat4 uMVMatrix;
	mat4 uProjMatrix;
	float uProbeRadius;
};

smooth in vec3 viewImpPos;
flat in vec3   viewSpherePos;
flat in vec3   sphereColor;
flat in float  sphereRad;
flat in float  dotViewSpherePos;

// 3 16 bits for position.
// 3 16 bits for normal.
// 1 32 bits for padding.
layout( location = 0 ) out uvec4 outViewPositionNormal;
// 3 32 bits for color.
// 1 32 bits for specular.
layout( location = 1 ) out vec4 outColor;

float computeDepth( const vec3 v )
{
	// Computes 'v' NDC depth ([-1,1])
	const float ndcDepth = ( v.z * uProjMatrix[ 2 ].z + uProjMatrix[ 3 ].z ) / -v.z;
	// Return depth according to depth range
	return ( gl_DepthRange.diff * ndcDepth + gl_DepthRange.near + gl_DepthRange.far ) * 0.5f;
}

void main()
{
	const float a = dot( viewImpPos, viewImpPos );
	// b = -dot(viewImpPos, viewSpherePos);
	// But '-' is useless since 'b' is squared for 'delta'.
	// So for 't', '-' is also removed.
	const float b	  = dot( viewImpPos, viewSpherePos );
	const float c	  = dotViewSpherePos - sphereRad * sphereRad;
	const float delta = b * b - a * c;

	if ( delta < 0.f )
	{
#ifdef SHOW_IMPOSTORS
		// Show impostors for debugging purpose.
		uvec4 colorNormal = uvec4( 0 );
		// fill G-buffers.
		uvec4 viewPositionNormalCompressed;
		viewPositionNormalCompressed.x = packHalf2x16( viewImpPos.xy );
		viewPositionNormalCompressed.y = packHalf2x16( vec2( viewImpPos.z, -viewSpherePos.x ) );
		viewPositionNormalCompressed.z = packHalf2x16( -viewSpherePos.yz );
		viewPositionNormalCompressed.w = 0; // Padding.

		// Output data.
		outViewPositionNormal = viewPositionNormalCompressed;
		outColor			  = vec4( 1.f, 0.f, 0.f, 32.f ); // w = specular shininess.

		gl_FragDepth = computeDepth( viewImpPos );
#else
		discard;
#endif
	}
	else
	{
		// Solve equation (only first intersection).
		// '-' is removed (see 'b').
		const float t = ( b - sqrt( delta ) ) / a;

		// Compute hit point and normal (always in view space).
		const vec3 hit	  = viewImpPos * t;
		const vec3 normal = normalize( hit - viewSpherePos );

		// Fill depth buffer.
		gl_FragDepth = computeDepth( hit );

		// Compress position and normal.
		uvec4 viewPositionNormalCompressed;
		viewPositionNormalCompressed.x = packHalf2x16( hit.xy );
		viewPositionNormalCompressed.y = packHalf2x16( vec2( hit.z, normal.x ) );
		viewPositionNormalCompressed.z = packHalf2x16( normal.yz );
		viewPositionNormalCompressed.w = 0; // Padding.

		// Output data.
		outViewPositionNormal = viewPositionNormalCompressed;
		outColor			  = vec4( sphereColor, 32.f ); // w = specular shininess.
	}
}
