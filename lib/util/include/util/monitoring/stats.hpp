#ifndef __VTX_UTIL_MONITORING_STAT__
#define __VTX_UTIL_MONITORING_STAT__

#include "frame_info.hpp"
#include <list>
#include <util/callback.hpp>
#include <util/chrono.hpp>
#include <util/hashing.hpp>
#include <util/types.hpp>

namespace VTX::Util::Monitoring
{
	class Stats
	{
	  public:
		FrameInfo & newFrame();

		inline FrameInfo & getCurrentFrame() { return _frames.back(); }

		template<typename T>
		T getAverage( const Hash & p_hashedKey ) const
		{
			if ( _frames.size() == 0 )
			{
				return 0;
			}

			T res = 0;
			for ( const FrameInfo & frameInfo : _frames )
			{
				if ( frameInfo.has( p_hashedKey ) )
				{
					res += frameInfo.get<T>( p_hashedKey );
				}
			}

			return res / T( _frames.size() );
		}

		float getAverage() const
		{
			if ( _frames.size() < 2 )
			{
				return 0;
			}

			float res = Util::Chrono::elapsedTime( _frames.front().getTimepoint(), _frames.back().getTimepoint() );

			return res / float( _frames.size() - 1 );
		}

	  private:
		inline static const uint ACTIVE_FRAME_COUNT = 1000;

		std::list<FrameInfo> _frames;
	};
} // namespace VTX::Util::Monitoring
#endif
