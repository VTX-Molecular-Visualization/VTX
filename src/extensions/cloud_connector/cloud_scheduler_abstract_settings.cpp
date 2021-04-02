#include "cloud_scheduler_abstract_settings.h"

AbstractSchedulerSettingsWidget::AbstractSchedulerSettingsWidget( QWidget * parentObject ) :
	QWidget( parentObject ), m_isDirty( true )
{
}
