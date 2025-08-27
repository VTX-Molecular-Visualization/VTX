#ifndef __VTX_APP_ACTION_REPRESENTATION__
#define __VTX_APP_ACTION_REPRESENTATION__

#include "app/action/library.hpp"
#include "app/library/preset/representation.hpp"

namespace VTX::App::Action::Representation
{
	template<VTX::Core::Struct::E_REPRESENTATION_VALUES S, typename T>
	class Change final : public App::Core::Action::BaseActionPreset<T>
	{
	  public:
		Change( App::Library::Preset::Representation * const, const T p_value ) :
			App::Core::Action::BaseActionPreset<T>( p_preset ), _value( p_value )
		{
		}
		Change( const std::string_view p_preset, const T p_value ) :
			App::Core::Action::BaseActionPreset<T>( p_preset ), _value( p_value )
		{
		}

		void execute() override { _preset->setValue<S>( _value ); }

	  private:
		const T _value;
	};

} // namespace VTX::App::Action::Representation
#endif
