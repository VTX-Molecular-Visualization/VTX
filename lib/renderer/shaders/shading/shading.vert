#version 460 core

#include "../layout_uniforms_camera.glsl"

layout( location = 0 ) in vec3 inPosition;

out vec3 worldDirection;

void main()
{
	gl_Position = vec4( inPosition, 1.f );

	const float aspect		  = float( uniformsCamera.resolution.x ) / float( uniformsCamera.resolution.y );
	const vec3	viewDirection = vec3(
		inPosition.x * aspect * uniformsCamera.cameraTanHalfFov, inPosition.y * uniformsCamera.cameraTanHalfFov, -1.f
	);
	worldDirection = mat3( uniformsCamera.matrixViewInv ) * viewDirection;
}
