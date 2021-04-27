#version 450

#define ATOM_POSITION 0
#define ATOM_COLOR 1
#define ATOM_RADIUS 2
#define ATOM_VISIBILITY 3
#define ATOM_SELECTION 4

layout( location = ATOM_POSITION ) in vec3 aVertexPosition;
layout( location = ATOM_COLOR ) in vec3 aVertexColor;
layout( location = ATOM_RADIUS ) in float aVertexRad;
layout( location = ATOM_VISIBILITY ) in unsigned short aVertexVis;
layout( location = ATOM_SELECTION ) in unsigned short aVertexSel;

uniform mat4 u_MVMatrix;
uniform mat4 u_projMatrix;

out VsOut
{
	flat vec3			vertexColor;
	flat unsigned short vertexVisible;
	flat unsigned short vertexSelected;
}
vsOut;

void main()
{
	vsOut.vertexColor	 = aVertexColor;
	vsOut.vertexVisible	 = aVertexVis;
	vsOut.vertexSelected = aVertexSel;

	// Vertex position in view space.
	gl_Position = u_MVMatrix * vec4( aVertexPosition, 1.f );
}
