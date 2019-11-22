#version 450

layout(location = 0) in vec3 aSpherePos;
layout(location = 1) in vec3 aSphereColor;
layout(location = 2) in float aSphereRad;

uniform mat4 uMVMatrix;
uniform mat4 uMVMatrixInverse;
uniform mat4 uProjMatrix;
uniform mat4 uProjMatrixTranspose;

flat out vec3 objSpherePos;
flat out vec3 sphereColor;
flat out float sphereRad;
flat out vec4 camPos;

void main()
{
	sphereColor = aSphereColor;
	sphereRad = aSphereRad;

	// position in object space
	objSpherePos = aSpherePos;
	// position in cam space
	vec3 camSpherePos = vec3(uMVMatrix * vec4(objSpherePos, 1.f));

	camPos = uMVMatrixInverse[3];
	camPos.xyz -= objSpherePos;

	// ray direction in cam space
	vec3 camRayDir = normalize(camSpherePos);

	// impostor position in cam space : in front of the sphere
	vec3 camImpPos = camSpherePos - (sphereRad * camRayDir);

	// compute impostor size in cam space
	const float dSphereCenter = length(camSpherePos);
	const float sinAngle = sphereRad / dSphereCenter;
	const float tanAngle = tan(asin(sinAngle));
	const float camImpSize = tanAngle * (dSphereCenter - sphereRad);

	// impostor vectors
	vec3 U = normalize(cross(camRayDir, vec3(1.f, 0.f, 0.f)));
	const vec3 V = cross(U, camRayDir) * camImpSize; // no need to normalize
	U *= camImpSize;

	// compute impostor corner position (x,y) in ndc and keep min and max
	vec4 camTest = vec4(camImpPos.xyz - U - V, 1.f);
	vec2 ndcTestXY
		= vec2(dot(camTest, uProjMatrix[0]), dot(camTest, uProjMatrix[1]))
		  / dot(camTest, uProjMatrixTranspose[3]);
	vec2 minPS = ndcTestXY;
	vec2 maxPS = minPS;

	camTest.xyz += U * 2.f;
	ndcTestXY = vec2(dot(camTest, uProjMatrix[0]), dot(camTest, uProjMatrix[1]))
				/ dot(camTest, uProjMatrixTranspose[3]);
	minPS = min(minPS, ndcTestXY);
	maxPS = max(maxPS, ndcTestXY);

	camTest.xyz += V * 2.f;
	ndcTestXY = vec2(dot(camTest, uProjMatrix[0]), dot(camTest, uProjMatrix[1]))
				/ dot(camTest, uProjMatrixTranspose[3]);
	minPS = min(minPS, ndcTestXY);
	maxPS = max(maxPS, ndcTestXY);

	camTest.xyz -= U * 2.f;
	ndcTestXY = vec2(dot(camTest, uProjMatrix[0]), dot(camTest, uProjMatrix[1]))
				/ dot(camTest, uProjMatrixTranspose[3]);
	minPS = min(minPS, ndcTestXY);
	maxPS = max(maxPS, ndcTestXY);

	vec2 clipImpPosZW
		= (camSpherePos.z + sphereRad) * uProjMatrix[2].zw + uProjMatrix[3].zw;

	float ndcZ = clipImpPosZW.x / clipImpPosZW.y;

	gl_Position = vec4((minPS + maxPS) * 0.5f, ndcZ, 1.f);

	gl_PointSize
		= (max(abs(maxPS.x - minPS.x) * 640, abs(maxPS.y - minPS.y) * 360));

	//	const float stochasticCoverage = gl_PointSize * gl_PointSize;
	//	const float threshold = 0.8f;
	//	if ((stochasticCoverage < threshold)
	//		&& ((gl_VertexID & 0xffff) > stochasticCoverage * (0xffff /
	// threshold)))
	//	{
	//		// "Cull" small voxels in a stable, stochastic way by moving past
	// the
	// z
	//		// = 0 plane. Assumes voxels are in randomized order.
	//		gl_Position = vec4(-1);
	//	}
	//	else
	//	{
	//		gl_Position = vec4((minPS + maxPS) * 0.5f, ndcZ, 1.f);
	//	}
}
