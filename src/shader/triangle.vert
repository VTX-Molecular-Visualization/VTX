#version 450

layout( location = 0 ) in vec3 aVertexPosition;
layout( location = 1 ) in vec3 aVertexNormal;
layout( location = 2 ) in vec3 aVertexColor;
layout( location = 3 ) in uint aVertexVis;

uniform mat4 u_MVMatrix;
uniform mat4 u_projMatrix;
uniform mat4 u_normalMatrix;

out VsOut
{
	smooth vec3 viewPosition;
	smooth vec3 normal;
	flat vec3	color;
}
vsOut;

void main()
{
	if ( aVertexVis == 0 )
	{
		return;
	}

	vsOut.viewPosition = vec3( u_MVMatrix * vec4( aVertexPosition, 1.f ) );
	vsOut.normal	   = vec3( u_normalMatrix * vec4( aVertexNormal, 1.f ) );
	vsOut.color		   = aVertexColor;

	gl_Position = u_projMatrix * vec4( vsOut.viewPosition, 1.f );
}
