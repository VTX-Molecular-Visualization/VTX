#version 460 core

#include "../constant.glsl"
#include "../layout_uniforms_camera.glsl"
#include "../layout_uniforms_color.glsl"
#include "../layout_uniforms_model.glsl"
#include "struct_cylinder.glsl"

// In.
layout( location = 0 ) in vec3  inVertexPosition;
layout( location = 1 ) in uint  inVertexColor;
layout( location = 2 ) in float inVertexRad;
layout( location = 3 ) in uint  inVertexId;
layout( location = 4 ) in uint  inVertexFlag;
layout( location = 5 ) in uint  inVertexRepresentation;

// Out.
flat out StructCylinder vsCylinder;

void main()
{
	vsCylinder.color			= uniformsColor[ inVertexColor ];
	//vsCylinder.color			 = vec4( 1.f, 1.f, 1.f, 1.f );
	vsCylinder.isVisible		= int( inVertexFlag ) & ( 1 << FLAG_VISIBILITY );
	vsCylinder.isSelected		= int( inVertexFlag ) & ( 1 << FLAG_SELECTION );
	vsCylinder.id				= inVertexId;
	vsCylinder.representation	= inVertexRepresentation;

	// Vertex position in view space.
	gl_Position = uniformsModel[ gl_DrawID ].matrixModelView * vec4( inVertexPosition, 1.f );
}
