#version 450 core

#include "../struct_data_packed.glsl"
#include "../layout_uniforms_camera.glsl"
#include "../layout_uniforms_representation.glsl"
#include "struct_geometry_shader.glsl"
#include "struct_cylinder.glsl"

//#define SHOW_IMPOSTORS

// In.
smooth in StructGeometryShaderSmooth gsDataSmooth;
flat in StructGeometryShaderFlat gsDataFlat[ 2 ];
flat in StructCylinder gsCylinder[ 2 ];

 // Out.
layout( location = 0 ) out PackedData outDataPacked;
layout( location = 1 ) out vec4 outColor;
layout( location = 2 ) out uvec2 outId;

float computeDepth( const vec3 p_v )
{
	// Computes 'v' NDC depth ([-1,1]).
	const float ndcDepth = ( p_v.z * uniformsCamera.matrixProjection[ 2 ].z + uniformsCamera.matrixProjection[ 3 ].z ) / -p_v.z;
	// Return depth according to depth range.
	return ( gl_DepthRange.diff * ndcDepth + gl_DepthRange.near + gl_DepthRange.far ) * 0.5f;
}

float computeDepthOrtho( const vec3 p_v )
{
	// Computes 'v' NDC depth ([-1,1]).
	const float ndcDepth = ( p_v.z * uniformsCamera.matrixProjection[ 2 ].z + uniformsCamera.matrixProjection[ 3 ].z );
	// Return depth according to depth range.
	return ( gl_DepthRange.diff * ndcDepth + gl_DepthRange.near + gl_DepthRange.far ) * 0.5f;
}

