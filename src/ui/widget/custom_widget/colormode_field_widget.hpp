#ifndef __VTX_UI_WIDGET_COLORMODE_FIELD_WIDGET__
#define __VTX_UI_WIDGET_COLORMODE_FIELD_WIDGET__

#ifdef _MSC_VER
#pragma once
#endif

#include "color/rgb.hpp"
#include "generic/base_colorable.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include <QBoxLayout>
#include <QColorDialog>
#include <QComboBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QWidget>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace CustomWidget
			{
				class ColorModeFieldWidget : public BaseManualWidget<QWidget>
				{
					Q_OBJECT
					VTX_WIDGET

				  public:
					~ColorModeFieldWidget() {};
					void localize() override;

					const Generic::COLOR_MODE & getColorMode() const { return _colorMode; };
					void						setColorMode( const Generic::COLOR_MODE p_colorMode )
					{
						_colorMode = p_colorMode;
						_colorModeComboBox->setCurrentIndex( (int)_colorMode );

						emit dataChanged();
					}

				  signals:
					void dataChanged();

				  protected:
					ColorModeFieldWidget( QWidget * p_parent ) : BaseManualWidget( p_parent ) {};

					void _setupUi( const QString & p_name ) override;
					void _setupSlots() override;
					void _openColorDialog();

				  private:
					Generic::COLOR_MODE _colorMode;
					Color::Rgb			_color;

					QHBoxLayout * _layout;

					QPushButton *  _colorSetButton;
					QComboBox *	   _colorModeComboBox;
					QColorDialog * _colorDialog;
				};
			} // namespace CustomWidget
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX
#endif
