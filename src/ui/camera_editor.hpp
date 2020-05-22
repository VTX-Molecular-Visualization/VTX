#ifndef __VTX_COMPONENT_CAMERA_EDITOR__
#define __VTX_COMPONENT_CAMERA_EDITOR__

#ifdef _MSC_VER
#pragma once

#endif
#include "base_component_window.hpp"

namespace VTX
{
	namespace UI
	{
		class CameraEditor : public BaseComponentWindow
		{
		  public:
			virtual const std::string & getName() const override { return ID::UI::CAMERA_EDITOR; }
			virtual const char *		getTitle() const override { return LOCALE( "CameraEditor.CameraEditor" ); }
		};
	} // namespace UI
} // namespace VTX
#endif
