#ifndef __VTX_APP_COMPONENT_RENDER_VIEWPOINT__
#define __VTX_APP_COMPONENT_RENDER_VIEWPOINT__

#include "app/core/ecs/base_component.hpp"
#include <string>
#include <util/constants.hpp>
#include <util/types.hpp>

namespace VTX::App::Component::Render
{
	class Viewpoint : public Core::ECS::BaseComponent
	{
	  public:
		Viewpoint();

		// inline const VTX::App::Old::VTX_ID getController() const { return _controller; }
		// inline void			 setController( const VTX::App::Old::VTX_ID p_controller ) { _controller = p_controller;
		// }
		inline const Vec3f & getPosition() const { return _position; }
		void				 setPosition( const Vec3f & p_position );
		inline const Quatf & getRotation() const { return _rotation; }
		void				 setRotation( const Quatf & p_rotation );
		// inline const Vec3f & getTarget() const { return _data._target._target; }
		// inline void			 setTarget( const Vec3f & p_target ) { _data._target._target = p_target; }
		// inline const float	 getDistance() const { return _data._target._distance; }
		// inline void			 setDistance( const float p_distance ) { _data._target._distance = p_distance; }

		/*
		inline const std::vector<std::string> & getActions() const { return _actions; }
		inline void addAction( const std::string & p_action ) { _actions.emplace_back( p_action ); }
		inline void removeAction( const std::vector<std::string>::const_iterator & p_action )
		{
			_actions.erase( p_action );
		}
		*/

	  private:
		Quatf _rotation = QUATF_ID;
		Vec3f _position = VEC3F_ZERO;

		// union
		//{
		//	struct
		//	{
		//		Vec3f _target;
		//		float _distance;
		//	} _target;

		//} _data;

		// VTX::App::Old::VTX_ID _controller = Application::Setting::CONTROLLER_MODE_DEFAULT;
	};
} // namespace VTX::App::Component::Render
#endif
