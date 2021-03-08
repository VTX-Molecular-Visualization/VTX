#version 450

layout( location = 0 ) in vec4 vao_controlPointPosition;
layout( location = 1 ) in vec3 vao_controlPointDirection;
layout( location = 2 ) in unsigned short vao_ssType;
layout( location = 3 ) in vec3 vao_color;
layout( location = 4 ) in unsigned short vao_controlPointSelection;

out VsOut
{
	vec4				position;
	vec3				direction;
	flat vec3			color;
	flat unsigned short ssType;
	flat unsigned short selection;
}
vsOut;

void main()
{
	vsOut.position	= vao_controlPointPosition;
	vsOut.direction = vao_controlPointDirection;
	/// TODO: change it! Just for debug
	if (vao_ssType < 5us) vsOut.color = vec3(0.8f,0.f,0.f);
	else if (vao_ssType == 5us) vsOut.color = vec3( 0.9f,0.9f,0.f);
	else vsOut.color = vsOut.color = vec3(0.1f,0.1f,0.9f);
	//vsOut.color		= vao_color;
	vsOut.ssType	= vao_ssType;
	vsOut.selection = vao_controlPointSelection;
}
