#version 460 core

#include "../layout_indexed_indirect_draws.glsl"
#include "../layout_uniforms_camera.glsl"
#include "../layout_uniforms_model.glsl"
#include "struct_vertex_shader.glsl"

// In.
layout( location = 0 ) in vec3 inVertexPosition;
layout( location = 1 ) in vec3 inVertexNormal;

// Out.
smooth out StructVertexShaderSmooth vsDataSmooth;

void main()
{
	const uint idModel = indexedDraws[ gl_DrawID ].idModel;

	vsDataSmooth.viewPosition = vec3( uniformsModel[ idModel ].matrixModelView * vec4( inVertexPosition, 1.f ) );
	vsDataSmooth.normal = normalize( vec3( uniformsModel[ idModel ].matrixNormal * vec4( inVertexNormal, 0.f ) ) );

	gl_Position = uniformsCamera.matrixProjection * vec4( vsDataSmooth.viewPosition, 1.f );
}
