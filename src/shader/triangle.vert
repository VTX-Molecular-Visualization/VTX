#version 450

layout( location = 0 ) in vec3 aVertexPosition;
layout( location = 1 ) in vec3 aVertexNormal;
layout( location = 2 ) in vec3 aVertexColor;
layout( location = 3 ) in uint aVertexVis;

uniform mat4 uMVMatrix;
uniform mat4 uProjMatrix;
uniform mat4 uNormalMatrix;

smooth out vec3 vViewPosition;
smooth out vec3 vNormal;
flat out vec3 vColor;

void main()
{
	if ( aVertexVis == 0 )
	{
		return;
	}

	vViewPosition = vec3( uMVMatrix * vec4( aVertexPosition, 1.f ) );
	vNormal		  = vec3( uNormalMatrix * vec4( aVertexNormal, 1.f ) );
	vColor		  = aVertexColor;

	gl_Position = uProjMatrix * vec4( vViewPosition, 1.f );
}
