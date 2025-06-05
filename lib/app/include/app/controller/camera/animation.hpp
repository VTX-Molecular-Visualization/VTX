#ifndef __VTX_APP_CONTROLLER_CAMERA_ANIMATION__
#define __VTX_APP_CONTROLLER_CAMERA_ANIMATION__

#include "app/core/animation/concepts.hpp"
#include "app/core/controller/base_controller.hpp"
#include "app/core/input/key_mapping.hpp"
#include "app/settings.hpp"
#include <util/hashing.hpp>

namespace VTX::App::Controller::Camera
{
	template<Core::Animation::ConceptAnimation A>
	class Animation : public Core::Controller::BaseController
	{
	  public:
		template<typename... Args>
		Animation( Args &&... p_args ) : _animation( std::make_unique<A>( std::forward<Args>( p_args )... ) )
		{
		}

		inline void play() { _animation->play(); }

		inline void update( const float p_deltaTime, const float p_elapsedTime )
		{
			_animation->update( p_deltaTime, p_elapsedTime );
		}

		inline Util::Callback<const Vec3f &, const Quatf &> & onAnimationProgress() { return _animation->onProgress; }
		inline Util::Callback<const Vec3f &> &				  onAnimationEnd() { return _animation->onEnd; }

	  private:
		std::unique_ptr<A> _animation;
	};

	/**
	 * @brief Class responsible allowing animation manipulation to consumers
	 */
	class GenericAnimation : public Core::Controller::BaseController
	{
	  public:
		GenericAnimation() = default;

		/**
		 * @brief TODO
		 */
		inline void play()
		{
			if ( _ptr )
				_ptr->play();
		}

		/**
		 * @brief Make the animation move forward.
		 * @param p_deltaTime time since last call
		 * @param p_elapsedTime time since the start of the process ( TODO : confirm that)
		 */
		inline void update( const float p_deltaTime, const float p_elapsedTime )
		{
			if ( _ptr )
				_ptr->update( p_deltaTime, p_elapsedTime );
		}

		///**
		// * @brief TODO
		// */
		// inline void enter()
		//{
		//	if ( _ptr )
		//		_ptr->enter();
		//}

		///**
		// * @brief TODO
		// */
		// inline void exit()
		//{
		//	if ( _ptr )
		//		_ptr->exit();
		//}

		/**
		 * @brief Returns an hash unique to the type of animation played.
		 * @return
		 */
		friend Hash hash( const GenericAnimation & p_animation )
		{
			if ( p_animation._ptr )
				return p_animation._ptr->hash();
			return 0;
		}

		inline Util::Callback<const Vec3f &, const Quatf &> & onAnimationProgress()
		{
			if ( _ptr )
				return _ptr->onAnimationProgress();
			throw VTX::VTXException( "Tried to see onAnimationProgress callbacks of an empty animation" );
		}
		inline Util::Callback<const Vec3f &> & onAnimationEnd()
		{
			if ( _ptr )
				return _ptr->onAnimationEnd();
			throw VTX::VTXException( "Tried to see onAnimationEnd callbacks of an empty animation" );
		}

	  private:
		struct _interface
		{
			virtual ~_interface()							= default;
			virtual void play()								= 0;
			virtual void update( const float, const float ) = 0;
			// virtual void enter()							= 0;
			// virtual void exit()								= 0;
			virtual Hash hash() = 0;

			virtual Util::Callback<const Vec3f &, const Quatf &> & onAnimationProgress() = 0;
			virtual Util::Callback<const Vec3f &> &				   onAnimationEnd()		 = 0;
		};
		template<typename T>
		class _wrapper final : public _interface
		{
			T _obj;

		  public:
			_wrapper( T && p_obj ) : _obj( std::forward<T>( p_obj ) ) {}

			virtual void play() override { _obj.play(); }
			virtual void update( const float p_deltaTime, const float p_elapsedTime ) override
			{
				_obj.update( p_deltaTime, p_elapsedTime );
			}
			// virtual void enter() override { _obj.enter(); }
			// virtual void exit() override { _obj.exit(); }
			virtual Hash hash() override { return Util::hash<T>(); }

			virtual Util::Callback<const Vec3f &, const Quatf &> & onAnimationProgress() override
			{
				return _obj.onProgress();
			}
			virtual Util::Callback<const Vec3f &> & onAnimationEnd() override { return _obj.onEnd(); }
		};

		std::unique_ptr<_interface> _ptr = nullptr;

	  public:
		template<typename T>
			requires( not std::same_as<std::remove_cvref_t<T>, GenericAnimation> )
		GenericAnimation( T && _ ) : _ptr( new _wrapper<T>( std::forward<T>( _ ) ) )
		{
		}
	};
} // namespace VTX::App::Controller::Camera
#endif
