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

#ifndef SSHCOMMANDFACTORY_H
#define SSHCOMMANDFACTORY_H

#include "ssh_command.h"

/**
 * @class SshCommandFactory sshcommandfactory.h <molequeue/sshcommandfactory.h>
 * @brief Used to construct the correct SshCommand implementation based on the
 * ssh client
 *
 * @author Chris Harris
 *
 */
class SshCommandFactory : public QObject
{
	Q_OBJECT

  public:
	/// Ssh clients
	enum SshClient
	{
		OpenSsh
#ifdef _WIN32
		,
		Putty
#endif
	};

	static SshCommandFactory * instance();
	static QString			   defaultSshCommand();
	static QString			   defaultScpCommand();

	/**
	 * @return a new SshCommand for this platform, the caller is responsible
	 * for cleanup
	 */
	SshCommand * newSshCommand( QObject * parentObject = 0 );
	SshCommand * newSshCommand( SshClient sshClient, QObject * parentObject = 0 );

  private:
	SshCommandFactory( QObject * parentObject = 0 );
};

#endif
