#ifndef __VTX_COMPONENT_SCENE__
#define __VTX_COMPONENT_SCENE__

#include "../lib/imgui/imgui.h"
#include "../localization/language.hpp"
#include "base_component.hpp"

namespace VTX
{
	namespace UI
	{
		class ComponentScene : public BaseComponent
		{
		  public:
			ComponentScene( bool * const );

		  protected:
			virtual void _draw() override;

		  private:
		};
	} // namespace UI
} // namespace VTX
#endif
