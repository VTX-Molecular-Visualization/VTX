#ifndef __VTX_COMPONENT_WINDOW__RENDER__
#define __VTX_COMPONENT_WINDOW__RENDER__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_component_window.hpp"

namespace VTX
{
	namespace UI
	{
		namespace Window
		{
			class Render : public BaseComponentWindow
			{
			  public:
				virtual const std::string & getName() const override { return ID::UI::RENDER; }
				virtual const char *		getTitle() const override { return LOCALE( "Render.Render" ); }

			  protected:
				virtual void _drawContent() override;

			  private:
				uint _width	 = 0;
				uint _height = 0;
			};
		} // namespace Window
	} // namespace UI
} // namespace VTX
#endif
