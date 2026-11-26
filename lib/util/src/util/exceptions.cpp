#include "util/exceptions.hpp"
#include <util/logger.hpp>

namespace VTX::Util
{
	void ExceptionCollector::collect( const std::string_view p_context ) noexcept
	{
		if ( _error )
		{
			Logger::logException( p_context );
		}
		else
		{
			_error = std::current_exception();
		}
	}
} // namespace VTX::Util
