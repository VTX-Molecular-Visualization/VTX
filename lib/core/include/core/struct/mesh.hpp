#ifndef __VTX_CORE_STRUCT_MESH__
#define __VTX_CORE_STRUCT_MESH__

#include <util/types.hpp>
#include <vector>

namespace VTX::Core::Struct
{

	struct Mesh
	{
		std::vector<Vec4f> vertices;
		std::vector<Vec4f> normales;
		std::vector<uint>  indices;

		// TODO: move.
		void computeNormales();
		void toIndexed();
		void toNonIndexed();
	};

} // namespace VTX::Core::Struct

#endif
