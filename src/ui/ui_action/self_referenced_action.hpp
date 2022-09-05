#ifndef __VTX_UI_ACTION_SELF_REFERENCED__
#define __VTX_UI_ACTION_SELF_REFERENCED__

#include <QAction>
#include <QIcon>
#include <QObject>
#include <QString>

namespace VTX::UI::UIAction
{
	class SelfReferencedAction : public QAction
	{
		Q_OBJECT

	  public:
		SelfReferencedAction( const QIcon & p_icon, const QString & p_text, QObject * p_parent = nullptr );
		SelfReferencedAction( const QString & p_text, QObject * p_parent = nullptr );
		SelfReferencedAction( QObject * p_parent = nullptr );

	  signals:
		void triggeredSelf( SelfReferencedAction * const p_linkedAction );

	  private:
		void _callTriggeredSignal();
	};
} // namespace VTX::UI::UIAction
#endif
