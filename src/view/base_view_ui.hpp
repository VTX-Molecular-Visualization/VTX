#ifndef __VTX_BASE_VIEW_UI__
#define __VTX_BASE_VIEW_UI__

#ifdef _MSC_VER
#pragma once
#endif

#include "../model/base_model.hpp"
#include "../ui/base_component.hpp"
#include "base_view.hpp"

namespace VTX
{
	namespace View
	{
		template<typename T, typename = std::enable_if<std::is_base_of<Model::BaseModel, T>::value>>
		class BaseViewUI : public BaseView<T>
		{
		  public:
			explicit BaseViewUI( bool * const p_visible ) { _visible = p_visible; }
		};
	} // namespace View
} // namespace VTX
#endif
