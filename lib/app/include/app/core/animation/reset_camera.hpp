#ifndef __VTX_UI_INTERNAL_ANIMATION_RESET_CAMERA__
#define __VTX_UI_INTERNAL_ANIMATION_RESET_CAMERA__

#include "app/core/animation/translation.hpp"
#include <app/component/render/camera.hpp>
#include <util/hashing.hpp>
#include <util/math/aabb.hpp>
#include <util/types.hpp>

namespace VTX::App::Core::Animation
{
	class ResetInfo
	{
	  public:
		ResetInfo( App::Component::Render::Camera & p_camera, const float p_duration );

		const TranslationInfo & getTranslationInfo() const { return _translationInfo; }

		App::Component::Render::Camera & camera;
		Vec3f							 targetStartPosition;
		Vec3f							 targetFinalPosition;

	  private:
		TranslationInfo _translationInfo;
	};

	class ResetCamera : public Translation
	{
	  public:
		inline static const Util::Hashing::Hash ANIMATION_ID = Util::Hashing::hash( "INTERNAL_RESET_CAMERA" );

	  public:
		ResetCamera( const ResetInfo & p_info );

	  private:
		void _enter() override;
		void _exit() override;

		const ResetInfo					 _resetInfo;
		App::Component::Render::Camera & _camera;
	};
} // namespace VTX::App::Core::Animation
#endif
