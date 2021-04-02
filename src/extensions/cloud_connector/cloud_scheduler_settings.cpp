#include "cloud_scheduler_settings.h"
#include "program.h"
#include "scheduler_remote_ssh.h"
#include "ssh_command_factory.h"
#include "template_keyword_dialog.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QProgressDialog>
#include <QSettings>
#include <QTextDocument>
#include <QTimer>
//#include <client.h>
//#include <jobobject.h>

SchedulerSettingsWidget::SchedulerSettingsWidget( SchedulerRemoteSsh * scheduler, QWidget * parentObject ) :
	AbstractSchedulerSettingsWidget( parentObject ), ui( new Ui::SchedulerSettingsWidget ), m_scheduler( scheduler ),
	m_client( NULL )
{
	ui->setupUi( this );

	reset();
}

SchedulerSettingsWidget::~SchedulerSettingsWidget() { delete ui; }

void SchedulerSettingsWidget::save()
{
	m_queue->setSubmissionCommand( ui->edit_submissionCommand->text() );
	m_queue->setKillCommand( ui->edit_killCommand->text() );
	m_queue->setRequestQueueCommand( ui->edit_requestQueueCommand->text() );
	m_queue->setLaunchScriptName( ui->edit_launchScriptName->text() );
	m_queue->setWorkingDirectoryBase( ui->edit_workingDirectoryBase->text() );
	m_queue->setSshExecutable( ui->sshExecutableEdit->text() );
	m_queue->setScpExecutable( ui->scpExecutableEdit->text() );
	m_queue->setHostName( ui->editHostName->text() );
	m_queue->setUserName( ui->editUserName->text() );
	m_queue->setIdentityFile( ui->editIdentityFile->text() );
	m_queue->setSshPort( ui->spinSshPort->value() );

	m_queue->setQueueUpdateInterval( ui->updateIntervalSpin->value() );

	QString text = ui->text_launchTemplate->document()->toPlainText();
	m_queue->setLaunchTemplate( text );

	int hours	= ui->wallTimeHours->value();
	int minutes = ui->wallTimeMinutes->value();
	m_queue->setDefaultMaxWallTime( minutes + ( hours * 60 ) );
	setDirty( false );
}

void SchedulerSettingsWidget::reset()
{
	ui->edit_submissionCommand->setText( m_queue->submissionCommand() );
	ui->edit_killCommand->setText( m_queue->killCommand() );
	ui->edit_requestQueueCommand->setText( m_queue->requestQueueCommand() );
	ui->edit_launchScriptName->setText( m_queue->launchScriptName() );
	ui->edit_workingDirectoryBase->setText( m_queue->workingDirectoryBase() );
	ui->updateIntervalSpin->setValue( m_queue->queueUpdateInterval() );
	int walltime = m_queue->defaultMaxWallTime();
	ui->wallTimeHours->setValue( walltime / 60 );
	ui->wallTimeMinutes->setValue( walltime % 60 );
	ui->sshExecutableEdit->setText( m_queue->sshExecutable() );
	ui->scpExecutableEdit->setText( m_queue->scpExectuable() );
	ui->editHostName->setText( m_queue->hostName() );
	ui->editUserName->setText( m_queue->userName() );
	ui->editIdentityFile->setText( m_queue->identityFile() );
	ui->spinSshPort->setValue( m_queue->sshPort() );
	ui->text_launchTemplate->document()->setPlainText( m_queue->launchTemplate() );
	setDirty( false );
}

