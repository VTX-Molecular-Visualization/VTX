#version 450

layout( location = 0 ) in vec4 vao_controlPointPosition;
layout( location = 1 ) in vec3 vao_controlPointDirection;
layout( location = 2 ) in uint vao_ssType;
layout( location = 3 ) in vec4 vao_color;
layout( location = 4 ) in uint vao_visibility;
layout( location = 5 ) in uint vao_controlPointSelection;
layout( location = 6 ) in uint vao_residueId;

out VsOut
{
	vec4	  position;
	vec3	  direction;
	flat vec4 color;
	flat uint ssType;
	flat uint visibility;
	flat uint selection;
	flat uint id;
}
vsOut;

void main()
{
	vsOut.position	 = vao_controlPointPosition;
	vsOut.direction	 = vao_controlPointDirection;
	vsOut.color		 = vao_color;
	vsOut.ssType	 = vao_ssType;
	vsOut.visibility = vao_visibility;
	vsOut.selection	 = vao_controlPointSelection;
	vsOut.id		 = vao_residueId;
}
