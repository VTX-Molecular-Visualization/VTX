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

#ifndef SSHCOMMAND_H
#define SSHCOMMAND_H

#include "sshconnection.h"
#include <QtCore/QStringList>

class TerminalProcess;

/**
 * @class SshCommand sshcommand.h <molequeue/sshcommand.h>
 * @brief Abstract subclass of SshConnection providing base implementaton using
 * commandline ssh/scp.
 *
 * @author Marcus D. Hanwell, David C. Lonie, Chris Harris
 *
 * The SshCommand provides an base implementation of the SshConnection interface
 * that calls the commandline ssh and scp executables in a TerminalProcess.
 *
 * When writing code that needs ssh functionality, the code should use the
 * SshConnection interface instead.
 */
class SshCommand : public SshConnection
{
	Q_OBJECT

  public:
	SshCommand( QObject * parentObject, QString sshCommand, QString scpCommand );
	~SshCommand();

	/** \return The SSH command that will be run. */
	QString sshCommand() { return m_sshCommand; }

	/** \return The SCP command that will be run. */
	QString scpCommand() { return m_scpCommand; }

	/** \return The merged stdout and stderr of the remote command */
	QString output() const;

	/** \return The exit code returned from the remote command. */
	int exitCode() const;

	/**
	 * Wait until the request has been completed.
	 *
	 * @param msecs Timeout in milliseconds. Default is 30 seconds.
	 *
	 * @return True if request finished, false on timeout.
	 */
	bool waitForCompletion( int msecs = 30000 );

	/** @return True if the request has completed. False otherwise. */
	bool isComplete() const;

  public slots:
	/**
	 * Set the SSH command for the class. Defaults to 'ssh', and would execute
	 * the SSH commnand in the user's path.
	 */
	void setSshCommand( const QString & command ) { m_sshCommand = command; }

	/**
	 * Set the SCP command for the class. Defaults to 'scp', and would execute
	 * the SCP commnand in the user's path.
	 */
	void setScpCommand( const QString & command ) { m_scpCommand = command; }

	/**
	 * Execute the supplied command on the remote host.
	 *
	 * \note The command is executed asynchronously, see requestComplete() or
	 * waitForCompletion() for results.
	 *
	 * \sa requestSent() requestCompleted() waitForCompeletion()
	 *
	 * \param command The command to execute.
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

  protected slots:

	/// Called when the TerminalProcess enters the Running state.
	void processStarted();

	/// Called when the TerminalProcess exits the Running state.
	void processFinished();

  protected:
	/// Send a request. This launches the process and connects the completion
	/// signals
	virtual void sendRequest( const QString & command, const QStringList & args );

	/// Initialize the TerminalProcess object.
	void initializeProcess();

	/// @return the arguments to be passed to the SSH command.
	virtual QStringList sshArgs() = 0;

	/// @return the arguments to be passed to the SCP command.
	virtual QStringList scpArgs() = 0;

	/// @return the remote specification, e.g. "user@host" or "host"
	QString remoteSpec();

	QString			  m_sshCommand;
	QString			  m_scpCommand;
	QString			  m_output;
	int				  m_exitCode;
	TerminalProcess * m_process;
	bool			  m_isComplete;
};

#endif
