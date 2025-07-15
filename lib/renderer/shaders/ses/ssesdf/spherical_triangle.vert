#version 450

layout(std140, binding = 0) uniform SsesdfSettings
{
	mat4 uMVMatrix;
	mat4 uProjMatrix;
	float uProbeRadius;
};

layout(location = 0) in vec4 aPosition;
layout(location = 1) in ivec4 aAtomIndices;

flat out vec3  vImpU;
flat out vec3  vImpV;
flat out float vDotViewSpherePos;
flat out vec3  vWsProbePos;
flat out vec4  vProbePos;
flat out uint  vProbeStartIntersection;
flat out uint  vProbeIntersectionNumber;
flat out vec3  vVert1;
flat out vec3  vVert2;
flat out vec3  vVert3;

layout(std140, binding = 1) readonly buffer Atoms {
	vec4 atoms[];
};

layout(std140, binding = 2) readonly buffer IntersectionBuffer {
	vec4 neighborsBuffer[];
};

void main() 
{
	vWsProbePos = aPosition.xyz;
    vec4 ai = atoms[aAtomIndices.x];
    vec3 aVert1 = ai.xyz - aPosition.xyz;
    vec4 aj = atoms[aAtomIndices.y];
    vec3 aVert2 = aj.xyz - aPosition.xyz;
    vec4 ak = atoms[aAtomIndices.z];
    vec3 aVert3 = ak.xyz - aPosition.xyz;

    vProbeStartIntersection = uint(aPosition.w);
    vProbeIntersectionNumber = uint(aAtomIndices.w);
    vProbePos.xyz = (uMVMatrix * vec4(vWsProbePos.xyz, 1.)).xyz;
	vProbePos.w   = uProbeRadius;

	vVert1 = (uMVMatrix * vec4(ai.xyz, 1.)).xyz - vProbePos.xyz;
    vVert2 = (uMVMatrix * vec4(aj.xyz, 1.)).xyz - vProbePos.xyz;
    vVert3 = (uMVMatrix * vec4(ak.xyz, 1.)).xyz - vProbePos.xyz;

	// Compute normalized view vector.
	vDotViewSpherePos		  = dot(vProbePos.xyz, vProbePos.xyz);
	const float dSphereCenter = sqrt(vDotViewSpherePos);
	const vec3 view			  = vProbePos.xyz / dSphereCenter;

	// Impostor in front of the sphere.
	vec3 viewImpPos = vProbePos.xyz - vProbePos.w * view;

	// Compute impostor size.
	const float sinAngle = vProbePos.w / dSphereCenter;
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
