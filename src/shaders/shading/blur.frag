#version 450

layout(binding = 0) uniform sampler2D aoFactor;

layout(location = 0) out float aoBlurredFactor;

const int blurSize = 9;

void main()
{
	const vec2 texelSize = 1.f / textureSize(aoFactor, 0);
	const vec2 texPos = gl_FragCoord.xy * texelSize;

	float ao = texture(aoFactor, texPos).x;

	const vec2 lim = vec2(float(-blurSize) * 0.5f + 0.5f);

	float res = 0.f;

	for (int i = 0; i < blurSize; ++i)
	{
		for (int j = 0; j < blurSize; ++j)
		{
			vec2 offset = (lim + vec2(i, j)) * texelSize;
			float aoNeighbour = texture(aoFactor, texPos + offset).x;
			res += aoNeighbour;
		}
	}
	aoBlurredFactor = res / float(blurSize * blurSize);
}
