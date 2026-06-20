#version 460 core

#include "layout_uniforms_camera.glsl"

layout( location = 0 ) in vec3 inPosition;

out vec3 worldDirection;

void main()
{
    gl_Position = vec4( inPosition, 1.f );

    if ( uniformsCamera.isCameraPerspective == 1u )
    {
        const vec4 viewDirection
            = inverse( uniformsCamera.matrixProjection ) * vec4( inPosition.xy, 1.f, 1.f );
        worldDirection = mat3( uniformsCamera.matrixViewInv ) * viewDirection.xyz;
    }
    else
    {
        worldDirection = mat3( uniformsCamera.matrixViewInv ) * vec3( 0.f, 0.f, -1.f );
    }
}
