#version 450

layout( location = 0 ) in vec3 aSpherePos;
layout( location = 1 ) in vec3 aSphereColor;
layout( location = 2 ) in float aSphereRad;
layout( location = 3 ) in uint aSphereVis;

uniform mat4  uMVMatrix;
uniform mat4  uProjMatrix;
uniform float uRadiusAdd	 = 0.f; // TODO: for SAS ?
uniform float uRadiusFixed	 = 1.f;
uniform bool  uIsRadiusFixed = false;

flat out vec3  vViewSpherePos; // sphere position in view space
flat out vec3  vSphereColor;
flat out float vSphereRad;
flat out uint  vSphereVis;
flat out vec3  vImpU; // impostor vectors
flat out vec3  vImpV;
flat out float vDotViewSpherePos;

void main()
{
	vViewSpherePos = vec3( uMVMatrix * vec4( aSpherePos, 1.f ) );
	vSphereColor   = aSphereColor;
	vSphereRad	   = uIsRadiusFixed ? uRadiusFixed : aSphereRad + uRadiusAdd;
	vSphereVis	   = aSphereVis;

	vDotViewSpherePos		  = dot( vViewSpherePos, vViewSpherePos );
	const float dSphereCenter = sqrt( vDotViewSpherePos );
	const vec3	dir			  = vViewSpherePos / dSphereCenter;

	// impostor in front of the sphere
	vec3 viewImpPos = vViewSpherePos - vSphereRad * dir;

	// compute impostor size
	const float sinAngle = vSphereRad / dSphereCenter;
	const float tanAngle = tan( asin( sinAngle ) );
	const float impSize	 = tanAngle * length( viewImpPos );

	// compute impostor vectors
	// TODO: simplify normalize ? (vImpU.x == 0) but normalize should be hard optimized on GPU...
	// But for cross always better doing no calculation
	// vImpU = normalize( cross( dir, vec3( 1.f, 0.f, 0.f ) ) ); becomes:
	vImpU = normalize( vec3( 0.f, dir.z, -dir.y ) );
	// TODO: simplify cross ? (vImpU.x == 0) but cross should be hard optimized on GPU...
	vImpV = cross( vImpU, dir ) * impSize; // no need to normalize
	vImpU *= impSize;


	gl_Position = vec4( viewImpPos, 1.f );
}
