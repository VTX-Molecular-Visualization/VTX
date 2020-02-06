#version 450

layout(binding = 0) uniform sampler2D aoFactor;

layout(location = 0) out float aoBlurredFactor;

uniform int uBlurSize;

void main()
{
	const vec2 texelSize = 1.f / textureSize(aoFactor, 0);
	const vec2 texPos = gl_FragCoord.xy * texelSize;
	

	float res = 0.f;
	
	/*const vec2 lim = vec2(fma(-uBlurSize, 0.5f, 0.5f));

	for (int i = 0; i < uBlurSize; ++i)
	{
		for (int j = 0; j < uBlurSize; ++j)
		{
			vec2 offset = (lim + vec2(i, j)) * texelSize;
			res += texture(aoFactor, texPos + offset).x;
		}
	}
	aoBlurredFactor = res / float(uBlurSize * uBlurSize);
	*/

	float lim  = fma(-uBlurSize, 0.5f, 0.5f);
	for (int i = 0; i < uBlurSize; ++i)
	{
		res += texture(aoFactor, texPos + vec2((lim + i) * texelSize.x, 0.f)).x;
	}
	res /= float(uBlurSize);
	for (int i = 0; i < uBlurSize; ++i)
	{
		res += texture(aoFactor, texPos + vec2(0.f), (lim + i) * texelSize.y).x;
	}
	res /= float(uBlurSize);
	aoBlurredFactor = res; 
	
}
