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

#ifndef OPENSSHCOMMAND_H
#define OPENSSHCOMMAND_H

#include "sshcommand.h"

namespace MoleQueue {

class TerminalProcess;


/**
 * @class OpenSshCommand opensshcommand.h <molequeue/opensshcommand.h>
 * @brief Concrete implementation of SshCommand using commandline open ssh/scp.
 * @author Marcus D. Hanwell, David C. Lonie, Chris Harris
 *
 * The OpenSshCommand provides an implementation of the SshCommand interface
 * that calls the commandline ssh and scp executables in a TerminalProcess.
 *
 * When writing code that needs ssh functionality, the code should use the
 * SshConnection interface instead.
 */
class OpenSshCommand : public SshCommand
{
  Q_OBJECT

public:
  OpenSshCommand(QObject *parentObject = 0);
  ~OpenSshCommand();

protected:

  /// @return the arguments to be passed to the SSH command.
  QStringList sshArgs();

  /// @return the arguments to be passed to the SCP command.
  QStringList scpArgs();
};

} // End namespace

#endif

