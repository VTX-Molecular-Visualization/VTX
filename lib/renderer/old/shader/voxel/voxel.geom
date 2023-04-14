#version 450

layout( points ) in;
layout(line_strip, max_vertices = 32) out;

uniform mat4 u_MVMatrix;
uniform mat4 u_projMatrix;

in VsOut
{
	flat vec3 voxelSize;
    flat vec3 center;
}
vsIn[];

out GsOut
{	
	flat vec3 center;
}
gsOut;

void AddQuad(vec4 center, vec4 dy, vec4 dx) {

    gl_Position = center + ( dx + dy);
    EmitVertex();
    gl_Position = center + ( dx - dy);
    EmitVertex();
    EndPrimitive();

    gl_Position = center + ( dx + dy);
    EmitVertex();
    gl_Position = center + (-dx + dy);
    EmitVertex();
    EndPrimitive();

    gl_Position = center + (-dx - dy);
    EmitVertex();
    gl_Position = center + (-dx + dy);
    EmitVertex();
    EndPrimitive();

    gl_Position = center + (-dx - dy);
    EmitVertex();
    gl_Position = center + ( dx - dy);
    EmitVertex();
    EndPrimitive();

}

void main() {
    vec4 center = gl_in[0].gl_Position;
    gsOut.center= center.xyz;
    
    vec4 dx = (u_projMatrix * u_MVMatrix)[0] * vsIn[0].voxelSize.x;
    vec4 dy = (u_projMatrix * u_MVMatrix)[1] * vsIn[0].voxelSize.y;
    vec4 dz = (u_projMatrix * u_MVMatrix)[2] * vsIn[0].voxelSize.z;

    AddQuad(center + dx, dy, dz);
    AddQuad(center - dx, dy, dz);
    
    AddQuad(center + dy, dz, dx);
    AddQuad(center - dy, dx, dz);
}
