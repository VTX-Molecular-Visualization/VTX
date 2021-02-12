#ifndef __VTX_ACTION_TRANSFORMABLE__
#define __VTX_ACTION_TRANSFORMABLE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "base_action_undonable.hpp"
#include "generic/base_transformable.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		namespace Transformable
		{
			class Rotate : public BaseAction
			{
			  public:
				explicit Rotate( Generic::BaseTransformable & p_transformable,
								 const float				  p_angle,
								 const Vec3f &				  p_axis ) :
					_transformable( p_transformable ),
					_angle( p_angle ), _axis( p_axis )
				{
				}

				virtual void execute() override
				{
					_transformable.rotate( _angle, _axis );
					VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
				}

			  private:
				Generic::BaseTransformable & _transformable;
				const float					 _angle;
				const Vec3f					 _axis;
			};

			template<typename T, typename = std::enable_if<std::is_base_of<Generic::BaseTransformable, T>::value>>
			class SetScale : public BaseActionUndonable
			{
			  public:
				explicit SetScale( T & p_transformable, const float p_scale ) :
					_transformable( p_transformable ), _scale( p_scale ),
					_scaleOld( p_transformable.getTransform().getScale() )
				{
				}

				virtual void execute() override
				{
					_transformable.setScale( _scale );
					VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
				}

				virtual void undo() override
				{
					_transformable.setScale( _scaleOld );
					VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
				}

			  private:
				Generic::BaseTransformable & _transformable;
				const float					 _scale;
				const Mat4f					 _scaleOld;
			};

			class SetTranslation : public BaseActionUndonable
			{
			  public:
				explicit SetTranslation( Generic::BaseTransformable & p_transformable, const Vec3f & p_translation ) :
					_transformable( p_transformable ), _translation( p_translation ),
					_translationOld( p_transformable.getTransform().getTranslation() )
				{
				}

				virtual void execute() override
				{
					_transformable.setTranslation( _translation );
					VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
				}

				virtual void undo() override
				{
					_transformable.setTranslation( _translationOld );
					VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
				}

			  private:
				Generic::BaseTransformable & _transformable;
				const Vec3f					 _translation;
				const Mat4f					 _translationOld;
			};
		} // namespace Transformable
	}	  // namespace Action
} // namespace VTX
#endif
