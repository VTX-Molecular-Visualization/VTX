#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_MD_ENGINE_FIELD_PLACER__
#define __VTX_TOOL_TOOLS_MDPREP_UI_MD_ENGINE_FIELD_PLACER__

#include <string>
//
#include <memory>
#include <type_traits>
#include <util/concepts.hpp>

namespace VTX::Tool::Mdprep::ui
{
	using namespace VTX::Tool::Mdprep::Gateway;

	// Class responsible for placing MD engine specific fields
	class MdEngineFieldPlacer
	{
		struct _interface
		{
			virtual ~_interface()			   = default;
			virtual void activate() noexcept   = 0;
			virtual void deactivate() noexcept = 0;
		};

		template<typename T>
		struct wrapper final : public _interface
		{
			T _t;
			wrapper( T && p_in ) : _t( std::forward<T>( p_in ) ) {}
			virtual void activate() noexcept override { _t.activate(); }
			virtual void deactivate() noexcept override { _t.deactivate(); }
		};

		std::unique_ptr<_interface> _ptr = nullptr;

	  public:
		template<typename SpecificEngine>
			requires( not SameUnalteredType<SpecificEngine, MdEngineFieldPlacer> )
		MdEngineFieldPlacer( SpecificEngine && p_in ) :
			_ptr( new wrapper<SpecificEngine>( std::forward<SpecificEngine>( p_in ) ) )
		{
		}
		MdEngineFieldPlacer() = default;

		// Add the engine-specific field in the right place.
		inline void activate() noexcept
		{
			if ( _ptr )
				_ptr->activate();
		}
		// Remove the engine-specific field.
		inline void deactivate() noexcept
		{
			if ( _ptr )
				_ptr->deactivate();
		}
	};

} // namespace VTX::Tool::Mdprep::ui

#endif
