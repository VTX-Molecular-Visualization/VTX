#ifndef __VTX_UI_QT_CONTROLLER_ORIENT_ANIMATION_FREEFLY__
#define __VTX_UI_QT_CONTROLLER_ORIENT_ANIMATION_FREEFLY__

#include <QWidget>
#include <app/component/render/camera.hpp>
#include <util/math/aabb.hpp>
#include <util/types.hpp>

namespace VTX::UI::QT::Controller
{
	class OrientAnimationFreefly
	{
	  public:
		explicit OrientAnimationFreefly(
			VTX::App::Component::Render::Camera & p_camera,
			const Util::Math::AABB &			  p_aabb
		);
		explicit OrientAnimationFreefly(
			VTX::App::Component::Render::Camera & p_camera,
			const Vec3f &						  p_position,
			const Quatf &						  p_orientation
		);

		~OrientAnimationFreefly() = default;

		inline const Vec3f & getOrientStartingPosition() const { return _orientStartingPosition; }
		inline const Vec3f & getOrientTargetPosition() const { return _orientTargetPosition; }

		void update( const float & p_deltaTime );

	  private:
		void _startOrient( const Util::Math::AABB & p_aabb )
		{
			_orientTime = 0.f;
			_computeOrientPositions( p_aabb );
		}
		void _startOrient( const Vec3f & p_position, const Quatf & p_orientation )
		{
			_orientTime = 0.f;
			_computeOrientPositions( p_position, p_orientation );
		}

		void _finish();

		const float ORIENT_DURATION	   = 0.5f;
		const float ORIENT_THRESHOLD   = 1e-4f;
		const float ORIENT_ZOOM_FACTOR = 0.666f;

		App::Component::Render::Camera & _camera;

		float _orientTime			  = 0.f;
		Vec3f _orientStartingPosition = VEC3F_ZERO;
		Vec3f _orientTargetPosition	  = VEC3F_ZERO;
		Quatf _orientStartingRotation = QUATF_ID;
		Quatf _orientTargetRotation	  = QUATF_ID;

		virtual void _computeOrientPositions( const Util::Math::AABB & p_aabb )						  = 0;
		virtual void _computeOrientPositions( const Vec3f & p_position, const Quatf & p_orientation ) = 0;
		virtual void _updateOrient( const float & )													  = 0;
	};
} // namespace VTX::UI::QT::Controller
#endif
