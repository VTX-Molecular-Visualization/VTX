#ifndef __VTX__TYPE__
#define __VTX__TYPE__

#ifdef _MSC_VER
#pragma once
#endif

#include "../model/base_model.hpp"
#include "../ui/base_component.hpp"
#include "../vtx_app.hpp"

namespace VTX
{
	namespace Util
	{
		namespace Type
		{
			template<typename T, typename = std::enable_if<std::is_base_of<Model::BaseModel, T>::value>>
			static std::shared_ptr<View::BaseView<Model::BaseModel>> componentToView( UI::COMPONENT_NAME p_type )
			{
				std::shared_ptr<UI::BaseComponent> component = VTXApp::get().getUIComponentByName( p_type );
				std::shared_ptr<View::BaseView<T>> view = std::dynamic_pointer_cast<View::BaseView<T>>( component );

				return std::reinterpret_pointer_cast<View::BaseView<Model::BaseModel>>( view );
			}

		} // namespace Type
	}	  // namespace Util
} // namespace VTX

#endif
