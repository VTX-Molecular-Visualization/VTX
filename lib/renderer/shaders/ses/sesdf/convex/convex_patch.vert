#version 460 core

#include "../../../constant.glsl"
#include "../../../layout_indexed_indirect_draws.glsl"
#include "../../../layout_uniforms_camera.glsl"
#include "../../../layout_uniforms_color.glsl"
#include "../../../layout_uniforms_model.glsl"
#include "struct_convex_patch.glsl"
#include "struct_vertex_shader.glsl"

// In.
layout( location = 0 ) in uvec2 elementIds;

layout( std140, binding = 1 ) readonly buffer SortedAtoms { vec4 atoms[]; };

layout( std430, binding = 7 ) readonly buffer SESAtomIds { uint rendererAtomIds[]; };

layout( std430, binding = 8 ) readonly buffer AtomColors { uint atomColorWords[]; };

layout( std430, binding = 9 ) readonly buffer AtomFlags { uint atomFlagWords[]; };

// Out.
flat out StructVertexShader vsData;
flat out StructConvexPatch	vsPatchData;

uint readPackedAtomColor( const uint p_index )
{
	const uint word = atomColorWords[ p_index >> 2 ];
	return ( word >> ( ( p_index & 3u ) * 8u ) ) & 0xFFu;
}

uint readPackedAtomFlag( const uint p_index )
{
	const uint word = atomFlagWords[ p_index >> 2 ];
	return ( word >> ( ( p_index & 3u ) * 8u ) ) & 0xFFu;
}

vec4 sesColor( const vec4 p_atomColor ) { return vec4( p_atomColor.rgb, 1.f ); }

void main()
{
	const DrawIndexedIndirectRecord draw			 = indexedDraws[ gl_DrawID ];
	const uint						idModel			 = draw.idModel;
	const uint						representationId = draw.padding1;
	const uint						atomId			 = draw.padding0 + uint( gl_VertexID );
	const uint						rendererAtomId	 = rendererAtomIds[ atomId ];

	const vec4 ithData		   = atoms[ atomId ];
	vsPatchData.model		   = idModel;
	vsPatchData.representation = representationId;
	vsPatchData.atomId		   = atomId;
	vsPatchData.rendererAtomId = rendererAtomId;
	vsPatchData.selection	   = readPackedAtomFlag( rendererAtomId ) & ( 1u << FLAG_SELECTION );
	vsPatchData.color		   = sesColor( uniformsColor[ readPackedAtomColor( rendererAtomId ) ] );
	vsPatchData.wsAtomData	   = ithData;
	vsPatchData.vAtomData.xyz  = ( uniformsModel[ idModel ].matrixModelView * vec4( ithData.xyz, 1. ) ).xyz;
	vsPatchData.vAtomData.w	   = ithData.w;
	vsPatchData.elementsId	   = elementIds;

	if ( uniformsCamera.isCameraPerspective == 1 )
	{
		// Compute normalized view vector.
		const float dotViewSpherePos = dot( vsPatchData.vAtomData.xyz, vsPatchData.vAtomData.xyz );
		const float dSphereCenter	 = sqrt( dotViewSpherePos );
		const vec3	view			 = vsPatchData.vAtomData.xyz / dSphereCenter;

		// Impostor in front of the sphere.
		const vec3 viewImpPos = vsPatchData.vAtomData.xyz - ithData.w * view;

		// Compute impostor size.
		const float sinAngle = ithData.w / dSphereCenter;
		const float tanAngle = tan( asin( sinAngle ) );
		const float impSize	 = tanAngle * length( viewImpPos );

		// Compute impostor vectors.
		// TODO: simplify normalize ? (vsData.vImpU.x == 0) but normalize should be hard optimized on GPU...
		// But for cross always better doing no calculation.
		// vsData.vImpU = normalize( cross( dir, vec3( 1.f, 0.f, 0.f ) ) ); becomes:
		vsData.vImpU = normalize( vec3( 0.f, view.z, -view.y ) );
		// TODO: simplify cross ? (vsData.vImpU.x == 0) but cross should be hard optimized on GPU...
		vsData.vImpV = cross( vsData.vImpU, view ) * impSize; // No need to normalize.
		vsData.vImpU *= impSize;

		gl_Position = vec4( viewImpPos, 1.f );
	}
	else
	{
		vsData.vImpU = vec3( -1.f, 0.f, 0.f ) * ithData.w;
		vsData.vImpV = vec3( 0.f, -1.f, 0.f ) * ithData.w;
		gl_Position	 = vec4( vsPatchData.vAtomData.xyz + vec3( 0.f, 0.f, ithData.w ), 1.f );
	}
}
