#ifndef __VTX_MODEL_VIEWPOINT__
#define __VTX_MODEL_VIEWPOINT__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_model.hpp"
#include "define.hpp"
#include <glm/gtc/quaternion.hpp>

namespace VTX
{
	namespace Model
	{
		class Path;
		class Viewpoint : public BaseModel
		{
		  public:
			explicit Viewpoint( Path * const p_path ) :
				_path( p_path ), _position( VEC3F_ZERO ), _rotation(), _duration( 0.f )
			{
			}

			explicit Viewpoint( Path * const p_path, const Vec3f & p_position, const Quatf & p_rotation ) :
				_path( p_path ), _position( p_position ), _rotation( p_rotation ), _duration( 0.f )
			{
			}
			explicit Viewpoint( Path * const  p_path,
								const Vec3f & p_position,
								const Quatf & p_rotation,
								const float	  p_duration ) :
				_path( p_path ),
				_position( p_position ), _rotation( p_rotation ), _duration( p_duration )
			{
			}

			inline Path * const getPathPtr() const { return _path; }
			inline float		getDuration() const { return _duration; }
			inline void			setDuration( const float p_duration ) { _duration = p_duration; }

			inline const Vec3f & getPosition() const { return _position; }
			inline const void	 setPosition( const Vec3f & p_position ) { _position = p_position; }
			inline const Quatf & getRotation() const { return _rotation; }
			inline const void	 setRotation( const Quatf & p_rotation ) { _rotation = p_rotation; }

			virtual void setSelected( const bool ) override;

		  private:
			Path * const _path;
			Vec3f		 _position;
			Quatf		 _rotation;
			float		 _duration;

		}; // namespace Camera
	}	   // namespace Model
} // namespace VTX
#endif
