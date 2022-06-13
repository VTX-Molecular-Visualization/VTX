#version 450

layout (std140, binding = 0) readonly buffer TorusesData
{ 
	vec4 torusesData[];
};

layout(std140) uniform TorusPatchSettings
{
	mat4 uMVMatrix;
	mat4 uProjMatrix;
	uint uTorusNb;
};

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
	const vec4 aTorusPosition	 = torusesData[0 * uTorusNb + gl_VertexID];
	const vec4 aTorusAxis		 = torusesData[1 * uTorusNb + gl_VertexID];
	const vec4 aVisibilitySphere = torusesData[2 * uTorusNb + gl_VertexID];

	outData.torusPosition = (uMVMatrix * vec4(aTorusPosition.xyz, 1.)).xyz;
	outData.radii = vec4(
		aTorusAxis.w,     aTorusAxis.w * aTorusAxis.w,
		aTorusPosition.w, aTorusPosition.w * aTorusPosition.w
	);
	
	// Find rotation matrix that map torus base vector to torusAxis
	const vec3 vTorusAxis = (uMVMatrix * vec4(aTorusAxis.xyz + aTorusPosition.xyz, 1.)).xyz - outData.torusPosition.xyz;
	const vec3 baseAxis   = (uMVMatrix * ((abs(vTorusAxis.x) > 0.9) ? vec4(0., 1., 0., 1.) : vec4(1., 0., 0., 1.))).xyz; // normal on tmp
	vec3 w = cross(vTorusAxis.xyz, baseAxis);
	w = normalize(w);
	const vec3 v = cross(w, vTorusAxis.xyz);

	outData.torusRotationMatrix = transpose(mat3(
		v, w, vTorusAxis.xyz
	));
	
	// Transfer VSphere and Torus positions in view space
	outData.osVisibilitySphere.xyz = outData.torusRotationMatrix * ((uMVMatrix * vec4(aVisibilitySphere.xyz + aTorusPosition.xyz, 1.)).xyz - outData.torusPosition.xyz);
	outData.osVisibilitySphere.w   = aVisibilitySphere.w;
	
	// Compute normalized view vector.
	const float vSphereRadius = outData.osVisibilitySphere.w + 0.5;
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
