#version 450

layout( location = 0 ) in vec3 vertexPosition;

uniform mat4 uMVMatrix;
uniform mat4 uProjMatrix;
uniform mat4 uNormalMatrix;

smooth out vec3 vViewPosition;
smooth out vec3 vNormal;

void main() { 
	vViewPosition = vec3( uMVMatrix * vec4( vertexPosition, 1.f ) );
	vNormal		  = vec3( uNormalMatrix * vec4( vertexPosition, 1.f ) );
	vNormal  = normalize(vNormal);
	gl_Position = uProjMatrix * vec4( vViewPosition, 1.f );
}
