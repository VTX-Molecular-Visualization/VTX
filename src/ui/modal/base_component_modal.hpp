#ifndef __VTX_BASE_COMPONENT_MODAL__
#define __VTX_BASE_COMPONENT_MODAL__

#ifdef _MSC_VER
#pragma once
#endif

#include "localization/language.hpp"
#include "style.hpp"
#include "ui/base_component.hpp"

namespace VTX
{
	namespace UI
	{
		namespace Modal
		{
			class BaseComponentModal : public BaseComponent
			{
			  public:
				virtual const char * getTitle() const = 0;

			  protected:
				virtual bool _drawHeader() override;
				virtual void _drawFooter() override;
			};
		} // namespace Modal
	}	  // namespace UI
} // namespace VTX
#endif
