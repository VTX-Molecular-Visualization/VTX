#version 450

layout( location = 0 ) in vec3 aVertexPosition;
layout( location = 1 ) in vec3 aVertexNormal;
layout( location = 2 ) in vec3 aVertexColor;
layout( location = 3 ) in unsigned short aVertexVis;

uniform mat4 u_MVMatrix;
uniform mat4 u_projMatrix;
uniform mat4 u_normalMatrix;

smooth out vec3 vViewPosition;
smooth out vec3 vNormal;
flat out vec3	vColor;

void main()
{
	if ( aVertexVis == 0us )
	{
		return;
	}

	vViewPosition = vec3( u_MVMatrix * vec4( aVertexPosition, 1.f ) );
	vNormal		  = vec3( u_normalMatrix * vec4( aVertexNormal, 1.f ) );
	vColor		  = aVertexColor;

	gl_Position = u_projMatrix * vec4( vViewPosition, 1.f );
}
