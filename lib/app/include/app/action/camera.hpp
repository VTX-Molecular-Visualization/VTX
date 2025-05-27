#ifndef __VTX_APP_ACTION_CAMERA__
#define __VTX_APP_ACTION_CAMERA__

#include <app/core/action/base_action.hpp>

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
	class MoveCameraPosition final : public Core::Action::BaseAction
	{
	  public:
		inline MoveCameraPosition( float p_x, float p_y, float p_z, float p_tmp = 1.f ) :
			_x( std::move( p_x ) ), _y( std::move( p_y ) ), _z( std::move( p_z ) ), _tmp( std::move( p_tmp ) )
		{
		}
		void execute() override;

	  private:
		float _x   = 0.f;
		float _y   = 0.f;
		float _z   = 0.f;
		float _tmp = 1.f; // Idk yet if this should be a speed, a movement per frame, an animation time ...
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
} // namespace VTX::App::Action::Camera
#endif
