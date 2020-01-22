#ifndef __VTX_COMPONENT_CAMERA_EDITOR__
#define __VTX_COMPONENT_CAMERA_EDITOR__

#ifdef _MSC_VER
#pragma once

#endif
#include "base_component.hpp"

namespace VTX
{
	namespace UI
	{
		class ComponentCameraEditor : public BaseComponent
		{
		  public:
			explicit ComponentCameraEditor( bool * const );

			virtual COMPONENT_NAME getComponentName() const override { return COMPONENT_NAME::CAMERA_EDITOR; }

		  protected:
			virtual void _addComponents() override;
			virtual void _draw() override;

		  private:
		};
	} // namespace UI
} // namespace VTX
#endif
