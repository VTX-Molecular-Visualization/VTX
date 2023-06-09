#ifndef __VTX_RENDERER_GL_STRUCT_PROXY_MOLECULE__
#define __VTX_RENDERER_GL_STRUCT_PROXY_MOLECULE__

#include <util/color/rgba.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX::Renderer::GL
{
	struct StructProxyMolecule
	{
		Mat4f *							 transform;
		std::vector<Vec3f> *			 atomPositions;
		std::vector<Util::Color::Rgba> * atomColors;
		std::vector<float> *			 atomRadii;
		std::vector<size_t> *			 atomVisibilities;
		std::vector<size_t> *			 atomSelections;
		std::vector<size_t> *			 atomIds;
		std::vector<size_t> *			 bonds;
	};

} // namespace VTX::Renderer::GL

#endif
