#include "ui/core/animation/base_animation.hpp"
#include <util/math.hpp>

namespace VTX::UI::Core::Animation
{
	BaseAnimation::BaseAnimation( const BaseAnimation & p_source )
	{
		_info	  = p_source._info;
		onStopped = Util::Callback<>();
	}
	BaseAnimation::BaseAnimation( const BaseAnimationInfo & p_info ) { _info = p_info; }

	void BaseAnimation::update( float p_deltaTime )
	{
		_currentTime += p_deltaTime;

		_internalUpdate( p_deltaTime );

		if ( _currentTime >= _info.duration )
			stop();
	}

	void BaseAnimation::play()
	{
		_isRunning	 = true;
		_currentTime = 0.f;
		_enter();
	}

	void BaseAnimation::stop()
	{
		_isRunning = false;
		_exit();

		onStopped();
	};

	float BaseAnimation::getRatio() const { return Util::Math::clamp( 0.f, 1.f, _currentTime / _info.duration ); }
} // namespace VTX::UI::Core::Animation
