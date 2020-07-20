#ifndef __VTX_BASE_VIEW_UI_WINDOWS__
#define __VTX_BASE_VIEW_UI_WINDOWS__

#ifdef _MSC_VER
#pragma once
#endif

#include "ui/window/base_component_window.hpp"
#include "view/base_view_ui.hpp"

namespace VTX
{
	namespace View
	{
		template<typename T, typename = std::enable_if<std::is_base_of<Model::BaseModel, T>::value>>
		class BaseViewUIWindows : public BaseViewUI<T>, public VTX::UI::Window::BaseComponentWindow
		{
		  public:
			explicit BaseViewUIWindows( T * const p_model ) : BaseViewUI( p_model ) {}
			virtual ~BaseViewUIWindows() {}

			virtual void init() override
			{
				BaseViewUI::init();
				BaseComponentWindow::init();
			}
			virtual void clean() override
			{
				BaseViewUI::clean();
				BaseComponentWindow::clean();
			}

		  protected:
			virtual void _draw() override { BaseComponentWindow::_draw(); }
		};
	} // namespace View
} // namespace VTX
#endif
