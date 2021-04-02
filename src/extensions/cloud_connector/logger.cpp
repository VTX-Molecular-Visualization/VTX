#include "logger.h"
#include "logentry.h"
#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSettings>

Logger * Logger::m_instance = NULL;

Logger::Logger() :
	m_printDebugMessages( false ), m_printNotifications( false ), m_printWarnings( false ), m_printErrors( false ),
	m_maxEntries( 1000 ), m_newErrorCount( 0 ), m_silenceNewErrors( false ), m_logFile( NULL )
{
	// Call destructor when program exits
	connect( QApplication::instance(), SIGNAL( aboutToQuit() ), SLOT( cleanUp() ) );

	QFile * lfile = logFile();

	if ( lfile )
	{
		if ( !lfile->open( QFile::ReadOnly | QFile::Text ) )
		{
			QSettings settings;
			if ( settings.value( "logWritten", false ).toBool() )
			{
				qWarning() << "MoleQueue::Logger::~Logger() -- Cannot open log "
							  "file "
								  + lfile->fileName() + "; cannot read log.";
			}
			return;
		}

		QByteArray logData = lfile->readAll();
		lfile->close();

		QJsonParseError error;
		QJsonDocument	doc = QJsonDocument::fromJson( logData, &error );
		if ( error.error != QJsonParseError::NoError )
		{
			qWarning() << "MoleQueue::Logger::~Logger() -- Error parsing log file" << lfile->fileName() << ":"
					   << error.errorString() << "(at offset" << error.offset << ").";
			return;
		}

		if ( !doc.isObject() )
		{
			qWarning() << "MoleQueue::Logger::~Logger() -- Error parsing log file" << lfile->fileName()
					   << ": Invalid format, expected JSON "
						  "object at top level.";
			return;
		}

		QJsonObject logObject = doc.object();

		if ( logObject.value( "maxEntries" ).isDouble() )
		{
			m_maxEntries = static_cast<int>( logObject.value( "maxEntries" ).toDouble() + 0.5 );
		}

		if ( logObject.value( "entries" ).isArray() )
		{
			const QJsonArray & entries = logObject.value( "entries" ).toArray();
			foreach ( QJsonValue val, entries )
			{
				if ( val.isObject() )
					m_log.push_back( LogEntry( val.toObject() ) );
			}
		}
	} // end if (lfile)
}

Logger::~Logger()
{
	QFile * lfile = logFile();

	if ( lfile )
	{
		if ( !lfile->open( QFile::WriteOnly | QFile::Text | QFile::Truncate ) )
		{
			qWarning() << "MoleQueue::Logger::~Logger() -- Cannot create log "
						  "file"
							  + lfile->fileName() + "; cannot save log.";
			return;
		}

		QJsonObject root;
		root.insert( "maxEntries", static_cast<double>( m_maxEntries ) );

		QJsonArray entriesArray;
		foreach ( const LogEntry & entry, m_log )
		{
			QJsonObject entryObject;
			entry.writeSettings( entryObject );
			entriesArray.append( entryObject );
		}
		root.insert( "entries", entriesArray );

		lfile->write( QJsonDocument( root ).toJson() );
		lfile->close();

		QSettings settings;
		settings.setValue( "logWritten", true );
	} // end if (lfile)
}

void Logger::resetNewErrorCount()
{
	Logger * instance = Logger::getInstance();
	if ( instance->m_newErrorCount == 0 )
		return;

	emit instance->newErrorCountReset();
	instance->m_newErrorCount = 0;
}

void Logger::cleanUp()
{
	delete m_instance;
	m_instance = NULL;
}

QFile * Logger::logFile()
{
	if ( !m_logFile )
	{
		QSettings settings;
		QString	  workDir = settings.value( "workingDirectoryBase" ).toString();

		if ( workDir.isEmpty() )
		{
			qWarning() << "MoleQueue::Logger::~Logger() -- Cannot determine working "
						  "directory.";
			return NULL;
		}

		QDir logDir( workDir + "/log" );
		if ( !logDir.exists() )
		{
			if ( !logDir.mkpath( logDir.absolutePath() ) )
			{
				qWarning() << "MoleQueue::Logger::~Logger() -- Cannot create log "
							  "directory"
								  + logDir.absolutePath();
				return NULL;
			}
		}

		m_logFile = new QFile( logDir.absoluteFilePath( "log.json" ) );
	}

	if ( m_logFile )
	{
		if ( m_logFile->isOpen() )
			m_logFile->close();
	}

	return m_logFile;
}

Logger * Logger::getInstance()
{
	if ( !m_instance )
		m_instance = new Logger();

	return m_instance;
}

void Logger::handleNewLogEntry( LogEntry & entry )
{
	entry.setTimeStamp();
	m_log.push_back( entry );

	trimLog();

	switch ( entry.entryType() )
	{
	case LogEntry::DebugMessage: handleNewDebugMessage( entry ); break;
	case LogEntry::Notification: handleNewNotification( entry ); break;
	case LogEntry::Warning: handleNewWarning( entry ); break;
	case LogEntry::Error: handleNewError( entry ); break;
	}
	emit newLogEntry( entry );
}

inline void Logger::handleNewDebugMessage( const LogEntry & debug )
{
	if ( m_printDebugMessages )
	{
		qDebug() << "Debugging message:"
				 << "Message: " << debug.message() << "MoleQueueId: (" << debug.moleQueueId() << ")";
	}
	emit newDebugMessage( debug );
}

inline void Logger::handleNewNotification( const LogEntry & notif )
{
	if ( m_printNotifications )
	{
		qDebug() << "Notification:"
				 << "Message: " << notif.message() << "MoleQueueId: (" << notif.moleQueueId() << ")";
	}
	emit newNotification( notif );
}

inline void Logger::handleNewWarning( const LogEntry & warning )
{
	if ( m_printWarnings )
	{
		qDebug() << "Warning:"
				 << "Message: " << warning.message() << "MoleQueueId: (" << warning.moleQueueId() << ")";
	}
	emit newWarning( warning );
}

inline void Logger::handleNewError( const LogEntry & error )
{
	if ( m_printErrors )
	{
		qDebug() << "Error occurred:"
				 << "Message: " << error.message() << "MoleQueueId: (" << error.moleQueueId() << ")";
	}

	++m_newErrorCount;

	emit newError( error );

	if ( !m_silenceNewErrors && m_newErrorCount == 1 )
		emit firstNewErrorOccurred();
}

void Logger::trimLog()
{
	if ( m_log.size() > m_maxEntries )
	{
		m_log.erase( m_log.begin(), m_log.begin() + ( m_log.size() - m_maxEntries ) );
	}
}
