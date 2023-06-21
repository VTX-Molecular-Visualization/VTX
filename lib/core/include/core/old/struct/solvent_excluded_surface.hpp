#ifndef __VTX_CORE_STRUCT_SOLVENT_EXCLUDED_SURFACE__
#define __VTX_CORE_STRUCT_SOLVENT_EXCLUDED_SURFACE__

#include "_fwd.hpp"
#include <util/math/range.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX::Core::Old::Struct
{
	class SolventExcludedSurface
	{
	  public:
		using Range = Util::Math::Range<uint>;

		SolventExcludedSurface();
		~SolventExcludedSurface() = default;

		inline const std::vector<Range> & getAtomsToTriangles() const { return _atomsToTriangles; }
		void							  appendRange( const Range & p_range );

	  private:
		std::vector<Range> _atomsToTriangles = std::vector<Range>();
		uint			   _indiceCount		 = 0;
	};
} // namespace VTX::Core::Old::Struct

#endif
