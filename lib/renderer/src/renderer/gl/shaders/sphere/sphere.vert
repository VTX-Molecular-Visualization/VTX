#version 450 core

#include "../global_uniforms.glsl"

// In.
layout( location = 0 ) in vec3  inSpherePos;
layout( location = 1 ) in vec4  inSphereColor;
layout( location = 2 ) in float inSphereRadius;
layout( location = 3 ) in uint  inSphereVisible;
layout( location = 4 ) in uint  inSphereSelected;
layout( location = 5 ) in uint  inSphereId;


uniform float u_radiusAdd	  = 0.f;
uniform float u_radiusFixed	  = 1.f;
uniform bool  u_isRadiusFixed = false;

// Out.
out 
#include "struct_vertex_shader.glsl"
outData;

void main()
{
	if ( isCameraPerspective() )
	{
		outData.viewSpherePos	 = vec3( getMatrixView() * getMatrixModel() * vec4( inSpherePos, 1.f ) );
		outData.sphereColor		 = inSphereColor;
		outData.sphereRadius	 = u_isRadiusFixed ? u_radiusFixed : inSphereRadius + u_radiusAdd;
		outData.sphereVisible	 = inSphereVisible;
		outData.sphereSelected	 = inSphereSelected;
		outData.sphereId		 = inSphereId;

		// Compute normalized view vector.
		outData.dotViewSpherePos	  = dot( outData.viewSpherePos, outData.viewSpherePos );
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
		outData.viewSpherePos	 = vec3( getMatrixView() * getMatrixModel() * vec4( inSpherePos, 1.f ) );
		outData.sphereColor		 = inSphereColor;
		outData.sphereRadius	 = u_isRadiusFixed ? u_radiusFixed : inSphereRadius + u_radiusAdd;
		outData.sphereVisible	 = inSphereVisible;
		outData.sphereSelected	 = inSphereSelected;
		outData.sphereId		 = inSphereId;

		outData.vImpU = vec3(-1, 0, 0) * outData.sphereRadius;
		outData.vImpV = vec3(0, -1, 0) * outData.sphereRadius; 

		gl_Position = vec4( outData.viewSpherePos + vec3(0, 0, outData.sphereRadius), 1.f );
	}
}
