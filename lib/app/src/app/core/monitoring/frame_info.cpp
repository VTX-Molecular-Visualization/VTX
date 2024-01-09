#include "app/core/monitoring/frame_info.hpp"
#include <util/chrono.hpp>

namespace VTX::App::Core::Monitoring
{
	FrameInfo::FrameInfo() : _timestamp( Util::Chrono::getTimestamp() ) {}

	bool FrameInfo::isValid() const { return _metricsMap.size() > 0; }

} // namespace VTX::App::Core::Monitoring
