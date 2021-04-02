/******************************************************************************

  This source file is part of the MoleQueue project.

  Copyright 2012 Kitware, Inc.

  This source code is released under the New BSD License, (the "License").

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

******************************************************************************/

#include "scheduler.h"
#include "cloud_connector.h"
#include "file_specification.h"
#include "file_system_tools.h"
#include "job.h"
#include "job_manager.h"
#include "logentry.h"
#include "logger.h"
#include "program.h"
#include "scheduler_remote.h"
#include "server.h"
#include <QDir>
#include <QFile>
#include <qjsondocument.h>

Scheduler::Scheduler( const QString & queueName, CloudConnector * parentManager ) :
	QObject( parentManager ), m_cloud_connector( parentManager ),
	m_server( ( m_cloud_connector ) ? m_cloud_connector->server() : NULL ), m_name( queueName )
{
	qRegisterMetaType<Program *>( "Program*" );
	qRegisterMetaType<const Program *>( "Program*" );
	qRegisterMetaType<IdType>( "IdType" );
	qRegisterMetaType<JobState>( "JobState" );

	if ( m_server )
	{
		connect( m_server->jobManager(),
				 SIGNAL( jobAboutToBeRemoved( const MoleQueue::Job & ) ),
				 this,
				 SLOT( jobAboutToBeRemoved( const MoleQueue::Job & ) ) );
	}
}

Scheduler::~Scheduler()
{
	QList<Program *> programList = m_programs.values();
	m_programs.clear();
	qDeleteAll( programList );
}

bool Scheduler::readSettings( const QString & stateFilename )
{
	return readJsonSettingsFromFile( stateFilename, false, true );
}

bool Scheduler::writeSettings() const
{
	QString fileName = stateFileName();
	if ( fileName.isEmpty() )
	{
		Logger::logError( tr( "Cannot write settings for Scheduler %1: Cannot determine "
							  "config filename." )
							  .arg( name() ) );
		return false;
	}

	// Create directory if needed.
	QDir queueDir( QFileInfo( fileName ).dir() );
	if ( !queueDir.exists() )
	{
		if ( !queueDir.mkpath( queueDir.absolutePath() ) )
		{
			Logger::logError( tr( "Cannot write settings for Scheduler %1: Cannot create "
								  "config directory %2." )
								  .arg( name() )
								  .arg( queueDir.absolutePath() ) );
			return false;
		}
	}

	return writeJsonSettingsToFile( fileName, false, true );
}

bool Scheduler::exportSettings( const QString & fileName, bool includePrograms ) const
{
	return writeJsonSettingsToFile( fileName, true, includePrograms );
}

bool Scheduler::importSettings( const QString & fileName, bool includePrograms )
{
	return readJsonSettingsFromFile( fileName, true, includePrograms );
}

QString Scheduler::queueTypeFromFile( const QString & mqqFile )
{
	QString result;
	if ( !QFile::exists( mqqFile ) )
		return result;

	QFile stateFile( mqqFile );
	if ( !stateFile.open( QFile::ReadOnly | QFile::Text ) )
		return result;
	QByteArray inputText = stateFile.readAll();
	stateFile.close();

	// Try to read existing data in
	QJsonParseError error;
	QJsonDocument	doc = QJsonDocument::fromJson( inputText, &error );
	if ( error.error != QJsonParseError::NoError || !doc.isObject() )
		return result;

	if ( doc.object().value( "type" ).isString() )
		result = doc.object().value( "type" ).toString();

	return result;
}

QString Scheduler::stateFileName() const
{
	QString workDir;
	if ( m_queueManager )
	{
		workDir = m_queueManager->queueConfigDirectory();
	}
	if ( workDir.isEmpty() )
	{
		Logger::logError( tr( "Cannot determine stateFileName for queue '%1'" ) );
		return "";
	}

	return QDir::cleanPath( workDir + "/" + name() + ".mqq" );
}

bool Scheduler::writeJsonSettingsToFile( const QString & stateFilename, bool exportOnly, bool includePrograms ) const
{
	QFile stateFile( stateFilename );
	if ( !stateFile.open( QFile::ReadWrite | QFile::Text | QFile::Truncate ) )
	{
		Logger::logError( tr( "Cannot save queue information for queue %1 in %2: "
							  "Cannot open file." )
							  .arg( name() )
							  .arg( stateFilename ) );
		return false;
	}

	QJsonObject root;
	if ( !this->writeJsonSettings( root, exportOnly, includePrograms ) )
	{
		stateFile.close();
		return false;
	}

	// Write the data back out:
	stateFile.write( QJsonDocument( root ).toJson() );
	stateFile.close();

	return true;
}

