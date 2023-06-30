layout ( std140, binding = 15 ) uniform Uniforms
{
	mat4 matrixModel;
	mat4 matrixNormal;
	mat4 matrixView;	
	mat4 matrixProjection;

	vec4 cameraClipInfos; // _near * _far, _far, _far - _near, _near

	vec4 colorBackground;
	vec4 colorLight;	
	vec4 colorFog;
	vec4 colorOutline;
	vec4 colorSelection;

	float specularFactor;
	float fogNear;
	float fogFar;
	float fogDensity;

	float ssaoIntensity;
	float blurSize;
	float outlineSensivity;
	float outlineThickness;

	int shadingMode;
	bool isCameraPerspective;
	uint pixelSize;
	bool pixelizeBackground;	
	
} uniforms;