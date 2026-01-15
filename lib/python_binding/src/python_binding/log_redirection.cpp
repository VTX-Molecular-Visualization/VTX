#include "python_binding/log_redirection.hpp"
#include <util/logger.hpp>

namespace VTX::PythonBinding
{
	void LogRedirection::write( std::string str ) { VTX_INFO( "{}", str ); }
	void LogRedirection::flush()
	{
		// LOGGER::flush(); // flushing seems to cause crash for some reason
	}

} // namespace VTX::PythonBinding
