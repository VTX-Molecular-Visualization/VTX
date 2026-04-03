#version 460 core

#include "../../../constant.glsl"
#include "../../../layout_uniforms_model.glsl"
#include "struct_convex_patch.glsl"
#include "struct_vertex_shader.glsl"

// In.
layout(location = 0) in uvec2 elementIds;
layout(std140, binding = 1) readonly buffer SortedAtoms {
	vec4 atoms[];
};

// Out.
flat out StructVertexShader vsData;
flat out StructConvexPatch vsPatchData;

void main()
{
	const vec4 ithData		 = atoms[gl_VertexID];
	vsPatchData.atomId = gl_VertexID;
	vsPatchData.wsAtomData	 = ithData;
	vsPatchData.vAtomData.xyz = (uniformsModel[ 0 ].matrixModelView * vec4(ithData.xyz, 1.)).xyz;
	vsPatchData.vAtomData.w   = ithData.w;
	vsPatchData.elementsId	 = elementIds;

	// Compute normalized view vector.
	const float dotViewSpherePos  = dot( vsPatchData.vAtomData.xyz, vsPatchData.vAtomData.xyz );
	const float dSphereCenter	  = sqrt( dotViewSpherePos );
	const vec3	view			  = vsPatchData.vAtomData.xyz / dSphereCenter;

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
