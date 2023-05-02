#version 450

//#define SHOW_IMPOSTORS

uniform mat4 u_projMatrix;
uniform bool u_isPerspective;

in GsOut
{
	smooth vec3 viewImpPos;	   // Impostor position in view space.
	flat vec3	viewSpherePos; // Sphere position in view space.
	flat vec4	sphereColor;
	flat float	sphereRadius;
	flat float  dotViewSpherePos;
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
layout( location = 2 ) out uvec2 outId;

float computeDepth( const vec3 v )
{
	// Computes 'v' NDC depth ([-1,1])
	const float ndcDepth = ( v.z * u_projMatrix[ 2 ].z + u_projMatrix[ 3 ].z ) / -v.z;
	// Return depth according to depth range
	return ( gl_DepthRange.diff * ndcDepth + gl_DepthRange.near + gl_DepthRange.far ) * 0.5f;
}

float computeDepthOrtho( const vec3 v )
{
	// Computes 'v' NDC depth ([-1,1])
	const float ndcDepth = ( v.z * u_projMatrix[ 2 ].z + u_projMatrix[ 3 ].z );
	// Return depth according to depth range
	return ( gl_DepthRange.diff * ndcDepth + gl_DepthRange.near + gl_DepthRange.far ) * 0.5f;
}

void main()
{
	if (u_isPerspective){
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
			outColor			  = vec4( gsIn.sphereColor.xyz, 32.f ); // w = specular shininess.
			outId				  = uvec2( gsIn.sphereId, 0 );
		}
	}
	else // Orthographic
	{ 
		const vec3 OmC    = gsIn.viewImpPos - gsIn.viewSpherePos;
		const float b	  = OmC.z;
		const float c	  = dot(OmC, OmC) - gsIn.sphereRadius * gsIn.sphereRadius;
		const float delta = OmC.z * OmC.z - c;

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
			outColor			  = vec4( 1.f, 0.f, 1.f, 32.f ); // w = specular shininess.

			gl_FragDepth = computeDepth( gsIn.viewImpPos );
	#else
			discard;
	#endif
		}
		else
		{
			const float t = b - sqrt( delta );

			// Compute hit point and normal (always in view space).
			const vec3 hit	  = gsIn.viewImpPos + vec3(0, 0, -t);
			const vec3 normal = normalize( hit - gsIn.viewSpherePos );

			// Fill depth buffer.
			gl_FragDepth = computeDepthOrtho( hit );

			// Compress position and normal.
			uvec4 viewPositionNormalCompressed;
			viewPositionNormalCompressed.x = packHalf2x16( hit.xy );
			viewPositionNormalCompressed.y = packHalf2x16( vec2( hit.z, normal.x ) );
			viewPositionNormalCompressed.z = packHalf2x16( normal.yz );
			viewPositionNormalCompressed.w = packHalf2x16( vec2( gsIn.sphereSelected, 0 ) );

			// Output data.
			outViewPositionNormal = viewPositionNormalCompressed;
			outColor			  = vec4( gsIn.sphereColor.xyz, 32.f ); // w = specular shininess.
			outId				  = uvec2( gsIn.sphereId, 0 );
		}
	}
}
