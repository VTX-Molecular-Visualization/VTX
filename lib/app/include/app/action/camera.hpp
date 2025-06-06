#ifndef __VTX_APP_ACTION_CAMERA__
#define __VTX_APP_ACTION_CAMERA__

#include <app/core/action/base_action.hpp>
#include <util/math/aabb.hpp>

namespace VTX::App::Action::Camera
{

	class SetCameraProjectionOrthographic final : public Core::Action::BaseAction
	{
	  public:
		SetCameraProjectionOrthographic() {}
		void execute() override;
	};

	class SetCameraProjectionPerspective final : public Core::Action::BaseAction
	{
	  public:
		SetCameraProjectionPerspective() {}
		void execute() override;
	};

	class ToggleCameraProjection final : public Core::Action::BaseAction
	{
	  public:
		ToggleCameraProjection() {}
		void execute() override;
	};

	class Reset final : public Core::Action::BaseAction
	{
	  public:
		Reset() {}
		void execute() override;
	};

	/**
	 * @brief Move the camera in a straight line from the current position to the one in arguments
	 */
	class MoveCamera final : public Core::Action::BaseAction
	{
	  public:
		inline MoveCamera(
			float p_positionX,
			float p_positionY,
			float p_positionZ,
			float p_rotationW,
			float p_rotationX,
			float p_rotationY,
			float p_rotationZ,
			float p_duration
		) :
			_positionX( std::move( p_positionX ) ), _positionY( std::move( p_positionY ) ),
			_positionZ( std::move( p_positionZ ) ), _rotationW( std::move( p_rotationW ) ),
			_rotationX( std::move( p_rotationX ) ), _rotationY( std::move( p_rotationY ) ),
			_rotationZ( std::move( p_rotationZ ) ), _duration( std::move( p_duration ) )

		{
		}
		void execute() override;

	  private:
		float _positionX = 0.f;
		float _positionY = 0.f;
		float _positionZ = 0.f;
		float _rotationW = 0.f;
		float _rotationX = 0.f;
		float _rotationY = 0.f;
		float _rotationZ = 0.f;
		float _duration	 = 0.f;
	};

	/**
	 * @brief Set the camera position to the target location, effectively "teleporting" the point of view
	 */
	class SetCameraPosition final : public Core::Action::BaseAction
	{
	  public:
		inline SetCameraPosition( float p_x, float p_y, float p_z ) :
			_x( std::move( p_x ) ), _y( std::move( p_y ) ), _z( std::move( p_z ) )
		{
		}
		void execute() override;

	  private:
		float _x = 0.f;
		float _y = 0.f;
		float _z = 0.f;
	};

	class Orient final : public App::Core::Action::BaseAction
	{
	  public:
		inline Orient( const float & p_x, const float & p_y, const float & p_z ) : _target( Vec3f { p_x, p_y, p_z } ) {}
		inline Orient( const VTX::Util::Math::AABB & p_target ) : _target( p_target ) {}
		Orient();

		void execute() override;

	  private:
		const Util::Math::AABB _target;
	};
} // namespace VTX::App::Action::Camera
#endif
