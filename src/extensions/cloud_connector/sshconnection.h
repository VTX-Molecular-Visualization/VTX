/******************************************************************************

  This source file is part of the MoleQueue project.

  Copyright 2011-2012 Kitware, Inc.

  This source code is released under the New BSD License, (the "License").

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

******************************************************************************/

#ifndef SSHCONNECTION_H
#define SSHCONNECTION_H

#include <QtCore/QObject>
#include <QtCore/QVariant>

/**
 * @class SshConnection sshconnection.h <molequeue/sshconnection.h>
 * @brief Abstract base class defining remote execution and file transfer
 * operations over the ssh protocol.
 * @author Marcus D. Hanwell, David C. Lonie
 *
 * The SshConnection is the interface to use when writing code that requires
 * interactions with a remote host. Subclasses provide concrete implementations
 * of the interface, e.g. SshCommand, which calls the ssh and scp commands in a
 * TerminalProcess.
 */
class SshConnection : public QObject
{
	Q_OBJECT

  public:
	SshConnection( QObject * parentObject = 0 );
	~SshConnection();

	/** \return If the SSH connection is set as persistent or not. */
	bool isPersistent() const { return m_persistent; }

	/** \return The user name that will be used. */
	QString userName() const { return m_userName; }

	/** \return The host that will be used. */
	QString hostName() const { return m_hostName; }

	/** \return The path to the identity file that will be used. */
	QString identityFile() const { return m_identityFile; }

	/** \return The port that will be used. */
	int portNumber() const { return m_portNumber; }

	/** \return Whether the connection is valid, at a minimum need a host name. */
	virtual bool isValid() const;

	/** \return The merged stdout and stderr of the remote command. */
	virtual QString output() const;

	/** \return The exit code returned from a remote command. */
	virtual int exitCode() const;

	/**
	 * Wait until the request has been completed.
	 *
	 * @param msecs Timeout in milliseconds. Default is 30 seconds.
	 *
	 * @return True if request finished, false on timeout.
	 */
	virtual bool waitForCompletion( int msecs = 30000 );

	/** @return True if the request has completed. False otherwise. */
	virtual bool isComplete() const;

	/** @return A reference to arbitrary data stored in the command. */
	QVariant & data() { return m_data; }

	/** @return A reference to arbitrary data stored in the command. */
	const QVariant & data() const { return m_data; }

	/** @param newData Arbitrary data to store in the command. */
	void setData( const QVariant & newData ) { m_data = newData; }

  public slots:
	/**
	 * Set whether the connection should be persistent, or each issuesd command
	 * uses a short-lived connection, e.g. on the command line a non-persistent
	 * connection would be the equivalent of,
	 *
	 * ssh user@host ls
	 */
	void setPersistent( bool persist ) { m_persistent = persist; }

	/**
	 * Set the user name to use for the connection.
	 */
	void setUserName( const QString & newUserName ) { m_userName = newUserName; }

	/**
	 * Set the host name to use for the connection.
	 */
	void setHostName( const QString & newHostName ) { m_hostName = newHostName; }

	/**
	 * Set the identity file to use for the connection. This is the path to the
	 * private key to be used when establishing the connection.
	 */
	void setIdentityFile( const QString & newIdentityFile ) { m_identityFile = newIdentityFile; }

	/**
	 * Set the host name to use for the connection.
	 */
	void setPortNumber( int newPortNumber ) { m_portNumber = newPortNumber; }

	/**
	 * Execute the supplied command on the remote host.
	 *
	 * \note The command is executed asynchronously, see requestComplete() or
	 * waitForCompletion() for results.
	 *
	 * \sa requestSent() requestCompleted() waitForCompeletion()
	 *
	 * \param command The command to execute.
	 *
	 * \return True on success, false on failure.
	 */
	virtual bool execute( const QString & command );

	/**
	 * Copy a local file to the remote system.
	 *
	 * \note The command is executed asynchronously, see requestComplete() or
	 * waitForCompletion() for results.
	 *
	 * \sa requestSent() requestCompleted() waitForCompeletion()
	 *
	 * \param localFile The path of the local file.
	 * \param remoteFile The path of the file on the remote system.
	 * \return True on success, false on failure.
	 */
	virtual bool copyTo( const QString & localFile, const QString & remoteFile );

	/**
	 * Copy a remote file to the local system.
	 *
	 * \note The command is executed asynchronously, see requestComplete() or
	 * waitForCompletion() for results.
	 *
	 * \sa requestSent() requestCompleted() waitForCompeletion()
	 *
	 * \param remoteFile The path of the file on the remote system.
	 * \param localFile The path of the local file.
	 * \return True on success, false on failure.
	 */
	virtual bool copyFrom( const QString & remoteFile, const QString & localFile );

	/**
	 * Copy a local directory recursively to the remote system.
	 *
	 * \note The command is executed asynchronously, see requestComplete() or
	 * waitForCompletion() for results.
	 *
	 * \sa requestSent() requestCompleted() waitForCompeletion()
	 *
	 * \param localDir The path of the local directory.
	 * \param remoteDir The path of the directory on the remote system.
	 * \return True on success, false on failure.
	 */
	virtual bool copyDirTo( const QString & localDir, const QString & remoteDir );

	/**
	 * Copy a remote directory recursively to the local system.
	 *
	 * \note The command is executed asynchronously, see requestComplete() or
	 * waitForCompletion() for results.
	 *
	 * \sa requestSent() requestCompleted() waitForCompeletion()
	 *
	 * \param remoteDir The path of the directory on the remote system.
	 * \param localFile The path of the local directory.
	 * \return True on success, false on failure.
	 */
	virtual bool copyDirFrom( const QString & remoteDir, const QString & localDir );

  signals:
	/**
	 * Emitted when the request has been sent to the server.
	 */
	void requestSent();

	/**
	 * Emitted when the request has been sent and the reply (if any) received.
	 */
	void requestComplete();

  protected:
	static bool debug();
	bool		m_persistent;
	QVariant	m_data;
	QString		m_userName;
	QString		m_hostName;
	QString		m_identityFile;
	int			m_portNumber;
};

#endif
