#version 450

//#define SHOW_IMPOSTORS

uniform mat4  uProjMatrix;
uniform float uZNear;
uniform float uZFar;

smooth in vec3 viewImpPos;
flat in vec3   viewSpherePos;
flat in vec3   sphereColor;
flat in float  sphereRad;
flat in float  dotViewSpherePos;

// 3 16 bits for color
// 3 16 bits for normal
// 1 32 bits for padding
layout( location = 0 ) out uvec4 outColorNormal;
// 3 32 bits for position in view space
// 1 32 bits for specular
layout( location = 1 ) out vec4 outViewPosition;

float computeDepth( const vec3 v )
{
	// Computes 'v' clip depth then NDC depth ([-1,1]) mapped to [0,1]
	const float clipDetph  = ( v.z * uProjMatrix[ 2 ].z + uProjMatrix[ 3 ].z );
	const float ndcDepth01 = fma( clipDetph / -v.z, 0.5f, 0.5f );
	// Return depth according to depth range
	return ( gl_DepthRange.diff * ndcDepth01 + gl_DepthRange.near + gl_DepthRange.far ) * 0.5f;
}

void main()
{
	const float a = dot( viewImpPos, viewImpPos );
	// b = -dot(viewImpPos, viewSpherePos);
	// but '-' is useless since 'b' is squared for 'delta'
	// so for 't', '-' is also removed
	const float b	  = dot( viewImpPos, viewSpherePos );
	const float c	  = dotViewSpherePos - sphereRad * sphereRad;
	const float delta = b * b - a * c;

	if ( delta < 0.f )
	{
#ifdef SHOW_IMPOSTORS
		// Show impostors for debugging purpose
		uvec4 colorNormal = uvec4( 0 );
		// fill G-buffers
		const vec3 normal = normalize( -viewSpherePos );
		colorNormal.x	  = packHalf2x16( vec2( 1.0, 0.0 ) );
		colorNormal.y	  = packHalf2x16( vec2( 0.f, normal.x ) );
		colorNormal.z	  = packHalf2x16( normal.yz );
		colorNormal.w	  = 0; // padding

		outColorNormal	= colorNormal;
		outViewPosition = vec4( viewImpPos, 64.f ); // w = specular shininess;

		gl_FragDepth = computeDepth( viewImpPos );
#else
		discard;
#endif
	}
	else
	{
		// Solve equation (only first intersection)
		// '-' is removed (see 'b')
		const float t = ( b - sqrt( delta ) ) / a;

		// Compute hit point and normal (always in view space)
		const vec3 hit	  = viewImpPos * t;
		const vec3 normal = normalize( hit - viewSpherePos );

		// Fill depth buffer
		float depth	 = computeDepth( hit );
		gl_FragDepth = depth; // computeDepth( hit );

		// Compress color and normal
		uvec4 colorNormal = uvec4( 0 );
		colorNormal.x	  = packHalf2x16( sphereColor.xy );
		colorNormal.y	  = packHalf2x16( vec2( sphereColor.z, normal.x ) );
		colorNormal.z	  = packHalf2x16( normal.yz );
		colorNormal.w	  = 0; // padding

		// Output data
		outColorNormal	= colorNormal;
		outViewPosition = vec4( hit, 64.f ); // w = specular shininess
	}
}
