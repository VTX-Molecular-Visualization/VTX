#version 450 core

#include "../constant.glsl"
#include "../layout_uniforms_color.glsl"

// In.
layout( location = 0 ) in vec4 inControlPointPosition;
layout( location = 1 ) in vec3 inControlPointDirection;
layout( location = 2 ) in uint inType;
layout( location = 3 ) in uint inColor;
layout( location = 4 ) in uint inResidueId;
layout( location = 5 ) in uint inResidueFlag;
layout( location = 6 ) in uint inResidueModel;
layout( location = 6 ) in uint inResidueRepresentation;

// Out.
out 
#include "struct_vertex_shader.glsl"
dataOut;

void main()
{
	dataOut.position		= inControlPointPosition;
	dataOut.direction		= inControlPointDirection;
	dataOut.color			= uniformsColor[ inColor ];
	dataOut.ssType			= inType;
	dataOut.id				= inResidueId;
	dataOut.visibility		= int( inResidueFlag ) & ( 1 << FLAG_VISIBILITY );
	dataOut.selection		= int( inResidueFlag ) & ( 1 << FLAG_SELECTION );	
	dataOut.model			= inResidueModel;
	dataOut.representation	= inResidueRepresentation;
}
