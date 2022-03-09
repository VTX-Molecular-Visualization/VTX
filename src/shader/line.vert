#version 450

layout( location = 0 ) in vec3 aVertexPosition;

uniform mat4 u_MVMatrix;
uniform mat4 u_projMatrix;
uniform mat4 u_normalMatrix;

out VsOut
{
	smooth vec3 viewPosition;
	smooth vec3 normal;
}
vsOut;

void main()
{
	vsOut.viewPosition = vec3( u_MVMatrix * vec4( aVertexPosition, 1.f ) );
	vsOut.normal	   = normalize( vec3( u_normalMatrix * vec4( aVertexPosition, 1.f ) ) );
	gl_Position		   = u_projMatrix * vec4( vsOut.viewPosition, 1.f );
}
