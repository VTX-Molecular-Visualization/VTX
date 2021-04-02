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

#include "opensshcommand.h"
#include "terminalprocess.h"
#include <QtCore/QProcessEnvironment>
#include <QtCore/QDir>
#include <QtCore/QDebug>

namespace MoleQueue {

OpenSshCommand::OpenSshCommand(QObject *parentObject) : SshCommand(parentObject,
                                                                   "ssh", "scp")
{
}

OpenSshCommand::~OpenSshCommand()
{
}

QStringList OpenSshCommand::sshArgs()
{
  QStringList args;
  // Suppress login banners
  args << "-q";
  if (!m_identityFile.isEmpty())
    args << "-i" << m_identityFile;
  if (m_portNumber >= 0 && m_portNumber != 22)
    args << "-p" << QString::number(m_portNumber);

  return args;
}

QStringList OpenSshCommand::scpArgs()
{
  QStringList args;
  // Suppress login banners
  args << "-q";
  // Ensure the same SSH used for commands is used by scp.
  args << "-S" << m_sshCommand;
  if (!m_identityFile.isEmpty())
    args << "-i" << m_identityFile;
  if (m_portNumber >= 0 && m_portNumber != 22)
    args << "-P" << QString::number(m_portNumber);
  return args;
}

} // End namespace
