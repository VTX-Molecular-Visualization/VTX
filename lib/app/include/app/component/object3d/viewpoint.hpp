#ifndef __VTX_APP_COMPONENT_OBJECT3D_VIEWPOINT__
#define __VTX_APP_COMPONENT_OBJECT3D_VIEWPOINT__

#include "app/application/setting.hpp"
#include "app/component/video/_fwd.hpp"
#include "app/core/model/base_model.hpp"
#include "app/id.hpp"
#include <util/types.hpp>

namespace VTX::App::Component::Object3D
{
	class Viewpoint : public Core::Model::BaseModel
	{
		VTX_MODEL
		friend Video::Path; // To allow constructor in path interpolation.

	  public:
		inline Video::Path * const getPathPtr() const { return _path; }
		inline float			   getDuration() const { return _duration; }
		inline void				   setDuration( const float p_duration ) { _duration = p_duration; }

		inline const VTX::App::VTX_ID getController() const { return _controller; }
		inline void			 setController( const VTX::App::VTX_ID p_controller ) { _controller = p_controller; }
		inline const Vec3f & getPosition() const { return _position; }
		void				 setPosition( const Vec3f & p_position );
		inline const Quatf & getRotation() const { return _rotation; }
		void				 setRotation( const Quatf & p_rotation );
		inline const Vec3f & getTarget() const { return _data._target._target; }
		inline void			 setTarget( const Vec3f & p_target ) { _data._target._target = p_target; }
		inline const float	 getDistance() const { return _data._target._distance; }
		inline void			 setDistance( const float p_distance ) { _data._target._distance = p_distance; }
		/*
		inline const std::vector<std::string> & getActions() const { return _actions; }
		inline void addAction( const std::string & p_action ) { _actions.emplace_back( p_action ); }
		inline void removeAction( const std::vector<std::string>::const_iterator & p_action )
		{
			_actions.erase( p_action );
		}
		*/

		inline void setName( const std::string & p_name ) { _storedName = p_name; }

	  protected:
		Viewpoint( Component::Video::Path * const p_path );

	  private:
		Component::Video::Path * const _path;
		float						   _duration   = 0.f;
		Quatf						   _rotation   = Quatf();
		Vec3f						   _position   = VEC3F_ZERO;
		std::string					   _storedName = "";

		union
		{
			struct
			{
				Vec3f _target;
				float _distance;
			} _target;

		} _data;

		VTX::App::VTX_ID _controller = Application::Setting::CONTROLLER_MODE_DEFAULT;

		// std::vector<std::string> _actions = std::vector<std::string>();
	};
} // namespace VTX::App::Component::Object3D
#endif
