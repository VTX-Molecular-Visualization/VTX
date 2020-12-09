#ifndef __VTX_UI_WIDGET_REPRESENTABLE_FIELD_WIDGET__
#define __VTX_UI_WIDGET_REPRESENTABLE_FIELD_WIDGET__

#ifdef _MSC_VER
#pragma once
#endif

#include "generic/base_representable.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include <QBoxLayout>
#include <QDragEnterEvent>
#include <QLabel>
#include <QMouseEvent>
#include <QPixmap>
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
				class RepresentableFieldWidget : public BaseManualWidget<QLabel>
				{
					Q_OBJECT
					VTX_MANUAL_WIDGET_DECLARATION

				  public:
					~RepresentableFieldWidget() {};
					void localize() override;

					inline Generic::BaseRepresentable * const		getRepresentable() { return _representable; };
					inline const Generic::BaseRepresentable * const getRepresentable() const { return _representable; };
					inline void										setValue( Generic::BaseRepresentable * const p_representable ) { _representable = p_representable; };

				  signals:
					void dataChanged();

				  protected:
					RepresentableFieldWidget( QWidget * p_parent ) : BaseManualWidget( p_parent ) {};
					void _setupUi( const QString & p_name ) override;
					void _setupSlots() override;

					void dragEnterEvent( QDragEnterEvent * event ) override;
					void dropEvent( QDropEvent * event ) override;

				  private:
					Generic::BaseRepresentable * _representable = nullptr;
				};
			} // namespace CustomWidget
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX
#endif
