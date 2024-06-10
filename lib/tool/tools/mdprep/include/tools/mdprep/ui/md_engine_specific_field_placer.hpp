#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_MD_ENGINE_SPECIFIC_FIELD_PLACER__
#define __VTX_TOOL_TOOLS_MDPREP_UI_MD_ENGINE_SPECIFIC_FIELD_PLACER__

#include <memory>
#include <util/concepts.hpp>
class QFormLayout;

namespace VTX::Tool::Mdprep::ui
{

	struct EngineSpecificCommonInformationFieldUpdate;

	template<typename T>
	concept IsUpdatable = requires( T t, const EngineSpecificCommonInformationFieldUpdate & arg ) {
		{
			t.update( arg )
		};
	};

	// Class responsible for MD Engines fields specific to a context
	class MdEngineSpecificFieldPlacer
	{
		struct _interface
		{
			virtual ~_interface()															   = default;
			virtual void placeFields( QFormLayout * ) noexcept								   = 0;
			virtual void apply() noexcept													   = 0;
			virtual void update( const EngineSpecificCommonInformationFieldUpdate & ) noexcept = 0;
		};

		template<typename T>
		struct wrapper final : public _interface
		{
			T _t;
			wrapper( T && p_in ) : _t( std::forward<T>( p_in ) ) {}
			virtual void placeFields( QFormLayout * p_ ) noexcept override { _t.placeFields( p_ ); }
			virtual void apply() noexcept override { _t.apply(); }
			virtual void update( const EngineSpecificCommonInformationFieldUpdate & p_ ) noexcept override
			{
				if constexpr ( IsUpdatable<T> )
					_t.update( p_ );
			}
		};

		std::unique_ptr<_interface> _ptr = nullptr;

	  public:
		template<typename T>
			requires( SameUnalteredType<T, MdEngineSpecificFieldPlacer> == false )
		MdEngineSpecificFieldPlacer( T && p_in ) : _ptr( new wrapper<T>( std::forward<T>( p_in ) ) )
		{
		}
		MdEngineSpecificFieldPlacer() = default;

		// Adds fields to the input layout
		inline void placeFields( QFormLayout * p_ ) noexcept
		{
			if ( _ptr )
				_ptr->placeFields( p_ );
		}

		// Apply changes to the fields values into data that'll be send to the backend.
		inline void apply() noexcept
		{
			if ( _ptr )
				_ptr->apply();
		}
		// OPTIONAL : Inform the field placer that a value has changed.
		// DO nothing if the method isn't implemented
		inline void update( const EngineSpecificCommonInformationFieldUpdate & p_ ) noexcept
		{
			if ( _ptr )
				_ptr->update( p_ );
		}
	};
} // namespace VTX::Tool::Mdprep::ui
#endif
