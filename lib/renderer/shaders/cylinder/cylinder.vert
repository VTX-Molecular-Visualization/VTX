#version 450 core

#include "../constant.glsl"
#include "../layout_uniforms_camera.glsl"
#include "../layout_uniforms_color.glsl"
#include "../layout_uniforms_model.glsl"

// In.
layout( location = 0 ) in vec3  inVertexPosition;
layout( location = 1 ) in uint  inVertexColor;
layout( location = 2 ) in float inVertexRad;
layout( location = 3 ) in uint  inVertexId;
layout( location = 4 ) in uint  inVertexFlag;
layout( location = 5 ) in uint  inVertexModel;
layout( location = 6 ) in uint  inVertexRepresentation;

// Out.
out
#include "struct_vertex_shader.glsl"
outData;

void main()
{
	outData.vertexColor				= uniformsColor[ inVertexColor ];
	//outData.vertexColor					 = vec4( 1.f, 1.f, 1.f, 1.f );
	outData.vertexVisible			= int( inVertexFlag ) & ( 1 << FLAG_VISIBILITY );
	outData.vertexSelected			= int( inVertexFlag ) & ( 1 << FLAG_SELECTION );
	outData.vertexId				= inVertexId;
	outData.vertexIdRepresentation	= inVertexRepresentation;

	// Vertex position in view space.
	gl_Position = uniformsModel[ inVertexModel ].matrixModelView * vec4( inVertexPosition, 1.f );
}
