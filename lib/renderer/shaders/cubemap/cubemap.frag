#version 450 core

// In.
in vec3 texCoord;
layout( binding = 0 ) uniform samplerCube inTextureCubemap;

// Out.
out vec4 FragColor;

void main()
{
    FragColor = texture( inTextureCubemap, texCoord );
}