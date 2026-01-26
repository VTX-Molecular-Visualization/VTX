#include "util/players.hpp"
#include <math.h>

namespace VTX::Util::Players
{
	Forward::Forward( uint p_stepNum, uint p_startingStep ) :
		_lastIndex( p_stepNum - 1 ), _currentStep( p_startingStep )
	{
	}
	void Forward::jumpTo( const uint & p_step ) noexcept { _currentStep = std::min( p_step, _lastIndex ); }
	void Forward::next( uint & p_out ) const noexcept { p_out = std::min( _lastIndex, _currentStep + 1 ); }
	void Forward::increment() noexcept { _currentStep = std::min( _lastIndex, _currentStep + 1 ); }

} // namespace VTX::Util::Players
