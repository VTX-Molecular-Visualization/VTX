#ifndef SCHEDULERSETTINGSWIDGET_H
#define SCHEDULERSETTINGSWIDGET_H

#include "cloud_scheduler_abstract_settings.h"
#include "ui/qt_form/ui_cloud_scheduler_settings_widget.h"
#include "ui/widget/custom_widget/custom_qdockwidget.hpp"

class Client;
class SchedulerRemoteSsh;

/**
 * @class RemoteSchedulerWidget remotequeuewidget.h <molequeue/remotequeuewidget.h>
 *
 * @brief A generic configuration dialog for remote queuing systems.
 *
 * @author David C. Lonie
 */
class SchedulerSettingsWidget : public AbstractSchedulerSettingsWidget
{
	Q_OBJECT

  public:
	explicit SchedulerSettingsWidget( SchedulerRemoteSsh * scheduler, QWidget * parentObject = 0 );
	~SchedulerSettingsWidget();

  public slots:
	void save();
	void reset();

  protected slots:
	void testConnection();
	void sleepTest();

  private slots:
	void browseIdentityFile();

  private:
	Ui::SchedulerSettingsWidget * ui;
	SchedulerRemoteSsh *		  m_scheduler;
	Client *					  m_client; // Used for submitting test jobs.
};

#endif // SCHEDULERSETTINGSWIDGET_H
