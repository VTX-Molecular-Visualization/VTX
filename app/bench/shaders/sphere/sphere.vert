#version 450 core

#include "../constant.glsl"
#include "../layout_uniforms_camera.glsl"
#include "../layout_uniforms_color.glsl"
#include "../layout_uniforms_model.glsl"
#include "../layout_uniforms_representation.glsl"

// In.
layout( location = 0 ) in vec3  inSpherePos;
layout( location = 1 ) in uint  inSphereColor;
layout( location = 2 ) in float inSphereRadius;
layout( location = 3 ) in uint  inSphereId;
layout( location = 4 ) in uint  inSphereFlag;
layout( location = 5 ) in uint  inSphereModel;
layout( location = 6 ) in uint  inSphereRepresentation;

// Out.
out 
#include "struct_vertex_shader.glsl"
outData;

void main()
{
	outData.viewSpherePos	 = vec3( uniformsModel[ inSphereModel ].matrixModelView * vec4( inSpherePos, 1.f ) );
	outData.sphereColor		 = uniformsColor[ inSphereColor ];
	//outData.sphereColor		 = vec4( 1.f, 1.f, 1.f, 1.f );
	Representation representation = uniformsRepresentation[ inSphereRepresentation ];
	outData.sphereRadius	 = bool( representation.isRadiusSphereFixed ) ? representation.radiusSphereFixed : inSphereRadius + representation.radiusSphereAdd;
	outData.sphereId		 = inSphereId;
	outData.sphereVisible	 = int( inSphereFlag ) & ( 1 << FLAG_VISIBILITY );
	outData.sphereSelected	 = int( inSphereFlag ) & ( 1 << FLAG_SELECTION );	

	if ( uniformsCamera.isCameraPerspective == 1 )
	{
		// Compute normalized view vector.
		outData.dotViewSpherePos  = dot( outData.viewSpherePos, outData.viewSpherePos );
		const float dSphereCenter = sqrt( outData.dotViewSpherePos );
		const vec3	view		  = outData.viewSpherePos / dSphereCenter;

		// Impostor in front of the sphere.
		vec3 viewImpPos = outData.viewSpherePos - outData.sphereRadius * view;

		// Compute impostor size.
		const float sinAngle = outData.sphereRadius / dSphereCenter;
		const float tanAngle = tan( asin( sinAngle ) );
		const float impSize	 = tanAngle * length( viewImpPos );

		// Compute impostor vectors.
		// TODO: simplify normalize ? (vImpU.x == 0) but normalize should be hard optimized on GPU...
		// But for cross always better doing no calculation.
		// vImpU = normalize( cross( dir, vec3( 1.f, 0.f, 0.f ) ) ); becomes:
		outData.vImpU = normalize( vec3( 0.f, view.z, -view.y ) );
		// TODO: simplify cross ? (vImpU.x == 0) but cross should be hard optimized on GPU...
		outData.vImpV = cross( outData.vImpU, view ) * impSize; // No need to normalize.
		outData.vImpU *= impSize;

		gl_Position = vec4( viewImpPos, 1.f );
	}
	else // Orthographic
	{
		outData.vImpU = vec3(-1, 0, 0) * outData.sphereRadius;
		outData.vImpV = vec3(0, -1, 0) * outData.sphereRadius;

		gl_Position = vec4( outData.viewSpherePos + vec3(0, 0, outData.sphereRadius), 1.f );
	}
}
