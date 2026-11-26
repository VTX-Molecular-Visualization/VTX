#ifndef VTX_PYTHON_BINDING_LOG_REDIRECTION_HPP
#define VTX_PYTHON_BINDING_LOG_REDIRECTION_HPP

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
