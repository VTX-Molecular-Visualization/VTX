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
		class ComponentScene : public BaseComponent
		{
		  public:
			explicit ComponentScene( bool * const );

			virtual COMPONENT_NAME getComponentName() const override { return COMPONENT_NAME::SCENE; }

		  protected:
			virtual void _addComponents() override;
			virtual void _draw() override;

		  private:
		};
	} // namespace UI
} // namespace VTX
#endif
