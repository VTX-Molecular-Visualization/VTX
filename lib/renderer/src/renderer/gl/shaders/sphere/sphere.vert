#version 450

#define ATOM_POSITION 0
#define ATOM_COLOR 1
#define ATOM_RADIUS 2
#define ATOM_VISIBILITY 3
#define ATOM_SELECTION 4
#define ATOM_ID 5

layout( location = ATOM_POSITION ) in vec3 aSpherePos;
layout( location = ATOM_COLOR ) in vec4 aSphereColor;
layout( location = ATOM_RADIUS ) in float aSphereRadius;
layout( location = ATOM_VISIBILITY ) in uint aSphereVisible;
layout( location = ATOM_SELECTION ) in uint aSphereSelected;
layout( location = ATOM_ID ) in uint aSphereId;

uniform mat4  u_MVMatrix;
uniform mat4  u_projMatrix;
uniform float u_radiusAdd	  = 0.f; // TODO: for SAS ?
uniform float u_radiusFixed	  = 1.f;
uniform bool  u_isRadiusFixed = false;
uniform bool  u_isPerspective;

out 
#include "struct_vertex_shader.glsl"
dataOut;

void main()
{
	if (u_isPerspective)
	{
		dataOut.viewSpherePos	 = vec3( u_MVMatrix * vec4( aSpherePos, 1.f ) );
		dataOut.sphereColor	 = aSphereColor;
		dataOut.sphereRadius	 = u_isRadiusFixed ? u_radiusFixed : aSphereRadius + u_radiusAdd;
		dataOut.sphereVisible	 = aSphereVisible;
		dataOut.sphereSelected = aSphereSelected;
		dataOut.sphereId		 = aSphereId;

		// Compute normalized view vector.
		dataOut.dotViewSpherePos	  = dot( dataOut.viewSpherePos, dataOut.viewSpherePos );
		const float dSphereCenter = sqrt( dataOut.dotViewSpherePos );
		const vec3	view		  = dataOut.viewSpherePos / dSphereCenter;

		// Impostor in front of the sphere.
		vec3 viewImpPos = dataOut.viewSpherePos - dataOut.sphereRadius * view;

		// Compute impostor size.
		const float sinAngle = dataOut.sphereRadius / dSphereCenter;
		const float tanAngle = tan( asin( sinAngle ) );
		const float impSize	 = tanAngle * length( viewImpPos );

		// Compute impostor vectors.
		// TODO: simplify normalize ? (vImpU.x == 0) but normalize should be hard optimized on GPU...
		// But for cross always better doing no calculation.
		// vImpU = normalize( cross( dir, vec3( 1.f, 0.f, 0.f ) ) ); becomes:
		dataOut.vImpU = normalize( vec3( 0.f, view.z, -view.y ) );
		// TODO: simplify cross ? (vImpU.x == 0) but cross should be hard optimized on GPU...
		dataOut.vImpV = cross( dataOut.vImpU, view ) * impSize; // No need to normalize.
		dataOut.vImpU *= impSize;

		gl_Position = vec4( viewImpPos, 1.f );
	}
	else // Orthographic
	{ 
		dataOut.viewSpherePos	 = vec3( u_MVMatrix * vec4( aSpherePos, 1.f ) );
		dataOut.sphereColor	 = aSphereColor;
		dataOut.sphereRadius	 = u_isRadiusFixed ? u_radiusFixed : aSphereRadius + u_radiusAdd;
		dataOut.sphereVisible	 = aSphereVisible;
		dataOut.sphereSelected = aSphereSelected;
		dataOut.sphereId		 = aSphereId;

		dataOut.vImpU = vec3(-1, 0, 0) * dataOut.sphereRadius;
		dataOut.vImpV = vec3(0, -1, 0) * dataOut.sphereRadius; 

		gl_Position = vec4( dataOut.viewSpherePos + vec3(0, 0, dataOut.sphereRadius), 1.f );
	}
}
