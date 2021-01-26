#include "base_transformable.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Generic
	{
		void BaseTransformable::rotate( const float p_angle, const Vec3f & p_axis )
		{
			_transform.rotate( p_angle, p_axis );
			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

		void BaseTransformable::setScale( const float & p_scale )
		{
			_transform.setScale( p_scale );
			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

		void BaseTransformable::setScale( const Mat4f & p_scale )
		{
			_transform.setScale( p_scale );
			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

		void BaseTransformable::setTranslation( const Vec3f & p_translation )
		{
			_transform.setTranslation( p_translation );
			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

		void BaseTransformable::setTranslation( const Mat4f & p_translation )
		{
			_transform.setTranslation( p_translation );
			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

		void BaseTransformable::applyTransform( const Math::Transform & p_transform )
		{
			_transform = p_transform;
			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}
	} // namespace Generic
} // namespace VTX
