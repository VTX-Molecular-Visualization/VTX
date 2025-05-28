#ifndef __VTX_PYTHON_BINDING_BINDER__
#define __VTX_PYTHON_BINDING_BINDER__

#include "python_binding/vtx_python_module.hpp"
#include <string>

namespace VTX::PythonBinding
{
	class PyTXModule;
	class Binder;

	template<typename BinderType>
	concept BinderConcept = requires( BinderType & o, PyTXModule & p ) {
		{ o.bind( p ) };
	};

	/**
	 * @brief Generic class responsible for associating actions with python commands
	 */
	class Binder
	{
	  public:
		Binder() = default;

		/**
		 * @brief Method to implement to bind actions to commands.
		 * @param p_pytxModule Module provided by the interpretor to bind actions into.
		 */
		inline void bind( PyTXModule & p_ )
		{
			if ( _ptr )
				_ptr->bind( p_ );
		}

		/**
		 * @brief [Optional] Submitted class can optionally re-implement this method to add module import instructions
		 * (e.g. when dealing with package dependancies )
		 */
		inline void importHeaders()
		{
			if ( _ptr )
				_ptr->importHeaders();
		}

	  private:
		struct _interface
		{
			virtual ~_interface()				 = default;
			virtual void bind( PyTXModule & p_ ) = 0;
			virtual void importHeaders()		 = 0;
		};
		template<typename T>
		class _wrapper final : public _interface
		{
			T _obj;

		  public:
			_wrapper( T && p_ ) : _obj( std::forward<T>( p_ ) ) {}
			virtual void bind( PyTXModule & p_ ) override { _obj.bind( p_ ); }
			virtual void importHeaders()
			{
				if constexpr ( requires( T & o ) {
								   { o.importHeaders() };
							   } )
					_obj.importHeaders();
			}
		};
		std::unique_ptr<_interface> _ptr = nullptr;

	  public:
		template<typename T>
			requires( not std::same_as<std::remove_cv<T>, Binder> )
		Binder( T && p_ ) : _ptr( new _wrapper<T>( std::forward<T>( p_ ) ) )
		{
		}
	};
	void importFullModule( const std::string & p_moduleName ) noexcept;
	void importObject( const std::string & p_moduleName, const std::string & p_objectName ) noexcept;
} // namespace VTX::PythonBinding

#endif
