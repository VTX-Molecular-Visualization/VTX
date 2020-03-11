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
			enum class THEME : int
			{
				LIGHT,
				DARK,
				CLASSIC
			};

			enum class SYMBOL_DISPLAY_MODE : int
			{
				SHORT,
				LONG
			};

			extern THEME			   theme;
			extern SYMBOL_DISPLAY_MODE symbolDisplayMode;
		}; // namespace UI

		namespace Rendering
		{
			extern bool							 isActive;
			extern View::MOLECULE_REPRESENTATION representation;
			extern View::MOLECULE_COLOR_MODE	 colorMode;
			extern Renderer::MODE				 mode;
			extern Renderer::SHADING			 shading;
			extern bool							 useVSync;
			extern bool							 useAA;
			extern bool							 useSSAO;
			extern float						 aoRadius;
			extern int							 aoIntensity;
			extern int							 aoBlurSize;
		}; // namespace Rendering

		namespace Controller
		{
			extern bool	 yAxisInverted;
			extern Vec3f autoRotateSpeed;
			extern float translationSpeed;
			extern float translationFactorSpeed;
			extern float rotationSpeed;
		} // namespace Controller
	}	  // namespace Setting
} // namespace VTX
#endif