bool Scheduler::readJsonSettingsFromFile( const QString & stateFilename, bool importOnly, bool includePrograms )
{
	if ( !QFile::exists( stateFilename ) )
		return false;

	QFile stateFile( stateFilename );
	if ( !stateFile.open( QFile::ReadOnly | QFile::Text ) )
	{
		Logger::logError( tr( "Cannot read queue information from %1." ).arg( stateFilename ) );
		return false;
	}

	// Try to read existing data in
	QByteArray		inputText = stateFile.readAll();
	QJsonParseError error;
	QJsonDocument	doc = QJsonDocument::fromJson( inputText, &error );
	if ( error.error != QJsonParseError::NoError )
	{
		Logger::logError( tr( "Error parsing queue state from %1: %2\n%3" )
							  .arg( stateFilename )
							  .arg( tr( "%1 (at offset %2)" ).arg( error.errorString() ).arg( error.offset ) )
							  .arg( inputText.data() ) );
		stateFile.close();
		return false;
	}

	if ( !doc.isObject() )
	{
		Logger::logError( tr( "Error reading queue state from %1: "
							  "root is not an object!\n%2" )
							  .arg( stateFilename )
							  .arg( inputText.data() ) );
		stateFile.close();
		return false;
	}

	return readJsonSettings( doc.object(), importOnly, includePrograms );
}

bool Scheduler::writeJsonSettings( QJsonObject & root, bool exportOnly, bool includePrograms ) const
{
	root.insert( "type", typeName() );
	root.insert( "launchTemplate", m_launchTemplate );
	root.insert( "launchScriptName", m_launchScriptName );

	if ( !exportOnly )
	{
		QJsonObject jobIdMap;
		foreach ( IdType key, m_jobs.keys() )
			jobIdMap.insert( idTypeToString( key ), idTypeToJson( m_jobs[ key ] ) );
		root.insert( "jobIdMap", jobIdMap );
	}

	if ( includePrograms )
	{
		QJsonObject programsObject;
		foreach ( const Program * prog, programs() )
		{
			QJsonObject programObject;
			if ( prog->writeJsonSettings( programObject, exportOnly ) )
			{
				programsObject.insert( prog->name(), programObject );
			}
			else
			{
				Logger::logError(
					tr( "Could not save program %1 in queue %2's settings." ).arg( prog->name(), name() ) );
			}
		}
		root.insert( "programs", programsObject );
	}

	return true;
}

bool Scheduler::readJsonSettings( const QJsonObject & root, bool importOnly, bool includePrograms )
{
	// Verify JSON:
	if ( !root.value( "type" ).isString() || !root.value( "launchTemplate" ).isString()
		 || !root.value( "launchScriptName" ).isString()
		 || ( root.contains( "programs" ) && !root.value( "programs" ).isObject() ) )
	{
		Logger::logError( tr( "Error reading queue settings: Invalid format:\n%1" )
							  .arg( QString( QJsonDocument( root ).toJson() ) ) );
		return false;
	}

	if ( typeName() != root.value( "type" ).toString() )
	{
		Logger::logError( tr( "Error reading queue settings: Types do not match.\n"
							  "Expected %1, got %2." )
							  .arg( typeName() )
							  .arg( root.value( "type" ).toString() ) );
		return false;
	}

	QMap<IdType, IdType> jobIdMap;
	if ( !importOnly && root.contains( "jobIdMap" ) )
	{
		if ( !root.value( "jobIdMap" ).isObject() )
		{
			Logger::logError( tr( "Error reading queue settings: Invalid format:\n%1" )
								  .arg( QString( QJsonDocument( root ).toJson() ) ) );
			return false;
		}

		QJsonObject jobIdObject = root.value( "jobIdMap" ).toObject();

		foreach ( const QString & key, jobIdObject.keys() )
		{
			IdType jobId	   = toIdType( key );
			IdType moleQueueId = toIdType( jobIdObject.value( key ) );
			jobIdMap.insert( jobId, moleQueueId );
		}
	}

	QMap<QString, Program *> programMap;
	if ( includePrograms && root.contains( "programs" ) )
	{
		if ( !root.value( "programs" ).isObject() )
		{
			Logger::logError( tr( "Error reading queue settings: Invalid format:\n%1" )
								  .arg( QString( QJsonDocument( root ).toJson() ) ) );
			return false;
		}

		QJsonObject programObject = root.value( "programs" ).toObject();

		foreach ( const QString & progName, programObject.keys() )
		{
			if ( !programObject.value( progName ).isObject() )
			{
				Logger::logError( tr( "Error loading configuration for program %1 in "
									  "queue %2." )
									  .arg( progName )
									  .arg( name() ) );
				qDeleteAll( programMap.values() );
				return false;
			}

			Program * prog = new Program( this );
			programMap.insert( progName, prog );
			prog->setName( progName );
			if ( !prog->readJsonSettings( programObject.value( progName ).toObject(), importOnly ) )
			{
				Logger::logError( tr( "Error loading configuration for program %1 in "
									  "queue %2." )
									  .arg( progName )
									  .arg( name() ) );
				qDeleteAll( programMap.values() );
				return false;
			}
		}
	}

	// Everything is verified -- go ahead and update queue.
	m_launchTemplate   = root.value( "launchTemplate" ).toString();
	m_launchScriptName = root.value( "launchScriptName" ).toString();

	if ( !importOnly )
		m_jobs = jobIdMap;

	if ( includePrograms )
	{
		for ( QMap<QString, Program *>::const_iterator it = programMap.constBegin(), it_end = programMap.constEnd();
			  it != it_end;
			  ++it )
		{
			if ( !addProgram( it.value() ) )
			{
				Logger::logDebugMessage( tr( "Cannot add program '%1' to queue '%2': "
											 "program name already exists!" )
											 .arg( it.key() )
											 .arg( name() ) );
				it.value()->deleteLater();
				continue;
			}
		}
	}

	return true;
}

