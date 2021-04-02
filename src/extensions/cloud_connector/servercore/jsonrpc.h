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

#ifndef MOLEQUEUE_JSONRPC_H
#define MOLEQUEUE_JSONRPC_H

#include "message.h"
#include "servercoreglobal.h"

#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QObject>

class JsonRpcTest;

namespace MoleQueue {
class Connection;
class ConnectionListener;

/**
 * @class JsonRpc jsonrpc.h <molequeue/servercore/jsonrpc.h>
 * @brief The JsonRpc class manages ConnectionListener and Connection instances,
 * and emits incoming JSON-RPC Messages.
 * @author David C. Lonie
 *
 * To use the JsonRpc class, create one or more ConnectionListener instances
 * and call addConnectionListener(). Connect a slot to messageReceived and
 * handle any incoming messages.
 *
 * This class will handle the following standard JSON-RPC errors:
 *
 * - -32600 Invalid request
 *   - The message type could not be determined.
 * - -32603 Internal error
 *   - Internal JSON-RPC error
 * - -32700 Parse error
 *   - Invalid JSON received, an error occurred during parsing.
 *
 * The remaining standard JSON-RPC error codes should be handled by the
 * application developer when messageReceived:
 *
 * - -32601 Method not found
 *   - Method not supported by application
 * - -32602 Invalid params
 *   - Inappropriate parameters supplied for requested method.
 *
 * Incoming requests with method="internalPing" will be automatically replied
 * to with result="pong". This can be used to test if a server is alive or not.
 * messageReceived will not be emitted in this case.
 *
 * Use Message::generateResponse() and Message::generateErrorResponse() to
 * easily create replies to incoming requests.
 */
class MOLEQUEUESERVERCORE_EXPORT JsonRpc : public QObject
{
  Q_OBJECT
public:
  friend class ::JsonRpcTest;

  explicit JsonRpc(QObject *parent_ = 0);
  ~JsonRpc();

  /**
   * @brief Register a connection listener with this
   * JsonRpc instance. Any incoming connections on the listener will be
   * monitored by this class and all incoming messages will be treated as
   * JSON-RPC transmissions. This class does not take ownership of the listener,
   * and will automatically remove it from any internal data structures if
   * it is destroyed.
   */
  void addConnectionListener(MoleQueue::ConnectionListener *connlist);

  /**
   * @brief Unregister a connection listener from this
   * JsonRpc instance. Any connections owned by this listener will be
   * unregistered as well.
   * @param connlist
   */
  void removeConnectionListener(MoleQueue::ConnectionListener *connlist);

signals:
  /**
   * @brief Emitted when a valid message is received.
   */
  void messageReceived(const MoleQueue::Message &message);

private slots:
  /**
   * @brief Register a connection with this JsonRpc instance.
   * @note The sender must be the connection listener (connect to
   * MoleQueue::ConnectionListener::newConnection(MoleQueue::Connection*))
   */
  void addConnection(MoleQueue::Connection *conn);

  /**
   * @brief Unregister a connection with this JsonRpc instance.
   * @param connlist The connection listener which owns the connection.
   * @return True on success, false on failure (e.g. connection not found).
   */
  void removeConnection(MoleQueue::ConnectionListener *connlist,
                        MoleQueue::Connection *conn);

  /**
   * @brief Unregister a connection with this JsonRpc instance.
   * @return True on success, false on failure (e.g. connection not found).
   * @overload
   */
  void removeConnection(MoleQueue::Connection *conn);

  /**
   * @brief Unregister a connection from this JsonRpc instance.
   * The sender that triggers this slot must be a subclass of
   * Connection, which will be removed.
   * @overload
   */
  void removeConnection();

  /**
   * @brief Unregister a connection listener
   * from this JsonRpc instance. The sender that triggers this slot must be a
   * subclass of Connection, which will be removed.
   * @overload
   */
  void removeConnectionListenerInternal();

  /**
   * @brief Called when a registered connection emits a new packet.
   * The packet is parsed into JSON and split if it is a batch request. Each
   * requested is parsed into a Message and messageReceived is emitted.
   * @note The sender must be the connection from which the packet originates.
   */
  void newPacket(const MoleQueue::PacketType &packet,
                 const MoleQueue::EndpointIdType &endpoint);

private:
  /**
   * Helper function for newPacket.
   */
  void handleJsonValue(Connection *conn, const EndpointIdType &endpoint,
                       const QJsonValue &json);

  /// Container of all known connections and listeners.
  QMap<ConnectionListener*, QList<Connection*> > m_connections;
};

} // namespace MoleQueue

#endif // MOLEQUEUE_JSONRPC_H
