#version 450

layout( location = 0 ) in vec3 aSpherePosition;
layout( location = 1 ) in vec3 aSphereColor;

uniform mat4 uMVPMatrix;

flat out vec3 sphereColor;

void main()
{
	sphereColor = aSphereColor;

	gl_Position = uMVPMatrix * vec4( aSpherePosition, 1.f );
}
