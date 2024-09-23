struct Camera
{
    mat4 matrixView;    
    mat4 matrixProjection;
    vec3 cameraPosition;
    vec4 cameraClipInfos; // _near * _far, _far, _far - _near, _near
    ivec2 resolution;
    ivec2 mousePosition;    
    uint isCameraPerspective;
};

layout( std140, binding = 15 ) uniform UniformsCamera
{
    Camera uniformsCamera;
};