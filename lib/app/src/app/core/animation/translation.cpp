#include "app/core/animation/translation.hpp"
#include <util/math.hpp>

namespace VTX::App::Core::Animation
{
	Translation::Translation( const TranslationInfo & p_info ) :
		BaseAnimation( p_info.toBaseAnimationInfo() ), _info( p_info )
	{
	}

	void Translation::_enter()
	{
		BaseAnimation::_enter();

		_info.targetPtr->setPosition( _info.startPosition );
		_info.targetPtr->setRotation( _info.startRotation );

		const float translationDistance = Util::Math::distance( _info.startPosition, _info.finalPosition );
		const bool	skipAnimation
			= translationDistance < TRANSLATION_THRESHOLD && _info.startRotation == _info.finalRotation;

		if ( skipAnimation )
			stop();
	}

	void Translation::_internalUpdate( float p_deltaTime )
	{
		const float ratio = getRatio();

		const Vec3f newPos = Util::Math::easeInOutInterpolation( _info.startPosition, _info.finalPosition, ratio );
		_info.targetPtr->setPosition( newPos );

		const Quatf newRotation = Util::Math::easeInOutInterpolation( _info.startRotation, _info.finalRotation, ratio );
		_info.targetPtr->setRotation( newRotation );
	}

} // namespace VTX::App::Core::Animation
