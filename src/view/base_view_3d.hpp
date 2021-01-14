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
#include <type_traits>

namespace VTX::View
{
	template<typename T,
			 typename = std::enable_if<std::is_base_of<Model::BaseModel3D<Buffer::BaseBufferOpenGL>, T>::value>>
	class BaseView3D : public BaseView<T>, public Generic::BaseRenderable
	{
	  public:
	  protected:
		Renderer::GL::Program * _program = nullptr;

		explicit BaseView3D( T * const p_model ) : BaseView( p_model ) {}
		virtual ~BaseView3D() = default;
		inline OpenGLFunctions * const _gl() { return _model->getBuffer()->gl(); }
	};
} // namespace VTX::View
#endif
