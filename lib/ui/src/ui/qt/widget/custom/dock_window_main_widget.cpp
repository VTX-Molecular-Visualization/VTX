#include "ui/qt/widget/custom/dock_window_main_widget.hpp"

namespace VTX::UI::QT::Widget::Custom
{
	DockWindowMainWidget::DockWindowMainWidget(
		const QSize & p_sizeHint,
		const QSize & p_minimumSizeHint,
		QWidget *	  p_parent
	) :
		_overriddenSizeHint( p_sizeHint ),
		_overriddenMinimumSizeHint( p_minimumSizeHint ), QWidget( p_parent )
	{
		QWidget::setMinimumSize( p_minimumSizeHint );
		QWidget::setFocusPolicy( Qt::FocusPolicy( Qt::FocusPolicy::ClickFocus | Qt::FocusPolicy::TabFocus ) );
	};
	DockWindowMainWidget::DockWindowMainWidget( QWidget * p_parent ) : QWidget( p_parent ) {};

	QSize DockWindowMainWidget::sizeHint() const
	{
		return _overriddenSizeHint ? *_overriddenSizeHint : QWidget::sizeHint();
	};

	QSize DockWindowMainWidget::minimumSizeHint() const
	{
		return _overriddenMinimumSizeHint ? *_overriddenMinimumSizeHint : QWidget::minimumSizeHint();
	};

	inline void DockWindowMainWidget::setSizeHint( const QSize & p_sizeHint ) { _overriddenSizeHint = p_sizeHint; };
	inline void DockWindowMainWidget::setMinimumSizeHint( const QSize & p_sizeHint )
	{
		_overriddenMinimumSizeHint = p_sizeHint;
		setMinimumSize( p_sizeHint );
	};

} // namespace VTX::UI::QT::Widget::Custom
