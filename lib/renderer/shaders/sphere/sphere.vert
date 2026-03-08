#version 460 core

#include "../constant.glsl"
#include "../chemdb/atoms.glsl"
#include "../layout_uniforms_camera.glsl"
#include "../layout_uniforms_color.glsl"
#include "../layout_uniforms_model.glsl"
#include "../layout_uniforms_representation.glsl"
#include "struct_sphere.glsl"
#include "struct_vertex_shader.glsl"

// In.
layout( location = 0 ) in vec3 inSpherePos;
layout( location = 1 ) in uint inSphereColor;
layout( location = 2 ) in uint inSphereSymbol;
layout( location = 3 ) in uint inSphereId;
layout( location = 4 ) in uint inSphereFlag;
layout( location = 5 ) in uint inSphereRepresentation;

// Out.
flat out StructVertexShader vsData;
flat out StructSphere vsSphere;

void main()
{
	vsSphere.viewPos	 = vec3( uniformsModel[ gl_DrawID ].matrixModelView * vec4( inSpherePos, 1.f ) );
	vsSphere.color		 = uniformsColor[ inSphereColor ];
	//vsSphere.sphereColor		 = vec4( 1.f, 1.f, 1.f, 1.f );
	Representation representation = uniformsRepresentation[ inSphereRepresentation ];
	vsSphere.radius	 = bool( representation.isRadiusSphereFixed ) ? representation.radiusSphereFixed : getVdwRadius( inSphereSymbol ) + representation.radiusSphereAdd;
	vsSphere.id		 = inSphereId;
	vsSphere.isVisible	 = int( inSphereFlag ) & ( 1 << FLAG_VISIBILITY );
	vsSphere.isSelected	 = int( inSphereFlag ) & ( 1 << FLAG_SELECTION );	

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
