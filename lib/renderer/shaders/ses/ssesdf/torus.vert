#version 450

layout(location = 0) in vec4 aTorusPosition;
layout(location = 1) in vec4 aTorusAxis;
layout(location = 2) in vec4 aVisibilitySphere;

layout(std140, binding = 0) uniform SsesdfSettings
{
	mat4 uMVMatrix;
	mat4 uProjMatrix;
	float uProbeRadius;
};

flat out vec3 vImpU; // Impostor vectors.
flat out vec3 vImpV;
flat out vec3 vTorusPosition;
flat out vec4 vRadii;
flat out mat3 vTorusRotationMatrix; 
flat out vec4 vOsVisibilitySphere; 

void main() 
{
	vTorusPosition = (uMVMatrix * vec4(aTorusPosition.xyz, 1.)).xyz;
	vRadii = vec4(
		aTorusAxis.w,     aTorusAxis.w * aTorusAxis.w,
		aTorusPosition.w, aTorusPosition.w * aTorusPosition.w
	);
	
	// Find rotation matrix that map torus base vector to torusAxis
	const vec3 vTorusAxis = (uMVMatrix * vec4(aTorusAxis.xyz + aTorusPosition.xyz, 1.)).xyz - vTorusPosition.xyz;
	const vec3 baseAxis   = (uMVMatrix * ((abs(vTorusAxis.x) > 0.9) ? vec4(0., 1., 0., 1.) : vec4(1., 0., 0., 1.))).xyz; // normal on tmp
	vec3 w = cross(vTorusAxis.xyz, baseAxis);
	w = normalize(w);
	const vec3 v = cross(w, vTorusAxis.xyz);

	vTorusRotationMatrix = transpose(mat3(
		v, w, vTorusAxis.xyz
	));
	
	// Transfer VSphere and Torus positions in view space
	vOsVisibilitySphere.xyz = vTorusRotationMatrix * ((uMVMatrix * vec4(aVisibilitySphere.xyz + aTorusPosition.xyz, 1.)).xyz - vTorusPosition.xyz);
	vOsVisibilitySphere.w   = aVisibilitySphere.w;
	
	// Compute normalized view vector.
	const float vSphereRadius = vOsVisibilitySphere.w + 0.5;
	const vec3 wsVSphere	  = vOsVisibilitySphere.xyz + vTorusPosition.xyz;

	const float dotViewVSPherePos = dot( wsVSphere, wsVSphere.xyz );
	const float dVSphereCenter	  = sqrt( dotViewVSPherePos );
	const vec3	view			  = wsVSphere.xyz / dVSphereCenter;

	// Compute impostor size based on VSphere
	// Impostor in front of the sphere.
	const vec3 viewImpPos = wsVSphere.xyz - vSphereRadius * view;

	// Compute impostor size.
	const float sinAngle = vSphereRadius / dVSphereCenter;
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
