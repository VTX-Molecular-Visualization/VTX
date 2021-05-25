#version 450

#define ATOM_POSITION 0
#define ATOM_COLOR 1
#define ATOM_RADIUS 2
#define ATOM_VISIBILITY 3
#define ATOM_SELECTION 4

layout( location = ATOM_POSITION ) in vec3 aSpherePos;
layout( location = ATOM_COLOR ) in vec3 aSphereColor;
layout( location = ATOM_RADIUS ) in float aSphereRadius;
layout( location = ATOM_VISIBILITY ) in uint aSphereVisible;
layout( location = ATOM_SELECTION ) in uint sphereSelected;

uniform mat4  u_MVMatrix;
uniform mat4  u_projMatrix;
uniform float u_radiusAdd	  = 0.f; // TODO: for SAS ?
uniform float u_radiusFixed	  = 1.f;
uniform bool  u_isRadiusFixed = false;

out VsOut
{
	flat vec3  viewSpherePos; // Sphere position in view space.
	flat vec3  sphereColor;
	flat float sphereRadius;
	flat uint  sphereVisible;
	flat uint  sphereSelected;
	flat vec3  vImpU; // Impostor vectors.
	flat vec3  vImpV;
	flat float dotViewSpherePos;
}
vsOut;

void main()
{
	vsOut.viewSpherePos	 = vec3( u_MVMatrix * vec4( aSpherePos, 1.f ) );
	vsOut.sphereColor	 = aSphereColor;
	vsOut.sphereRadius	 = u_isRadiusFixed ? u_radiusFixed : aSphereRadius + u_radiusAdd;
	vsOut.sphereVisible	 = aSphereVisible;
	vsOut.sphereSelected = sphereSelected;

	// Compute normalized view vector.
	vsOut.dotViewSpherePos	  = dot( vsOut.viewSpherePos, vsOut.viewSpherePos );
	const float dSphereCenter = sqrt( vsOut.dotViewSpherePos );
	const vec3	view		  = vsOut.viewSpherePos / dSphereCenter;

	// Impostor in front of the sphere.
	vec3 viewImpPos = vsOut.viewSpherePos - vsOut.sphereRadius * view;

	// Compute impostor size.
	const float sinAngle = vsOut.sphereRadius / dSphereCenter;
	const float tanAngle = tan( asin( sinAngle ) );
	const float impSize	 = tanAngle * length( viewImpPos );

	// Compute impostor vectors.
	// TODO: simplify normalize ? (vImpU.x == 0) but normalize should be hard optimized on GPU...
	// But for cross always better doing no calculation.
	// vImpU = normalize( cross( dir, vec3( 1.f, 0.f, 0.f ) ) ); becomes:
	vsOut.vImpU = normalize( vec3( 0.f, view.z, -view.y ) );
	// TODO: simplify cross ? (vImpU.x == 0) but cross should be hard optimized on GPU...
	vsOut.vImpV = cross( vsOut.vImpU, view ) * impSize; // No need to normalize.
	vsOut.vImpU *= impSize;

	gl_Position = vec4( viewImpPos, 1.f );
}
