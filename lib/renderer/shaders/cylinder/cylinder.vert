#version 460 core

#include "../constant.glsl"
#include "../layout_uniforms_camera.glsl"
#include "../layout_uniforms_color.glsl"
#include "../layout_uniforms_model.glsl"
#include "struct_cylinder.glsl"

// In.
#include "../layout/atom.glsl"

// Out.
flat out StructCylinder vsCylinder;

void main()
{
	vsCylinder.color			= uniformsColor[ inAtomColor ];
	vsCylinder.isSelected		= int( inAtomFlag ) & ( 1 << FLAG_SELECTION );
	vsCylinder.id				= inAtomId;
	vsCylinder.representation	= inAtomRepresentation;

	// Vertex position in view space.
	gl_Position = uniformsModel[ gl_DrawID ].matrixModelView * vec4( inAtomPosition, 1.f );
}
