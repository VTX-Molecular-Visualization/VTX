#ifndef __VTX_APP_CORE_ANIMATION_ORIENT__
#define __VTX_APP_CORE_ANIMATION_ORIENT__

#include "app/component/render/camera.hpp"
#include "translation.hpp"
#include <util/hashing.hpp>
#include <util/math/aabb.hpp>
#include <util/types.hpp>

namespace VTX::App::Core::Animation
{
	class OrientInfo
	{
	  private:
		static const float ORIENT_ZOOM_FACTOR;

	  public:
		OrientInfo(
			App::Component::Render::Camera & p_camera,
			const float						 p_duration,
			const Util::Math::AABB &		 p_targetAabb
		);
		OrientInfo(
			App::Component::Render::Camera & p_camera,
			const float						 p_duration,
			const Vec3f &					 p_finalPosition,
			const Quatf &					 p_finalRotation,
			const Vec3f &					 p_targetPosition
		);

		const TranslationInfo & getTranslationInfo() const { return _translationInfo; }

		App::Component::Render::Camera & camera;
		Vec3f							 targetStartPosition;
		Vec3f							 targetFinalPosition;

	  private:
		TranslationInfo _translationInfo;
	};

	class Orient : public Translation
	{
	  public:
		inline static const Util::Hashing::Hash ANIMATION_ID = Util::Hashing::hash( "INTERNAL_ORIENT" );

	  public:
		Orient( const OrientInfo & p_info );

	  private:
		void _enter() override;
		void _exit() override;

		const OrientInfo				 _orientInfo;
		App::Component::Render::Camera & _camera;
	};
} // namespace VTX::App::Core::Animation
#endif
