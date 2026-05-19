#version 460 core

#include "../constant.glsl"
#include "../layout_uniforms_color.glsl"

// In.
#include "../layout/ribbon.glsl"

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
	dataOut.selection		= int( inResidueFlag ) & ( 1 << FLAG_SELECTION );	
	dataOut.model			= gl_DrawID;
	dataOut.representation	= inResidueRepresentation;
}
