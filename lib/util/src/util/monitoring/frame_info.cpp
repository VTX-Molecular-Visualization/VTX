#include "util/monitoring/frame_info.hpp"
#include <util/chrono.hpp>

namespace VTX::Util::Monitoring
{
	FrameInfo::FrameInfo() : _timestamp( Util::Chrono::getTimestamp() ) {}

	bool FrameInfo::isValid() const { return _metricsMap.size() > 0; }

} // namespace VTX::Util::Monitoring
