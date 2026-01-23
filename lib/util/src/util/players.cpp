#include "util/players.hpp"

namespace VTX::Util::Players
{
	Forward::Forward( uint p_stepNum, uint p_startingStep ) :
		_lastIndex( p_stepNum - 1 ), _currentStep( p_startingStep )
	{
	}
	void Forward::jumpTo( const uint & p_step ) noexcept { _currentStep = p_step; }
	void Forward::next( uint & p_out ) const noexcept
	{
		p_out = _lastIndex == _currentStep ? _currentStep : _currentStep + 1;
	}
	void Forward::increment() noexcept { _currentStep = _lastIndex == _currentStep ? _currentStep : _currentStep + 1; }

} // namespace VTX::Util::Players
