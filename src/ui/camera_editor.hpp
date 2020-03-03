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
		class CameraEditor : public BaseComponent
		{
		  public:
			virtual const std::string & getName() const override { return ID::UI::CAMERA_EDITOR; }

		  protected:
			virtual void _draw() override;

		  private:
		};
	} // namespace UI
} // namespace VTX
#endif
