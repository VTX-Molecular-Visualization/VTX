#version 450 core

#include "../layout_uniforms_camera.glsl"
#include "../struct_data_packed.glsl"
#include "struct_sphere.glsl"
#include "struct_geometry_shader.glsl"

//#define SHOW_IMPOSTORS

// In.
smooth in StructGeometryShader gsData;
flat in StructSphere gsSphere;

 // Out.
layout( location = 0 ) out PackedData outDataPacked;
layout( location = 1 ) out vec4 outColor;
layout( location = 2 ) out uvec2 outId;

float computeDepth( const vec3 p_v )
{
	// Computes 'v' NDC depth ([-1,1])
	const float ndcDepth = ( p_v.z * uniformsCamera.matrixProjection[ 2 ].z + uniformsCamera.matrixProjection[ 3 ].z ) / -p_v.z;
	// Return depth according to depth range
	return ( gl_DepthRange.diff * ndcDepth + gl_DepthRange.near + gl_DepthRange.far ) * 0.5f;
}

float computeDepthOrtho( const vec3 p_v )
{
	// Computes 'v' NDC depth ([-1,1])
	const float ndcDepth = ( p_v.z * uniformsCamera.matrixProjection[ 2 ].z + uniformsCamera.matrixProjection[ 3 ].z );
	// Return depth according to depth range
	return ( gl_DepthRange.diff * ndcDepth + gl_DepthRange.near + gl_DepthRange.far ) * 0.5f;
}

void main()
{
	if ( uniformsCamera.isCameraPerspective == 1 )
	{
		const float a = dot( gsData.viewImpPos, gsData.viewImpPos );
		// b = -dot(viewImpPos, viewSpherePos);
		// But '-' is useless since 'b' is squared for 'delta'.
		// So for 't', '-' is also removed.
		const float b	  = dot( gsData.viewImpPos, gsSphere.viewPos );
		const float c	  = gsSphere.dotViewPos - gsSphere.radius * gsSphere.radius;
		const float delta = b * b - a * c;

		if ( delta < 0.f )
		{
	#ifdef SHOW_IMPOSTORS
			// Show impostors for debugging purpose.
			uvec4 colorNormal = uvec4( 0 );
			packData( gsData.viewImpPos, -gsSphere.viewPos, gsSphere.isSelected, outDataPacked );
			outColor			  = vec4( 1.f, 0.f, 0.f, 1.f ); // w = specular shininess.

			gl_FragDepth = computeDepth( gsData.viewImpPos );
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
			const vec3 hit	  = gsData.viewImpPos * t;
			const vec3 normal = normalize( hit - gsSphere.viewPos );

			// Fill depth buffer.
			gl_FragDepth = computeDepth( hit );

			packData( hit, normal, gsSphere.isSelected, outDataPacked );
			outColor			  = gsSphere.color;
			outId				  = uvec2( gsSphere.id, 0 );
		}
	}
	else // Orthographic
	{ 
		const vec3 OmC    = gsData.viewImpPos - gsSphere.viewPos;
		const float b	  = OmC.z;
		const float c	  = dot(OmC, OmC) - gsSphere.radius * gsSphere.radius;
		const float delta = OmC.z * OmC.z - c;

		if ( delta < 0.f )
		{
	#ifdef SHOW_IMPOSTORS
			// Show impostors for debugging purpose.
			uvec4 colorNormal = uvec4( 0 );

			// Output data.
			packData( gsData.viewImpPos, -gsSphere.viewPos, gsSphere.isSelected, outDataPacked );
			outViewPositionNormal = viewPositionNormalCompressed;
			outColor			  = vec4( 1.f, 0.f, 1.f, 1.f );

			gl_FragDepth = computeDepth( gsData.viewImpPos );
	#else
			discard;
	#endif
		}
		else
		{
			const float t = b - sqrt( delta );

			// Compute hit point and normal (always in view space).
			const vec3 hit	  = gsData.viewImpPos + vec3(0, 0, -t);
			const vec3 normal = normalize( hit - gsSphere.viewPos );

			// Fill depth buffer.
			gl_FragDepth = computeDepthOrtho( hit );

			// Output data.
			packData( hit, normal, gsSphere.isSelected, outDataPacked );
			outColor			  = gsSphere.color;
			outId				  = uvec2( gsSphere.id, 0 );
		}
	}
}
