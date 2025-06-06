#version 450

layout( points ) in;
layout( triangle_strip, max_vertices = 4 ) out;

layout(std140, binding = 0) uniform SsesdfSettings
{
	mat4 uMVMatrix;
	mat4 uProjMatrix;
	float uProbeRadius;
};

layout(std140, binding = 1) readonly buffer Atoms {
	vec4 atoms[];
};

layout(std140, binding = 2) readonly buffer IntersectionBuffer {
	vec4 neighborsBuffer[];
};

flat in vec3  vImpU[];
flat in vec3  vImpV[];
flat in float vDotViewSpherePos[];
flat in vec3  vWsProbePos[];
flat in vec4  vProbePos[];
flat in uint  vProbeStartIntersection[];
flat in uint  vProbeIntersectionNumber[];
flat in vec3  vVert1[];
flat in vec3  vVert2[];
flat in vec3  vVert3[];

smooth out vec3  viewImpPos;	  // Impostor position in view space.
flat   out float dotViewSpherePos;
flat   out vec3  wsProbePos;
flat   out vec4  probePos;
flat   out uint  probeStartIntersection;
flat   out uint  probeIntersectionNumber;
flat   out vec3  vert1;
flat   out vec3  vert2;
flat   out vec3  vert3;

void emitQuad( const vec3 v1, const vec3 v2, const vec3 v3, const vec3 v4 )
{
	viewImpPos	= v1;
	gl_Position = uProjMatrix * vec4( viewImpPos, 1.f );
	EmitVertex();

	viewImpPos	= v2;
	gl_Position = uProjMatrix * vec4( viewImpPos, 1.f );
	EmitVertex();

	viewImpPos	= v3;
	gl_Position = uProjMatrix * vec4( viewImpPos, 1.f );
	EmitVertex();

	viewImpPos	= v4;
	gl_Position = uProjMatrix * vec4( viewImpPos, 1.f );
	EmitVertex();

	EndPrimitive();
}

void main()
{
	// Output data.
	dotViewSpherePos		= vDotViewSpherePos[0];
	wsProbePos				= vWsProbePos[0];
	probePos				= vProbePos[0];
	probeStartIntersection  = vProbeStartIntersection[0];
	probeIntersectionNumber = vProbeIntersectionNumber[0];
	vert1					= vVert1[0];
	vert2					= vVert2[0];
	vert3					= vVert3[0];

	// Compute impostors vertices.
	const vec3 v1 = gl_in[ 0 ].gl_Position.xyz - vImpU[0] - vImpV[0];
	const vec3 v2 = gl_in[ 0 ].gl_Position.xyz + vImpU[0] - vImpV[0];
	const vec3 v3 = gl_in[ 0 ].gl_Position.xyz - vImpU[0] + vImpV[0];
	const vec3 v4 = gl_in[ 0 ].gl_Position.xyz + vImpU[0] + vImpV[0];

	emitQuad( v1, v2, v3, v4 );
}
