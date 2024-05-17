#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_MDENGINE__
#define __VTX_TOOL_TOOLS_MDPREP_UI_MDENGINE__

#include <string>
//
#include <memory>
#include <type_traits>
#include <util/concepts.hpp>

class QLayout;

namespace VTX::Tool::Mdprep::ui
{
	struct EngineSpecificCommonFormData;

	// Class responsible for framing md engine form specificities
	class MdEngineForm
	{
		struct model
		{
			virtual ~model()														 = default;
			virtual void get( const EngineSpecificCommonFormData *& ) const noexcept = 0;
			virtual void activate() noexcept										 = 0;
			virtual void deactivate() noexcept										 = 0;
		};

		template<typename T>
		struct wrapper final : public model
		{
			T _t;
			wrapper( T && p_in ) : _t( std::forward<T>( p_in ) ) {}
			virtual void get( const EngineSpecificCommonFormData *& p_ ) const noexcept override { _t.get( p_ ); }
			virtual void activate() noexcept override {}
			virtual void deactivate() noexcept override {}
		};

		std::unique_ptr<model> _ptr = nullptr;

	  public:
		template<typename SpecificEngine>
			requires( SameUnalteredType<SpecificEngine, MdEngineForm> == false )
		MdEngineForm( SpecificEngine && p_in ) :
			_ptr( new wrapper<SpecificEngine>( std::forward<SpecificEngine>( p_in ) ) )
		{
		}

		void get( const EngineSpecificCommonFormData *& ) noexcept;

		// Add the engine-specific field in the right place.
		void activate() noexcept;

		// Remove the engine-specific field.
		void deactivate() noexcept;
	};

} // namespace VTX::Tool::Mdprep::ui

#endif
