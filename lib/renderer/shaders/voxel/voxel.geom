#version 450 core

#include "../layout_uniforms_camera.glsl"

layout( points ) in;
layout( line_strip, max_vertices = 32 ) out;

in 
#include "struct_vertex_shader.glsl"
inData[];

out 
#include "struct_geometry_shader.glsl"
outData;

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
    outData.center = center.xyz;
    
    vec4 dx = ( uniformsCamera.matrixProjection *  uniformsCamera.matrixView *  uniformsCamera.matrixModel )[ 0 ] * inData[ 0 ].voxelSize.x;
    vec4 dy = ( uniformsCamera.matrixProjection *  uniformsCamera.matrixView *  uniformsCamera.matrixModel )[ 1 ] * inData[ 0 ].voxelSize.y;
    vec4 dz = ( uniformsCamera.matrixProjection *  uniformsCamera.matrixView *  uniformsCamera.matrixModel )[ 2 ] * inData[ 0 ].voxelSize.z;

    addQuad( center + dx, dy, dz );
    addQuad( center - dx, dy, dz );
    
    addQuad( center + dy, dz, dx );
    addQuad( center - dy, dx, dz );
}
