layout ( std140, binding = 0 ) uniform Uniforms
{
	vec4  matrixView;
	vec4  matrixProjection;
	bool  isCameraPerspective;
	float cameraNear;
	float cameraFar;
	vec4  clipInfos;
	bool ssao;
	uint ssaoIntensity;
	uint blurSize;
	vec4 backgroundColor;
	vec4 lightColor;
	uint  shadingMode;	
	float specularFactor;
	bool  fog;
	float fogNear;
	float fogFar;
	float fogDensity;
	vec4  fogColor;
	bool  outline;
	vec4  outlineColor;
	int	  outlineThickness;
	float outlineSensivity;
	vec4 selectionColor;
	bool aa;
} uniforms;