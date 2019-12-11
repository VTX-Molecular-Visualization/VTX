#ifndef __VTX_SETTINGS__
#define __VTX_SETTINGS__

#ifdef _MSC_VER
#pragma once
#endif

#include "renderer/base_renderer.hpp"
#include "ui/user_interface.hpp"
#include "view/base_view_3d_molecule.hpp"

namespace VTX
{
	namespace Setting
	{
		namespace UI
		{
			enum class SYMBOL_DISPLAY_MODE : int
			{
				SHORT,
				LONG
			};

			extern VTX::UI::THEME	   theme;
			extern SYMBOL_DISPLAY_MODE symbolDisplayMode;
		}; // namespace UI

		namespace Rendering
		{
			extern View::MOLECULE_REPRESENTATION reprensation;
			extern Renderer::MODE				 mode;
			extern Renderer::SHADING			 shading;
			extern bool							 useAA;
			extern bool							 useSSAO;
		}; // namespace Rendering
	}	   // namespace Setting
} // namespace VTX
#endif
