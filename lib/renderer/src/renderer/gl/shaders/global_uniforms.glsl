layout ( std140, binding = 0 ) uniform Uniforms
{
	mat4 matrixView;
	mat4 matrixProjection;
	vec4 cameraNearFar; // near, far
	vec4 cameraClipInfos;
	vec4 backgroundColor;
	vec4 lightColor;
	vec4 fog; // fogNear, fogFar, fogDensity
	vec4 fogColor;
	vec4 outlineColor;
	vec4 selectionColor;
	// Compress data into vec4.
	ivec4 boolData; // isCameraPerspective
	ivec4 intData; // outlineThickness
	uvec4 uintData; // ssaoIntensity, blurSize, shadingMode
	vec4 floatData;  // specularFactor, outlineSensivity
} uniforms;

layout (std140, binding = 10) buffer Debug
{
	vec4[] debug;
};