#version 450 core

// In.
layout( location = 0 ) in vec4 inControlPointPosition;
layout( location = 1 ) in vec3 inControlPointDirection;
layout( location = 2 ) in uint inType;
layout( location = 3 ) in vec4 inColor;
layout( location = 4 ) in uint inVisibility;
layout( location = 5 ) in uint inControlPointSelection;
layout( location = 6 ) in uint inResidueId;

// Out.
out 
#include "struct_vertex_shader.glsl"
dataOut;

void main()
{
	dataOut.position	 = inControlPointPosition;
	dataOut.direction	 = inControlPointDirection;
	dataOut.color		 = inColor;
	dataOut.ssType	     = inType;
	dataOut.visibility   = inVisibility;
	dataOut.selection	 = inControlPointSelection;
	dataOut.id		     = inResidueId;
}
