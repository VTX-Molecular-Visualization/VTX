#include "render_widget.hpp"

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			RenderWidget::RenderWidget( QWidget * p_parent ) : BaseManualWidget( p_parent ) {}
			RenderWidget::~RenderWidget() {};

			void RenderWidget::_setupUi( const QString & p_name ) { BaseManualWidget::_setupUi( p_name ); }
			void RenderWidget::_setupSlots() {}
			void RenderWidget::localize()
			{
				setWindowTitle( "Render" );
				// setWindowTitle( QCoreApplication::translate( "RenderWidget", "Render", nullptr ) );
			}

		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
