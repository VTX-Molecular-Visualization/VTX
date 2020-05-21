#ifndef __VTX_COMPONENT_RENDER__
#define __VTX_COMPONENT_RENDER__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_component.hpp"
#include "localization/language.hpp"

namespace VTX
{
	namespace UI
	{
		class Render : public BaseComponent
		{
		  public:
			virtual const std::string & getName() const override { return ID::UI::RENDER; }

		  protected:
			virtual void _draw() override;

		  private:
		};
	} // namespace UI
} // namespace VTX
#endif
