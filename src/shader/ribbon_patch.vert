#version 450

layout( location = 0 ) in vec3 aControlPointPosition;
layout( location = 1 ) in vec3 aControlPointDirection;
layout( location = 2 ) in vec3 aControlPointSecondaryStructure;
layout( location = 3 ) in uint aControlPointVisibility;

uniform mat4 uMVMatrix;
uniform mat4 uProjMatrix;
uniform mat4 uNormalMatrix;

smooth out vec3 vViewPosition;
smooth out vec3 vNormal;

void main()
{
	vViewPosition = vec3( uMVMatrix * vec4( aControlPointPosition, 1.f ) );
	vNormal		  = vec3( uNormalMatrix * vec4( 1.f, 1.f, 1.f, 1.f ) );

	gl_Position = uProjMatrix * vec4( vViewPosition, 1.f );
}
