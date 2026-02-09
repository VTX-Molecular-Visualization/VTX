#ifndef __VTX_UTIL_MONITORING_STAT__
#define __VTX_UTIL_MONITORING_STAT__

#include "frame_info.hpp"
#include <list>
#include <util/callback.hpp>
#include <util/hashing.hpp>
#include <util/types.hpp>

namespace VTX::Util::Monitoring
{
	/**
	 * @brief Maximum number of stored frames.
	 */
	constexpr uint ACTIVE_FRAME_COUNT = 10;

	/**
	 * @brief Store frames to compute duration statistics.
	 */
	class Stats
	{
	  public:
		/**
		 * @brief Creates and returns a reference to a new FrameInfo object.
		 */
		FrameInfo & newFrame();

		/**
		 * @brief Returns a reference to the current (most recent) frame.
		 */
		inline FrameInfo & currentFrame() { return _frames.back(); }

		/**
		 * @brief Calculates the average elapsed time between recorded frames.
		 */
		float average() const;

		/**
		 * @brief Calculates the average value for the given hash.
		 */
		template<typename T>
		T average( const Hash & p_hashedKey ) const
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

	  private:
		/**
		 * @brief Frame storage.
		 */
		std::list<FrameInfo> _frames;
	};
} // namespace VTX::Util::Monitoring
#endif
