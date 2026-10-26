#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_MD_ENGINE_SPECIFIC_FIELD_PLACER__
#define __VTX_TOOL_TOOLS_MDPREP_UI_MD_ENGINE_SPECIFIC_FIELD_PLACER__

#include <memory>
#include <util/concepts.hpp>
class QFormLayout;

namespace VTX::Tool::Mdprep::Gateway
{

	struct EngineSpecificCommonInformation;
}

namespace VTX::Tool::Mdprep::ui
{

	struct EngineSpecificCommonInformationFieldUpdate;
	class MdEngine;

	template<typename T>
	concept IsUpdatable = requires( T t, const EngineSpecificCommonInformationFieldUpdate & arg ) {
		{ t.update( arg ) };
	};

	// Class responsible for MD Engines fields specific to a context
	class MdEngineSpecificFieldPlacer
	{
	  public:
		template<typename T>
			requires( not SameUnalteredType<T, MdEngineSpecificFieldPlacer> )
		MdEngineSpecificFieldPlacer( T && p_in ) : _ptr( new wrapper<T>( std::forward<T>( p_in ) ) )
		{
			static_assert(
				requires( T t, QFormLayout * f ) {
					{ t.placeFields( f ) };
				}, "You must implement 'void placeFields( QFormLayout * p_ )' class method."
			);
			static_assert(
				requires( T t ) {
					{ t.apply() };
				}, "You must implement 'void apply()' class method."
			);
		}

		MdEngineSpecificFieldPlacer() = default;

		// Adds fields to the input layout
		void placeFields( QFormLayout * const p_ )
		{
			if ( _ptr )
			{
				_ptr->placeFields( p_ );
			}
		}

		// Apply changes to the fields values into data that'll be send to the backend.
		void apply()
		{
			if ( _ptr )
			{
				_ptr->apply();
			}
		}

		// OPTIONAL : Inform the field placer that a value has changed.
		// DO nothing if the method isn't implemented
		void update( const EngineSpecificCommonInformationFieldUpdate & p_ )
		{
			if ( _ptr )
			{
				_ptr->update( p_ );
			}
		}

	  private:
		struct _interface
		{
			virtual ~_interface()													  = default;
			virtual void placeFields( QFormLayout * )								  = 0;
			virtual void apply()													  = 0;
			virtual void update( const EngineSpecificCommonInformationFieldUpdate & ) = 0;
		};

		template<typename T>
		struct wrapper final : public _interface
		{
			T _t;

			wrapper( T && p_in ) : _t( std::forward<T>( p_in ) ) {}

			void placeFields( QFormLayout * const p_ ) override { _t.placeFields( p_ ); }

			void apply() override { _t.apply(); }

			void update( const EngineSpecificCommonInformationFieldUpdate & p_ ) override
			{
				if constexpr ( IsUpdatable<T> )
				{
					_t.update( p_ );
				}
			}
		};

		std::unique_ptr<_interface> _ptr = nullptr;
	};
} // namespace VTX::Tool::Mdprep::ui
#endif
