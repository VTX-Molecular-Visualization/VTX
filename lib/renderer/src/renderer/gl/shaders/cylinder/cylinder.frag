#version 450

//#define SHOW_IMPOSTORS

uniform mat4  u_projMatrix;
uniform float u_cylRad;
uniform uint  u_colorBlendingMode;
uniform bool  u_isPerspective;

in
#include "struct_geometry_shader.glsl"
dataIn;

// 3 16 bits for position.
// 3 16 bits for normal.
// 1 16 bits for selection.
// 1 16 bits for padding.
layout( location = 0 ) out uvec4 outViewPositionNormal;
// 3 32 bits for color.
// 1 32 bits for specular.
layout( location = 1 ) out vec4 outColor;
layout( location = 2 ) out uvec2 outId;

float computeDepth( const vec3 v )
{
	// Computes 'v' NDC depth ([-1,1]).
	const float ndcDepth = ( v.z * u_projMatrix[ 2 ].z + u_projMatrix[ 3 ].z ) / -v.z;
	// Return depth according to depth range.
	return ( gl_DepthRange.diff * ndcDepth + gl_DepthRange.near + gl_DepthRange.far ) * 0.5f;
}

float computeDepthOrtho( const vec3 v )
{
	// Computes 'v' NDC depth ([-1,1]).
	const float ndcDepth = ( v.z * u_projMatrix[ 2 ].z + u_projMatrix[ 3 ].z );
	// Return depth according to depth range.
	return ( gl_DepthRange.diff * ndcDepth + gl_DepthRange.near + gl_DepthRange.far ) * 0.5f;
}

