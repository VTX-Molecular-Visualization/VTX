struct StructGeometryShaderSmooth
{
	// Impostor position in view space.
    vec3 viewImpPos;  
};

struct StructGeometryShaderFlat
{
    vec4 color;
    vec3 obbRadius;
    vec3 obbPos;
    vec4 orientation;
};