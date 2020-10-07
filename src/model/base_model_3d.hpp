#ifndef __VTX_BASE_MODEL_3D__
#define __VTX_BASE_MODEL_3D__

#ifdef _MSC_VER
#pragma once
#endif

#include "generic/base_renderable.hpp"
#include "generic/base_transformable.hpp"
#include "generic/base_visible.hpp"
#include "math/aabb.hpp"
#include "model/base_model.hpp"
#include <QOpenGLFunctions_4_5_Core>

namespace VTX
{
	namespace Model
	{
		template<typename T>
		class BaseModel3D : public BaseModel<T>, public Generic::BaseTransformable, public Generic::BaseRenderable, public Generic::BaseVisible, public QOpenGLFunctions_4_5_Core
		{
		  public:
			BaseModel3D() { initializeOpenGLFunctions(); }
			virtual ~BaseModel3D() = default;

			inline const Math::AABB & getAABB() const { return _aabb; }

			virtual void render()
			{
				bindBuffers();
				_notifyViews( Event::VTX_EVENT_MODEL::RENDER );
				unbindBuffers();
			}

			virtual void bindBuffers()	 = 0;
			virtual void unbindBuffers() = 0;

		  protected:
			Math::AABB _aabb;
		};
	} // namespace Model
} // namespace VTX
#endif
