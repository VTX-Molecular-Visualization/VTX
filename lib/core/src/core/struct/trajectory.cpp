#include "core/struct/trajectory.hpp"

namespace VTX::Core::Struct
{

	ByteNumber dynamicMemoryUsage( const Trajectory & p_trj ) noexcept
	{
		ByteNumber out = 0;

		// FIXME needs refacto to handle circular buffers
		if ( !p_trj.isOptimized() )
		{
			// heap size
			out += sizeof( FramesDataVector );
			for ( auto & it_frame : p_trj.getFramesPlain().getFramesVector() )
			{
				out += sizeof( Frame );
				out += it_frame.size() * sizeof( Vec3f );
			}
		}

		return out;
	}

} // namespace VTX::Core::Struct
