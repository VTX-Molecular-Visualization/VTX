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

out vec3	   vCamImpPos;	  // impostor position in cam space
flat out vec3  vCamSpherePos; // sphere impostor in cam space
flat out vec3  vSphereColor;
flat out float vSphereRad;
flat out uint  vSphereVis;
// impostor vectors
flat out vec3  vImpU;
flat out vec3  vImpV;
flat out float vDotCamSpherePos;

void main()
{
	vCamSpherePos = vec3( uMVMatrix * vec4( aSpherePos, 1.f ) );
	vSphereColor  = aSphereColor;
	vSphereRad	  = uIsRadiusFixed ? uRadiusFixed : aSphereRad + uRadiusAdd;
	vSphereVis	  = aSphereVis;

	vDotCamSpherePos		  = dot( vCamSpherePos, vCamSpherePos );
	const float dSphereCenter = sqrt( vDotCamSpherePos );
	const vec3	dir			  = vCamSpherePos / dSphereCenter;

	// impostor in front of the sphere
	vCamImpPos = vCamSpherePos - vSphereRad * dir;

	// compute impostor size
	const float sinAngle = vSphereRad / dSphereCenter;
	const float tanAngle = tan( asin( sinAngle ) );
	const float impSize	 = tanAngle * length( vCamImpPos );

	// compute impostor vectors
	// vImpU = normalize( cross( dir, vec3( 1.f, 0.f, 0.f ) ) );
	vImpU = normalize( vec3( 0.f, dir.z, -dir.y ) );
	vImpV = cross( vImpU, dir ) * impSize; // no need to normalize
	vImpU *= impSize;

	gl_Position = vec4( vCamImpPos, 1.0 );
}
