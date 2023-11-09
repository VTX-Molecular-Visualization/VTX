#ifndef __VTX_PYTHON_BINDING_INTERPRETOR__
#define __VTX_PYTHON_BINDING_INTERPRETOR__

#include "python_binding/binder.hpp"
#include <memory>
#include <string>
#include <util/types.hpp>

namespace VTX::PythonBinding
{
	class Interpretor
	{
	  public:
		inline static const std::string SYSTEM_KEY = "PYTHON_INTERPRETOR";

	  public:
		Interpretor();
		~Interpretor();

		void init();
		void addBinder( std::unique_ptr<Binder> p_binder );

		template<typename TBinder>
		void addBinder()
		{
			addBinder( std::make_unique<TBinder>() );
		}

		void print( const std::string & p_line ) const;
		void runCommand( const std::string & p_line ) const;
		void runScript( const FilePath & p_path ) const;

	  private:
		struct Impl;
		std::unique_ptr<Impl> _impl;
	};
} // namespace VTX::PythonBinding

#endif
