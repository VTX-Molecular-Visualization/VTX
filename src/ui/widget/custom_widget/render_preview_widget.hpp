#ifndef __VTX_UI_WIDGET_CUSTOM_RENDER_PREVIEW_WIDGET__
#define __VTX_UI_WIDGET_CUSTOM_RENDER_PREVIEW_WIDGET__

#include "io/struct/image_export.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include <QLabel>
#include <QWidget>
#include <QPixmap>

namespace VTX::UI::Widget::CustomWidget
{
	class RenderPreviewWidget : public BaseManualWidget<QLabel>
	{
		VTX_WIDGET
		Q_OBJECT

	  public:
		void localize() override;
		void takeSnapshot( const IO::Struct::ImageExport & p_exportData);

	  protected:
		RenderPreviewWidget( QWidget * p_parent = nullptr );

		QSize sizeHint() const override;

		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

	  private:
		QPixmap								   _previewPixmpap;
	};
} // namespace VTX::UI::Widget::CustomWidget

#endif
