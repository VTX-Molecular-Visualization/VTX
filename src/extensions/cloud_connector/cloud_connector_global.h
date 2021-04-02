#ifndef CLOUDCONNECTORGLOBAL_H
#define CLOUDCONNECTORGLOBAL_H

#include <QHash>
#include <QMetaType>
#include <QString>
#include <QStringList>
#include <limits>

/// Type for various ids
typedef qint64 IdType;

/// Constant value used for invalid ids
const IdType InvalidId = ( std::numeric_limits<IdType>::max )();

/// Type for list queue/program names. Key is queue, value is list of supported
/// programs
typedef QHash<QString, QStringList> CloudListType;

/**
 * Enumeration defining states that jobs are allowed to be in.
 */
enum JobState
{
	/// Unknown status
	Unknown = -1,
	/// Initial state of job, should never be entered.
	None = 0,
	/// Job has been accepted by the server and is being prepared (Writing input files, etc).
	Accepted,
	/// Job is being queued locally, either waiting for local execution or remote submission.
	QueuedLocal,
	/// Job has been submitted to a remote queuing system.
	Submitted,
	/// Job is pending execution on a remote queuing system.
	QueuedRemote,
	/// Job is running locally.
	RunningLocal,
	/// Job is running remotely.
	RunningRemote,
	/// Job has completed.
	Finished,
	/// Job has been terminated at a user request.
	Canceled,
	/// Job has been terminated due to an error.
	Error
};

/**
 * Convert a JobState value to a string.
 *
 * @param state JobState
 * @return C string
 */
inline const char * jobStateToString( JobState state )
{
	switch ( state )
	{
	case None: return "None";
	case Accepted: return "Accepted";
	case QueuedLocal: return "QueuedLocal";
	case Submitted: return "Submitted";
	case QueuedRemote: return "QueuedRemote";
	case RunningLocal: return "RunningLocal";
	case RunningRemote: return "RunningRemote";
	case Finished: return "Finished";
	case Canceled: return "Canceled";
	case Error: return "Error";
	default:
	case Unknown: return "Unknown";
	}
}

/**
 * Convert a JobState value to a string that may be displayed in a GUI.
 *
 * @param state JobState
 * @return C string
 */
inline const char * jobStateToGuiString( JobState state )
{
	switch ( state )
	{
	case None: return "None";
	case Accepted: return "Accepted";
	case QueuedLocal: return "Queued local";
	case Submitted: return "Submitted";
	case QueuedRemote: return "Queued remote";
	case RunningLocal: return "Running local";
	case RunningRemote: return "Running remote";
	case Finished: return "Finished";
	case Canceled: return "Canceled";
	case Error: return "Error";
	default:
	case Unknown: return "Unknown";
	}
}

/**
 * Convert a string to a JobState value.
 *
 * @param state JobState string
 * @return JobState
 */
inline JobState stringToJobState( const char * str )
{
	if ( qstrcmp( str, "None" ) == 0 )
		return None;
	else if ( qstrcmp( str, "Accepted" ) == 0 )
		return Accepted;
	else if ( qstrcmp( str, "QueuedLocal" ) == 0 )
		return QueuedLocal;
	else if ( qstrcmp( str, "Submitted" ) == 0 )
		return Submitted;
	else if ( qstrcmp( str, "QueuedRemote" ) == 0 )
		return QueuedRemote;
	else if ( qstrcmp( str, "RunningLocal" ) == 0 )
		return RunningLocal;
	else if ( qstrcmp( str, "RunningRemote" ) == 0 )
		return RunningRemote;
	else if ( qstrcmp( str, "Finished" ) == 0 )
		return Finished;
	else if ( qstrcmp( str, "Canceled" ) == 0 )
		return Canceled;
	else if ( qstrcmp( str, "Error" ) == 0 )
		return Error;
	else
		return Unknown;
}

/// @overload
inline JobState stringToJobState( const QByteArray & str ) { return stringToJobState( str.constData() ); }

/// @overload
inline JobState stringToJobState( const QString & str ) { return stringToJobState( qPrintable( str ) ); }

/**
 * Enumeration defining possible error codes.
 */
enum ErrorCode
{
	/// No error occurred.
	NoError = 0,
	/// Requested queue does not exist.
	InvalidQueue,
	/// Requested program does not exist on queue.
	InvalidProgram,
	/// Job with specified MoleQueue id does not exist.
	InvalidMoleQueueId,
	/// Job is not in the proper state for the requested operation
	InvalidJobState
};

/// Default time in between remote queue updates in minutes.
const int DEFAULT_REMOTE_QUEUE_UPDATE_INTERVAL = 3;

/// Default number of processor cores for a job
const int DEFAULT_NUM_CORES = 1;

/// Default walltime limit for a job
const int DEFAULT_MAX_WALLTIME = 1440;

// Valid names for queues/programs
const char VALID_NAME_REG_EXP[]
	= "[0-9A-za-z()[\\]{}]"
	  "[0-9A-Za-z()[\\]{}\\-_+=.@ ]*";

Q_DECLARE_METATYPE( IdType )
Q_DECLARE_METATYPE( CloudListType )
Q_DECLARE_METATYPE( JobState )
Q_DECLARE_METATYPE( ErrorCode )

#endif
