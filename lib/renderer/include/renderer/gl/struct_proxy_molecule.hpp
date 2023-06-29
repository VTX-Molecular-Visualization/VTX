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
		std::vector<uint> *				 atomVisibilities;
		std::vector<uint> *				 atomSelections;
		std::vector<uint> *				 atomIds;
		std::vector<uint> *				 bonds;
	};

} // namespace VTX::Renderer::GL

#endif
