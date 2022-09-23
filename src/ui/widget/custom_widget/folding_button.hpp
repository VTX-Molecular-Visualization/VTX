#ifndef __VTX_UI_WIDGET_CUSTOM_FOLDING_BUTTON__
#define __VTX_UI_WIDGET_CUSTOM_FOLDING_BUTTON__

#include "ui/widget/base_manual_widget.hpp"
#include <QPushButton>
#include <QWidget>

namespace VTX::UI::Widget::CustomWidget
{
	class FoldingButton : public BaseManualWidget<QWidget>
	{
		VTX_WIDGET
		Q_OBJECT

	  public:
		~FoldingButton() = default;
		void localize() override;

		void setTitle( const QString & p_title ) const;
		void toggleFoldState() const;
		void setFoldState( const bool p_expanded ) const;

	  signals:
		void onFoldStateChange( const bool p_folded );

	  protected:
		FoldingButton( QWidget * p_content, QWidget * p_parent = nullptr );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

	  private:
		QPushButton *	_foldButton = nullptr;
		QWidget * const _content	= nullptr;
	};
} // namespace VTX::UI::Widget::CustomWidget
#endif
