#include "arrow_navigation.hpp"

namespace VTX::UI::Navigation
{
	bool ArrowNavigation::keyPressed( QKeyEvent * const p_event ) const
	{
		if ( p_event->key() == Qt::Key::Key_Left )
			emit navigateToLeft();
		else if ( p_event->key() == Qt::Key::Key_Right )
			emit navigateToRight();
		else if ( p_event->key() == Qt::Key::Key_Up )
			emit navigateToUp();
		else if ( p_event->key() == Qt::Key::Key_Down )
			emit navigateToDown();
		else
			return false;

		p_event->accept();
		return true;
	}
} // namespace VTX::UI::Navigation
