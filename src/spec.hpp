#ifndef __VTX_SPEC__
#define __VTX_SPEC__

namespace VTX
{
	struct Spec
	{
		std::string glVersion;
		std::string glslVersion;
		std::string glVendor;
		std::string glRenderer;
		int			glMaxTextureSize;
		int			glMaxPatchVertices;
		int			glMaxTessGenLevel;
	};
} // namespace VTX
#endif
