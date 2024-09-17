#ifndef __VTX_APP_CORE_ANIMATION_CONCEPTS__
#define __VTX_APP_CORE_ANIMATION_CONCEPTS__

#include <util/callback.hpp>
#include <util/collection.hpp>
#include <util/math.hpp>

namespace VTX::App::Core::Animation
{

	/*
	class BaseAnimation
	{
	  public:
		virtual ~BaseAnimation() = default;

		inline void init( const float p_duration )
		{
			_duration	 = p_duration;
			_isRunning	 = true;
			_currentTime = 0.f;
		}

		inline void checkFinished( float p_deltaTime )
		{
			_currentTime += p_deltaTime;

			if ( _currentTime >= _duration )
			{
				stop();
			}
		}

		inline void stop()
		{
			_isRunning = false;
			onStopped();
		};

		inline bool	 isRunning() { return _isRunning; }
		inline float getRatio() const { return Util::Math::clamp( 0.f, 1.f, _currentTime / _duration ); }

		Util::Callback<> onStopped;

	  protected:
		float _currentTime = 0.f;
		float _duration	   = 0.f;
		bool  _isRunning   = false;
	};
	*/

	/*
	template<typename A>
	concept ConceptAnimation
		= std::is_base_of<BaseAnimation, A>::value && requires( A p_animation, const float p_deltaTime ) {
			  { p_animation.play() } -> std::same_as<void>;
			  { p_animation.update( p_deltaTime ) } -> std::same_as<void>;
			  //
			  { p_animation.enter() } -> std::same_as<void>;
			  { p_animation.exit() } -> std::same_as<void>;
		  };
		  */

} // namespace VTX::App::Core::Animation

#endif
