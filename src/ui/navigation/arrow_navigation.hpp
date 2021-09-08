#ifndef __VTX_UI_NAVIGATION_ARROW__
#define __VTX_UI_NAVIGATION_ARROW__

#include <QKeyEvent>
#include <QWidget>

namespace VTX::UI::Navigation
{
	class ArrowNavigation : public QObject
	{
		Q_OBJECT

	  public:
		bool keyPressed( QKeyEvent * const p_event ) const;

	  signals:
		void navigateToLeft() const;
		void navigateToRight() const;
		void navigateToUp() const;
		void navigateToDown() const;
	};

} // namespace VTX::UI::Navigation

#endif
