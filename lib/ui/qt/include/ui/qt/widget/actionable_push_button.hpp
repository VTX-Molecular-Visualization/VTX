#ifndef __VTX_UI_QT_WIDGET_ACTIONABLE_PUSH_BUTTON__
#define __VTX_UI_QT_WIDGET_ACTIONABLE_PUSH_BUTTON__

#include <QPushButton>

namespace VTX::UI::QT::Widget
{
	/**
	 * @brief QPushButton that handle QAction.
	 */
	class ActionablePushButton : public QPushButton
	{
	  public:
		explicit ActionablePushButton( const QAction * const p_action, QWidget * p_parent ) : QPushButton( p_parent )
		{
			this->setText( p_action->text() );
			this->setIcon( p_action->icon() );
			this->setToolTip( p_action->toolTip() );
			this->setWhatsThis( p_action->whatsThis() );
			this->setStatusTip( p_action->statusTip() );
			this->setShortcut( p_action->shortcut() );
			this->setCheckable( p_action->isCheckable() );
			this->setChecked( p_action->isChecked() );
			this->setEnabled( p_action->isEnabled() );

			connect( this, &QPushButton::clicked, p_action, &QAction::trigger );
			// connect( _action, &QAction::changed, this, &ActionablePushButton::updateFromAction );
		}
	};
} // namespace VTX::UI::QT::Widget
#endif
