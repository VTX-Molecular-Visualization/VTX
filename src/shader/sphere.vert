#version 450

#define ATOM_POSITION 0
#define ATOM_COLOR 1
#define ATOM_RADIUS 2
#define ATOM_VISIBILITY 3
#define ATOM_SELECTION 4

layout( location = ATOM_POSITION ) in vec3 aSpherePos;
layout( location = ATOM_COLOR ) in vec3 aSphereColor;
layout( location = ATOM_RADIUS ) in float aSphereRad;
layout( location = ATOM_VISIBILITY ) in unsigned short aSphereVis;
layout( location = ATOM_SELECTION ) in unsigned short aSphereSel;

uniform mat4  uMVMatrix;
uniform mat4  uProjMatrix;
uniform float uRadiusAdd	 = 0.f; // TODO: for SAS ?
uniform float uRadiusFixed	 = 1.f;
uniform bool  uIsRadiusFixed = false;

flat out vec3			vViewSpherePos; // Sphere position in view space.
flat out vec3			vSphereColor;
flat out float			vSphereRad;
flat out unsigned short vSphereVis;
flat out unsigned short vSphereSel;
flat out vec3			vImpU; // Impostor vectors.
flat out vec3			vImpV;
flat out float			vDotViewSpherePos;

void main()
{
	vViewSpherePos = vec3( uMVMatrix * vec4( aSpherePos, 1.f ) );
	vSphereColor   = aSphereColor;
	vSphereRad	   = uIsRadiusFixed ? uRadiusFixed : aSphereRad + uRadiusAdd;
	vSphereVis	   = aSphereVis;
	vSphereSel	   = aSphereSel;

	// Compute normalized view vector.
	vDotViewSpherePos		  = dot( vViewSpherePos, vViewSpherePos );
	const float dSphereCenter = sqrt( vDotViewSpherePos );
	const vec3	view		  = vViewSpherePos / dSphereCenter;

	// Impostor in front of the sphere.
	vec3 viewImpPos = vViewSpherePos - vSphereRad * view;

	// Compute impostor size.
	const float sinAngle = vSphereRad / dSphereCenter;
	const float tanAngle = tan( asin( sinAngle ) );
	const float impSize	 = tanAngle * length( viewImpPos );

	// Compute impostor vectors.
	// TODO: simplify normalize ? (vImpU.x == 0) but normalize should be hard optimized on GPU...
	// But for cross always better doing no calculation.
	// vImpU = normalize( cross( dir, vec3( 1.f, 0.f, 0.f ) ) ); becomes:
	vImpU = normalize( vec3( 0.f, view.z, -view.y ) );
	// TODO: simplify cross ? (vImpU.x == 0) but cross should be hard optimized on GPU...
	vImpV = cross( vImpU, view ) * impSize; // No need to normalize.
	vImpU *= impSize;

	gl_Position = vec4( viewImpPos, 1.f );
}
