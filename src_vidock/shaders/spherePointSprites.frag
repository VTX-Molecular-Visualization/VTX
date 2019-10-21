#version 450

uniform mat4 uProjMatrix;
uniform vec2 uScreenSize;

in SphereData
{
	flat in vec4 spherePosition;
	flat in vec3 sphereColor;
	flat in float sphereRadius;
};

out vec4 fragColor;

void main()
{
	vec3 normal;
	normal.xy = gl_PointCoord * 2.f - vec2(1.f);

	float r2 = dot(normal.xy, normal.xy);

	if (r2 > 1.f)
		discard;

	normal.z = sqrt(1.f - r2);
	vec4 pos = spherePosition;
	pos.z += normal.z;
	pos = uProjMatrix * pos;

	gl_FragDepth = 0.5f + 0.5f * pos.z / pos.w;
	vec3 lightDir = vec3(0.5f, 0.5f, 0.5f);
	float diffuseTerm = clamp(dot(normal, lightDir), 0.f, 1.f);
	fragColor = vec4(vec3(0.1f, 0.1f, 0.1f) + sphereColor * diffuseTerm, 1.f);
}
