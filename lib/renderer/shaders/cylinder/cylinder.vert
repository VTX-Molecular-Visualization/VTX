#version 460 core

#include "../constant.glsl"
#include "../layout_uniforms_camera.glsl"
#include "../layout_uniforms_color.glsl"
#include "../layout_uniforms_model.glsl"
#include "../struct/draw_indexed_indirect.glsl"
#include "struct_cylinder.glsl"

// In.
#include "../layout/atom.glsl"

// Out.
flat out StructCylinder vsCylinder;

layout( std430, binding = 21 ) readonly buffer CylinderIndirectDraws
{
	uint cylinderDrawCount;
	uint cylinderDrawPadding0;
	uint cylinderDrawPadding1;
	uint cylinderDrawPadding2;
	DrawIndexedIndirectRecord cylinderDraws[];
};

void main()
{
	const uint idModel = cylinderDraws[ gl_DrawID ].idModel;

	vsCylinder.color			= uniformsColor[ inAtomColor ];
	vsCylinder.isSelected		= int( inAtomFlag ) & ( 1 << FLAG_SELECTION );
	vsCylinder.id				= inAtomId;
	vsCylinder.representation	= inAtomRepresentation;

	// Vertex position in view space.
	gl_Position = uniformsModel[ idModel ].matrixModelView * vec4( inAtomPosition, 1.f );
}
