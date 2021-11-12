#version 450

//#define SHOW_IMPOSTORS

uniform mat4 u_projMatrix;

in GsOut
{
	smooth vec3 viewImpPos;	   // Impostor position in view space.
	flat vec3	viewSpherePos; // Sphere position in view space.
	flat vec3	sphereColor;
	flat float	sphereRadius;
	flat float	dotViewSpherePos;
	flat uint	sphereSelected;
	flat uint	sphereId;
}
gsIn;

// 3 16 bits for position.
// 3 16 bits for normal.
// 1 16 bits for padding.
// 1 16 bits for selection.
layout( location = 0 ) out uvec4 outViewPositionNormal;
// 3 32 bits for color.
// 1 32 bits for specular.
layout( location = 1 ) out vec4 outColor;
// 1 32 bits for id.
layout( location = 2 ) out uint outId;

float computeDepth( const vec3 v )
{
	// Computes 'v' NDC depth ([-1,1])
	const float ndcDepth = ( v.z * u_projMatrix[ 2 ].z + u_projMatrix[ 3 ].z ) / -v.z;
	// Return depth according to depth range
	return ( gl_DepthRange.diff * ndcDepth + gl_DepthRange.near + gl_DepthRange.far ) * 0.5f;
}

void main()
{
	const float a = dot( gsIn.viewImpPos, gsIn.viewImpPos );
	// b = -dot(viewImpPos, viewSpherePos);
	// But '-' is useless since 'b' is squared for 'delta'.
	// So for 't', '-' is also removed.
	const float b	  = dot( gsIn.viewImpPos, gsIn.viewSpherePos );
	const float c	  = gsIn.dotViewSpherePos - gsIn.sphereRadius * gsIn.sphereRadius;
	const float delta = b * b - a * c;

	if ( delta < 0.f )
	{
#ifdef SHOW_IMPOSTORS
		// Show impostors for debugging purpose.
		uvec4 colorNormal = uvec4( 0 );
		// Compress position and normal.
		uvec4 viewPositionNormalCompressed;
		viewPositionNormalCompressed.x = packHalf2x16( gsIn.viewImpPos.xy );
		viewPositionNormalCompressed.y = packHalf2x16( vec2( gsIn.viewImpPos.z, -gsIn.viewSpherePos.x ) );
		viewPositionNormalCompressed.z = packHalf2x16( -gsIn.viewSpherePos.yz );
		viewPositionNormalCompressed.w = packHalf2x16( vec2( gsIn.sphereSelected, 0 ) );

		// Output data.
		outViewPositionNormal = viewPositionNormalCompressed;
		outColor			  = vec4( 1.f, 0.f, 0.f, 32.f ); // w = specular shininess.

		gl_FragDepth = computeDepth( gsIn.viewImpPos );
#else
		outId				  = 0;
		discard;
#endif
	}
	else
	{
		// Solve equation (only first intersection).
		// '-' is removed (see 'b').
		const float t = ( b - sqrt( delta ) ) / a;

		// Compute hit point and normal (always in view space).
		const vec3 hit	  = gsIn.viewImpPos * t;
		const vec3 normal = normalize( hit - gsIn.viewSpherePos );

		// Fill depth buffer.
		gl_FragDepth = computeDepth( hit );

		// Compress position and normal.
		uvec4 viewPositionNormalCompressed;
		viewPositionNormalCompressed.x = packHalf2x16( hit.xy );
		viewPositionNormalCompressed.y = packHalf2x16( vec2( hit.z, normal.x ) );
		viewPositionNormalCompressed.z = packHalf2x16( normal.yz );
		viewPositionNormalCompressed.w = packHalf2x16( vec2( gsIn.sphereSelected, 0 ) );

		// Output data.
		outViewPositionNormal = viewPositionNormalCompressed;
		outColor			  = vec4( gsIn.sphereColor, 32.f ); // w = specular shininess.
		outId				  = gsIn.sphereId;
	}
}