void main()
{
	uint representationId = gsCylinder[ 0 ].representation;
	float radiusCylinder	= uniformsRepresentation[ representationId ].radiusCylinder;
	uint colorBlendingMode	= uniformsRepresentation[ representationId ].cylinderColorBlendingMode;

	if ( uniformsCamera.isCameraPerspective == 1 )
	{
		// Only consider cylinder body.
		const vec3 v1v0	  = gsDataFlat[ 1 ].viewVertice - gsDataFlat[ 0 ].viewVertice;
		const vec3 v0	  = -gsDataFlat[ 0 ].viewVertice;
		const vec3 rayDir = normalize( gsDataSmooth.viewImpostorPosition );

		const float d0 = dot( v1v0, v1v0 );
		const float d1 = dot( v1v0, rayDir );
		const float d2 = dot( v1v0, v0 );

		const float a = d0 - d1 * d1;
		const float b = d0 * dot( v0, rayDir ) - d2 * d1;
		const float c = d0 * dot( v0, v0 ) - d2 * d2 - radiusCylinder * radiusCylinder * d0;

		const float h = b * b - a * c;

		if ( h < 0.f )
		{
	#ifdef SHOW_IMPOSTORS		
			// Output data.
			packData( gsDataSmooth.viewImpostorPosition, -rayDir, gsCylinder[ 0 ].isSelected & gsCylinder[ 1 ].isSelected, outDataPacked );
			outColor			  = vec4( 1.f, 0.f, 0.f, 1.f );

			gl_FragDepth = computeDepth( gsDataSmooth.viewImpostorPosition );
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
				// Output data.
				packData( gsDataSmooth.viewImpostorPosition, -rayDir, gsCylinder[ 0 ].isSelected & gsCylinder[ 1 ].isSelected, outDataPacked );
				outColor			  = vec4( 1.f, 0.f, 0.f, 1.f );

				gl_FragDepth = computeDepth( gsDataSmooth.viewImpostorPosition );
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
				vec4 color = gsCylinder[ int( y > d0 * 0.5f ) ].color;			
				if( colorBlendingMode == 1 ) // Gradient.
				{
					color = mix( gsCylinder[ 0 ].color, gsCylinder[ 1 ].color, y / d0 );
				}

				// Output data.
				packData( hit, normal, gsCylinder[ 0 ].isSelected & gsCylinder[ 1 ].isSelected, outDataPacked );
				outColor			  = color;

				const int selectAtom0 = 1 - int( floor( 0.85f + ( y / d0 ) ) );
				const int selectAtom1 = int( ceil( ( y / d0) - 0.85f ) );

				const uint id0 = selectAtom0 * gsCylinder[ 0 ].id + selectAtom1 * gsCylinder[ 1 ].id + (1-(selectAtom0 + selectAtom1)) * gsCylinder[ 0 ].id;
				const uint id1 = (1-(selectAtom0 + selectAtom1)) * gsCylinder[ 1 ].id; 

				outId				  = uvec2( id0, id1 );
			}
		}
	}
	else // Orthographic
	{ 
		// Only consider cylinder body.
		const vec3 v1v0	  = gsDataFlat[ 1 ].viewVertice - gsDataFlat[ 0 ].viewVertice;
		const vec3 v0	  = gsDataSmooth.viewImpostorPosition - gsDataFlat[ 0 ].viewVertice;
		const vec3 rayDir = vec3(0, 0, -1);

		const float d0 = dot( v1v0, v1v0 );
		const float d1 = -v1v0.z;
		const float d2 = dot( v1v0, v0 );

		const float a = d0 - d1 * d1;
		const float b = d0 * -v0.z - d2 * d1;
		const float c = d0 * dot( v0, v0 ) - d2 * d2 - radiusCylinder * radiusCylinder * d0;

		const float h = b * b - a * c;

		if ( h < 0.f )
		{
	#ifdef SHOW_IMPOSTORS
			// Output data.
			packData( gsDataSmooth.viewImpostorPosition, -rayDir, gsCylinder[ 0 ].isSelected & gsCylinder[ 1 ].isSelected, outDataPacked );
			outColor			  = vec4( 1.f, 0.f, 1.f, 1.f );

			gl_FragDepth = computeDepthOrtho( gsDataSmooth.viewImpostorPosition );
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
				// Output data.
				packData( gsDataSmooth.viewImpostorPosition, -rayDir, gsCylinder[ 0 ].isSelected & gsCylinder[ 1 ].isSelected, outDataPacked );
				outColor			  = vec4( 1.f, 0.f, 1.f, 1.f );

				gl_FragDepth = computeDepthOrtho( gsDataSmooth.viewImpostorPosition );
	#else
				discard;
	#endif
			}
			else
			{
				// Compute hit point and normal (always in view space).
				const vec3 hit	  = gsDataSmooth.viewImpostorPosition + vec3(0, 0, -t);
				const vec3 normal = normalize( hit - gsDataFlat[ 0 ].viewVertice - v1v0 * y / d0 );

				// Fill depth buffer.
				gl_FragDepth = computeDepthOrtho( hit );

				// Color with good color extremity.			
				vec4 color = gsCylinder[ int( y > d0 * 0.5f ) ].color;			
				if( colorBlendingMode == 1 ) // Gradient.
				{
					color = mix( gsCylinder[0].color, gsCylinder[1].color, y / d0 );
				}

				// Output data.
				packData( hit, normal, gsCylinder[ 0 ].isSelected & gsCylinder[ 1 ].isSelected, outDataPacked );
				outColor			  = color;

				const int selectAtom0 = 1 - int(floor(0.85f + (y / d0)) );
				const int selectAtom1 = int(ceil((y / d0) - 0.85f));

				const uint id0 = selectAtom0 * gsCylinder[ 0 ].id + selectAtom1 * gsCylinder[ 1 ].id + (1-(selectAtom0 + selectAtom1)) * gsCylinder[ 0 ].id;
				const uint id1 = (1-(selectAtom0 + selectAtom1)) * gsCylinder[ 1 ].id; 

				outId = uvec2( id0, id1 );
			}
		}
	}
}
