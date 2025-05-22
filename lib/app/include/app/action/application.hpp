#ifndef __VTX_APP_ACTION_APPLICATION__
#define __VTX_APP_ACTION_APPLICATION__

#include "app/core/action/base_action.hpp"
#include <util/types.hpp>
#include <vector>

namespace VTX::App::Action::Application
{
	class NewScene final : public Core::Action::BaseAction
	{
	  public:
		void execute() override;
	};

	class Quit final : public Core::Action::BaseAction
	{
	  public:
		Quit() {};
		void execute() override;
	};

	class Resize final : public Core::Action::BaseAction
	{
	  public:
		Resize( const size_t p_width, const size_t p_height ) : _width( p_width ), _height( p_height ) {}
		void execute() override;

	  private:
		const size_t _width;
		const size_t _height;
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

} // namespace VTX::App::Action::Application
#endif
