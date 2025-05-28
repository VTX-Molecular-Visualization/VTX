#ifndef __VTX_PYTHON_BINDING_ACTION__
#define __VTX_PYTHON_BINDING_ACTION__

#include <util/types.hpp>

namespace VTX::PythonBinding::Action
{
	class RunScript //: public App::Core::Action::BaseAction
	{
	  public:
		explicit RunScript( const FilePath & p_path ) : _path( p_path ) {}
		void execute();

	  private:
		FilePath _path;
	};

} // namespace VTX::PythonBinding::Action
#endif
