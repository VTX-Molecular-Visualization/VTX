#ifndef __VTX_UI_WIDGET_BASE_MENUTOOLBLOCK_WIDGET__
#define __VTX_UI_WIDGET_BASE_MENUTOOLBLOCK_WIDGET__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_manual_widget.hpp"
#include "menu_toolbutton_widget.hpp"
#include <QGridLayout>
#include <QLabel>
#include <QWidget>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			class MenuToolBlockWidget : public BaseManualWidget<QWidget>
			{
				VTX_MANUAL_WIDGET_DECLARATION

			  public:
				void localize() override;

				void setTitle( const QString & p_title );
				void validate();

				void addButton( MenuToolButtonWidget & p_toolButton );
				void addButton( MenuToolButtonWidget & p_toolButton,
								const int			   p_row,
								const int			   p_column,
								const int			   p_rowspan   = 1,
								const int			   p_colspan   = 1,
								const Qt::Alignment	   p_alignment = Qt::Alignment::QFlags() );

			  protected:
				MenuToolBlockWidget( QWidget * p_parent ) : BaseManualWidget( p_parent ) {};
				virtual void _setupUi( const QString & p_name ) override;
				virtual void _setupSlots() override;

			  private:
				QGridLayout * _gridLayout = nullptr;
				QLabel *	  _title	  = nullptr;
			};
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
#endif
