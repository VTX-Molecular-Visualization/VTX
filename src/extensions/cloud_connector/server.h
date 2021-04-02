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

#ifndef SERVER_H
#define SERVER_H

#include "job.h"
#include <QList>
#include <QObject>
#include <extensions/cloud_connector/servercore/connectionlistener.h>
#include <extensions/cloud_connector/servercore/jsonrpc.h>

class ServerTest;

class QSettings;

class Connection;
class Job;
class JobManager;
class CloudConnector;
class JsonRpc;

/**
 * @brief The Server class handles incoming JSON-RPC messages.
 * @author David C. Lonie
 *
 * The Server class is the root of the server-side heirarchy. It owns the
 * JobManager, QueueManager, and JsonRpc listener.
 */
class Server : public QObject
{
	Q_OBJECT
  public:
	/**
	 * Constructor.
	 *
	 * @param parentObject The parent.
	 */
	explicit Server( QObject * parentObject = 0, QString serverName_ = "MoleQueue" );

	/**
	 * Destructor.
	 */
	~Server();

	/**
	 * @return A pointer to the Server JobManager.
	 */
	JobManager * jobManager() { return m_jobManager; }

	/**
	 * @return A pointer to the Server JobManager.
	 */
	const JobManager * jobManager() const { return m_jobManager; }

	/**
	 * @return A pointer to the Server QueueManager.
	 */
	CloudConnector * queueManager() { return m_queueManager; }

	/**
	 * @return A pointer to the Server QueueManager.
	 */
	const CloudConnector * queueManager() const { return m_queueManager; }

	/// @param settings QSettings object to write state to.
	void readSettings( QSettings & settings );
	/// @param settings QSettings object to read state from.
	void writeSettings( QSettings & settings ) const;

	/// The working directory where running job file are kept.
	QString workingDirectoryBase() const { return m_workingDirectoryBase; }

	/// The string the server uses to listen for connections.
	QString serverName() const { return m_serverName; }

	/// Used for unit testing
	friend class ::ServerTest;

  signals:
	/**
	 * Emitted when a connection listener fails to start.
	 */
	void connectionError( ConnectionListener::Error error, const QString & message );

  public slots:

	/**
	 * Start listening for incoming connections.
	 *
	 * If an error occurs, connectionError will be emitted. If an
	 * AddressInUseError occurs on Unix due to a crashed Server that failed to
	 * clean up, call forceStart to remove any existing sockets.
	 */
	void start();

	/**
	 * Start listening for incoming connections, removing any existing socket
	 * handles first.
	 */
	void forceStart();

	/**
	 * Terminate the socket server.
	 *
	 * @param Server will pass the value of force when stop it connections.
	 */
	void stop( bool force );

	/**
	 * Terminate the socket server.
	 *
	 * Same as stop(false)
	 */
	void stop();

	/**
	 * Find the client that owns @a job and send a notification to the client that
	 * the JobState has changed.
	 * @param job Job of interest.
	 * @param oldState Previous state of @a job.
	 * @param newState New state of @a job.
	 */
	void dispatchJobStateChange( const MoleQueue::Job & job,
								 MoleQueue::JobState	oldState,
								 MoleQueue::JobState	newState );

  protected slots:

	/**
	 * Set the MoleQueue Id of a job before it is added to the manager.
	 * @param job The new Job.
	 */
	void jobAboutToBeAdded( MoleQueue::Job job );

	/**
	 * Called when the internal socket server has a new connection ready.
	 */
	void newConnectionAvailable( MoleQueue::Connection * connection );

	/**
	 * Called when a client disconnects from the server. This function expects
	 * sender() to return a ServerConnection.
	 */
	void clientDisconnected();

	/**
	 * @brief handleMessage Called when the JsonRpc listener receives a message.
	 */
	void handleMessage( const MoleQueue::Message & message );

  private slots:

	/**
	 * Called to clean up connection map when a job is removed ...
	 */
	void jobRemoved( MoleQueue::IdType moleQueueId );

  private:
	/**
	 * @{
	 * Handlers for different message types.
	 */
	void handleRequest( const MoleQueue::Message & message );
	void handleUnknownMethod( const MoleQueue::Message & message );
	void handleInvalidParams( const MoleQueue::Message & message, const QString & description );
	void handleListQueuesRequest( const MoleQueue::Message & message );
	void handleSubmitJobRequest( const MoleQueue::Message & message );
	void handleCancelJobRequest( const MoleQueue::Message & message );
	void handleLookupJobRequest( const MoleQueue::Message & message );
	void handleRegisterOpenWithRequest( const MoleQueue::Message & message );
	void handleListOpenWithNamesRequest( const MoleQueue::Message & message );
	void handleUnregisterOpenWithRequest( const MoleQueue::Message & message );
	void handleRpcKillRequest( const MoleQueue::Message & message );
	/**@}*/

  protected:
	/**
	 * @brief timerEvent Reimplemented from QObject.
	 */
	void timerEvent( QTimerEvent * );

	/// List of active connections
	QList<Connection *> m_connections;

	/// The JobManager for this Server.
	JobManager * m_jobManager;

	/// The QueueManager for this Server.
	QueueManager * m_queueManager;

	/// The JsonRpc listener for this Server.
	JsonRpc * m_jsonrpc;

	/// Local directory for running jobs.
	QString m_workingDirectoryBase;

	/// Counter for MoleQueue job ids.
	IdType m_moleQueueIdCounter;

	// job id --> connection for notifications.
	QMap<IdType, Connection *> m_connectionLUT;

	// job id --> reply to endpoint for notifications
	QMap<IdType, EndpointIdType> m_endpointLUT;

  private:
	void	createConnectionListeners();
	QString m_serverName;
	/// The connection listeners
	QList<ConnectionListener *> m_connectionListeners;

	/// The timer id for the job sync event. jobManager()->syncJobState() is
	/// call regularly by this timer.
	int m_jobSyncTimer;
};

#endif // MOLEQUEUE_SERVER_H
