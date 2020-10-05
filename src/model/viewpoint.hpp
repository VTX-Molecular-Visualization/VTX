#ifndef __VTX_MODEL_VIEWPOINT__
#define __VTX_MODEL_VIEWPOINT__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_model.hpp"
#include "define.hpp"
#include "id.hpp"

namespace VTX
{
	namespace Model
	{
		class Path;
		class Viewpoint : public BaseModel<Viewpoint>
		{
		  public:
			explicit Viewpoint( Path * const p_path ) : _path( p_path ) {}

			inline Path * const getPathPtr() const { return _path; }
			inline float		getDuration() const { return _duration; }
			inline void			setDuration( const float p_duration ) { _duration = p_duration; }

			inline const ID::VTX_ID getController() const { return _controller; }
			inline const void		setController( const ID::VTX_ID p_controller ) { _controller = p_controller; }

			inline const Vec3d & getPosition() const { return _position; }
			inline const void	 setPosition( const Vec3d & p_position ) { _position = p_position; }
			inline const Quatd & getRotation() const { return _rotation; }
			inline const void	 setRotation( const Quatd & p_rotation ) { _rotation = p_rotation; }

			inline const Vec3d & getTarget() const { return _target; }
			inline const void	 setTarget( const Vec3d & p_target ) { _target = p_target; }
			inline const double	 getDistance() const { return _distance; }
			inline const void	 setDistance( const double p_distance ) { _distance = p_distance; }

			inline const std::vector<std::string> & getActions() const { return _actions; }
			inline void								addAction( const std::string & p_action ) { _actions.emplace_back( p_action ); }
			inline void								removeAction( const std::vector<std::string>::const_iterator & p_action ) { _actions.erase( p_action ); }

			void setSelected( const bool );

		  private:
			Path * const _path;
			float		 _duration = 0.f;
			/************************************/
			Vec3d	   _position   = VEC3F_ZERO;
			Vec3d	   _target	   = VEC3F_ZERO;
			double	   _distance   = 0.f;
			ID::VTX_ID _controller = ID::Controller::FREEFLY;
			/*************************************/
			Quatd					 _rotation;
			std::vector<std::string> _actions = std::vector<std::string>();

		}; // namespace Camera
	}	   // namespace Model
} // namespace VTX
#endif
