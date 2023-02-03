#ifndef __VTX_UI_QT_WIDGET_BASE_DIALOG__
#define __VTX_UI_QT_WIDGET_BASE_DIALOG__

#include "__new_archi/ui/qt/widget/base_manual_widget.hpp"
#include <QDialog>
#include <QWidget>

namespace VTX::UI::QT::Widget
{
	class BaseDialog : public BaseManualWidget<QDialog>
	{
	  public:
		BaseDialog( QWidget * p_parent = nullptr ) : BaseManualWidget( p_parent )
		{
			setAttribute( Qt::WA_DeleteOnClose );
		}
	};
} // namespace VTX::UI::QT::Widget

#endif
