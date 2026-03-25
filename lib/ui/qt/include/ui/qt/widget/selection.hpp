#ifndef __VTX_UI_QT_WIDGET_SELECTION__
#define __VTX_UI_QT_WIDGET_SELECTION__

#include <QPointer>
#include <QVBoxLayout>
#include <QWidget>

namespace VTX::UI::QT::Widget
{
	/**
	 * @brief Widget displaying the current selection as a list.
	 */
	class Selection : public QWidget
	{
	  public:
		Selection( QWidget * const p_parent = nullptr );
		void refresh();

	  private:
		QPointer<QVBoxLayout> _layout;
	};
} // namespace VTX::UI::QT::Widget

#endif
