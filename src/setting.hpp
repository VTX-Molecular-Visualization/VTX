#ifndef __VTX_SETTINGS__
#define __VTX_SETTINGS__

#ifdef _MSC_VER
#pragma once
#endif

#include "color/rgb.hpp"
#include "renderer/base_renderer.hpp"
#include "renderer/gl/pass/shading.hpp"
#include "ui/user_interface.hpp"

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
			extern bool							 useSSAO;
			extern float						 aoRadius;
			extern int							 aoIntensity;
			extern float						 aoFactor;
			extern int							 aoBlurSize;
			extern int							 aoBlurSharpness;
			extern bool							 useContourLines;
			extern bool							 useAA;

			// Camera.
			extern float camNear;
			extern float camFar;
			extern float camFov;

			extern Color::Rgb contourLineColor;
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
