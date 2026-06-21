#version 460 core

#include "../constant.glsl"
#include "../layout_indexed_indirect_draws.glsl"
#include "../layout_uniforms_color.glsl"

// In.
#include "../layout/ribbon.glsl"

// Out.
out
#include "struct_vertex_shader.glsl"
	dataOut;

void main()
{
	const uint idModel = indexedDraws[ gl_DrawID ].idModel;

	dataOut.position	   = inControlPointPosition;
	dataOut.direction	   = inControlPointDirection;
	dataOut.color		   = uniformsColor[ inColor ];
	dataOut.ssType		   = inType;
	dataOut.id			   = inResidueId;
	dataOut.selection	   = int( inResidueFlag ) & ( 1 << FLAG_SELECTION );
	dataOut.model		   = idModel;
	dataOut.representation = inResidueRepresentation;
}
