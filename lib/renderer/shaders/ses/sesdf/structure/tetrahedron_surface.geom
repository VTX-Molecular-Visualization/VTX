#version 450

layout(points) in;
layout(triangle_strip, max_vertices = 32) out;

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

void main() {
    color         = vColor[0];
    enableShading = 1;
    position      = vec3(1000.);
    normal        = vec3(0.);
    
    gl_Position = uMVPMatrix * vec4(vPoint1[0], 1.);
    position = gl_Position.xyz;
    EmitVertex();

    gl_Position = uMVPMatrix * vec4(vPoint2[0], 1.);
    position = gl_Position.xyz;
    EmitVertex();
    
    gl_Position = uMVPMatrix * vec4(vPoint3[0], 1.);
    position = gl_Position.xyz;
    EmitVertex();
    
    gl_Position = uMVPMatrix * vec4(vPoint4[0], 1.);
    position = gl_Position.xyz;
    EmitVertex();
    
    gl_Position = uMVPMatrix * vec4(vPoint1[0], 1.);
    position = gl_Position.xyz;
    EmitVertex();
    
    gl_Position = uMVPMatrix * vec4(vPoint2[0], 1.);
    position = gl_Position.xyz;
    EmitVertex();
    
    gl_Position = uMVPMatrix * vec4(vPoint4[0], 1.);
    position = gl_Position.xyz;
    EmitVertex();
    
    gl_Position = uMVPMatrix * vec4(vPoint3[0], 1.);
    position = gl_Position.xyz;
    EmitVertex();
    
    gl_Position = uMVPMatrix * vec4(vPoint2[0], 1.);
    position = gl_Position.xyz;
    EmitVertex();

    EndPrimitive();
}
