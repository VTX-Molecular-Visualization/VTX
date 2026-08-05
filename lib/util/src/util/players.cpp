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

	void Forward::next( const uint & p_incr, uint & p_out ) const noexcept
	{ p_out = std::min( _currentStep + p_incr, _lastIndex ); }

	void Forward::increment() noexcept { _currentStep = std::min( _lastIndex, _currentStep + 1 ); }

	void Forward::increment( const uint & p_N ) noexcept { _currentStep = std::min( _currentStep + p_N, _lastIndex ); }

	void Forward::current( uint & p_out ) const noexcept { p_out = _currentStep; }

	void Forward::setStepCount( const uint p_stepCount ) noexcept
	{
		_lastIndex	 = p_stepCount > 0 ? p_stepCount - 1 : 0;
		_currentStep = std::min( _currentStep, _lastIndex );
	}

	ForwardLoop::ForwardLoop( uint p_stepNum, uint p_startingStep ) :
		_lastIndex( p_stepNum - 1 ), _currentStep( p_startingStep )
	{
	}

	void ForwardLoop::jumpTo( const uint & p_step ) noexcept { _currentStep = std::min( p_step, _lastIndex ); }

	void ForwardLoop::next( const uint & p_incr, uint & p_out ) const noexcept
	{ p_out = ( _currentStep + p_incr ) % ( _lastIndex + 1 ); }

	void ForwardLoop::next( uint & p_out ) const noexcept { p_out = ( _currentStep + 1 ) % ( _lastIndex + 1 ); }

	void ForwardLoop::increment() noexcept { _currentStep = ( _currentStep + 1 ) % ( _lastIndex + 1 ); }

	void ForwardLoop::increment( const uint & p_N ) noexcept
	{ _currentStep = ( _currentStep + p_N ) % ( _lastIndex + 1 ); }

	void ForwardLoop::current( uint & p_out ) const noexcept { p_out = _currentStep; }

	void ForwardLoop::setStepCount( const uint p_stepCount ) noexcept
	{
		_lastIndex	 = p_stepCount > 0 ? p_stepCount - 1 : 0;
		_currentStep = std::min( _currentStep, _lastIndex );
	}

	Backward::Backward( uint p_stepNum, uint p_startingStep ) :
		_lastIndex( p_stepNum - 1 ), _currentStep( p_startingStep )
	{
	}

	void Backward::jumpTo( const uint & p_step ) noexcept { _currentStep = std::min( p_step, _lastIndex ); }

	void Backward::next( const uint & p_incr, uint & p_out ) const noexcept
	{ p_out = ( _currentStep > p_incr ) * ( _currentStep - p_incr ); }

	void Backward::next( uint & p_out ) const noexcept { p_out = ( _currentStep > 1 ) * ( _currentStep - 1 ); }

	void Backward::increment() noexcept { _currentStep = ( _currentStep > 1 ) * ( _currentStep - 1 ); }

	void Backward::increment( const uint & p_N ) noexcept
	{ _currentStep = ( _currentStep > p_N ) * ( _currentStep - p_N ); }

	void Backward::current( uint & p_out ) const noexcept { p_out = _currentStep; }

	void Backward::setStepCount( const uint p_stepCount ) noexcept
	{
		_lastIndex	 = p_stepCount > 0 ? p_stepCount - 1 : 0;
		_currentStep = std::min( _currentStep, _lastIndex );
	}

	BackwardLoop::BackwardLoop( uint p_stepNum, uint p_startingStep ) :
		_lastIndex( p_stepNum - 1 ), _currentStep( p_startingStep )
	{
	}

	void BackwardLoop::jumpTo( const uint & p_step ) noexcept { _currentStep = std::min( p_step, _lastIndex ); }

	void BackwardLoop::next( const uint & p_incr, uint & p_out ) const noexcept
	{
		const uint modulatedIncrement = ( p_incr % ( _lastIndex + 1 ) );
		p_out						  = ( ( _currentStep + _lastIndex + 1 ) - modulatedIncrement ) % ( _lastIndex + 1 );
	}

	void BackwardLoop::next( uint & p_out ) const noexcept
	{ p_out = _currentStep == 0 ? _lastIndex : _currentStep - 1; }

	void BackwardLoop::increment() noexcept { _currentStep = _currentStep == 0 ? _lastIndex : _currentStep - 1; }

	void BackwardLoop::increment( const uint & p_N ) noexcept
	{
		const uint modulatedIncrement = ( p_N % ( _lastIndex + 1 ) );
		_currentStep				  = ( ( _currentStep + _lastIndex + 1 ) - modulatedIncrement ) % ( _lastIndex + 1 );
	}

	void BackwardLoop::current( uint & p_out ) const noexcept { p_out = _currentStep; }

	void BackwardLoop::setStepCount( const uint p_stepCount ) noexcept
	{
		_lastIndex	 = p_stepCount > 0 ? p_stepCount - 1 : 0;
		_currentStep = std::min( _currentStep, _lastIndex );
	}

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

	namespace
	{
		uint pingpongNstepfurther(
			const bool & p_forward,
			const uint & p_lastIndex,
			const uint & p_currentStep,
			const uint & p_N,
			int64_t &	 p_outOfBoundNewStep
		)
		{
			/*
			The idea is that for a pingpong of 0..M, adding N is equivalent to adding N + 2M.
			Therefore, we take its value modulo 2M
			Then, we want to mirror the value ordering of 0..M..0 of a number N which is 0 <= N < 2M
			This is done by substracting M to the absolute value of N - M
			Since N < 2M, N - M < M so 0 <= M - abs(N-M) <= M
			*/
			const int64_t valueToAdd
				= ( p_forward * 2 - 1 )
				  * static_cast<int64_t>(
					  static_cast<uint64_t>( p_N ) % ( static_cast<uint64_t>( p_lastIndex ) << 1 /*Multiply by 2*/ )
				  );
			p_outOfBoundNewStep = p_currentStep + valueToAdd;
			return static_cast<uint>( std::abs(
				static_cast<int64_t>( p_lastIndex ) * p_forward
				- std::abs(
					valueToAdd + static_cast<int64_t>( p_currentStep ) - static_cast<int64_t>( p_lastIndex ) * p_forward
				)
			) );
		}
	} // namespace

	void PingPong::next( const uint & p_incr, uint & p_out ) const noexcept
	{
		if ( _lastIndex == 0 )
		{
			p_out = 0;
			return;
		}

		int64_t outOfBoundNewStep = 0;
		p_out = pingpongNstepfurther( _forward, _lastIndex, _currentStep, p_incr, outOfBoundNewStep );
	}

	void PingPong::increment( const uint & p_N ) noexcept
	{
		if ( _lastIndex == 0 )
		{
			_currentStep = 0;
			return;
		}

		int64_t outOfBoundNewStep = 0;
		_currentStep			  = pingpongNstepfurther( _forward, _lastIndex, _currentStep, p_N, outOfBoundNewStep );
		_forward				  = outOfBoundNewStep < 0 || outOfBoundNewStep > _lastIndex ? not _forward : _forward;
	}

	void PingPong::current( uint & p_out ) const noexcept { p_out = _currentStep; }

	void PingPong::setStepCount( const uint p_stepCount ) noexcept
	{
		_lastIndex	 = p_stepCount > 0 ? p_stepCount - 1 : 0;
		_currentStep = std::min( _currentStep, _lastIndex );
	}

} // namespace VTX::Util::Players
