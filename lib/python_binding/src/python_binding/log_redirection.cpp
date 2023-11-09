#include "python_binding/log_redirection.hpp"
#include <util/logger.hpp>

namespace VTX::PythonBinding
{

	void LogRedirection::write( std::string str ) { VTX_INFO( "{}", str ); }
	void LogRedirection::flush() { Util::Logger::get().flush(); }

} // namespace VTX::PythonBinding