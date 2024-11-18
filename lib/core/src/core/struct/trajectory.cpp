#include "core/struct/trajectory.hpp"

namespace VTX::Core::Struct
{

	ByteNumber dynamicMemoryUsage( const Trajectory & p_trj ) noexcept
	{
		ByteNumber out = 0;

		// heap size
		for ( auto & it_frame : p_trj.frames )
		{
			out += sizeof( Frame );
			out += it_frame.size() * sizeof( Vec3f );
		}
		return out;
	}

} // namespace VTX::Core::Struct
