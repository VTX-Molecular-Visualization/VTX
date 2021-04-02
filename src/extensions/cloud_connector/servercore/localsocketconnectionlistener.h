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

#ifndef MOLEQUEUE_LOCALSOCKETCONNECTIONLISTENER_H
#define MOLEQUEUE_LOCALSOCKETCONNECTIONLISTENER_H

#include "connectionlistener.h"

#include <QtNetwork/QAbstractSocket>

class QLocalServer;
class ServerTest;

namespace MoleQueue
{

/**
 * @class LocalSocketConnectionListener localsocketconnectionlistener.h
 *   <molequeue/servercore/localsocketconnectionlistener.h>
 * @brief Provides a implementation of ConnectionListener using QLocalServer.
 * Each connection made is emitted as a LocalSocketConnection.
 *
 * @see ConnectionListener
 */
class MOLEQUEUESERVERCORE_EXPORT LocalSocketConnectionListener
  : public ConnectionListener
{
  Q_OBJECT
public:

  /**
   * Constructor.
   *
   * @param parentObject parent
   * @param connectionString The address that the QLocalServer should listen on.
   */
  explicit LocalSocketConnectionListener(QObject *parentObject,
                                         const QString &connectionString);

  /**
   * Destructor.
   */
  ~LocalSocketConnectionListener();

  /**
   * Start listening for incoming connections.
   *
   * @see ConnectionListener::start()
   */
  void start();

  /**
   * Stops the connection listener.
   *
   * @param force If true use QLocalServer::removeServer(...) to remove server
   * instance.
   *
   * @see ConnectionListener::stop(bool)
   */
  void stop(bool force);

  /**
   * Calls stop(false)
   *
   * @see stop(bool)
   * @see ConnectionListener::stop()
   */
  void stop();

  /**
   * @return the address the QLocalServer is listening on.
   */
  QString connectionString() const;

  /**
   * @return the full address the QLocalServer is listening on.
   */
  QString fullConnectionString() const;

  /// Used for unit testing
  friend class ::ServerTest;

private slots:
  /**
   * Called when a new connection is established by the QLocalServer.
   */
  void newConnectionAvailable();

private:
  /// Method to map implementation specific error to generic errors.
  ConnectionListener::Error toConnectionListenerError(
      QAbstractSocket::SocketError error);

  /// The address the QLocalServer is listening on.
  QString m_connectionString;

  /// The internal local socket server
  QLocalServer *m_server;
};

} // namespace MoleQueue

#endif // MOLEQUEUE_LOCALSOCKETCONNECTIONLISTENER_H
