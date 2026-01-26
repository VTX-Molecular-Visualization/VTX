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
	PingPong::PingPong( uint p_stepNum, uint p_startingStep ) :
		_lastIndex( p_stepNum - 1 ), _currentStep( p_startingStep )
	{
	}
	void PingPong::jumpTo( const uint & p_step ) noexcept { _currentStep = std::min( p_step, _lastIndex ); }
	void PingPong::next( uint & p_out ) const noexcept
	{
		if ( _forward )
		{
			if ( _currentStep == _lastIndex )
			{
				p_out = _currentStep - 1;
			}
			else
			{
				p_out = _currentStep + 1;
			}
		}
		else
		{
			if ( _currentStep == 0 )
			{
				p_out = _currentStep + 1;
			}
			else
			{
				p_out = _currentStep - 1;
			}
		}
	}
	void PingPong::increment() noexcept
	{
		if ( _forward )
		{
			if ( _currentStep == _lastIndex )
			{
				_currentStep--;
				_forward = false;
			}
			else
			{
				_currentStep++;
			}
		}
		else
		{
			if ( _currentStep == 0 )
			{
				_currentStep++;
				_forward = true;
			}
			else
			{
				_currentStep--;
			}
		}
	}

} // namespace VTX::Util::Players
