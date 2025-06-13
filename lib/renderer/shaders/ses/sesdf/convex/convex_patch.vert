#version 450

struct DisplayConvexPatch
{
	uint atomId;
	vec4 wsAtomData;	 // world space ith pos + ith radius
	vec4 vAtomData;		 // View space ith pos + ith Extended radius
	uvec2 elementsId;
};

layout(location = 0) in uvec2 elementIds;

flat out vec3				vImpU; // Impostor vectors.
flat out vec3				vImpV;
flat out DisplayConvexPatch vPatchData;

layout(std140, binding = 0) uniform SesdfSettings
{
	mat4  uMVMatrix;
	mat4  uProjMatrix;
	mat4  uInvMVMatrix;
	float uProbeRadius;
	uint  uMaxProbeNeighborNb;
};

layout(std140, binding = 1) readonly buffer SortedAtoms {
	vec4 atoms[];
};

void main()
{
	const vec4 ithData		 = atoms[gl_VertexID];
	vPatchData.atomId = gl_VertexID;
	vPatchData.wsAtomData	 = ithData;
	vPatchData.vAtomData.xyz = (uMVMatrix * vec4(ithData.xyz, 1.)).xyz;
	vPatchData.vAtomData.w   = ithData.w;
	vPatchData.elementsId	 = elementIds;

	// Compute normalized view vector.
	const float dotViewSpherePos  = dot( vPatchData.vAtomData.xyz, vPatchData.vAtomData.xyz );
	const float dSphereCenter	  = sqrt( dotViewSpherePos );
	const vec3	view			  = vPatchData.vAtomData.xyz / dSphereCenter;

	// Impostor in front of the sphere.
	const vec3 viewImpPos = vPatchData.vAtomData.xyz - ithData.w * view;

	// Compute impostor size.
	const float sinAngle = ithData.w / dSphereCenter;
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

	gl_Position = vec4( viewImpPos, 1.f );}