#version 410

layout(points) in;
layout(line_strip, max_vertices = 32) out;

flat in vec3 vVoxelSize[];

layout(std140) uniform VoxelSettings
{
	mat4 uMVPMatrix;
	vec4 uColor;
};

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
    
    vec4 dx = uMVPMatrix[0] * vVoxelSize[0].x;
    vec4 dy = uMVPMatrix[1] * vVoxelSize[0].y;
    vec4 dz = uMVPMatrix[2] * vVoxelSize[0].z;

    AddQuad(center + dx, dy, dz);
    AddQuad(center - dx, dy, dz);
    
    AddQuad(center + dy, dz, dx);
    AddQuad(center - dy, dx, dz);
}
