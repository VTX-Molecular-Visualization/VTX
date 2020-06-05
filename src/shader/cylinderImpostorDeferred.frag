#version 450

//#define SHOW_IMPOSTORS

uniform mat4  uProjMatrix;
uniform float uCylRad;

smooth in vec3 viewImpPos;
flat in vec3   viewCylVert[ 2 ];
flat in vec3   colors[ 2 ];

// 3 16 bits for color
// 3 16 bits for normal
// 1 32 bits for padding
layout( location = 0 ) out uvec4 outColorNormal;
// 3 32 bits for position in view space
// 1 32 bits for specular
layout( location = 1 ) out vec4 outViewPosition;

float computeDepth( const vec3 v )
{
	// Computes 'v' NDC depth ([-1,1])
	const float ndcDepth = ( v.z * uProjMatrix[ 2 ].z + uProjMatrix[ 3 ].z ) / -v.z;
	// Return depth according to depth range
	return ( gl_DepthRange.diff * ndcDepth + gl_DepthRange.near + gl_DepthRange.far ) * 0.5f;
}

// only consider cylinder body
void main()
{
	const vec3 v1v0	  = viewCylVert[ 1 ] - viewCylVert[ 0 ];
	const vec3 v0	  = -viewCylVert[ 0 ];
	const vec3 rayDir = normalize( viewImpPos );

	const float d0 = dot( v1v0, v1v0 );
	const float d1 = dot( v1v0, rayDir );
	const float d2 = dot( v1v0, v0 );

	const float a = d0 - d1 * d1;
	const float b = d0 * dot( v0, rayDir ) - d2 * d1;
	const float c = d0 * dot( v0, v0 ) - d2 * d2 - uCylRad * uCylRad * d0;

	const float h = b * b - a * c;

	if ( h < 0.f )
	{
#ifdef SHOW_IMPOSTORS
		// Show impostors for debugging purpose
		uvec4 colorNormal = uvec4( 0 );
		// fill G-buffers
		const vec3 normal = normalize( -rayDir );
		colorNormal.x	  = packHalf2x16( vec2( 1.0, 0.0 ) );
		colorNormal.y	  = packHalf2x16( vec2( 0.f, normal.x ) );
		colorNormal.z	  = packHalf2x16( normal.yz );
		colorNormal.w	  = 0; // padding

		outColorNormal	= colorNormal;
		outViewPosition = vec4( viewImpPos, 32.f ); // w = specular shininess;

		gl_FragDepth = computeDepth( viewImpPos );
#else
		discard;
#endif
	}
	else
	{
		// Solve equation (only first intersection)
		const float t = ( -b - sqrt( h ) ) / a;

		const float y = d2 + t * d1;

		if ( y < 0.f || y > d0 )
		{
#ifdef SHOW_IMPOSTORS
			// Show impostors for debugging purpose
			uvec4 colorNormal = uvec4( 0 );
			// fill G-buffers
			const vec3 normal = normalize( -rayDir );
			colorNormal.x	  = packHalf2x16( vec2( 1.0, 0.0 ) );
			colorNormal.y	  = packHalf2x16( vec2( 0.f, normal.x ) );
			colorNormal.z	  = packHalf2x16( normal.yz );
			colorNormal.w	  = 0; // padding

			outColorNormal	= colorNormal;
			outViewPosition = vec4( viewImpPos, 32.f ); // w = specular shininess;

			gl_FragDepth = computeDepth( viewImpPos );
#else
			discard;
#endif
		}
		else
		{
			// Compute hit point and normal (always in view space)
			const vec3 hit	  = rayDir * t;
			const vec3 normal = ( v0 + hit - v1v0 * y / d0 ) / uCylRad;

			// Fill depth buffer
			gl_FragDepth = computeDepth( hit );

			// Color with good color extremity
			const vec3 color = colors[ int( y > d0 * 0.5f ) ];

			// Compress color and normal
			uvec4 colorNormal;
			colorNormal.x = packHalf2x16( color.xy );
			colorNormal.y = packHalf2x16( vec2( color.z, normal.x ) );
			colorNormal.z = packHalf2x16( normal.yz );
			colorNormal.w = 0; // padding

			// Output data
			outColorNormal	= colorNormal;
			outViewPosition = vec4( hit, 32.f ); // w = specular shininess
		}
	}
}
