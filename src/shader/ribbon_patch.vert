#version 450

layout( location = 0 ) in vec3 vao_controlPointPosition;
layout( location = 1 ) in vec3 vao_controlPointDirection;
layout( location = 2 ) in vec3 vao_controlPointNormal;
layout( location = 3 ) in uint vao_controlPointSecondaryStructure;
layout( location = 4 ) in vec3 vao_controlPointColor;

out vec3 vs_position;
out vec3 vs_direction;
out vec3 vs_normal;
out uint vs_secondaryStructure;
out vec3 vs_color;

void main()
{
	vs_position			  = vao_controlPointPosition;
	vs_direction		  = vao_controlPointDirection;
	vs_normal			  = vao_controlPointNormal;
	vs_secondaryStructure = vao_controlPointSecondaryStructure;
	vs_color			  = vao_controlPointColor;
}
