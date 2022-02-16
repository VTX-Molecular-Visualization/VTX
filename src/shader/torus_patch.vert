#version 450

struct TorusPatch
{
	vec4 torusPosition;		// Vec3 Torus Position   + float Torus Radius
	vec4 torusAxis;			// Vec3 Torus Axis       + float Probe Radius
	vec4 visibilitySphere;	// Vec3 VSphere Position + float VSphere Radius
};

layout (std140, binding = 0) readonly buffer TorusPatches
{ 
	TorusPatch toruses[];
};

uniform mat4  uMVMatrix;
uniform mat4  uProjMatrix;

out VERT_OUT
{
	flat vec3 impU; // Impostor vectors.
	flat vec3 impV;
	flat vec3 torusPosition;
	flat vec4 radii;
	flat mat3 torusRotationMatrix; 
	flat vec4 osVisibilitySphere; 
} outData;

void main()
{
	TorusPatch aTorusPatch = toruses[gl_VertexID];
	
	const vec4 aTorusPosition = aTorusPatch.torusPosition;
	const vec4 aTorusAxis = aTorusPatch.torusAxis;
	const vec4 aVisibilitySphere = aTorusPatch.visibilitySphere;

	outData.torusPosition   = (uMVMatrix * vec4(aTorusPosition.xyz, 1.)).xyz;
	outData.radii = vec4(
		aTorusAxis.w,     aTorusAxis.w * aTorusAxis.w,
		aTorusPosition.w, aTorusPosition.w * aTorusPosition.w
	);
	
	// Find rotation matrix that map torus base vector to torusAxis
	const vec3 vTorusAxis  = (uMVMatrix * vec4(aTorusAxis.xyz + aTorusPosition.xyz, 1.)).xyz - outData.torusPosition.xyz;
	const vec3 baseAxis = (uMVMatrix * ((abs(vTorusAxis.x) > 0.9) ? vec4(0., 1., 0., 1.) : vec4(1., 0., 0., 1.))).xyz; // normal on tmp
	vec3 w = cross(vTorusAxis.xyz, baseAxis);
	w = normalize(w);
	const vec3 v = cross(w, vTorusAxis.xyz);

	outData.torusRotationMatrix = transpose(mat3(
		v, w, vTorusAxis.xyz
	));
	
	// Transfer VSphere and Torus positions in view space
	outData.osVisibilitySphere.xyz = outData.torusRotationMatrix * ((uMVMatrix * vec4(aVisibilitySphere.xyz + aTorusPosition.xyz, 1.)).xyz - outData.torusPosition.xyz);
	outData.osVisibilitySphere.w = aVisibilitySphere.w;
	
	// Compute normalized view vector.
	const float vSphereRadius = outData.osVisibilitySphere.w;
	const vec3 wsVSphere	  = outData.osVisibilitySphere.xyz + outData.torusPosition.xyz;

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
	outData.impU = normalize( vec3( 0.f, view.z, -view.y ) );
	// TODO: simplify cross ? (vImpU.x == 0) but cross should be hard optimized on GPU...
	outData.impV = cross( outData.impU, view ) * impSize; // No need to normalize.
	outData.impU *= impSize;

	gl_Position = vec4( viewImpPos, 1.f );
}
