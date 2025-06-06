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
		 * @brief Allows the animation to unfold.
		 */
		inline void play() { _ptr->play(); }

		/**
		 * @brief Make the animation move forward.
		 * @param p_deltaTime time since last call
		 * @param p_elapsedTime time since the start of the process ( TODO : confirm that)
		 */
		inline void update( const float p_deltaTime, const float p_elapsedTime )
		{
			_ptr->update( p_deltaTime, p_elapsedTime );
		}

		/**
		 * @brief Returns an hash unique to the type of animation played.
		 * @return
		 */
		friend Hash hash( const GenericAnimation & p_animation ) { return p_animation._ptr->hash(); }

		/**
		 * @brief Add a callback that will be called at each step of the animation
		 */
		inline void subscribe( Core::Animation::ProgressCallback p_ ) { _ptr->subscribe( std::move( p_ ) ); }

		/**
		 * @brief Add a callback that will be called at the end of the animation
		 */
		inline void subscribe( Core::Animation::EndCallback p_ ) { _ptr->subscribe( std::move( p_ ) ); }

	  private:
		struct _interface
		{
			virtual ~_interface()							= default;
			virtual void play()								= 0;
			virtual void update( const float, const float ) = 0;
			virtual Hash hash() const						= 0;

			virtual void subscribe( Core::Animation::ProgressCallback ) = 0;
			virtual void subscribe( Core::Animation::EndCallback )		= 0;
		};
		struct _void // Small trick that allow GenericAnimation to be default constructible (requirement for being
					 // stored in some standard collections for instance)
		{
		};
		template<typename T>
		class _wrapper final : public _interface
		{
			T _obj;

		  public:
			_wrapper( T && p_obj ) : _obj( std::forward<T>( p_obj ) ) {}

			virtual void play() override
			{
				if constexpr ( not std::same_as<T, _void> )
					_obj.play();
			}
			virtual void update( const float p_deltaTime, const float p_elapsedTime ) override
			{
				if constexpr ( not std::same_as<T, _void> )
					_obj.update( p_deltaTime, p_elapsedTime );
			}
			virtual Hash hash() const override { return Util::hash<T>(); }

			virtual void subscribe( Core::Animation::EndCallback p_ ) override
			{
				if constexpr ( not std::same_as<T, _void> )
					_obj.subscribe( std::move( p_ ) );
			}
			virtual void subscribe( Core::Animation::ProgressCallback p_ ) override
			{
				if constexpr ( not std::same_as<T, _void> )
					_obj.subscribe( std::move( p_ ) );
			}
		};

		std::unique_ptr<_interface> _ptr = std::unique_ptr<_wrapper<_void>>();

	  public:
		template<typename T>
			requires( not std::same_as<std::remove_cvref_t<T>, GenericAnimation> )
		GenericAnimation( T && _ ) : _ptr( new _wrapper<T>( std::forward<T>( _ ) ) )
		{
		}
	};
} // namespace VTX::App::Controller::Camera
#endif
