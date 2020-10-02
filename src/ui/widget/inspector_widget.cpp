#include "inspector_widget.hpp"

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			InspectorWidget::InspectorWidget( QWidget * p_parent ) : BaseManualWidget( p_parent ) {}
			InspectorWidget::~InspectorWidget() { delete _listWidget; }

			void InspectorWidget::setupUi( const QString & p_name )
			{
				BaseManualWidget::setupUi( p_name );

				_listWidget = new QListWidget();
				QSizePolicy sizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
				this->setSizePolicy( sizePolicy );

				this->setWidget( _listWidget );
			}

			void InspectorWidget::setupSlots() {}
			void InspectorWidget::localize()
			{
				this->setWindowTitle( "Inspector" );
				// this->setWindowTitle( QCoreApplication::translate( "InspectorWidget", "Inspector", nullptr ) );
			}
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
