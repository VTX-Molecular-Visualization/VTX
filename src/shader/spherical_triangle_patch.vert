#version 450

struct SphericalTrianglePatch
{
	vec4 probePosition; // + Probe radius
	vec4 v1;			// + probeHash
	vec4 v2;			// + probeIntersectionNb
	vec4 v3;			// + padding
};

layout (std140, binding = 0) readonly buffer SphericalTrianglePatches
{ 
	uint spTriangleNb;
	SphericalTrianglePatch spTriangles[];
};

layout(std140, binding = 1) buffer IntersectionBuffer {
	vec4 neighborsBuffer[];
};

uniform mat4 uMVMatrix;
uniform mat4 uProjMatrix;
uniform uint uMaxProbeNeighborNb;

out VERT_OUT 
{
	flat vec3  impU;
	flat vec3  impV;
	flat float dotViewSpherePos;
	flat vec4  probePos;
	flat int   probeHash;
	flat float probeIntersectionNumber;
	flat vec3  vert1;
	flat vec3  vert2;
	flat vec3  vert3;
} outData;

void main() {
	const SphericalTrianglePatch aSpTrianglePatch = spTriangles[gl_VertexID];
	const vec4 aProbePos = aSpTrianglePatch.probePosition;
	const vec4 aVert1 = aSpTrianglePatch.v1;
	const vec4 aVert2 = aSpTrianglePatch.v2;
	const vec4 aVert3 = aSpTrianglePatch.v3;

    outData.probeIntersectionNumber = aVert2.w;
    outData.probeHash = int(aVert1.w);

    outData.probePos.xyz = (uMVMatrix * vec4(aProbePos.xyz, 1.)).xyz;
	outData.probePos.w   = aProbePos.w;

	outData.vert1 = (uMVMatrix * vec4(aVert1.xyz + aProbePos.xyz, 1.)).xyz - outData.probePos.xyz;
    outData.vert2 = (uMVMatrix * vec4(aVert2.xyz + aProbePos.xyz, 1.)).xyz - outData.probePos.xyz;
    outData.vert3 = (uMVMatrix * vec4(aVert3.xyz + aProbePos.xyz, 1.)).xyz - outData.probePos.xyz;

	// Compute normalized view vector.
	outData.dotViewSpherePos  = dot(outData.probePos.xyz, outData.probePos.xyz);
	const float dSphereCenter = sqrt(outData.dotViewSpherePos);
	const vec3 view			  = outData.probePos.xyz / dSphereCenter;

	// Impostor in front of the sphere.
	vec3 viewImpPos = outData.probePos.xyz - outData.probePos.w * view;

	// Compute impostor size.
	const float sinAngle = outData.probePos.w / dSphereCenter;
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
