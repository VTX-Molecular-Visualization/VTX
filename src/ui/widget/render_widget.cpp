#include "render_widget.hpp"
#include <iostream>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			RenderWidget::RenderWidget( QWidget * p_parent ) : BaseManualWidget( p_parent ) {}

			RenderWidget::~RenderWidget()
			{
				delete _verticalLayout;
				delete _verticalLayoutWidget;
				delete _openGLWidget;
			};

			void RenderWidget::_setupUi( const QString & p_name )
			{
				BaseManualWidget::_setupUi( p_name );

				_verticalLayoutWidget = new QWidget();
				_verticalLayoutWidget->setObjectName( QString::fromUtf8( "verticalLayoutWidget" ) );
				_verticalLayout = new QVBoxLayout( _verticalLayoutWidget );
				_verticalLayout->setObjectName( QString::fromUtf8( "verticalLayout" ) );
				setWidget( _verticalLayoutWidget );

				_verticalLayout->addWidget( _openGLWidget );
			}

			void RenderWidget::_setupSlots() {}

			void RenderWidget::localize()
			{
				setWindowTitle( "Render" );
				// setWindowTitle( QCoreApplication::translate( "RenderWidget", "Render", nullptr ) );
			}

		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
