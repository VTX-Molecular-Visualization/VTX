#ifndef __VTX_UI_WIDGET_SEQUENCE__
#define __VTX_UI_WIDGET_SEQUENCE__

#include "molecule_sequence_widget.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include "ui/widget/custom_widget/dock_window_main_widget.hpp"
#include <QDockWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <unordered_set>

namespace VTX::UI::Widget::Sequence
{
	class SequenceWidget : public BaseManualWidget<QDockWidget>
	{
		VTX_WIDGET

	  public:
		void receiveEvent( const Event::VTXEvent & p_event ) override;
		void localize() override;
		void refreshSelection() const;

	  protected:
		SequenceWidget( QWidget * p_parent );

		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

	  private:
		CustomWidget::DockWindowMainWidget<QScrollArea> * _scrollArea = nullptr;

		QVBoxLayout *								 _layout = nullptr;
		std::unordered_set<MoleculeSequenceWidget *> _moleculeWidgets;
	};
} // namespace VTX::UI::Widget::Sequence
#endif
