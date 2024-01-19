#version 450 core

#include "../layout_uniforms_camera.glsl"
#include "../struct_data_packed.glsl"

//#define SHOW_IMPOSTORS

// In.
in
#include "struct_geometry_shader.glsl"
inData;

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
		const float a = dot( inData.viewImpPos, inData.viewImpPos );
		// b = -dot(viewImpPos, viewSpherePos);
		// But '-' is useless since 'b' is squared for 'delta'.
		// So for 't', '-' is also removed.
		const float b	  = dot( inData.viewImpPos, inData.viewSpherePos );
		const float c	  = inData.dotViewSpherePos - inData.sphereRadius * inData.sphereRadius;
		const float delta = b * b - a * c;

		if ( delta < 0.f )
		{
	#ifdef SHOW_IMPOSTORS
			// Show impostors for debugging purpose.
			uvec4 colorNormal = uvec4( 0 );
			packData( inData.viewImpPos, -inData.viewSpherePos, inData.sphereSelected, outDataPacked );
			outColor			  = vec4( 1.f, 0.f, 0.f, 1.f ); // w = specular shininess.

			gl_FragDepth = computeDepth( inData.viewImpPos );
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
			const vec3 hit	  = inData.viewImpPos * t;
			const vec3 normal = normalize( hit - inData.viewSpherePos );

			// Fill depth buffer.
			gl_FragDepth = computeDepth( hit );

			packData( hit, normal, inData.sphereSelected, outDataPacked );
			outColor			  = inData.sphereColor;
			outId				  = uvec2( inData.sphereId, 0 );
		}
	}
	else // Orthographic
	{ 
		const vec3 OmC    = inData.viewImpPos - inData.viewSpherePos;
		const float b	  = OmC.z;
		const float c	  = dot(OmC, OmC) - inData.sphereRadius * inData.sphereRadius;
		const float delta = OmC.z * OmC.z - c;

		if ( delta < 0.f )
		{
	#ifdef SHOW_IMPOSTORS
			// Show impostors for debugging purpose.
			uvec4 colorNormal = uvec4( 0 );

			// Output data.
			packData( inData.viewImpPos, -inData.viewSpherePos, inData.sphereSelected, outDataPacked );
			outViewPositionNormal = viewPositionNormalCompressed;
			outColor			  = vec4( 1.f, 0.f, 1.f, 1.f );

			gl_FragDepth = computeDepth( inData.viewImpPos );
	#else
			discard;
	#endif
		}
		else
		{
			const float t = b - sqrt( delta );

			// Compute hit point and normal (always in view space).
			const vec3 hit	  = inData.viewImpPos + vec3(0, 0, -t);
			const vec3 normal = normalize( hit - inData.viewSpherePos );

			// Fill depth buffer.
			gl_FragDepth = computeDepthOrtho( hit );

			// Output data.
			packData( hit, normal, inData.sphereSelected, outDataPacked );
			outColor			  = inData.sphereColor;
			outId				  = uvec2( inData.sphereId, 0 );
		}
	}
}
