#include "app/core/animation/base_animation.hpp"
#include <util/math.hpp>

namespace VTX::App::Core::Animation
{

	BaseAnimation::BaseAnimation(
		Util::Math::Transform & p_target,
		const Vec3f				p_finalPosition,
		const Quatf				p_finalRotation,
		const float				p_duration
	) :
		_target( &p_target ), _finalPosition( p_finalPosition ), _finalRotation( p_finalRotation ),
		_duration( p_duration )
	{
		_startPosition = _target->getTranslationVector();
		_startRotation = _target->getRotation();
	}

	void BaseAnimation::update( const float p_delta, const float p_elapsed )
	{
		_currentTime += p_delta;

		if ( _currentTime >= _duration )
		{
			stop();
		}
	}

	void BaseAnimation::play()
	{
		_currentTime = 0.f;
		_isRunning	 = true;

		// Check threshold.
		const float translationDistance = Util::Math::distance( _startPosition, _finalPosition );
		const bool	skipAnimation = translationDistance < TRANSLATION_THRESHOLD && _startRotation == _finalRotation;
		if ( skipAnimation )
		{
			stop();
		}
	}

	void BaseAnimation::stop()
	{
		_target->setTranslation( _finalPosition );
		_target->setRotation( _finalRotation );

		_isRunning = false;

		onEnd();
	};

} // namespace VTX::App::Core::Animation
