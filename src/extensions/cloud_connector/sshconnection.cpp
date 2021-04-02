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

#include "sshconnection.h"

#include <cstdlib>

namespace MoleQueue {

SshConnection::SshConnection(QObject *parentObject) : QObject(parentObject),
  m_persistent(false), m_portNumber(-1)
{
}

SshConnection::~SshConnection()
{
}

bool SshConnection::isValid() const
{
  if (m_hostName.isEmpty())
    return false;
  else
    return true;
}

QString SshConnection::output() const
{
  return "";
}

int SshConnection::exitCode() const
{
  return -1;
}

bool SshConnection::waitForCompletion(int)
{
  return false;
}

bool SshConnection::isComplete() const
{
  return false;
}

bool SshConnection::execute(const QString &)
{
  // Always fails in the base class - no valid transport.
  return false;
}

bool SshConnection::copyTo(const QString &, const QString &)
{
  return false;
}

bool SshConnection::copyFrom(const QString &, const QString &)
{
  return false;
}

bool SshConnection::copyDirTo(const QString &, const QString &)
{
  return false;
}

bool SshConnection::copyDirFrom(const QString &, const QString &)
{
  return false;
}

bool SshConnection::debug()
{
  const char *val = qgetenv("MOLEQUEUE_DEBUG_SSH");
  return (val != NULL && val[0] != '\0');
}

} // End of namespace
