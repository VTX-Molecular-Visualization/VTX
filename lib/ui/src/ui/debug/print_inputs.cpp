#include "ui/debug/print_inputs.hpp"
#include "app/core/input/input_manager.hpp"
#include <util/enum.hpp>
#include <util/logger.hpp>

namespace VTX::UI::Debug
{
	PrintInputs::PrintInputs()
	{
		App::INPUT_MANAGER().onKeyPressed += [ this ]( App::Core::Input::Key p_key ) { _logKeyPressed( p_key ); };
		App::INPUT_MANAGER().onKeyReleased += [ this ]( App::Core::Input::Key p_key ) { _logKeyReleased( p_key ); };
	}

	void PrintInputs::_logKeyPressed( App::Core::Input::Key p_key ) const
	{
		VTX_DEBUG( "Key pressed : {}", Util::Enum::enumName<App::Core::Input::Key>( p_key ) );
	}
	void PrintInputs::_logKeyReleased( App::Core::Input::Key p_key ) const
	{
		VTX_DEBUG( "Key released : {}", Util::Enum::enumName<App::Core::Input::Key>( p_key ) );
	}
} // namespace VTX::UI::Debug