void SchedulerSettingsWidget::testConnection()
{
	// Verify information
	QString sshCommand	 = ui->sshExecutableEdit->text();
	QString host		 = ui->editHostName->text();
	QString user		 = ui->editUserName->text();
	QString identityFile = ui->editIdentityFile->text();
	int		port		 = ui->spinSshPort->value();

	if ( host.isEmpty() || user.isEmpty() )
	{
		QMessageBox::warning( this,
							  tr( "Cannot connect to remote host." ),
							  tr( "Cannot connect to remote host: invalid host "
								  "specification: %1@%2" )
								  .arg( host, user ) );
		return;
	}

	// Create SSH connection
	SshCommand * conn = SshCommandFactory::instance()->newSshCommand();
	conn->setSshCommand( sshCommand );
	conn->setHostName( host );
	conn->setUserName( user );
	conn->setIdentityFile( identityFile );
	conn->setPortNumber( port );

	// Create ProgressDialog
	QProgressDialog * prog = new QProgressDialog( this );
	prog->setWindowTitle( tr( "Testing remote connection..." ) );
	prog->setLabelText( tr( "Attempting to connect to %1@%2:%3..." ).arg( user ).arg( host ).arg( port ) );
	prog->setMinimumDuration( 0 );
	prog->setWindowModality( Qt::WindowModal );
	prog->setRange( 0, 0 );
	prog->setValue( 0 );

	QTimer * timeout = new QTimer( this );
	connect( conn, SIGNAL( requestComplete() ), prog, SLOT( accept() ) );
	connect( timeout, SIGNAL( timeout() ), prog, SLOT( reject() ) );

	// Wait 15 seconds for timeout
	timeout->start( 15000 );
	conn->execute( "echo ok" );
	prog->exec();
	prog->hide();

	if ( prog->wasCanceled() )
	{
		conn->deleteLater();
		prog->deleteLater();
		return;
	}

	if ( prog->result() == QProgressDialog::Rejected )
	{
		QMessageBox::critical( this,
							   tr( "Connection timeout" ),
							   tr( "The connection to %1@%2:%3 failed: connection"
								   " timed out." )
								   .arg( user )
								   .arg( host )
								   .arg( port ) );
		conn->deleteLater();
		prog->deleteLater();
		return;
	}

	prog->hide();
	prog->deleteLater();

	// Verify output and exit code
	if ( conn->exitCode() != 0 || conn->output().trimmed() != "ok" )
	{
		QMessageBox::critical( this,
							   tr( "SSH Error" ),
							   tr( "The connection to %1@%2:%3 failed: "
								   "exit code: %4. Output:\n\n%5" )
								   .arg( user )
								   .arg( host )
								   .arg( port )
								   .arg( conn->exitCode() )
								   .arg( conn->output() ) );
		conn->deleteLater();
		return;
	}

	QMessageBox::information(
		this, tr( "Success" ), tr( "SSH connection to %1@%2:%3 succeeded!" ).arg( user ).arg( host ).arg( port ) );
	conn->deleteLater();

	return;
}

void SchedulerSettingsWidget::sleepTest()
{
	QString promptString;
	if ( isDirty() )
	{
		promptString = tr(
			"Would you like to apply the current settings and submit "
			"a test job? The job will run 'sleep 30' on the remote "
			"queue." );
	}
	else
	{
		promptString = tr(
			"Would you like to submit a test job? The job will run "
			"'sleep 30' on the remote queue." );
	}

	QMessageBox::StandardButton response = QMessageBox::question(
		this, tr( "Submit test job?" ), promptString, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes );

	if ( response != QMessageBox::Yes )
		return;

	if ( isDirty() )
		save();

	// Check that important variables are set:
	QString missingVariable = "";
	if ( m_queue->hostName().isEmpty() )
		missingVariable = tr( "server hostname" );
	else if ( m_queue->userName().isEmpty() )
		missingVariable = tr( "server username" );
	else if ( m_queue->submissionCommand().isEmpty() )
		missingVariable = tr( "job submission command" );
	else if ( m_queue->killCommand().isEmpty() )
		missingVariable = tr( "job cancel command" );
	else if ( m_queue->requestQueueCommand().isEmpty() )
		missingVariable = tr( "queue request command" );
	else if ( m_queue->workingDirectoryBase().isEmpty() )
		missingVariable = tr( "remote working directory" );

	if ( !missingVariable.isEmpty() )
	{
		QMessageBox::critical( this,
							   tr( "Missing information" ),
							   tr( "Refusing to test job submission: %1 not set." ).arg( missingVariable ) );
		return;
	}

	Program * sleepProgram = m_queue->lookupProgram( "sleep (testing)" );

	if ( sleepProgram == NULL )
	{
		// Add sleep if it's not present
		sleepProgram = new Program( m_queue );
		sleepProgram->setName( "sleep (testing)" );
		sleepProgram->setArguments( "30" );
		sleepProgram->setExecutable( "sleep" );
		sleepProgram->setOutputFilename( "" );
		sleepProgram->setLaunchSyntax( Program::PLAIN );
		m_queue->addProgram( sleepProgram );
	}

	if ( !m_client )
	{
		m_client = new Client( this );
		m_client->connectToServer();
	}

	JobObject sleepJob;
	sleepJob.setQueue( m_queue->name() );
	sleepJob.setProgram( sleepProgram->name() );
	sleepJob.setDescription( "sleep 30 (test)" );

	m_client->submitJob( sleepJob );
}

void SchedulerSettingsWidget::browseIdentityFile()
{
	// Get initial dir:
	QSettings settings;
	QString	  initialDir = settings.value( "ssh/identity/lastIdentityFile", ui->editIdentityFile->text() ).toString();
	if ( initialDir.isEmpty() )
	{
		initialDir = QDir::homePath();
#ifndef _WIN32
		initialDir += "/.ssh/";
#endif
	}

	initialDir = QFileInfo( initialDir ).dir().absolutePath();

	// Get filename
	QString identityFileName = QFileDialog::getOpenFileName( this, tr( "Select identity file" ), initialDir );
	// User cancel:
	if ( identityFileName.isNull() )
		return;

	// Set location for next time
	settings.setValue( "ssh/identity/lastIdentityFile", identityFileName );

	ui->editIdentityFile->setText( identityFileName );
}
