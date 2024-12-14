#include "app/core/animation/base_animation.hpp"
#include <util/math.hpp>

namespace VTX::App::Core::Animation
{

	BaseAnimation::BaseAnimation(
		const Util::Math::Transform & p_source,
		const Vec3f &				  p_finalPosition,
		const Quatf &				  p_finalRotation,
		const Vec3f &				  p_target,
		const float					  p_duration
	) :
		_finalPosition( p_finalPosition ), _finalRotation( p_finalRotation ), _target( p_target ),
		_duration( p_duration )
	{
		_startPosition = p_source.getTranslationVector();
		_startRotation = p_source.getRotation();
	}

	void BaseAnimation::update( const float p_delta, const float p_elapsed )
	{
		if ( not _isRunning )
		{
			return;
		}

		_currentTime += p_delta;

		if ( _currentTime >= _duration )
		{
			stop();
			return;
		}

		const Vec3f position = _positionFunc( _startPosition, _finalPosition, getRatio() );
		const Quatf rotation = _rotationFunc( _startRotation, _finalRotation, getRatio() );

		onProgress( position, rotation );
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
		_isRunning = false;

		onProgress( _finalPosition, _finalRotation );
		onEnd( _target );
	};

} // namespace VTX::App::Core::Animation
