#version 450

layout( location = 0 ) in vec3 aVertexPosition;

uniform mat4 uMVMatrix;
uniform mat4 uProjMatrix;
uniform mat4 uNormalMatrix;

out VsOut
{
	smooth vec3 viewPosition;
	smooth vec3 normal;
}
vsOut;

void main()
{
	vsOut.viewPosition = vec3( uMVMatrix * vec4( aVertexPosition, 1.f ) );
	vsOut.normal	   = normalize( vec3( uNormalMatrix * vec4( aVertexPosition, 1.f ) ) );
	gl_Position		   = uProjMatrix * vec4( vsOut.viewPosition, 1.f );
}
