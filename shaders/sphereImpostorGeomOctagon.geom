#version 450

layout(points) in;
layout(triangle_strip, max_vertices = 8) out;

uniform mat4 uProjMatrix;

smooth in vec3 vCamImpPos[];  // impostor position in cam space
flat in vec3 vCamSpherePos[]; // sphere impostor in cam space
flat in vec3 vSphereColor[];
flat in float vSphereRad[];
// impostor vectors
flat in vec3 vImpU[];
flat in vec3 vImpV[];
flat in float vDotCamSpherePos[];

smooth out vec3 camImpPos;  // impostor position in cam space
flat out vec3 camSpherePos; // sphere impostor in cam space
flat out vec3 sphereColor;
flat out float sphereRad;
flat out float dotCamSpherePos;

void main()
{
	// Output vertex position
	camSpherePos = vCamSpherePos[0];
	sphereColor = vSphereColor[0];
	sphereRad = vSphereRad[0];
	dotCamSpherePos = vDotCamSpherePos[0];

	const float halfSide = 0.41421356f; // sqrt(2.f) - 1.f;
	const vec3 hU = halfSide * vImpU[0];
	const vec3 hV = halfSide * vImpV[0];

	// Vertex 0
	camImpPos = gl_in[0].gl_Position.xyz + hU + vImpV[0];
	gl_Position = uProjMatrix * vec4(camImpPos, 1.f);
	EmitVertex();

	// Vertex 1
	camImpPos = gl_in[0].gl_Position.xyz + vImpU[0] + hV;
	gl_Position = uProjMatrix * vec4(camImpPos, 1.f);
	EmitVertex();

	// Vertex 2
	camImpPos = gl_in[0].gl_Position.xyz - hU + vImpV[0];
	gl_Position = uProjMatrix * vec4(camImpPos, 1.f);
	EmitVertex();

	// Vertex 3
	camImpPos = gl_in[0].gl_Position.xyz + vImpU[0] - hV;
	gl_Position = uProjMatrix * vec4(camImpPos, 1.f);
	EmitVertex();

	// Vertex 4
	camImpPos = gl_in[0].gl_Position.xyz - vImpU[0] + hV;
	gl_Position = uProjMatrix * vec4(camImpPos, 1.f);
	EmitVertex();

	// Vertex 5
	camImpPos = gl_in[0].gl_Position.xyz + hU - vImpV[0];
	gl_Position = uProjMatrix * vec4(camImpPos, 1.f);
	EmitVertex();

	// Vertex 6
	camImpPos = gl_in[0].gl_Position.xyz - vImpU[0] - hV;
	gl_Position = uProjMatrix * vec4(camImpPos, 1.f);
	EmitVertex();

	// Vertex 7
	camImpPos = gl_in[0].gl_Position.xyz - hU - vImpV[0];
	gl_Position = uProjMatrix * vec4(camImpPos, 1.f);
	EmitVertex();

	EndPrimitive();
}
