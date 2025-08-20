#ifndef __VTX_APP_ACTION_REPRESENTATION__
#define __VTX_APP_ACTION_REPRESENTATION__

#include "app/core/action/base_action.hpp"
#include "app/core/library/library_system.hpp"
#include "app/library/preset/representation.hpp"
#include <string>
#include <variant>

namespace VTX::App::Action::Representation
{
	template<App::Library::Preset::E_REPRESENTATION_SETTINGS S, typename T>
	class ChangeRepresentation final : public App::Core::Action::BaseAction
	{
	  public:
		ChangeRepresentation(
			const std::variant<App::Library::Preset::Representation *, std::string> p_preset,
			const T																	p_value
		) : _preset( p_preset ), _value( p_value )
		{
		}

		void execute() override
		{
			if ( std::holds_alternative<std::string>( _preset ) )
			{
				auto * const library = LIBRARY_SYSTEM().getLibrary<App::Library::Preset::Representation>();
				library->getPreset( std::get<std::string>( _preset ) )->setValue<S>( _value );
			}
			else
			{
				assert( std::holds_alternative<App::Library::Preset::Representation *>( _preset ) );
				std::get<App::Library::Preset::Representation *>( _preset )->setValue<S>( _value );
			}
		}

	  private:
		const std::variant<App::Library::Preset::Representation *, std::string> _preset;
		const T																	_value;
	};

} // namespace VTX::App::Action::Representation
#endif
