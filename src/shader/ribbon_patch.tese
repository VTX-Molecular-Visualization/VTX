#version 450

layout( triangles, equal_spacing, ccw ) in;

uniform mat4 uMVMatrix;
uniform mat4 uProjMatrix;
uniform mat4 uNormalMatrix;

in vec3 vPositionTC[];

smooth out vec3 vViewPosition;

vec3 interpolate3D(vec3 p_v0, vec3 p_v1, vec3 p_v2)
{
    return vec3(gl_TessCoord.x) * p_v0 + vec3(gl_TessCoord.y) * p_v1 + vec3(gl_TessCoord.z) * p_v2;
}

void main()
{
	vViewPosition = vec3( uMVMatrix * vec4( interpolate3D(vPositionTC[0], vPositionTC[1], vPositionTC[2]), 1.f ) );

	gl_Position = uProjMatrix * vec4( vViewPosition, 1.f );
}
