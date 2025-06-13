#version 450

layout( points ) in;
layout( triangle_strip, max_vertices = 4 ) out;

struct DisplaySegment
{
	vec3  v1;
	vec3  v2;
	vec4  startAtom;
	vec4  circle; // circle center + radius
	vec4  normal; // normal + segment angle
	vec3  bbPos;
	vec3  bbDim;
	vec4  rot;
	vec4  vSphere;
};

flat in vec3		   vImpU[]; // Impostor vectors.
flat in vec3		   vImpV[];
flat in DisplaySegment vSegment[];
flat in vec4		   vColor[];

smooth out vec3			  viewImpPos;  // Impostor position in view space.
flat   out DisplaySegment segment;
flat   out vec4			  color;
flat   out vec3			  obbRadius;
flat   out vec3			  obbPos;
flat   out vec4			  orientation;

layout(std140, binding = 0) uniform SesdfSettings
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
	segment		= vSegment[ 0 ];
	color		= vColor[ 0 ];

	// Compute impostors vertices.
	const vec3 v1 = gl_in[ 0 ].gl_Position.xyz - vImpU[ 0 ] - vImpV[ 0 ];
	const vec3 v2 = gl_in[ 0 ].gl_Position.xyz + vImpU[ 0 ] - vImpV[ 0 ];
	const vec3 v3 = gl_in[ 0 ].gl_Position.xyz - vImpU[ 0 ] + vImpV[ 0 ];
	const vec3 v4 = gl_in[ 0 ].gl_Position.xyz + vImpU[ 0 ] + vImpV[ 0 ];

	emitQuad( v1, v2, v3, v4 );
}
