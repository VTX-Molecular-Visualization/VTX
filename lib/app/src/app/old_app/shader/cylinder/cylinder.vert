#version 450

#define ATOM_POSITION 0
#define ATOM_COLOR 1
#define ATOM_RADIUS 2
#define ATOM_VISIBILITY 3
#define ATOM_SELECTION 4
#define ATOM_ID 5

layout( location = ATOM_POSITION ) in vec3 aVertexPosition;
layout( location = ATOM_COLOR ) in vec4 aVertexColor;
// TODO: this is not used!
layout( location = ATOM_RADIUS ) in float aVertexRad;
layout( location = ATOM_VISIBILITY ) in uint aVertexVis;
layout( location = ATOM_SELECTION ) in uint aVertexSel;
layout( location = ATOM_ID ) in uint aVertexId;

uniform mat4 u_MVMatrix;
uniform mat4 u_projMatrix;

out VsOut
{
	flat vec4 vertexColor;
	flat uint vertexVisible;
	flat uint vertexSelected;
	flat uint vertexId;
}
vsOut;

void main()
{
	vsOut.vertexColor	 = aVertexColor;
	vsOut.vertexVisible	 = aVertexVis;
	vsOut.vertexSelected = aVertexSel;
	vsOut.vertexId		 = aVertexId;

	// Vertex position in view space.
	gl_Position = u_MVMatrix * vec4( aVertexPosition, 1.f );
}
