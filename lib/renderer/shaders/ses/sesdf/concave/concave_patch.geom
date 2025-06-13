#version 450

layout( points ) in;
layout( triangle_strip, max_vertices = 4 ) out;

struct Plane 
{
	vec3 n;
	float d;
};

struct DisplayTetrahedron
{
	vec4 point;			  // (x, y, z): coordinate of the point
	Plane plane1;
	Plane plane2;
	Plane plane3;
	int  startNeighborId;
	int  neighborNb;
	vec3 color;
};

flat in vec3			   vImpU[]; // Impostor vectors.
flat in vec3			   vImpV[];
flat in DisplayTetrahedron vTetrahedron[];

layout(std140, binding = 0) uniform SesdfSettings
{
	mat4  uMVMatrix;
	mat4  uProjMatrix;
	mat4  uInvMVMatrix;
	float uProbeRadius;
	uint  uMaxProbeNeighborNb;
};

smooth out vec3				  viewImpPos;  // Impostor position in view space.
flat   out DisplayTetrahedron tetrahedron;

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
	tetrahedron = vTetrahedron[ 0 ];

	// Compute impostors vertices.
	const vec3 v1 = gl_in[ 0 ].gl_Position.xyz - vImpU[ 0 ] - vImpV[ 0 ];
	const vec3 v2 = gl_in[ 0 ].gl_Position.xyz + vImpU[ 0 ] - vImpV[ 0 ];
	const vec3 v3 = gl_in[ 0 ].gl_Position.xyz - vImpU[ 0 ] + vImpV[ 0 ];
	const vec3 v4 = gl_in[ 0 ].gl_Position.xyz + vImpU[ 0 ] + vImpV[ 0 ];

	emitQuad( v1, v2, v3, v4 );
}
