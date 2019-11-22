#version 450

layout(lines) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 uProjMatrix;
uniform float uCylRad;

flat in vec3 vVertexColor[]; // on color per atom

smooth out vec3 impPos;
flat out vec3 cylCenter;
flat out vec3 cylV0;
flat out vec3 cylV1;
flat out vec3 colors[2];

// all is done in cam space
void main()
{
	colors[0] = vVertexColor[0];
	colors[1] = vVertexColor[1];

	// two vertices
	cylV0 = gl_in[0].gl_Position.xyz;
	cylV1 = gl_in[1].gl_Position.xyz;
	cylCenter = (cylV0 + cylV1) * 0.5f;

	// compupte normalized view vector
	const float dCenter = length(cylCenter);
	const vec3 view = cylCenter / dCenter;

	// compute cylinder coordinates system with 'x' orthogonal to 'view'
	const vec3 z = normalize(cylV1 - cylV0);
	const vec3 x = normalize(cross(view, z));
	const vec3 y = cross(x, z); // no need to normalize

	vec3 v0, v1;
	if (cylV0.z > cylV1.z)
	{
		v0 = cylV0;
		v1 = cylV1;
	}
	else
	{
		v0 = cylV1;
		v1 = cylV0;
	}

	// compute impostor vertices
	const float dV0 = length(v0);
	const float dV1 = length(v1);

	float sinAngle = uCylRad / dV0;
	float angle = asin(sinAngle);

	const vec3 y1 = y * uCylRad;
	const vec3 x2 = x * uCylRad * cos(angle);
	const vec3 y2 = y1 * sinAngle;

	impPos = v0 + x2 + y2;
	gl_Position = uProjMatrix * vec4(impPos, 1.f);
	EmitVertex();

	impPos = v0 - x2 + y2;
	gl_Position = uProjMatrix * vec4(impPos, 1.f);
	EmitVertex();

	angle = asin(uCylRad / dV1);
	const vec3 x3 = x * (dV1 - uCylRad) * tan(angle);

	impPos = v1 + x3 + y1;
	gl_Position = uProjMatrix * vec4(impPos, 1.f);
	EmitVertex();

	impPos = v1 - x3 + y1;
	gl_Position = uProjMatrix * vec4(impPos, 1.f);
	EmitVertex();

	EndPrimitive();
}
