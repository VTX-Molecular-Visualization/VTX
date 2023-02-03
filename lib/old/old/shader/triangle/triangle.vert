#version 450

layout( location = 0 ) in vec4 aVertexPosition;
layout( location = 1 ) in vec4 aVertexNormal;
layout( location = 2 ) in vec3 aVertexColor;
layout( location = 3 ) in uint aVertexVis;
layout( location = 4 ) in uint aVertexSelected;
layout( location = 5 ) in uint aVertexId;

uniform mat4 u_MVMatrix;
uniform mat4 u_projMatrix;
uniform mat4 u_normalMatrix;

out VsOut
{
	smooth vec3 viewPosition;
	smooth vec3 normal;
	smooth vec3	color;
	flat uint   selected;
	flat uint   id;
}
vsOut;

void main()
{
	if ( aVertexVis == 0 )
	{
		return;
	}

	vsOut.viewPosition = vec3( u_MVMatrix * aVertexPosition );
	vsOut.normal	   = vec3( u_normalMatrix * aVertexNormal );
	vsOut.color		   = aVertexColor;
	vsOut.selected	   = aVertexSelected;
	vsOut.id		   = aVertexId;

	gl_Position = u_projMatrix * vec4( vsOut.viewPosition, 1.f );
}
