#version 460 core

#include "../layout_uniforms_camera.glsl"
#include "struct_vertex_shader.glsl"
#include "struct_geometry_shader.glsl"

layout( points ) in;
layout( line_strip, max_vertices = 32 ) out;

// In.
flat in StructVertexShader vsData[];

// Out.
flat out StructGeometryShader gsData;

void addQuad( vec4 p_center, vec4 p_dy, vec4 p_dx ) {

    gl_Position = p_center + ( p_dx + p_dy );
    EmitVertex();
    gl_Position = p_center + ( p_dx - p_dy );
    EmitVertex();
    EndPrimitive();

    gl_Position = p_center + ( p_dx + p_dy );
    EmitVertex();
    gl_Position = p_center + ( -p_dx + p_dy );
    EmitVertex();
    EndPrimitive();

    gl_Position = p_center + ( -p_dx - p_dy );
    EmitVertex();
    gl_Position = p_center + ( -p_dx + p_dy );
    EmitVertex();
    EndPrimitive();

    gl_Position = p_center + ( -p_dx - p_dy );
    EmitVertex();
    gl_Position = p_center + ( p_dx - p_dy );
    EmitVertex();
    EndPrimitive();

}

void main() {
    vec4 center = gl_in[0].gl_Position;
    gsData.center = center.xyz;
    
    mat4 matrix = uniformsCamera.matrixProjection * uniformsCamera.matrixView;
    vec4 dx = matrix[ 0 ] * vsData[ 0 ].voxelSize.x / 2.f;
    vec4 dy = matrix[ 1 ] * vsData[ 0 ].voxelSize.y / 2.f;
    vec4 dz = matrix[ 2 ] * vsData[ 0 ].voxelSize.z / 2.f;

    addQuad( center + dx, dy, dz );
    addQuad( center - dx, dy, dz );
    
    addQuad( center + dy, dz, dx );
    addQuad( center - dy, dx, dz );
}
