#ifndef __VTX_MODEL_VIEWPOINT__
#define __VTX_MODEL_VIEWPOINT__

#include "base_model.hpp"
#include "define.hpp"
#include "id.hpp"
#include "setting.hpp"

namespace VTX
{
	namespace Model
	{
		class Path;
		class Viewpoint : public BaseModel
		{
			VTX_MODEL
			friend Path; // To allow constructor in path interpolation.

		  public:
			inline Path * const		   getPathPtr() const { return _path; }
			inline const std::string & getName() const { return _name; }
			inline void				   setName( const std::string & p_name ) { _name = p_name; }
			inline const float		   getDuration() const { return _duration; }
			inline void				   setDuration( const float p_duration ) { _duration = p_duration; }
			inline const ID::VTX_ID	   getController() const { return _controller; }
			inline void				   setController( const ID::VTX_ID p_controller ) { _controller = p_controller; }
			inline const Vec3f &	   getPosition() const { return _position; }
			inline void				   setPosition( const Vec3f & p_position ) { _position = p_position; }
			inline const Quatf &	   getRotation() const { return _rotation; }
			inline void				   setRotation( const Quatf & p_rotation ) { _rotation = p_rotation; }
			inline const Vec3f &	   getTarget() const { return _data._target._target; }
			inline void				   setTarget( const Vec3f & p_target ) { _data._target._target = p_target; }
			inline const float		   getDistance() const { return _data._target._distance; }
			inline void				   setDistance( const float p_distance ) { _data._target._distance = p_distance; }
			/*
			inline const std::vector<std::string> & getActions() const { return _actions; }
			inline void addAction( const std::string & p_action ) { _actions.emplace_back( p_action ); }
			inline void removeAction( const std::vector<std::string>::const_iterator & p_action )
			{
				_actions.erase( p_action );
			}
			*/
		  private:
			std::string	 _name = "";
			Path * const _path;
			float		 _duration = 0.f;
			Quatf		 _rotation = Quatf();
			Vec3f		 _position = VEC3F_ZERO;

			union
			{
				struct
				{
					Vec3f _target;
					float _distance;
				} _target;

			} _data;

			ID::VTX_ID _controller = Setting::CONTROLLER_MODE_DEFAULT;

			// std::vector<std::string> _actions = std::vector<std::string>();

			Viewpoint( Path * const p_path ) : BaseModel( ID::Model::MODEL_VIEWPOINT ), _path( p_path ) {}

		}; // namespace Camera
	}	   // namespace Model
} // namespace VTX
#endif