void main()
{
	if (u_isPerspective)
	{
		// Only consider cylinder body.
		const vec3 v1v0	  = dataIn.viewVertices[ 1 ] - dataIn.viewVertices[ 0 ];
		const vec3 v0	  = -dataIn.viewVertices[ 0 ];
		const vec3 rayDir = normalize( dataIn.viewImpostorPosition );

		const float d0 = dot( v1v0, v1v0 );
		const float d1 = dot( v1v0, rayDir );
		const float d2 = dot( v1v0, v0 );

		const float a = d0 - d1 * d1;
		const float b = d0 * dot( v0, rayDir ) - d2 * d1;
		const float c = d0 * dot( v0, v0 ) - d2 * d2 - u_cylRad * u_cylRad * d0;

		const float h = b * b - a * c;

		if ( h < 0.f )
		{
	#ifdef SHOW_IMPOSTORS
			// Show impostors for debugging purpose.
			uvec4 colorNormal = uvec4( 0 );
			// Compress position and normal.
			uvec4 viewPositionNormalCompressed;
			viewPositionNormalCompressed.x = packHalf2x16( dataIn.viewImpostorPosition.xy );
			viewPositionNormalCompressed.y = packHalf2x16( vec2( dataIn.viewImpostorPosition.z, -rayDir.x ) );
			viewPositionNormalCompressed.z = packHalf2x16( -rayDir.yz );
			viewPositionNormalCompressed.w = packHalf2x16( vec2( dataIn.vertexSelected[ 0 ] & dataIn.vertexSelected[ 1 ], 0 ) );

			// Output data.
			outViewPositionNormal = viewPositionNormalCompressed;
			outColor			  = vec4( 1.f, 0.f, 0.f, 32.f ); // w = specular shininess.

			gl_FragDepth = computeDepth( dataIn.viewImpostorPosition );
	#else
			discard;
	#endif
		}
		else
		{
			// Solve equation (only first intersection).
			const float t = ( -b - sqrt( h ) ) / a;

			const float y = d2 + t * d1;

			if ( y < 0.f || y > d0 )
			{
	#ifdef SHOW_IMPOSTORS
				// Compress position and normal.
				uvec4 viewPositionNormalCompressed;
				viewPositionNormalCompressed.x = packHalf2x16( dataIn.viewImpostorPosition.xy );
				viewPositionNormalCompressed.y = packHalf2x16( vec2( dataIn.viewImpostorPosition.z, -rayDir.x ) );
				viewPositionNormalCompressed.z = packHalf2x16( -rayDir.yz );
				viewPositionNormalCompressed.w
					= packHalf2x16( vec2( dataIn.vertexSelected[ 0 ] & dataIn.vertexSelected[ 1 ], 0 ) );

				// Output data.
				outViewPositionNormal = viewPositionNormalCompressed;
				outColor			  = vec4( 1.f, 0.f, 0.f, 32.f ); // w = specular shininess.

				gl_FragDepth = computeDepth( dataIn.viewImpostorPosition );
	#else
				discard;
	#endif
			}
			else
			{
				// Compute hit point and normal (always in view space).
				const vec3 hit	  = rayDir * t;
				const vec3 normal = normalize( v0 + hit - v1v0 * y / d0 );

				// Fill depth buffer.
				gl_FragDepth = computeDepth( hit );

				// Color with good color extremity.			
				vec4 color = dataIn.colors[ int( y > d0 * 0.5f ) ];			
				if( u_colorBlendingMode == 1 ) // Gradient.
				{
					color = mix( dataIn.colors[0], dataIn.colors[1], y / d0 );
				}

				// Compress position and normal.
				uvec4 viewPositionNormalCompressed;
				viewPositionNormalCompressed.x = packHalf2x16( hit.xy );
				viewPositionNormalCompressed.y = packHalf2x16( vec2( hit.z, normal.x ) );
				viewPositionNormalCompressed.z = packHalf2x16( normal.yz );
				viewPositionNormalCompressed.w
					= packHalf2x16( vec2( dataIn.vertexSelected[ 0 ] & dataIn.vertexSelected[ 1 ], 0 ) );

				// Output data.
				outViewPositionNormal = viewPositionNormalCompressed;
				outColor			  = vec4( color.xyz, 32.f ); // w = specular shininess.

				const int selectAtom0 = 1 - int(floor(0.85f + (y / d0)) );
				const int selectAtom1 = int(ceil((y / d0) - 0.85f));

				const uint id0 = selectAtom0 * dataIn.vertexId[ 0 ] + selectAtom1 * dataIn.vertexId[ 1 ] + (1-(selectAtom0 + selectAtom1)) *dataIn.vertexId[ 0 ];
				const uint id1 = (1-(selectAtom0 + selectAtom1)) *dataIn.vertexId[ 1 ]; 

				outId				  = ivec2( id0, id1 );
			}
		}
	}
	else // Orthographic
	{ 
		// Only consider cylinder body.
		const vec3 v1v0	  = dataIn.viewVertices[ 1 ] - dataIn.viewVertices[ 0 ];
		const vec3 v0	  = dataIn.viewImpostorPosition - dataIn.viewVertices[ 0 ];
		const vec3 rayDir = vec3(0, 0, -1);

		const float d0 = dot( v1v0, v1v0 );
		const float d1 = -v1v0.z;
		const float d2 = dot( v1v0, v0 );

		const float a = d0 - d1 * d1;
		const float b = d0 * -v0.z - d2 * d1;
		const float c = d0 * dot( v0, v0 ) - d2 * d2 - u_cylRad * u_cylRad * d0;

		const float h = b * b - a * c;

		if ( h < 0.f )
		{
	#ifdef SHOW_IMPOSTORS
			// Show impostors for debugging purpose.
			uvec4 colorNormal = uvec4( 0 );
			// Compress position and normal.
			uvec4 viewPositionNormalCompressed;
			viewPositionNormalCompressed.x = packHalf2x16( dataIn.viewImpostorPosition.xy );
			viewPositionNormalCompressed.y = packHalf2x16( vec2( dataIn.viewImpostorPosition.z, -rayDir.x ) );
			viewPositionNormalCompressed.z = packHalf2x16( -rayDir.yz );
			viewPositionNormalCompressed.w = packHalf2x16( vec2( dataIn.vertexSelected[ 0 ] & dataIn.vertexSelected[ 1 ], 0 ) );

			// Output data.
			outViewPositionNormal = viewPositionNormalCompressed;
			outColor			  = vec4( 1.f, 0.f, 1.f, 32.f ); // w = specular shininess.

			gl_FragDepth = computeDepthOrtho( dataIn.viewImpostorPosition );
	#else
			discard;
	#endif
		}
		else
		{
			// Solve equation (only first intersection).
			const float t = ( -b - sqrt( h ) ) / a;

			const float y = d2 + t * d1;

			if ( y < 0.f || y > d0 )
			{
	#ifdef SHOW_IMPOSTORS
				// Compress position and normal.
				uvec4 viewPositionNormalCompressed;
				viewPositionNormalCompressed.x = packHalf2x16( dataIn.viewImpostorPosition.xy );
				viewPositionNormalCompressed.y = packHalf2x16( vec2( dataIn.viewImpostorPosition.z, -rayDir.x ) );
				viewPositionNormalCompressed.z = packHalf2x16( -rayDir.yz );
				viewPositionNormalCompressed.w
					= packHalf2x16( vec2( dataIn.vertexSelected[ 0 ] & dataIn.vertexSelected[ 1 ], 0 ) );

				// Output data.
				outViewPositionNormal = viewPositionNormalCompressed;
				outColor			  = vec4( 1.f, 0.f, 1.f, 32.f ); // w = specular shininess.

				gl_FragDepth = computeDepthOrtho( dataIn.viewImpostorPosition );
	#else
				discard;
	#endif
			}
			else
			{
				// Compute hit point and normal (always in view space).
				const vec3 hit	  = dataIn.viewImpostorPosition + vec3(0, 0, -t);
				const vec3 normal = normalize( hit - dataIn.viewVertices[ 0 ] - v1v0 * y / d0 );

				// Fill depth buffer.
				gl_FragDepth = computeDepthOrtho( hit );

				// Color with good color extremity.			
				vec4 color = dataIn.colors[ int( y > d0 * 0.5f ) ];			
				if( u_colorBlendingMode == 1 ) // Gradient.
				{
					color = mix( dataIn.colors[0], dataIn.colors[1], y / d0 );
				}

				// Compress position and normal.
				uvec4 viewPositionNormalCompressed;
				viewPositionNormalCompressed.x = packHalf2x16( hit.xy );
				viewPositionNormalCompressed.y = packHalf2x16( vec2( hit.z, normal.x ) );
				viewPositionNormalCompressed.z = packHalf2x16( normal.yz );
				viewPositionNormalCompressed.w
					= packHalf2x16( vec2( dataIn.vertexSelected[ 0 ] & dataIn.vertexSelected[ 1 ], 0 ) );

				// Output data.
				outViewPositionNormal = viewPositionNormalCompressed;
				outColor			  = vec4( color.xyz, 32.f ); // w = specular shininess.

				const int selectAtom0 = 1 - int(floor(0.85f + (y / d0)) );
				const int selectAtom1 = int(ceil((y / d0) - 0.85f));

				const uint id0 = selectAtom0 * dataIn.vertexId[ 0 ] + selectAtom1 * dataIn.vertexId[ 1 ] + (1-(selectAtom0 + selectAtom1)) *dataIn.vertexId[ 0 ];
				const uint id1 = (1-(selectAtom0 + selectAtom1)) *dataIn.vertexId[ 1 ]; 

				outId				  = ivec2( id0, id1 );
			}
		}
	}
}
