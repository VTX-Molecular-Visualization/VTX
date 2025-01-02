#ifndef __VTX_APP_CORE_ANIMATION_BASE_ANIMATION__
#define __VTX_APP_CORE_ANIMATION_BASE_ANIMATION__

#include <util/callback.hpp>
#include <util/math/transform.hpp>

namespace VTX::App::Core::Animation
{

	class BaseAnimation
	{
	  public:
		using InterpolationPositionFunc = std::function<Vec3f( const Vec3f &, const Vec3f &, const float )>;
		using InterpolationRotationFunc = std::function<Quatf( const Quatf &, const Quatf &, const float )>;

		inline static const float TRANSLATION_THRESHOLD = 0.1f;
		inline static const float DEFAULT_DURATION		= 500.f;

		BaseAnimation(
			const Util::Math::Transform &,
			const Vec3f &,
			const Quatf &,
			const Vec3f &,
			const float = DEFAULT_DURATION
		);
		virtual ~BaseAnimation() = default;

		void update( const float p_delta, const float p_elasped );

		void play();
		void stop();

		inline float getRatio() const { return Util::Math::clamp( 0.f, 1.f, _currentTime / _duration ); }

		Util::Callback<const Vec3f &, const Quatf &> onProgress;
		Util::Callback<const Vec3f &>				 onEnd;

	  protected:
		float _duration	   = DEFAULT_DURATION;
		float _currentTime = 0.f;
		bool  _isRunning   = false;

		// Util::Math::Transform * _target;

		InterpolationPositionFunc _positionFunc;
		InterpolationRotationFunc _rotationFunc;

		Vec3f _startPosition = VEC3F_ZERO;
		Quatf _startRotation = QUATF_ID;

		Vec3f _finalPosition = VEC3F_ZERO;
		Quatf _finalRotation = QUATF_ID;

		Vec3f _target = VEC3F_ZERO;
	};
} // namespace VTX::App::Core::Animation

#endif
