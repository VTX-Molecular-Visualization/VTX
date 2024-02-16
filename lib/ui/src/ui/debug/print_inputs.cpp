#include "ui/debug/print_inputs.hpp"
#include "ui/qt/input/input_manager.hpp"
#include <util/enum.hpp>
#include <util/logger.hpp>

namespace VTX::UI::Debug
{
	PrintInputs::PrintInputs()
	{
		QT::INPUT_MANAGER().onKeyPressed.addCallback( this, [ this ]( Qt::Key p_key ) { _logKeyPressed( p_key ); } );
		QT::INPUT_MANAGER().onKeyReleased.addCallback( this, [ this ]( Qt::Key p_key ) { _logKeyReleased( p_key ); } );
	}

	void PrintInputs::_logKeyPressed( Qt::Key p_key ) const
	{
		VTX_DEBUG( "Key pressed : {}", Util::Enum::enumName<Qt::Key>( p_key ) );
	}
	void PrintInputs::_logKeyReleased( Qt::Key p_key ) const
	{
		VTX_DEBUG( "Key released : {}", Util::Enum::enumName<Qt::Key>( p_key ) );
	}
} // namespace VTX::UI::Debug
