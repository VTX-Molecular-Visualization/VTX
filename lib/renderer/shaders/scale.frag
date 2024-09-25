#version 450 core

// In.
layout( binding = 0 ) uniform sampler2D inTextureColor;

const float scale = 2.f;
// Out.
layout( location = 0 ) out float outScaled;

void main(){
    ivec2 texCoord = ivec2(gl_FragCoord.xy);

    int x = int(texCoord.x/scale);
    int y = int(texCoord.y/scale);

    ivec2 newPoint= ivec2(x,y);

    outScaled = texelFetch(inTextureColor,newPoint.xy,0 ).x;
}