#version 450

layout(binding = 0) uniform sampler2D aoFactor;

layout(location = 0) out float aoBlurredFactor;

uniform int uBlurSize;

void main()
{
	const vec2 texelSize = 1.f / textureSize(aoFactor, 0);
	const vec2 texPos = gl_FragCoord.xy * texelSize;
	
	const vec2 lim = vec2(fma(-uBlurSize, 0.5f, 0.5f));

	float res = 0.f;

	for (int i = 0; i < uBlurSize; ++i)
	{
		for (int j = 0; j < uBlurSize; ++j)
		{
			vec2 offset = (lim + vec2(i, j)) * texelSize;
			res += texture(aoFactor, texPos + offset).x;
		}
	}
	aoBlurredFactor = res / float(uBlurSize * uBlurSize);
}
