#version 450

layout( location = 0 ) in vec3 aControlPointPosition;
layout( location = 1 ) in vec3 aControlPointDirection;
layout( location = 2 ) in uint aControlPointSecondaryStructure;
layout( location = 3 ) in vec3 aControlPointColor;
layout( location = 4 ) in uint aControlPointVisibility;

// uniform mat4 uMVMatrix;
// uniform mat4 uProjMatrix;
// uniform mat4 uNormalMatrix;

out vec3 vPositionVS;
out vec3 vDirectionVS;
out uint vSecondaryStructureVS;
out vec3 vColorVS;

void main()
{
	vPositionVS			  = aControlPointPosition;
	vDirectionVS		  = aControlPointDirection;
	vSecondaryStructureVS = aControlPointSecondaryStructure;
	vColorVS			  = aControlPointColor;
}
