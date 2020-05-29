#version 450

layout( points ) in;
layout( triangle_strip, max_vertices = 4 ) out;

uniform mat4  uProjMatrix;
uniform float uZNear;
uniform float uZFar;

flat in vec3  vViewSpherePos[]; // sphere position in view space
flat in vec3  vSphereColor[];
flat in float vSphereRad[];
flat in uint  vSphereVis[];
// impostor vectors
flat in vec3  vImpU[];
flat in vec3  vImpV[];
flat in float vDotViewSpherePos[];

smooth out vec3 viewImpPos;	   // impostor position in view space
flat out vec3	viewSpherePos; // sphere position in view space
flat out vec3	sphereColor;
flat out float	sphereRad;
flat out float	dotViewSpherePos;

void main()
{
	// Do not emit primitive if sphere is not visible
	if ( vSphereVis[ 0 ] == 0 )
	{
		return;
	}

	const vec3 v1 = gl_in[ 0 ].gl_Position.xyz - vImpU[ 0 ] - vImpV[ 0 ];
	const vec3 v2 = gl_in[ 0 ].gl_Position.xyz + vImpU[ 0 ] - vImpV[ 0 ];
	const vec3 v3 = gl_in[ 0 ].gl_Position.xyz - vImpU[ 0 ] + vImpV[ 0 ];
	const vec3 v4 = gl_in[ 0 ].gl_Position.xyz + vImpU[ 0 ] + vImpV[ 0 ];

	// Hack for Z-clipping: avoid artifacts due to near clipping (incomplete spheres)
	// For near clipping plane:
	// Get the nearest impostor vertex from camera (max because direction is -z)
	// If it is in front of near clipping plane, then do not emit primitive
	// If uZNear is 0, GL_DEPTH_CLAMP is enabled, do not reject
	const float n = max( v1.z, max( v2.z, max( v3.z, v4.z ) ) );
	if ( n > -uZNear && uZNear != 0.f)
		return;
	// For far clipping plane...
	const float f = min( v1.z, min( v2.z, min( v3.z, v4.z ) ) );
	if ( f < -uZFar )
		return;

	// Output data
	viewSpherePos	 = vViewSpherePos[ 0 ];
	sphereColor		 = vSphereColor[ 0 ];
	sphereRad		 = vSphereRad[ 0 ];
	dotViewSpherePos = vDotViewSpherePos[ 0 ];

	// Creates quad impostor
	// Vertex 1
	viewImpPos	= v1;
	gl_Position = uProjMatrix * vec4( viewImpPos, 1.f );
	EmitVertex();

	// Vertex 2
	viewImpPos	= v2;
	gl_Position = uProjMatrix * vec4( viewImpPos, 1.f );
	EmitVertex();

	// Vertex 3
	viewImpPos	= v3;
	gl_Position = uProjMatrix * vec4( viewImpPos, 1.f );
	EmitVertex();

	// Vertex 4
	viewImpPos	= v4;
	gl_Position = uProjMatrix * vec4( viewImpPos, 1.f );
	EmitVertex();

	EndPrimitive();
}
