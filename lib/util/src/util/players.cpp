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
	void Forward::increment( const uint & p_N ) noexcept { _currentStep = std::min( _currentStep + p_N, _lastIndex ); }
	void Forward::current( uint & p_out ) const noexcept { p_out = _currentStep; }

	PingPong::PingPong( uint p_stepNum, uint p_startingStep ) :
		_lastIndex( p_stepNum - 1 ), _currentStep( p_startingStep )
	{
	}
	void PingPong::jumpTo( const uint & p_step ) noexcept
	{
		_forward	 = p_step > _currentStep;
		_currentStep = std::min( p_step, _lastIndex );
	}
	void PingPong::next( uint & p_out ) const noexcept
	{
		if ( _lastIndex == 0 )
		{
			p_out = 0;
			return;
		}
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
	void PingPong::increment( const uint & p_N ) noexcept
	{
		/*
		The idea is that for a pingpong of 0..M, adding N is equivalent to adding N + 2M.
		Therefore, we take its value modulo 2M
		Then, we want to mirror the value ordering of 0..M..0 of a number N which is 0 <= N < 2M
		This is done by substracting M to the absolute value of N - M
		Since N < 2M, N - M < M so 0 <= M - abs(N-M) <= M
		*/
		const int64_t valueToAdd
			= ( _forward * 2 - 1 )
			  * static_cast<int64_t>(
				  static_cast<uint64_t>( p_N ) % ( static_cast<uint64_t>( _lastIndex ) << 1 /*Multiply by 2*/ )
			  );
		const int64_t outOfBoundNewStep = _currentStep + valueToAdd;
		_currentStep					= static_cast<uint>( std::abs(
			   static_cast<int64_t>( _lastIndex ) * _forward
			   - std::abs(
				   valueToAdd + static_cast<int64_t>( _currentStep ) - static_cast<int64_t>( _lastIndex ) * _forward
			   )
		   ) );
		_forward = outOfBoundNewStep < 0 || outOfBoundNewStep > _lastIndex ? not _forward : _forward;
	}
	void PingPong::current( uint & p_out ) const noexcept { p_out = _currentStep; }

} // namespace VTX::Util::Players
