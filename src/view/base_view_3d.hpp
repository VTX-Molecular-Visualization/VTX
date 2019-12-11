#ifndef __VTX_BASE_VIEW_3D__
#define __VTX_BASE_VIEW_3D__

#ifdef _MSC_VER
#pragma once
#endif

#include "../model/base_model.hpp"
#include "../object3d/camera.hpp"
#include "../shader/glsl_program_manager.hpp"
#include "base_view.hpp"

namespace VTX
{
	namespace View
	{
		template<typename T, typename = std::enable_if<std::is_base_of<Model::BaseModel, T>::value>>
		class BaseView3D : public BaseView<T>
		{
		  public:
			virtual void draw()										   = 0;
			virtual void setupShaders( Shader::GLSLProgramManager & )  = 0;
			virtual void useShaders( Shader::GLSLProgramManager & )	   = 0;
			virtual void setCameraUniforms( const Object3D::Camera & ) = 0;
			virtual void bind() const								   = 0;
			virtual void unbind() const								   = 0;

		  private:
		};
	} // namespace View
} // namespace VTX
#endif
