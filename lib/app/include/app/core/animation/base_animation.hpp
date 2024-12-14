#ifndef __VTX_APP_CORE_ANIMATION_BASE_ANIMATION__
#define __VTX_APP_CORE_ANIMATION_BASE_ANIMATION__

#include <util/callback.hpp>
#include <util/math/transform.hpp>

namespace VTX::App::Core::Animation
{

	class BaseAnimation
	{
	  public:
		inline static const float TRANSLATION_THRESHOLD = 0.1f;
		inline static const float DEFAULT_DURATION		= 500.f;

		BaseAnimation( Util::Math::Transform &, const Vec3f, const Quatf, const float = DEFAULT_DURATION );
		virtual ~BaseAnimation() = default;

		void update( const float p_delta, const float p_elasped );

		void play();
		void stop();

		inline float getRatio() const { return Util::Math::clamp( 0.f, 1.f, _currentTime / _duration ); }

		Util::Callback<> onEnd;

	  protected:
		float _duration	   = DEFAULT_DURATION;
		float _currentTime = 0.f;
		bool  _isRunning   = false;

		Util::Math::Transform * _target;

		Vec3f _startPosition = VEC3F_ZERO;
		Quatf _startRotation = QUATF_ID;

		Vec3f _finalPosition = VEC3F_ZERO;
		Quatf _finalRotation = QUATF_ID;
	};
} // namespace VTX::App::Core::Animation

#endif
