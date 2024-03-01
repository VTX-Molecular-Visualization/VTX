#ifndef __VTX_UI_INTERNAL_ANIMATION_TRANSLATION__
#define __VTX_UI_INTERNAL_ANIMATION_TRANSLATION__

#include "ui/core/animation/base_animation.hpp"
#include <app/component/scene/transform_component.hpp>
#include <util/hashing.hpp>
#include <util/types.hpp>

namespace VTX::UI::Internal::Animation
{
	struct TranslationInfo
	{
		App::Component::Scene::Transform * targetPtr;

		float duration;
		Vec3f startPosition = VEC3F_ZERO;
		Vec3f finalPosition = VEC3F_ZERO;
		Quatf startRotation = QUATF_ID;
		Quatf finalRotation = QUATF_ID;

		Core::Animation::BaseAnimationInfo toBaseAnimationInfo() const
		{
			Core::Animation::BaseAnimationInfo res = Core::Animation::BaseAnimationInfo();
			res.duration						   = duration;

			return res;
		}
	};

	class Translation : public UI::Core::Animation::BaseAnimation
	{
	  private:
		inline static const float TRANSLATION_THRESHOLD = 1e-4f;

	  public:
		inline static const Util::Hashing::Hash ANIMATION_ID = Util::Hashing::hash( "INTERNAL_TRANSLATION" );

	  public:
		Translation( const TranslationInfo & p_info );
		virtual ~Translation() = default;

	  protected:
		virtual void _enter() override;
		virtual void _internalUpdate( float p_deltaTime ) override;

		const TranslationInfo _info;
	};
} // namespace VTX::UI::Internal::Animation
#endif
