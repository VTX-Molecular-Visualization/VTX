#version 450

layout(points) in;
layout(triangle_strip, max_vertices = 3) out;

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

	const float halfSide = 1.73205081f; // sqrt(3.f);
	const vec3 hV = halfSide * vImpV[0];

	// Vertex 0
	camImpPos = gl_in[0].gl_Position.xyz - vImpU[0] + hV;
	gl_Position = uProjMatrix * vec4(camImpPos, 1.f);
	EmitVertex();

	// Vertex 1
	camImpPos = gl_in[0].gl_Position.xyz - vImpU[0] - hV;
	gl_Position = uProjMatrix * vec4(camImpPos, 1.f);
	EmitVertex();

	// Vertex 2
	camImpPos = gl_in[0].gl_Position.xyz + 2.f * vImpU[0];
	gl_Position = uProjMatrix * vec4(camImpPos, 1.f);
	EmitVertex();

	EndPrimitive();
}
