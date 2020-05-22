#ifndef __VTX_BASE_COMPONENT_WINDOW__
#define __VTX_BASE_COMPONENT_WINDOW__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_component.hpp"
#include "localization/language.hpp"
#include "style.hpp"

namespace VTX
{
	namespace UI
	{
		class BaseComponentWindow : public BaseComponent
		{
		  public:
			virtual const char * getTitle() const = 0;

		  protected:
			virtual bool _drawHeader();
			virtual void _drawContent();
			virtual void _drawFooter();

			virtual void _draw() override;

		  private:
		};
	} // namespace UI
} // namespace VTX
#endif
