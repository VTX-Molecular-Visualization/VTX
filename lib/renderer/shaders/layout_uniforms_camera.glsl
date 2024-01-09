layout ( std140, binding = 15 ) uniform UniformsCamera
{
	vec4 colorLayout[256];
	mat4 matrixModel;
	mat4 matrixNormal;
	mat4 matrixView;	
	mat4 matrixProjection;
	vec4 cameraClipInfos; // _near * _far, _far, _far - _near, _near
	ivec2 mousePosition;
	bool isCameraPerspective;	
} uniformsCamera;