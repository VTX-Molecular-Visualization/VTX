#ifndef __VTX_COMPONENT_SCENE__
#define __VTX_COMPONENT_SCENE__

#ifdef _MSC_VER
#pragma once

#endif
#include "base_component.hpp"

namespace VTX
{
	namespace UI
	{
		class Scene : public BaseComponent
		{
		  public:
			virtual const std::string & getName() const override { return ID::UI::SCENE; }

		  protected:
			virtual void _draw() override;

		  private:
		};
	} // namespace UI
} // namespace VTX
#endif
