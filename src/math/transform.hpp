#ifndef __VTX__TRANSFORM__
#define __VTX__TRANSFORM__

#ifdef _MSC_VER
#pragma once
#endif

#include "define.hpp"
#include "util/math.hpp"

namespace VTX
{
	namespace Math
	{
		class Transform
		{
		  public:
			Transform()	 = default;
			~Transform() = default;

			inline const Mat4f & get() const { return _transform; }
			inline const Mat4f & getTranslation() const { return _translation; };
			inline const Mat4f & getRotation() const { return _rotation; };
			inline const Mat4f & getScale() const { return _scale; };

			inline Vec3f getTranslationVector() const
			{
				return Vec3f( _translation[ 3 ][ 0 ], _translation[ 3 ][ 1 ], _translation[ 3 ][ 2 ] );
			};

			inline Vec3f getEulerAngles() const
			{
				if ( std::isnan( _internalEulerCache.x ) )
					_internalEulerCache = Util::Math::rotationMatrixToEuler( _rotation );
				return _internalEulerCache;
			};

			inline Vec3f getScaleVector() const
			{
				return Vec3f( _scale[ 0 ][ 0 ], _scale[ 1 ][ 1 ], _scale[ 2 ][ 2 ] );
			}

			inline void reset()
			{
				_transform	 = MAT4F_ID;
				_translation = MAT4F_ID;
				_rotation	 = MAT4F_ID;
				_scale		 = MAT4F_ID;

				_internalEulerCache = VEC3F_ZERO;
			}

			inline void translate( const Vec3f & p_vec )
			{
				_translation = Util::Math::translate( _translation, p_vec );
				update();
			}

			inline void setTranslation( const float p_x, const float p_y, const float p_z )
			{
				_translation[ 3 ][ 0 ] = p_x;
				_translation[ 3 ][ 1 ] = p_y;
				_translation[ 3 ][ 2 ] = p_z;
				update();
			}

			inline void setTranslation( const Vec3f & p_vec )
			{
				setTranslation( p_vec.x, p_vec.y, p_vec.z );
				update();
			}

			inline void setTranslation( const Mat4f & p_mat )
			{
				_translation = p_mat;
				update();
			}

			inline void rotate( const float p_angle, const Vec3f & p_axis )
			{
				_rotation			  = Util::Math::rotate( _rotation, p_angle, p_axis );
				_internalEulerCache.x = std::nanf( "" );
				update();
			}

			inline void setRotation( const float p_pitch, const float p_yaw, const float p_roll )
			{
				_rotation = Util::Math::getRotation(
					Util::Math::radians( p_pitch ), Util::Math::radians( p_yaw ), Util::Math::radians( p_roll ) );
				_internalEulerCache = Vec3f( p_pitch, p_yaw, p_roll );

				update();
			}

			inline void setRotation( const Vec3f & p_vec ) { setRotation( p_vec.x, p_vec.y, p_vec.z ); }

			inline void setRotation( const Mat4f & p_mat )
			{
				_rotation			  = p_mat;
				_internalEulerCache.x = std::nanf( "" );
				update();
			}

			inline void scale( const Vec3f & p_vec )
			{
				_scale = Util::Math::scale( _scale, p_vec );
				update();
			}

			inline void setScale( const Vec3f & p_scale )
			{
				_scale[ 0 ][ 0 ] = p_scale.x;
				_scale[ 1 ][ 1 ] = p_scale.y;
				_scale[ 2 ][ 2 ] = p_scale.z;
				update();
			}

			inline void setScale( const float p_scale )
			{
				_scale[ 0 ][ 0 ] = p_scale;
				_scale[ 1 ][ 1 ] = p_scale;
				_scale[ 2 ][ 2 ] = p_scale;
				update();
			}

			inline void setScale( const Mat4f & p_mat )
			{
				_scale = p_mat;
				update();
			}

		  private:
			inline void update() { _transform = _translation * _rotation * _scale; }

			Mat4f _transform   = MAT4F_ID;
			Mat4f _translation = MAT4F_ID;
			Mat4f _rotation	   = MAT4F_ID;
			Mat4f _scale	   = MAT4F_ID;

			mutable Vec3f _internalEulerCache = VEC3F_ZERO;
		};
	} // namespace Math
} // namespace VTX

#endif
