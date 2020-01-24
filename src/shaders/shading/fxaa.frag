#version 450

layout(binding = 0) uniform sampler2D image;

out vec4 fragColor;

float rgb2lum(const vec3 rgb)
{
	return sqrt(dot(rgb, vec3(0.299f, 0.587f, 0.114f)));
}

const float FXAA_SPAN_MAX = 4.f;
const float FXAA_REDUCE_MIN = 1.f / 128.f;
const float FXAA_REDUCE_MUL = 1.f / 8.f;

// effet bourré haha

void main()
{
	const vec2 invTexSize = 1.f / textureSize(image, 0);
	const vec2 texPos = gl_FragCoord.xy * invTexSize;

	//	fragColor = texture(image, texPos);
	//	return;

	// get current pixel in rgb and compute luma
	const vec3 rgbC = texture(image, texPos).xyz;
	const float lC = rgb2lum(rgbC);

	// compute luma of 4 neighbours (up-left, up-right, down-left, down-right)
	const float lUL = rgb2lum(textureOffset(image, texPos, ivec2(1, -1)).xyz);
	const float lUR = rgb2lum(textureOffset(image, texPos, ivec2(1, 1)).xyz);
	const float lDL = rgb2lum(textureOffset(image, texPos, ivec2(-1, -1)).xyz);
	const float lDR = rgb2lum(textureOffset(image, texPos, ivec2(-1, 1)).xyz);

	float lMin = min(lC, min(min(lUL, lUR), min(lDL, lDR)));
	float lMax = max(lC, max(max(lUL, lUR), max(lDL, lDR)));

	vec2 dir = vec2(-(lUL + lUR) - (lDL + lDR), (lUL + lDL) - (lUR + lUL));

	const float dirReduce = max(
		(lUL + lUR + lDL + lDR) * (0.25f * FXAA_REDUCE_MUL), FXAA_REDUCE_MIN);
	const float rcpDirMin = 1.f / (min(abs(dir.x), abs(dir.y)) + dirReduce);

	dir = min(vec2(FXAA_SPAN_MAX, FXAA_SPAN_MAX),
			  max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX), dir * rcpDirMin))
		  * invTexSize;
	/*--------------------------------------------------------*/
	vec3 rgbA = 0.5f
				* (texture(image, texPos + dir * (1.f / 3.f - 0.f)).xyz
				   + texture(image, texPos + dir * (2.f / 3.f - 0.f)).xyz);
	vec3 rgbB = rgbA * 0.5f
				+ 0.25f
					  * (texture(image, texPos + dir * -0.5f).xyz
						 + texture(image, texPos + dir * 0.5f).xyz);
	float lB = rgb2lum(rgbB);
	if ((lB < lMin) || (lB > lMax))
		fragColor = vec4(rgbA, 1.f);
	else
		fragColor = vec4(rgbB, 1.f);
}

//// method:
//// http://blog.simonrodriguez.fr/articles/30-07-2016_implementing_fxaa.html
// const float EDGE_THRESHOLD_MIN = 0.0312f;
// const float EDGE_THRESHOLD_MAX = 0.125f;
// void main()
//{
//	const vec2 invTexSize = 1.f / textureSize(image, 0);
//	const vec2 texPos = gl_FragCoord.xy * invTexSize;
//
//	// get current pixel in rgb and compute luma
//	const vec3 rgbC = texture(image, texPos).xyz;
//	const float lC = rgb2lum(rgbC);
//
//	// ====== dectect edges
//	// compute luma of 4 neighbours (left, right, down, up)
//	const float lL = rgb2lum(textureOffset(image, texPos, ivec2(-1, 0)).xyz);
//	const float lR = rgb2lum(textureOffset(image, texPos, ivec2(1, 0)).xyz);
//	const float lU = rgb2lum(textureOffset(image, texPos, ivec2(0, 1)).xyz);
//	const float lD = rgb2lum(textureOffset(image, texPos, ivec2(0, -1)).xyz);
//
//	// get min max
//	const float lMin = min(lC, min(min(lL, lR), min(lD, lU)));
//	const float lMax = max(lC, max(max(lL, lR), max(lD, lU)));
//
//	const float lDelta = lMax - lMin;
//
//	if (lDelta < max(EDGE_THRESHOLD_MIN, lMax * EDGE_THRESHOLD_MAX))
//	{
//		fragColor = vec4(rgbC, 1.f);
//		return;
//	}
//
//	// ====== estimate gradient
//	// compute lum of the 4 remaining neighbours
//	const float lLU = rgb2lum(textureOffset(image, texPos, ivec2(-1, 1)).xyz);
//	const float lLD = rgb2lum(textureOffset(image, texPos, ivec2(-1, -1)).xyz);
//	const float lRU = rgb2lum(textureOffset(image, texPos, ivec2(1, 1)).xyz);
//	const float lRD = rgb2lum(textureOffset(image, texPos, ivec2(1, -1)).xyz);
//
//	// estimated gradients in horizontal and vertical
//	const float egHor = abs(-2.f * lL + lLU + lLD)
//						+ abs(-2.f * lC + lU + lD) * 2.f
//						+ abs(-2.f * lR + lRU + lRD);
//	const float egVer = abs(-2.f * lU + lRU + lLU)
//						+ abs(-2.f * lC + lL + lR) * 2.f
//						+ abs(-2.f * lD + lRD + lLD);
//
//	const bool isHor = egHor >= egVer; // is horizontal ?
//
//	// choose edge direction
//	const float lum1 = isHor ? lD : lL;
//	const float lum2 = isHor ? lU : lR;
//
//	const float g1 = abs(lum1 - lC);
//	const float g2 = abs(lum2 - lC);
//
//	const bool isG1Steepest = g1 >= g2; // is g1 steepest ?
//
//	// normalized gradient in steppest direction
//	const float gNorm = 0.25f * max(g1, g2);
//
//	// choose step distance  w.r.t texel size
//	float stepDist = isHor ? invTexSize.y : invTexSize.x;
//
//	// average luma in the chosen direction
//	float lAvg = 0.f;
//	if (isG1Steepest)
//	{
//		stepDist = -stepDist;
//		lAvg = 0.5f * (lum1 + lC);
//	}
//	else
//	{
//		lAvg = 0.5f * (lum2 + lC);
//	}
//}
