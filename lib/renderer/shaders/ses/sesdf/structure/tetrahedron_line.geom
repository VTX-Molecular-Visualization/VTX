#version 450

layout(points) in;
layout(line_strip, max_vertices = 32) out;

flat in vec3 vPoint1[];
flat in vec3 vPoint2[];
flat in vec3 vPoint3[];
flat in vec3 vPoint4[];
flat in vec3 vColor[];

layout(std140, binding=0) uniform SesdfSettings
{
	mat4 uMVPMatrix;
};

out vec3 position;
out vec3 normal;
flat out vec3 color;
flat out int  enableShading;

void AddLine(vec3 p1, vec3 p2) {
    gl_Position = uMVPMatrix * vec4(p1, 1.);
    EmitVertex();

    gl_Position = uMVPMatrix * vec4(p2, 1.);
    EmitVertex();

    EndPrimitive();
}

void main() {
    color         = vColor[0];
    enableShading = 1;
    position      = vec3(1000.);
    normal        = vec3(0.);

    // Top to bottom points
    AddLine(vPoint1[0], vPoint2[0]);
    AddLine(vPoint1[0], vPoint3[0]);
    AddLine(vPoint1[0], vPoint4[0]);

    // Bottom points between each others
    AddLine(vPoint2[0], vPoint3[0]);
    AddLine(vPoint2[0], vPoint4[0]);
    AddLine(vPoint3[0], vPoint4[0]);
}
