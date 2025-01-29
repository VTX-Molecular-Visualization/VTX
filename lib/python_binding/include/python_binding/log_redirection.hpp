#ifndef __VTX_PYTHON_LOG_REDIRECTION__
#define __VTX_PYTHON_LOG_REDIRECTION__

#include <string>

namespace VTX::PythonBinding
{
	/**
	 * @brief Class responsible for funneling python print through VTX console.
	 */
	class LogRedirection
	{
	  public:
		static void write( std::string str );
		static void flush();
	};

} // namespace VTX::PythonBinding

#endif
