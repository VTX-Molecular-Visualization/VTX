#version 450

struct Sphere
{
	vec3 pos;
	float rad;
	vec3 color;
};

layout(std430, binding = 0) readonly buffer sphereData
{
	Sphere data[];
};

uniform mat4 uMVMatrix;
uniform mat4 uProjMatrix;

smooth out vec3 camImpPos;  // impostor position in cam space
flat out vec3 camSpherePos; // sphere impostor in cam space
flat out vec3 sphereColor;
flat out float sphereRad;
flat out float dotCamSpherePos;

const float impCorners[8] = { -1.f, -1.f, 1.f, -1.f, -1.f, 1.f, 1.f, 1.f };

void main()
{
	const int id = gl_VertexID >> 2;
	camSpherePos = vec3(uMVMatrix * vec4(data[id].pos, 1.f));
	sphereColor = data[id].color;
	sphereRad = data[id].rad;

	// camera coordinates system
	const vec3 dir = normalize(camSpherePos);
	const vec3 U = normalize(cross(dir, vec3(1.f, 0.f, 0.f)));
	const vec3 V = cross(U, dir); // no need to normalize

	// impostor in front of the sphere
	camImpPos = camSpherePos - (sphereRad * dir);

	// compute impostor size
	dotCamSpherePos = dot(camSpherePos, camSpherePos);
	const float dSphereCenter = sqrt(dotCamSpherePos);
	const float sinAngle = sphereRad / dSphereCenter;
	const float tanAngle = tan(asin(sinAngle));
	const float impSize = tanAngle * (dSphereCenter - sphereRad);

	// compute impostor corner position
	const int i = 2 * (gl_VertexID & 3);
	camImpPos += (impCorners[i] * U + impCorners[i + 1] * V) * impSize;

	gl_Position = uProjMatrix * vec4(camImpPos, 1.f);
}
