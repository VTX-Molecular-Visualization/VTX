#version 450

uniform mat4 uProjMatrix;
uniform float uCylinderRad;

smooth in vec3 camImpPos;
flat in vec3 cylCenter;
flat in vec3 A;
flat in vec3 B;
flat in vec3 BA;
flat in vec3 color0;
flat in vec3 color1;
flat in float cylRadSqr;

out vec4 fragColor;

void main()
{
	vec3 ba = BA;

	vec3 oc = -A;
	vec3 rd = normalize(camImpPos);

	float baba = dot(ba, ba);
	float bard = dot(ba, rd);
	float baoc = dot(ba, oc);

	float k2 = baba - bard * bard;
	float k1 = baba * dot(oc, rd) - baoc * bard;
	float k0 = baba * dot(oc, oc) - baoc * baoc - cylRadSqr * baba;

	float h = k1 * k1 - k2 * k0;

	if (h < 0.f)
	{
		discard;
		// fragColor = vec4(1.f, 0.f, 1.f, 1.f); // vec4(vertexColor,
		// 0.5f);
	}
	else
	{
		float t = (-k1 - sqrt(h)) / k2;

		// body
		float y = baoc + t * bard;
		if (y > 0.0 && y < baba)
		{
			const vec3 camHit = rd * t;
			const vec2 clipZW
				= camHit.z * uProjMatrix[2].zw + uProjMatrix[3].zw;

			gl_FragDepth = ((clipZW.x / clipZW.y) + 1.f) * 0.5f;

			const vec3 normal = (oc + camHit - ba * y / baba) / uCylinderRad;
			const vec3 lightDir = vec3(0.5f, 0.5f, 0.5f);
			const float diffuse = clamp(dot(normal, lightDir), 0.f, 1.f);

			const vec3 col = y < baba * 0.5f ? color0 : color1;

			fragColor = vec4(vec3(0.1f, 0.1f, 0.1f) + col * diffuse, 1.f);
		}
		else
		{
			discard;
			// fragColor = vec4(1.f, 1.f, 0.f, 1.f); // vec4(vertexColor,
			// 0.5f);
		}
	}
	//	fragColor = vec4(1.f, 1.f, 0.f, 1.f);
}
