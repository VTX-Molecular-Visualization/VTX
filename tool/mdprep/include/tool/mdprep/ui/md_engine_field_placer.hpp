#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_MD_ENGINE_FIELD_PLACER__
#define __VTX_TOOL_TOOLS_MDPREP_UI_MD_ENGINE_FIELD_PLACER__

#include <string>
//
#include "tool/mdprep/ui/shared.hpp"
#include <memory>
#include <type_traits>
#include <util/concepts.hpp>

namespace VTX::Tool::Mdprep::ui
{
	class MdEngine;

	// Class responsible for placing MD engine specific fields
	class MdEngineFieldPlacer
	{
	  public:
		// Inform or update the field placer where to put its fields
		void assign( FormLayouts p_layouts ) noexcept
		{
			if ( _ptr )
				_ptr->assign( std::move( p_layouts ) );
		}
		// Add the engine-specific field in the right place.
		void activate() noexcept
		{
			if ( _ptr )
				_ptr->activate();
		}
		// Remove the engine-specific field.
		void deactivate() noexcept
		{
			if ( _ptr )
				_ptr->deactivate();
		}

		MdEngineFieldPlacer() = default;
		template<typename T>
			requires( not SameUnalteredType<T, MdEngineFieldPlacer> )
		MdEngineFieldPlacer( T && p_in ) : _ptr( new wrapper<T>( std::forward<T>( p_in ) ) )
		{
			static_assert(
				requires( T t, FormLayouts f ) {
					{ t.assign( f ) };
				}, "You must implement 'void assign( FormLayouts p_layouts ) noexcept' class method."
			);
			static_assert(
				requires( T t ) {
					{ t.activate() };
				}, "You must implement 'void activate() noexcept' class method."
			);
			static_assert(
				requires( T t ) {
					{ t.deactivate() };
				}, "You must implement 'void deactivate() noexcept' class method."
			);
			static_assert(
				not std::is_trivially_destructible<T>::value,
				"You must implement a destructor class method, as the responsability of the MdEngineFieldPlacer is "
				"also to cleanup his widget(s) on destruction."
			);
		}

	  private:
		struct _interface
		{
			virtual ~_interface()						= default;
			virtual void assign( FormLayouts ) noexcept = 0;
			virtual void activate() noexcept			= 0;
			virtual void deactivate() noexcept			= 0;
		};

		template<typename T>
		struct wrapper final : public _interface
		{
			T _t;
			wrapper( T && p_in ) : _t( std::forward<T>( p_in ) ) {}
			virtual void assign( FormLayouts p_layouts ) noexcept { _t.assign( std::move( p_layouts ) ); }
			virtual void activate() noexcept override { _t.activate(); }
			virtual void deactivate() noexcept override { _t.deactivate(); }
		};

		std::unique_ptr<_interface> _ptr = nullptr;
	};

} // namespace VTX::Tool::Mdprep::ui

#endif
