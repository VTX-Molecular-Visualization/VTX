#version 450

layout( points ) in;
layout( triangle_strip, max_vertices = 4 ) out;

struct DisplayFullCircle
{
	vec4  firstAtom;  // ith pos + ith radius
	vec4  secondAtom; // jth pos + jth radius
	vec4  center;	  // Circle center + circle radius
	vec4  normal;	  // normal + isExterior
	vec3  bbPos;
	vec3  bbDim;
	vec4  rot;
	vec4  vSphere;
};

flat in vec3			   vImpU[]; // Impostor vectors.
flat in vec3			   vImpV[];
flat in DisplayFullCircle vCircle[];

smooth out vec3				 viewImpPos;  // Impostor position in view space.
flat   out DisplayFullCircle circle;

layout(std140, binding=0) uniform SesdfSettings
{
	mat4  uMVMatrix;
	mat4  uProjMatrix;
	mat4  uInvMVMatrix;
	float uProbeRadius;
	uint  uMaxProbeNeighborNb;
};

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
	circle = vCircle[ 0 ];

	// Compute impostors vertices.
	const vec3 v1 = gl_in[ 0 ].gl_Position.xyz - vImpU[ 0 ] - vImpV[ 0 ];
	const vec3 v2 = gl_in[ 0 ].gl_Position.xyz + vImpU[ 0 ] - vImpV[ 0 ];
	const vec3 v3 = gl_in[ 0 ].gl_Position.xyz - vImpU[ 0 ] + vImpV[ 0 ];
	const vec3 v4 = gl_in[ 0 ].gl_Position.xyz + vImpU[ 0 ] + vImpV[ 0 ];

	emitQuad( v1, v2, v3, v4 );
}
