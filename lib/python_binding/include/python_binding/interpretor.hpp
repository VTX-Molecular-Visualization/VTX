#ifndef __VTX_PYTHON_BINDING_INTERPRETOR__
#define __VTX_PYTHON_BINDING_INTERPRETOR__

#include "python_binding/binder.hpp"
#include <memory>
#include <string>
#include <util/singleton.hpp>
#include <util/types.hpp>

namespace VTX::PythonBinding
{
	namespace Wrapper
	{
		class Module;
	}

	class Interpretor
	{
	  public:
		Interpretor();
		~Interpretor();

		void init();
		void addBinder( std::unique_ptr<Binder> p_binder );

		template<BinderConcept BinderType>
		void addBinder()
		{
			addBinder( std::make_unique<BinderType>() );
		}
		template<BinderConcept BinderType, typename... BinderArgs>
		void addBinder( BinderArgs... args )
		{
			addBinder( std::make_unique<BinderType>( args... ) );
		}
		void clearBinders();

		void			runCommand( const std::string & p_line ) const;
		void			runScript( const FilePath & p_path ) const;
		Wrapper::Module loadModule( const FilePath & p_path ) const;

		const PyTXModule & getModule() const;

		void print( const std::string & p_line ) const;

	  private:
		struct Impl;
		std::unique_ptr<Impl> _impl;
	};

} // namespace VTX::PythonBinding

namespace VTX
{
	inline PythonBinding::Interpretor & INTERPRETOR() { return Util::Singleton<PythonBinding::Interpretor>::get(); }

} // namespace VTX

#endif
