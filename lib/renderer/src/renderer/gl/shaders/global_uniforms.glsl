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
	// ivec2 blurDirection;

	vec4 backgroundColor;

	bool  fog;
	float fogNear;
	float fogFar;
	float fogDensity;
	vec4  fogColor;

	vec4 lightColor;

	bool  outline;
	vec4  outlineColor;
	int	  outlineThickness;
	float outlineSensivity ;

	vec4 selectionColor;

	bool aa;
} uniforms;