AbstractQueueSettingsWidget * Scheduler::settingsWidget() { return NULL; }

bool Scheduler::addProgram( Program * newProgram, bool replace )
{
	// Check for duplicates, unless we are replacing, and return false if found.
	if ( m_programs.contains( newProgram->name() ) )
	{
		if ( replace )
			m_programs.take( newProgram->name() )->deleteLater();
		else
			return false;
	}

	connect(
		newProgram, SIGNAL( nameChanged( QString, QString ) ), this, SLOT( programNameChanged( QString, QString ) ) );

	m_programs.insert( newProgram->name(), newProgram );

	if ( newProgram->parent() != this )
		newProgram->setParent( this );

	emit programAdded( newProgram->name(), newProgram );
	return true;
}

bool Scheduler::removeProgram( Program * programToRemove ) { return removeProgram( programToRemove->name() ); }

bool Scheduler::removeProgram( const QString & programName )
{
	if ( !m_programs.contains( programName ) )
		return false;

	Program * program = m_programs.take( programName );

	emit programRemoved( programName, program );
	return true;
}

void Scheduler::replaceKeywords( QString & launchScript, const Job & job, bool addNewline )
{
	if ( job.isValid() )
	{
		if ( Program * program = lookupProgram( job.program() ) )
		{
			// This will probably contain other keywords (like inputFileBaseName), so
			// keep it towards the top of the replacement list.
			launchScript.replace( "$$outputFileName$$", program->outputFilename() );
		}

		job.replaceKeywords( launchScript );
	}

	// Remove any unreplaced keywords
	QRegExp expr( "[^\\$]?(\\${2,3}[^\\$\\s]+\\${2,3})[^\\$]?" );
	while ( expr.indexIn( launchScript ) != -1 )
	{
		Logger::logWarning( tr( "Unhandled keyword in launch script: %1. Removing." ).arg( expr.cap( 1 ) ),
							job.moleQueueId() );
		launchScript.remove( expr.cap( 1 ) );
	}

	// Add newline at end if not present
	if ( addNewline && !launchScript.isEmpty() && !launchScript.endsWith( QChar( '\n' ) ) )
	{
		launchScript.append( QChar( '\n' ) );
	}
}

