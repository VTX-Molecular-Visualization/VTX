#ifndef __VTX_UTIL_UI__
#define __VTX_UTIL_UI__

#ifdef _MSC_VER
#pragma once
#endif

#include "color/rgb.hpp"
#include <QString>

namespace VTX
{
	namespace Util
	{
		namespace UI
		{
			static void appendColorHtmlTag( QString & p_txt, const VTX::Color::Rgb & p_color )
			{
				const QString colorString = QString::fromStdString( p_color.toHexaString() );
				const QString tag		  = QString( "<font color=" + colorString + ">" );

				p_txt.append( tag );
			}
			static void appendEndColorHtmlTag( QString & p_txt ) { p_txt.append( "</font>" ); }
		} // namespace UI
	}	  // namespace Util
} // namespace VTX

#endif
