#ifndef VTX_TOOL_MDPREP_UI_FORM_HPP
#define VTX_TOOL_MDPREP_UI_FORM_HPP

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
				"' class method" );
			static_assert(
				requires( T t, Gateway::MdParameters & e ) {
					{ t.get( e ) };
				}, "You need to implement 'void get( Gateway::MdParameters & p_ ) const' class method" );
			static_assert(
				requires( T t, FormLayouts & e ) {
					{ t.get( e ) };
				}, "You need to implement 'void get( FormLayouts & p_ ) const' class method" );
		}

		Form() = default;

		// Inform the form on new engine-dependant data.
		void update( const Gateway::EngineSpecificCommonInformation & p_data )
		{
			if ( _ptr )
			{
				_ptr->update( p_data );
			}
		}

		// Get MD parameters entered by the user
		void get( Gateway::MdParameters & p_ ) const
		{
			if ( _ptr )
			{
				_ptr->get( p_ );
			}
		}

		// Retrieve layouts for field placing
		void get( FormLayouts & p_ ) const
		{
			if ( _ptr )
			{
				_ptr->get( p_ );
			}
		}

		// Restore default empty form.
		void close() noexcept
		{
			if ( _ptr )
			{
				_ptr.reset();
			}
		}

	  private:
		class _interface
		{
		  public:
			virtual ~_interface()														   = default;
			virtual void update( const Gateway::EngineSpecificCommonInformation & p_data ) = 0;
			virtual void get( Gateway::MdParameters & p_ ) const						   = 0;
			virtual void get( FormLayouts & p_ ) const									   = 0;
		};

		template<typename T>
		class _wrapper final : public _interface
		{
			T _obj;

		  public:
			_wrapper( T && t ) : _obj( std::forward<T>( t ) ) {}

			void update( const Gateway::EngineSpecificCommonInformation & p_data ) override { _obj.update( p_data ); }

			void get( Gateway::MdParameters & p_ ) const override { _obj.get( p_ ); }

			void get( FormLayouts & p_ ) const override { _obj.get( p_ ); }
		};

		std::unique_ptr<_interface> _ptr = nullptr;
	};
} // namespace VTX::Tool::Mdprep::ui

#endif
