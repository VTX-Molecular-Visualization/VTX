#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_FORM__
#define __VTX_TOOL_TOOLS_MDPREP_UI_FORM__

#include <memory>
#include <util/concepts.hpp>

namespace VTX::Tool::Mdprep::Gateway
{
	struct MdParameters;
	struct EngineSpecificCommonInformation;
} // namespace VTX::Tool::Mdprep::Gateway
namespace VTX::Tool::Mdprep::ui
{
	struct FormLayouts;

	// Generic class for form behaviors
	class Form
	{
	  public:
		template<typename T>
			requires( not VTX::SameUnalteredType<Form, T> )
		Form( T && t ) : _ptr( new _wrapper<T>( std::forward<T>( t ) ) )
		{
			static_assert(
				requires( T t, const Gateway::EngineSpecificCommonInformation & e ) {
					{ t.update( e ) };
				},
				"You need to implement 'void update( const Gateway::EngineSpecificCommonInformation & p_data ) "
				"noexcept' class method"
			);
			static_assert(
				requires( T t, Gateway::MdParameters & e ) {
					{ t.get( e ) };
				}, "You need to implement 'void get( Gateway::MdParameters & p_ ) const noexcept' class method"
			);
			static_assert(
				requires( T t, FormLayouts & e ) {
					{ t.get( e ) };
				}, "You need to implement 'void get( FormLayouts & p_ ) const noexcept' class method"
			);
		}
		Form() = default;

		// Inform the form on new engine-dependant data.
		void update( const Gateway::EngineSpecificCommonInformation & p_data ) noexcept
		{
			if ( _ptr )
				_ptr->update( p_data );
		}

		// Get MD parameters entered by the user
		void get( Gateway::MdParameters & p_ ) const noexcept
		{
			if ( _ptr )
				_ptr->get( p_ );
		}
		// Retrieve layouts for field placing
		void get( FormLayouts & p_ ) const noexcept
		{
			if ( _ptr )
				_ptr->get( p_ );
		}
		// Restore default empty form.
		void close() noexcept
		{
			if ( _ptr )
				_ptr.reset();
		}

	  private:
		class _interface
		{
		  public:
			virtual ~_interface()																	= default;
			virtual void update( const Gateway::EngineSpecificCommonInformation & p_data ) noexcept = 0;
			virtual void get( Gateway::MdParameters & p_ ) const noexcept							= 0;
			virtual void get( FormLayouts & p_ ) const noexcept										= 0;
		};

		template<typename T>
		class _wrapper final : public _interface
		{
			T _obj;

		  public:
			_wrapper( T && t ) : _obj( std::forward<T>( t ) ) {}
			virtual void update( const Gateway::EngineSpecificCommonInformation & p_data ) noexcept override
			{
				_obj.update( p_data );
			}
			virtual void get( Gateway::MdParameters & p_ ) const noexcept override { _obj.get( p_ ); }
			virtual void get( FormLayouts & p_ ) const noexcept override { _obj.get( p_ ); }
		};

		std::unique_ptr<_interface> _ptr = nullptr;
	};
} // namespace VTX::Tool::Mdprep::ui

#endif