bool Scheduler::writeInputFiles( const Job & job )
{
	QString workdir = job.localWorkingDirectory();

	// Lookup program.
	if ( !m_server )
	{
		Logger::logError( tr( "Scheduler '%1' cannot locate Server instance!" ).arg( m_name ), job.moleQueueId() );
		return false;
	}
	const Program * program = lookupProgram( job.program() );
	if ( !program )
	{
		Logger::logError( tr( "Scheduler '%1' cannot locate program '%2'!" ).arg( m_name ).arg( job.program() ),
						  job.moleQueueId() );
		return false;
	}

	// Create directory
	QDir dir( workdir );

	/// Send a warning but don't bail if the path already exists.
	if ( dir.exists() )
	{
		Logger::logWarning( tr( "Directory already exists: %1" ).arg( dir.absolutePath() ), job.moleQueueId() );
	}
	else
	{
		if ( !dir.mkpath( dir.absolutePath() ) )
		{
			Logger::logError( tr( "Cannot create directory: %1" ).arg( dir.absolutePath() ), job.moleQueueId() );
			return false;
		}
	}

	// Create input files
	FileSpecification inputFile = job.inputFile();
	if ( inputFile.isValid() )
		inputFile.writeFile( dir );

	// Write additional input files
	QList<FileSpecification> additionalInputFiles = job.additionalInputFiles();
	foreach ( const FileSpecification & filespec, additionalInputFiles )
	{
		if ( !filespec.isValid() )
		{
			Logger::logError( tr( "Writing additional input files...invalid FileSpec:\n"
								  "%1" )
								  .arg( QString( filespec.toJson() ) ),
							  job.moleQueueId() );
			return false;
		}
		QFileInfo target( dir.absoluteFilePath( filespec.filename() ) );
		switch ( filespec.format() )
		{
		default:
		case FileSpecification::InvalidFileSpecification:
			Logger::logWarning(
				tr( "Cannot write input file. Invalid filespec:\n%1" ).arg( QString( filespec.toJson() ) ),
				job.moleQueueId() );
			continue;
		case FileSpecification::PathFileSpecification:
		{
			QFileInfo source( filespec.filepath() );
			if ( !source.exists() )
			{
				Logger::logError( tr( "Writing additional input files...Source file "
									  "does not exist! %1" )
									  .arg( source.absoluteFilePath() ),
								  job.moleQueueId() );
				return false;
			}
			if ( source == target )
			{
				Logger::logWarning( tr( "Refusing to copy additional input file...source "
										"and target refer to the same file!\nSource: %1"
										"\nTarget: %2" )
										.arg( source.absoluteFilePath() )
										.arg( target.absoluteFilePath() ),
									job.moleQueueId() );
				continue;
			}
		}
		case FileSpecification::ContentsFileSpecification:
			if ( target.exists() )
			{
				Logger::logWarning( tr( "Writing additional input files...Overwriting "
										"existing file: '%1'" )
										.arg( target.absoluteFilePath() ),
									job.moleQueueId() );
				QFile::remove( target.absoluteFilePath() );
			}
			filespec.writeFile( dir );
			continue;
		}
	}

	// Do we need a driver script?
	const QueueLocal *	localQueue	= qobject_cast<const QueueLocal *>( this );
	const QueueRemote * remoteQueue = qobject_cast<const QueueRemote *>( this );
	if ( ( localQueue && program->launchSyntax() == Program::CUSTOM ) || remoteQueue )
	{
		QFile launcherFile( dir.absoluteFilePath( launchScriptName() ) );
		if ( !launcherFile.open( QFile::WriteOnly | QFile::Text ) )
		{
			Logger::logError( tr( "Cannot open file for writing: %1." ).arg( launcherFile.fileName() ),
							  job.moleQueueId() );
			return false;
		}
		QString launchString = program->launchTemplate();

		replaceKeywords( launchString, job );

		launcherFile.write( launchString.toLatin1() );
		if ( !launcherFile.setPermissions( launcherFile.permissions() | QFile::ExeUser ) )
		{
			Logger::logError( tr( "Cannot set executable permissions on file: %1." ).arg( launcherFile.fileName() ),
							  job.moleQueueId() );
			return false;
		}
		launcherFile.close();
	}

	return true;
}

bool Scheduler::addJobFailure( IdType moleQueueId )
{
	if ( !m_failureTracker.contains( moleQueueId ) )
	{
		m_failureTracker.insert( moleQueueId, 1 );
		return true;
	}

	int failures = ++m_failureTracker[ moleQueueId ];

	if ( failures > 3 )
	{
		Logger::logError( tr( "Maximum number of retries for job %1 exceeded." ).arg( idTypeToString( moleQueueId ) ),
						  moleQueueId );
		clearJobFailures( moleQueueId );
		return false;
	}

	return true;
}

void Scheduler::jobAboutToBeRemoved( const Job & job )
{
	m_failureTracker.remove( job.moleQueueId() );
	m_jobs.remove( job.queueId() );
}

void Scheduler::programNameChanged( const QString & newName, const QString & oldName )
{
	if ( Program * prog = m_programs.value( oldName, NULL ) )
	{
		if ( prog->name() == newName )
		{
			// Reset the program map.
			m_programs.remove( oldName );
			m_programs.insert( newName, prog );

			// Update the configuration file.
			this->writeSettings();
			emit programRenamed( newName, prog, oldName );
		}
	}
}

void Scheduler::cleanLocalDirectory( const Job & job )
{
	if ( !FileSystemTools::recursiveRemoveDirectory( job.localWorkingDirectory(), true ) )
	{
		Logger::logError( tr( "Cannot remove '%1' from local filesystem." ).arg( job.localWorkingDirectory() ) );
	}
}
