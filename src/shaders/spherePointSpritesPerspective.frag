#version 450

uniform mat4 uMVPMatrixInverse;
uniform mat4 uMVPMatrixTranspose;

flat in vec3 objSpherePos;
flat in vec3 sphereColor;
flat in float sphereRad;
flat in vec4 camPos;

out vec4 fragColor;

// same as rasmol

void main()
{
	vec4 coord = gl_FragCoord * vec4(2.f / 1280.f, 2.f / 720.f, 2.0, 0.0)
				 + vec4(-1.0, -1.0, -1.0, 1.0);

	// transform fragment coordinates from view coordinates to object
	// coordinates.
	coord = uMVPMatrixInverse * coord;
	coord /= coord.w;
	coord.xyz -= objSpherePos; // ... and to glyph space

	vec3 ray;
	float lambda;

	// calc the viewing ray
	ray = normalize(coord.xyz - camPos.xyz);

	// calculate the geometry-ray-intersection
	float d1 = -dot(camPos.xyz,
					ray); // projected length of the cam-sphere-vector onto the
	float d2s = dot(camPos.xyz, camPos.xyz)
				- d1 * d1; // off axis of cam-sphere-vector and ray
	float radicand
		= sphereRad * sphereRad
		  - d2s; // square of difference of projected length and lambda
	if (radicand < 0.0)
	{
		// fragColor = vec4(sphereColor, 0.5f);
		discard;
	}
	else
	{
		// BACKSIDE_ENABLED
		lambda = d1 - sqrt(radicand);			 // lambda
		vec3 camHit = lambda * ray + camPos.xyz; // intersection

		// calculate depth
		vec4 Ding = vec4(camHit + objSpherePos, 1.0);
		float depth = dot(uMVPMatrixTranspose[2], Ding);
		float depthW = dot(uMVPMatrixTranspose[3], Ding);
		gl_FragDepth = ((depth / depthW) + 1.0) * 0.5;

		// normal at intersection point
		// vec3 normal = sphereintersection / sphereRad;

		fragColor = vec4(sphereColor, 1.f);
	}
	// fragColor = vec4(sphereColor, 0.5f);
}
