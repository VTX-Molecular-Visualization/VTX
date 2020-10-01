#include "render_widget.hpp"

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			RenderWidget::RenderWidget( QWidget * p_parent ) : BaseManualWidget( p_parent ) {}
			RenderWidget::~RenderWidget() {};

			void RenderWidget::setupUi() { setObjectName( QString::fromUtf8( "renderDockWidget" ) ); }
			void RenderWidget::setupSlots() {}
			void RenderWidget::localize()
			{
				setWindowTitle( "Render" );
				// setWindowTitle( QCoreApplication::translate( "RenderWidget", "Render", nullptr ) );
			}

		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
