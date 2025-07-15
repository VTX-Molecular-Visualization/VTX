#version 450

layout( points ) in;
layout( triangle_strip, max_vertices = 4 ) out;

layout(std140, binding = 0) uniform SsesdfSettings
{
	mat4 uMVMatrix;
	mat4 uProjMatrix;
	float uProbeRadius;
};

flat in vec3 vImpU[];
flat in vec3 vImpV[];
flat in vec3 vTorusPosition[];
flat in vec4 vRadii[];
flat in mat3 vTorusRotationMatrix[]; 
flat in vec4 vOsVisibilitySphere[]; 

smooth out vec3  viewImpPos;
flat   out vec3  viewTorusPosition;
flat   out vec4  radii;
flat   out mat3  viewTorusRotationMatrix; 
flat   out vec4  osVisibilitySphere; 

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
	viewTorusPosition		= vTorusPosition[0];
	radii					= vRadii[0];
	viewTorusRotationMatrix = vTorusRotationMatrix[0];
	osVisibilitySphere      = vOsVisibilitySphere[0];

	// Compute impostors vertices.
	const vec3 v1 = gl_in[ 0 ].gl_Position.xyz - vImpU[ 0 ] - vImpV[ 0 ];
	const vec3 v2 = gl_in[ 0 ].gl_Position.xyz + vImpU[ 0 ] - vImpV[ 0 ];
	const vec3 v3 = gl_in[ 0 ].gl_Position.xyz - vImpU[ 0 ] + vImpV[ 0 ];
	const vec3 v4 = gl_in[ 0 ].gl_Position.xyz + vImpU[ 0 ] + vImpV[ 0 ];

	emitQuad( v1, v2, v3, v4 );
}
