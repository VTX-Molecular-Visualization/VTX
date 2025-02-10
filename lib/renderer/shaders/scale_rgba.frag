#version 450 core

// In.
layout( binding = 0 ) uniform sampler2D inTextureColor;

const float scale = 2.f;
// Out.
layout( location = 0 ) out vec4 outScaled;

void main(){
    ivec2 texCoord = ivec2(gl_FragCoord.xy);

    int x = int(texCoord.x/scale);
    int y = int(texCoord.y/scale);

    ivec2 newPoint= ivec2(x,y);
    //get four neighbors colors
    //sum the neighboring colors
    vec4 colorSum = vec4(0.0f);
    
    colorSum += texelFetch(inTextureColor,newPoint.xy+ivec2(1,0),0 );
    colorSum += texelFetch(inTextureColor,newPoint.xy+ivec2(0, 1),0 );
    colorSum += texelFetch(inTextureColor,newPoint.xy+ivec2(-1,0),0 );
    colorSum += texelFetch(inTextureColor,newPoint.xy+ivec2(-1, -1),0 );
    colorSum+= 2.f*texelFetch(inTextureColor,newPoint.xy,0 );


    outScaled = colorSum/6.f;
}