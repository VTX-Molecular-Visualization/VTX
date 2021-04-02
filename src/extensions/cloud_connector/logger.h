#ifndef CLOUD_CONNECTOR_LOGGER_H
#define CLOUD_CONNECTOR_LOGGER_H

#include "logentry.h"
#include <QFile>
#include <QLinkedList>
#include <QObject>

/**
 * @class Logger logger.h <molequeue/logger.h>
 * @brief Manage log messages.
 * @author David C. Lonie
 *
 * The singleton Logger class is used to handle log messages in MoleQueue. Log
 * messages are represented as objects consisting of a user-friendly string,
 * an enum value representing a subtype, and an optional MoleQueue id for any
 * associated job.
 *
 * There are four levels of log messages:
 * - DebugMessage: Verbose debugging information.
 * - Notification: Routine information that is relevant to the user.
 * - Warning: Non-routine information that is relevant to the user, but does not
 *   indicate a serious problem.
 * - Error: Serious problem that will affect either the MoleQueue application
 *   or a Job's ability to perform properly.
 *
 * New log entries can be submitted using the static Logger::logEntry method or
 * the convenient logDebugMessage, logNotification, logWarning, or logError.
 * Each new log entry causes the newLogEntry signal to be emitted, as well as
 * one of newDebugMessage, newNotification, newWarning, or newError, depending
 * on the LogEntry type. Details of new log entries will be automatically
 * sent to qDebug() if the print* methods are set to true (false by default).
 */
class Logger : public QObject
{
	Q_OBJECT
  public:
	/// @return The singleton Logger instance
	static Logger * getInstance();

	/// @return Whether or not to print debugging messages to qDebug.
	/// Default: false
	static bool printDebugMessages() { return Logger::getInstance()->m_printDebugMessages; }

	/// @return Whether or not to print notifications to qDebug. Default: false
	static bool printNotifications() { return Logger::getInstance()->m_printNotifications; }

	/// @return Whether or not to print warnings to qDebug. Default: false
	static bool printWarnings() { return Logger::getInstance()->m_printWarnings; }

	/// @return Whether or not to print errors to qDebug. Default: false
	static bool printErrors() { return Logger::getInstance()->m_printErrors; }

	/// @return The maximum number of entries the Logger will track.
	/// Default: 1000
	static int maxEntries() { return Logger::getInstance()->m_maxEntries; }

	/// @return The number of new errors that have occurred since the last
	/// Logger::resetNewErrors call.
	static int numNewErrors() { return Logger::getInstance()->m_newErrorCount; }

  signals:

	/// Emitted when a new debugging message has been added to the log.
	void newDebugMessage( const LogEntry & debug );

	/// Emitted when a new notification has been added to the log.
	void newNotification( const LogEntry & notif );

	/// Emitted when a new warning has been added to the log.
	void newWarning( const LogEntry & warning );

	/// Emitted when a new error has been added to the log.
	void newError( const LogEntry & error );

	/// Emitted when any new log entry is added to the log.
	void newLogEntry( const LogEntry & entry );

	/// Emitted when the new error count becomes non-zero. Monitor this to check
	/// for the presence of errors without getting notified about each error.
	void firstNewErrorOccurred();

	/// Emitted when the new error count is reset.
	void newErrorCountReset();

  public slots:

	/// Add @a entry to the log.
	static void logEntry( LogEntry & entry ) { Logger::getInstance()->handleNewLogEntry( entry ); }

	/// Add a new log entry to the log.
	static void logEntry( LogEntry::LogEntryType type, const QString & message, const IdType & moleQueueId = InvalidId )
	{
		LogEntry entry( type, message, moleQueueId );
		Logger::logEntry( entry );
	}

	/// Add a new debugging message to the log.
	static void logDebugMessage( const QString & message, const IdType & moleQueueId = InvalidId )
	{
		LogEntry entry( LogEntry::DebugMessage, message, moleQueueId );
		Logger::logEntry( entry );
	}

	/// Add a new notification to the log.
	static void logNotification( const QString & message, const IdType & moleQueueId = InvalidId )
	{
		LogEntry entry( LogEntry::Notification, message, moleQueueId );
		Logger::logEntry( entry );
	}

	/// Add a new warning to the log.
	static void logWarning( const QString & message, const IdType & moleQueueId = InvalidId )
	{
		LogEntry entry( LogEntry::Warning, message, moleQueueId );
		Logger::logEntry( entry );
	}

	/// Add a new error to the log.
	static void logError( const QString & message, const IdType & moleQueueId = InvalidId )
	{
		LogEntry entry( LogEntry::Error, message, moleQueueId );
		Logger::logEntry( entry );
	}

	/// @param print Whether or not to print debugging messages to qDebug.
	/// Default: false
	static void setPrintDebugMessages( bool print ) { Logger::getInstance()->m_printDebugMessages = print; }

	/// @param print Whether or not to print notifications to qDebug. Default:
	/// false
	static void setPrintNotifications( bool print ) { Logger::getInstance()->m_printNotifications = print; }

	/// @param print Whether or not to print warnings to qDebug. Default: false
	static void setPrintWarnings( bool print ) { Logger::getInstance()->m_printWarnings = print; }

	/// @param print Whether or not to print errors to qDebug. Default: false
	static void setPrintErrors( bool print ) { Logger::getInstance()->m_printErrors = print; }

	/// @return A list of all log entries.
	static QLinkedList<LogEntry> log() { return Logger::getInstance()->m_log; }

	/// @param max The maximum number of entries the Logger will track.
	/// Default: 1000
	static void setMaxEntries( int max )
	{
		Logger::getInstance()->m_maxEntries = max;
		Logger::getInstance()->trimLog();
	}

	/// Remove all entries from the log
	static void clear() { Logger::getInstance()->m_log.clear(); }

	/// Reset the number of new errors.
	static void resetNewErrorCount();

	/// @param silence If true, the firstNewErrorOccurred signal will not be
	/// emitted until this function is called again with false.
	static void silenceNewErrors( bool silence = true ) { Logger::getInstance()->m_silenceNewErrors = silence; }

  protected slots:
	void cleanUp();

  private:
	Logger();
	~Logger();
	/// Create and return the log file. When finished, close but do not delete the
	/// object. If an error occurs, a message is printed to qWarning and NULL is
	/// returned.
	QFile *			logFile();
	static Logger * m_instance;

	void handleNewLogEntry( LogEntry & entry );
	void handleNewDebugMessage( const LogEntry & debug );
	void handleNewNotification( const LogEntry & notif );
	void handleNewWarning( const LogEntry & warning );
	void handleNewError( const LogEntry & error );

	void trimLog();

	bool m_printDebugMessages;
	bool m_printNotifications;
	bool m_printWarnings;
	bool m_printErrors;

	int	 m_maxEntries;
	int	 m_newErrorCount;
	bool m_silenceNewErrors;

	QFile *				  m_logFile;
	QLinkedList<LogEntry> m_log;
};

#endif // CLOUD_CONNECTOR_LOGGER_H
