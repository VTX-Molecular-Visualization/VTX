#version 450

layout( points ) in;
layout( triangle_strip, max_vertices = 4 ) out;

layout(std140, binding = 0) uniform SsesdfSettings
{
	mat4 uMVMatrix;
	mat4 uProjMatrix;
	float uProbeRadius;
};

flat in vec3  vViewSpherePos[]; // Sphere position in view space.
flat in vec3  vSphereColor[];
flat in float vSphereRad[];
flat out uint vVisible[];
// Impostor vectors.
flat in vec3  vImpU[];
flat in vec3  vImpV[];
flat in float vDotViewSpherePos[];

smooth out vec3 viewImpPos;	   // Impostor position in view space.
flat out vec3	viewSpherePos; // Sphere position in view space.
flat out vec3	sphereColor;
flat out float	sphereRad;
flat out float	dotViewSpherePos;

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
	if(vVisible[ 0 ] == 0)
		return;

	// Output data.
	viewSpherePos	 = vViewSpherePos[ 0 ];
	sphereColor		 = vSphereColor[ 0 ];
	sphereRad		 = vSphereRad[ 0 ];
	dotViewSpherePos = vDotViewSpherePos[ 0 ];

	// Compute impostors vertices.
	const vec3 v1 = gl_in[ 0 ].gl_Position.xyz - vImpU[ 0 ] - vImpV[ 0 ];
	const vec3 v2 = gl_in[ 0 ].gl_Position.xyz + vImpU[ 0 ] - vImpV[ 0 ];
	const vec3 v3 = gl_in[ 0 ].gl_Position.xyz - vImpU[ 0 ] + vImpV[ 0 ];
	const vec3 v4 = gl_in[ 0 ].gl_Position.xyz + vImpU[ 0 ] + vImpV[ 0 ];

	emitQuad( v1, v2, v3, v4 );
}
