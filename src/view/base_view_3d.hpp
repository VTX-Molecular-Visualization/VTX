#ifndef __VTX_BASE_VIEW_3D__
#define __VTX_BASE_VIEW_3D__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_view.hpp"
#include "generic/base_renderable.hpp"
#include "model/base_model_3d.hpp"
#include "object3d/camera.hpp"
#include "renderer/gl/program_manager.hpp"
#include "util/math.hpp"
#include <QOpenGLFunctions_4_5_Compatibility>

namespace VTX
{
	namespace View
	{
		template<typename T, typename = std::enable_if<std::is_base_of<Model::BaseModel3D<T>, T>::value>>
		class BaseView3D : public BaseView<T>, public Generic::BaseRenderable, public QOpenGLFunctions_4_5_Compatibility
		{
		  public:
			explicit BaseView3D( T * const p_model ) : BaseView( p_model ) {}
			virtual ~BaseView3D() {}

		  protected:
			Renderer::GLSL::Program * _program = nullptr;

			virtual void notify( const Event::VTX_EVENT_MODEL & p_event ) override
			{
				if ( p_event == Event::VTX_EVENT_MODEL::RENDER )
				{
					render();
				}
			}
		};
	} // namespace View
} // namespace VTX
#endif
