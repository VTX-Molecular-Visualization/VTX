#ifndef __VTX_BASE_TRANSFORMABLE__
#define __VTX_BASE_TRANSFORMABLE__

#ifdef _MSC_VER
#pragma once
#endif

#include "math/transform.hpp"
//#include "vtx_app.hpp"

namespace VTX
{
	namespace Generic
	{
		class BaseTransformable
		{
		  public:
			inline const Math::Transform & getTransform() const { return _transform; };

			inline void rotate( const float p_angle, const Vec3f & p_axis )
			{
				_transform.rotate( p_angle, p_axis );
				// VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
			}

			inline void setScale( const float & p_scale )
			{
				_transform.setScale( p_scale );
				// VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
			}
			inline void setScale( const Mat4f & p_scale )
			{
				_transform.setScale( p_scale );
				// VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
			}

			inline void setTranslation( const Vec3f & p_translation )
			{
				_transform.setTranslation( p_translation );
				// VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
			}
			inline void setTranslation( const Mat4f & p_translation )
			{
				_transform.setTranslation( p_translation );
				// VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
			}

			inline void applyTransform( const Math::Transform & p_transform )
			{
				_transform = p_transform;
				// VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
			};

		  protected:
			Math::Transform _transform;
		};
	} // namespace Generic
} // namespace VTX
#endif
