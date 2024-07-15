#ifndef __VTX_APP_CORE_MONITORING_STAT__
#define __VTX_APP_CORE_MONITORING_STAT__

#include "app/core/monitoring/frame_info.hpp"
#include <list>
#include <util/callback.hpp>
#include <util/chrono.hpp>
#include <util/hashing.hpp>
#include <util/types.hpp>

namespace VTX::App::Core::Monitoring
{
	struct Stats
	{
	  public:
		FrameInfo & newFrame();
		// bool		isEmpty() const { return _activeFrames.size() == 0; };

		inline FrameInfo & getCurrentFrame() { return _activeFrames.back(); }
		// const FrameInfo & getLastFrame() const;

		// const std::list<FrameInfo> & getActiveFrames() const { return _activeFrames; }
		// const std::list<FrameInfo> & getArchivedFrames() const { return _archivedFrames; };

		// void clearArchive();
		// void clear();

		template<typename T>
		T getAverage( const Util::Hashing::Hash & p_hashedKey ) const
		{
			if ( _activeFrames.size() == 0 )
			{
				return 0;
			}

			T res = 0;

			for ( const FrameInfo & frameInfo : _activeFrames )
			{
				if ( frameInfo.has( p_hashedKey ) )
				{
					res += frameInfo.get<T>( p_hashedKey );
				}
			}

			return res / T( _activeFrames.size() );
		}

	  private:
		// static const uint ARCHIVED_FRAME_COUNT = 1000;
		static const uint ACTIVE_FRAME_COUNT = 50;

		// const uint _activeFrameCount;
		// const uint _archivedFrameCount;

		std::list<FrameInfo> _activeFrames = std::list<FrameInfo>();
		// std::list<FrameInfo> _archivedFrames = std::list<FrameInfo>();

		Util::Chrono _chrono;

		void _pushFrame();
	};
} // namespace VTX::App::Core::Monitoring
#endif
