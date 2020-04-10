#ifndef __VTX_ACTION_CHANGE_AUTO_ROTATE_SPEED__
#define __VTX_ACTION_CHANGE_AUTO_ROTATE_SPEED__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		class ChangeAutoRotateSpeed : public BaseAction
		{
		  public:
			explicit ChangeAutoRotateSpeed() {}
			explicit ChangeAutoRotateSpeed( const Vec3f & p_value ) : _value( p_value ) {}

			virtual void setParameters( const std::vector<std::string> & p_parameters ) override
			{
				if ( p_parameters.size() == 2 )
				{
					_value = Vec3f( std::stof( p_parameters[ 1 ] ),
									std::stof( p_parameters[ 1 ] ),
									std::stof( p_parameters[ 1 ] ) );
				}
				else
				{
					_value = Vec3f( std::stof( p_parameters[ 1 ] ),
									std::stof( p_parameters[ 2 ] ),
									std::stof( p_parameters[ 3 ] ) );
				}
			}

			virtual void execute() override
			{
				Setting::Controller::autoRotateSpeed
					= Util::Math::clamp( _value, AUTO_ROTATE_SPEED_MIN, AUTO_ROTATE_SPEED_MAX );
			};

			virtual void displayUsage() override { VTX_INFO( "f f f|f" ); }

		  private:
			Vec3f _value = Setting::Controller::autoRotateSpeed;
		};
	} // namespace Action
} // namespace VTX
#endif
