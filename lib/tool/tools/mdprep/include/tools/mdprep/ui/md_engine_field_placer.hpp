#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_MD_ENGINE_FIELD_PLACER__
#define __VTX_TOOL_TOOLS_MDPREP_UI_MD_ENGINE_FIELD_PLACER__

#include <string>
//
#include <memory>
#include <type_traits>
#include <util/concepts.hpp>

class QLayout;
class QFormLayout;

namespace VTX::Tool::Mdprep::Gateway
{
	struct EngineSpecificCommonFormData;
}

namespace VTX::Tool::Mdprep::ui
{
	using namespace VTX::Tool::Mdprep::Gateway;

	class MdEngineSpecificFieldPlacer;

	// Class responsible for placing MD engine specific fields
	class MdEngineFieldPlacer
	{
		struct _interface
		{
			virtual ~_interface()													 = default;
			virtual void get( MdEngineSpecificFieldPlacer & ) noexcept				 = 0;
			virtual void get( const EngineSpecificCommonFormData *& ) const noexcept = 0;
			virtual void activate() noexcept										 = 0;
			virtual void deactivate() noexcept										 = 0;
		};

		template<typename T>
		struct wrapper final : public _interface
		{
			T _t;
			wrapper( T && p_in ) : _t( std::forward<T>( p_in ) ) {}
			virtual void get( MdEngineSpecificFieldPlacer & p_ ) noexcept override { _t.get( p_ ); }
			virtual void get( const EngineSpecificCommonFormData *& p_ ) const noexcept override { _t.get( p_ ); }
			virtual void activate() noexcept override { _t.activate(); }
			virtual void deactivate() noexcept override { _t.deactivate(); }
		};

		std::unique_ptr<_interface> _ptr = nullptr;

	  public:
		template<typename SpecificEngine>
			requires( SameUnalteredType<SpecificEngine, MdEngineFieldPlacer> == false )
		MdEngineFieldPlacer( SpecificEngine && p_in ) :
			_ptr( new wrapper<SpecificEngine>( std::forward<SpecificEngine>( p_in ) ) )
		{
		}

		inline void get( MdEngineSpecificFieldPlacer & p_ ) noexcept { _ptr->get( p_ ); }
		inline void get( const EngineSpecificCommonFormData *& p_data ) noexcept { _ptr->get( p_data ); }
		// Add the engine-specific field in the right place.
		inline void activate() noexcept { _ptr->activate(); }
		// Remove the engine-specific field.
		inline void deactivate() noexcept { _ptr->deactivate(); }
	};

} // namespace VTX::Tool::Mdprep::ui

#endif
