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

#ifndef MOLEQUEUE_LOCALSOCKETCONNECTION_H
#define MOLEQUEUE_LOCALSOCKETCONNECTION_H

#include "connection.h"

class QLocalSocket;

namespace MoleQueue
{

/**
 * @class LocalSocketConnection localsocketconnection.h
 *   <molequeue/servervcore/localsocketconnection.h>
 * @brief Provides am implementation of @Connection using QLocalSockets. Each
 * instance of the class wraps a QLocalSocket.
 */
class MOLEQUEUESERVERCORE_EXPORT LocalSocketConnection : public Connection
{
  Q_OBJECT
public:

  /**
   * Constructor used by @LocalSocketConnectionListener to create a new connection
   * based on an existing QLocalSocket.
   *
   * @param parentObject parent
   * @param socket The socket that this connection instance will operate on.
   */
  explicit LocalSocketConnection(QObject *parentObject, QLocalSocket *socket);

  /**
   * Constructor used by a client to connection to a server ( @ConnectionListener )
   *
   * @param parentObject parent
   * @param connectionString The "address" of server to connect to.
   */
  explicit LocalSocketConnection(QObject *parentObject,
                                 const QString &connectionString);
  /**
   * Destructor.
   */
  ~LocalSocketConnection();

  /**
   * Opens the connection the server i.e. QLocalSocket::connectToServer(...)
   *
   * @see Connection::open()
   */
  void open();

  /**
   * Start receiving messages on this connection.
   *
   * @see Connection::start
   */
  void start();

  /**
   * Close the underlying socket. Once closed the connection can no longer be used
   * to receive or send messages.
   *
   * @see Connection::close()
   */
  void close();

  /**
   * @return true is connection is open, false otherwise.
   *
   * @see Connection::isOpen()
   */
  bool isOpen();

  /**
   * @return The serverName from the underlying socket.
   *
   * @see Connection::connectionString()
   */
  QString connectionString() const;

  bool send(const PacketType &packet, const EndpointIdType &endpoint);

  void flush();

private slots:

  /**
   * Read data from the local socket until a complete packet has been obtained.
   */
  void readSocket();

  /**
   * Called when the underlying QLocalSocket is destroyed. This happens when
   * the connection listener associated with it is deleted.
   */
  void socketDestroyed();

private:

  /**
   * Sets the underlying local socket for this connection.
   *
   * @param socket The local socket to use on this connection.
   */
  void setSocket(QLocalSocket *socket);

  /// The address the socket is connected to.
  QString m_connectionString;

  /// The underlying local socket
  QLocalSocket *m_socket;

  /// The data stream used to interface with the local socket
  QDataStream *m_dataStream;

  /// If true, do not read incoming packets from the socket. This is to let
  /// the parent server create connections prior to processing requests.
  bool m_holdRequests;
};

} // namespace MoleQueue

#endif // MOLEQUEUE_LOCALSOCKETCONNECTION_H
