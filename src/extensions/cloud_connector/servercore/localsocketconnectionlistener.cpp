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

#include "localsocketconnectionlistener.h"
#include "localsocketconnection.h"

#include <QtNetwork/QLocalServer>
#include <QtNetwork/QLocalSocket>

namespace MoleQueue
{

LocalSocketConnectionListener::LocalSocketConnectionListener(QObject *parentObject,
                                                             const QString &connString)
  : ConnectionListener(parentObject),
    m_connectionString(connString),
    m_server(new QLocalServer ())
{
  connect(m_server, SIGNAL(newConnection()),
          this, SLOT(newConnectionAvailable()));
}

LocalSocketConnectionListener::~LocalSocketConnectionListener()
{
  // Make sure we are stopped
  stop();

  delete m_server;
  m_server = NULL;
}

void LocalSocketConnectionListener::start()
{
  if (!m_server->listen(m_connectionString)) {
    emit connectionError(toConnectionListenerError(m_server->serverError()),
               m_server->errorString());
    return;
  }
}

void LocalSocketConnectionListener::stop(bool force)
{
  if (force)
    QLocalServer::removeServer(m_connectionString);

  if (m_server)
    m_server->close();
}

void LocalSocketConnectionListener::stop()
{
  stop(false);
}

QString LocalSocketConnectionListener::connectionString() const
{
  return m_connectionString;
}

QString LocalSocketConnectionListener::fullConnectionString() const
{
  return m_server->fullServerName();
}

void LocalSocketConnectionListener::newConnectionAvailable()
{
  if (!m_server->hasPendingConnections())
    return;

  QLocalSocket *socket = m_server->nextPendingConnection();

  LocalSocketConnection *conn = new LocalSocketConnection(this, socket);

  emit newConnection(conn);
}

ConnectionListener::Error LocalSocketConnectionListener::toConnectionListenerError(
    QAbstractSocket::SocketError socketError)
{
  ConnectionListener::Error listenerError = UnknownError;

  switch (socketError) {
    case QAbstractSocket::AddressInUseError:
      listenerError = ConnectionListener::AddressInUseError;
      break;
    default:
      // UnknownError
      break;
  }

  return listenerError;
}

} /* namespace MoleQueue */
