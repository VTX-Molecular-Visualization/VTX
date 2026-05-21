#ifndef __VTX_RENDERER_TYPES__
#define __VTX_RENDERER_TYPES__

#include <core/chemdb/atom.hpp>
#include <map>
#include <unordered_map>
#include <util/enum.hpp>
#include <util/math/range_list.hpp>

namespace VTX::Renderer
{
	using Symbol				  = Core::ChemDB::Atom::SYMBOL;
	using ColorIndex			  = uint8_t;
	using RepresentationIndex	  = uint8_t;
	using Flag					  = uint8_t;
	using Indice				  = uint32_t;
	using BufferChunk			  = uint32_t;
	using IndexRange			  = Util::Math::Range<Index>;
	using IndexRangeList		  = Util::Math::RangeList<Index>;
	using MapRepresentationRanges = std::unordered_map<RepresentationIndex, IndexRangeList>;

	enum struct E_ELEMENT_FLAGS : uint8_t
	{
		VTX_ENUM_ENABLE_BITMASK,
		VISIBILITY = 0,
		SELECTION  = 1
	};

} // namespace VTX::Renderer

#endif
