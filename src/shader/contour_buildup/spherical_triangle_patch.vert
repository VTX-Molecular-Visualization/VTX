#version 450

layout (std140, binding = 0) readonly buffer ProbesData
{ 
	vec4 probesData[];
};

layout(std140) uniform SphericalTrianglePatchSettings
{
	mat4 uMVMatrix;
	mat4 uProjMatrix;
	uint uProbeNb;
	uint uMaxProbeNeighborNb;
};

out VERT_OUT 
{
	flat vec3  impU;
	flat vec3  impV;
	flat float dotViewSpherePos;
	flat vec3  wsProbePos;
	flat vec4  probePos;
	flat int   probeId;
	flat float probeIntersectionNumber;
	flat vec3  vert1;
	flat vec3  vert2;
	flat vec3  vert3;
} outData;

void main() 
{
	outData.probeId		= gl_VertexID;
	const vec4 probePos = probesData[uProbeNb * 0 + gl_VertexID /* Position */ ];
	outData.wsProbePos  = probePos.xyz;
	const vec4 aVert1   = probesData[uProbeNb * 1 + gl_VertexID /* Vec 1    */ ];
	const vec4 aVert2   = probesData[uProbeNb * 2 + gl_VertexID /* Vec 2    */ ];
	const vec4 aVert3   = probesData[uProbeNb * 3 + gl_VertexID /* Vec 3    */ ];

    outData.probeIntersectionNumber = aVert1.w;

    outData.probePos.xyz = (uMVMatrix * vec4(outData.wsProbePos.xyz, 1.)).xyz;
	outData.probePos.w   = probePos.w;

	outData.vert1 = (uMVMatrix * vec4(aVert1.xyz + outData.wsProbePos.xyz, 1.)).xyz - outData.probePos.xyz;
    outData.vert2 = (uMVMatrix * vec4(aVert2.xyz + outData.wsProbePos.xyz, 1.)).xyz - outData.probePos.xyz;
    outData.vert3 = (uMVMatrix * vec4(aVert3.xyz + outData.wsProbePos.xyz, 1.)).xyz - outData.probePos.xyz;

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
