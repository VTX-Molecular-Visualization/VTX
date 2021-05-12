#ifndef __VTX_UTIL_UI__
#define __VTX_UTIL_UI__

#ifdef _MSC_VER
#pragma once
#endif

#include "color/rgb.hpp"
#include <QString>
#include <QStyle>
#include <QVariant>
#include <QWidget>

namespace VTX::Util::UI
{
	static void appendColorHtmlTag( QString & p_txt, const VTX::Color::Rgb & p_color )
	{
		const QString colorString = QString::fromStdString( p_color.toHexaString() );
		const QString tag		  = QString( "<font color=" + colorString + ">" );

		p_txt.append( tag );
	}
	static void appendEndColorHtmlTag( QString & p_txt ) { p_txt.append( "</font>" ); }

	static void setDynamicProperty( QWidget * const p_widget, const char * const p_property, const QVariant & p_value )
	{
		p_widget->setProperty( p_property, p_value );

		// Need manual refresh when property changed when widget is displayed
		p_widget->style()->unpolish( p_widget );
		p_widget->style()->polish( p_widget );
	}

	static Qt::CheckState getCheckState( const bool p_boolValue )
	{
		return p_boolValue ? Qt::CheckState::Checked : Qt::CheckState::Unchecked;
	}
} // namespace VTX::Util::UI

#endif
