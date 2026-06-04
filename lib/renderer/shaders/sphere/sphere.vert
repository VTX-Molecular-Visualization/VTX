#version 460 core

#include "../constant.glsl"
#include "../chemdb/atoms.glsl"
#include "../layout_uniforms_camera.glsl"
#include "../layout_uniforms_color.glsl"
#include "../layout_uniforms_model.glsl"
#include "../layout_uniforms_representation.glsl"
#include "../struct/draw_indexed_indirect.glsl"
#include "struct_sphere.glsl"
#include "struct_vertex_shader.glsl"

// In.
#include "../layout/atom.glsl"

// Out.
flat out StructVertexShader vsData;
flat out StructSphere vsSphere;

layout( std430, binding = 10 ) readonly buffer SphereIndirectDraws
{
	uint sphereDrawCount;
	uint sphereDrawPadding0;
	uint sphereDrawPadding1;
	uint sphereDrawPadding2;
	DrawIndexedIndirectRecord sphereDraws[];
};

void main()
{
	const uint idModel = sphereDraws[ gl_DrawID ].idModel;

	vsSphere.viewPos	 = vec3( uniformsModel[ idModel ].matrixModelView * vec4( inAtomPosition, 1.f ) );
	vsSphere.color		 = uniformsColor[ inAtomColor ];
	//vsSphere.sphereColor		 = vec4( 1.f, 1.f, 1.f, 1.f );
	Representation representation = uniformsRepresentation[ inAtomRepresentation ];
	vsSphere.radius	 = bool( representation.isRadiusSphereFixed ) ? representation.radiusSphereFixed : getVdwRadius( inAtomSymbol ) + representation.radiusSphereAdd;
	vsSphere.id		 = inAtomId;
	vsSphere.isSelected	 = int( inAtomFlag ) & ( 1 << FLAG_SELECTION );	

	if ( uniformsCamera.isCameraPerspective == 1 )
	{
		// Compute normalized view vector.
		vsSphere.dotViewPos  = dot( vsSphere.viewPos, vsSphere.viewPos );
		const float dSphereCenter = sqrt( vsSphere.dotViewPos );
		const vec3	view		  = vsSphere.viewPos / dSphereCenter;

		// Impostor in front of the sphere.
		vec3 viewImpPos = vsSphere.viewPos - vsSphere.radius * view;

		// Compute impostor size.
		const float sinAngle = vsSphere.radius / dSphereCenter;
		const float tanAngle = tan( asin( sinAngle ) );
		const float impSize	 = tanAngle * length( viewImpPos );

		// Compute impostor vectors.
		// TODO: simplify normalize ? (vImpU.x == 0) but normalize should be hard optimized on GPU...
		// But for cross always better doing no calculation.
		// vImpU = normalize( cross( dir, vec3( 1.f, 0.f, 0.f ) ) ); becomes:
		vsData.vImpU = normalize( vec3( 0.f, view.z, -view.y ) );
		// TODO: simplify cross ? (vImpU.x == 0) but cross should be hard optimized on GPU...
		vsData.vImpV = cross( vsData.vImpU, view ) * impSize; // No need to normalize.
		vsData.vImpU *= impSize;

		gl_Position = vec4( viewImpPos, 1.f );
	}
	else // Orthographic
	{
		vsData.vImpU = vec3(-1, 0, 0) * vsSphere.radius;
		vsData.vImpV = vec3(0, -1, 0) * vsSphere.radius;

		gl_Position = vec4( vsSphere.viewPos + vec3(0, 0, vsSphere.radius), 1.f );
	}
}
