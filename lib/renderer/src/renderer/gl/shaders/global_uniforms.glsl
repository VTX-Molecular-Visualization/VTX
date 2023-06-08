layout ( std140, binding = 15 ) uniform Uniforms
{
	mat4 matrixModel;
	mat4 matrixNormal;
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

mat4 getMatrixModel()
{
	return uniforms.matrixModel;
}

mat4 getMatrixNormal()
{
	return uniforms.matrixNormal;
}

mat4 getMatrixView()
{
	return uniforms.matrixView;
}

mat4 getMatrixProjection()
{
	return uniforms.matrixProjection;
}

float getCameraNear()
{
	return uniforms.cameraNearFar.x;
}

float getCameraFar()
{
	return uniforms.cameraNearFar.y;
}

vec4 getCameraClipInfos()
{
	return uniforms.cameraClipInfos;
}

vec4 getBackgroundColor()
{
	return uniforms.backgroundColor;
}

vec4 getLightColor()
{
	return uniforms.lightColor;
}

float getFogNear()
{
	return uniforms.fog.x;
}

float getFogFar()
{
	return uniforms.fog.y;
}

float getFogDensity()
{
	return uniforms.fog.z;
}

vec4 getFogColor()
{
	return uniforms.fogColor;
}

vec4 getOutlineColor()
{
	return uniforms.outlineColor;
}

vec4 getSelectionColor()
{
	return uniforms.selectionColor;
}

bool isCameraPerspective()
{
	return uniforms.boolData.x == 1;
}

float getOutlineThickness()
{
	return float(uniforms.intData.x);
}

uint getSSAOIntensity()
{
	return uniforms.uintData.x;
}

uint getBlurSize()
{
	return uniforms.uintData.y;
}

uint getShadingMode()
{
	return uniforms.uintData.z;
}

float getSpecularFactor()
{
	return uniforms.floatData.x;
}

float getOutlineSensivity()
{
	return uniforms.floatData.y;
}