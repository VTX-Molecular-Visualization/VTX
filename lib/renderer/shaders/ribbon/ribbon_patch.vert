#version 460 core

#include "../constant.glsl"
#include "../layout_uniforms_color.glsl"
#include "../struct/draw_indexed_indirect.glsl"

// In.
#include "../layout/ribbon.glsl"

// Out.
out 
#include "struct_vertex_shader.glsl"
dataOut;

layout( std430, binding = 22 ) readonly buffer RibbonIndirectDraws
{
	uint ribbonDrawCount;
	uint ribbonDrawPadding0;
	uint ribbonDrawPadding1;
	uint ribbonDrawPadding2;
	DrawIndexedIndirectRecord ribbonDraws[];
};

void main()
{
	const uint idModel = ribbonDraws[ gl_DrawID ].idModel;

	dataOut.position		= inControlPointPosition;
	dataOut.direction		= inControlPointDirection;
	dataOut.color			= uniformsColor[ inColor ];
	dataOut.ssType			= inType;
	dataOut.id				= inResidueId;
	dataOut.selection		= int( inResidueFlag ) & ( 1 << FLAG_SELECTION );	
	dataOut.model			= idModel;
	dataOut.representation	= inResidueRepresentation;
}
