#ifndef __VTX_RENDERER_STRUCT_PROXY_MOLECULE__
#define __VTX_RENDERER_STRUCT_PROXY_MOLECULE__

#include <util/color/rgba.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX::Renderer
{
	struct StructProxyMolecule
	{
		const Mat4f * transform;
		// const Mat4f * normals;
		std::vector<Vec3f> * atomPositions;
		std::vector<uchar> * atomColors;

		// std::vector<size_t> * atomIdColors;
		// std::vector<size_t> * atomIdSymbol;

		// std::vector<float> * atomSymbolById;
		// std::vector<float> * atomRadiusById; // Radius layout. Maybe be change for infos by symbol layout.
		// std::vector<Util::Color::Rgba> * colorById; // Color layout.

		std::vector<float> * atomRadii;		   // To delete.
		std::vector<bool> *	 atomVisibilities; // ?
		std::vector<bool> *	 atomSelections;   // To redo.
		std::vector<uint> *	 atomIds;		   // Keep?
		std::vector<uint> *	 bonds;			   // Ok.
	};

} // namespace VTX::Renderer

#endif
