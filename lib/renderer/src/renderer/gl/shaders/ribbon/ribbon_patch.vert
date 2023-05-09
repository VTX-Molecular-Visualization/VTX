#version 450 core

layout( location = 0 ) in vec4 vao_controlPointPosition;
layout( location = 1 ) in vec3 vao_controlPointDirection;
layout( location = 2 ) in uint vao_ssType;
layout( location = 3 ) in vec4 vao_color;
layout( location = 4 ) in uint vao_visibility;
layout( location = 5 ) in uint vao_controlPointSelection;
layout( location = 6 ) in uint vao_residueId;

out 
#include "struct_vertex_shader.glsl"
dataOut;

void main()
{
	dataOut.position	 = vao_controlPointPosition;
	dataOut.direction	 = vao_controlPointDirection;
	dataOut.color		 = vao_color;
	dataOut.ssType	 = vao_ssType;
	dataOut.visibility = vao_visibility;
	dataOut.selection	 = vao_controlPointSelection;
	dataOut.id		 = vao_residueId;
}
