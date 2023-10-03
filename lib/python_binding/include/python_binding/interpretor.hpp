#ifndef __VTX_PYTHON_BINDING_INTERPRETOR__
#define __VTX_PYTHON_BINDING_INTERPRETOR__

#include <memory>
#include <string>

namespace VTX::PythonBinding
{
	class Interpretor
	{
	  public:
		Interpretor();
		~Interpretor();

		void print( const std::string & p_line );
		void runCommand( const std::string & p_line );

	  private:
		struct Impl;
		std::unique_ptr<Impl> _impl;
	};
} // namespace VTX::PythonBinding

#endif
