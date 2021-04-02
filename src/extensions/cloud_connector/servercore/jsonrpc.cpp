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

#include "jsonrpc.h"
#include "connectionlistener.h"

#include <QtCore/QMetaType>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>

namespace MoleQueue {

JsonRpc::JsonRpc(QObject *parent_) :
  QObject(parent_)
{
  qRegisterMetaType<Message>("MoleQueue::Message");
  qRegisterMetaType<PacketType>("MoleQueue::PacketType");
  qRegisterMetaType<EndpointIdType>("MoleQueue::EndpointIdType");
}

JsonRpc::~JsonRpc()
{
}

void JsonRpc::addConnectionListener(ConnectionListener *connlist)
{
  if (m_connections.keys().contains((connlist)))
    return;

  m_connections.insert(connlist, QList<Connection*>());
  connect(connlist, SIGNAL(newConnection(MoleQueue::Connection*)),
          SLOT(addConnection(MoleQueue::Connection*)));
  connect(connlist, SIGNAL(destroyed()),
          SLOT(removeConnectionListenerInternal()));
}

void JsonRpc::removeConnectionListener(ConnectionListener *connlist)
{
  disconnect(0, connlist);
  foreach(Connection *conn, m_connections.value(connlist))
    this->removeConnection(connlist, conn);

  m_connections.remove(connlist);
}

void JsonRpc::addConnection(Connection *conn)
{
  ConnectionListener *connlist = qobject_cast<ConnectionListener*>(sender());

  if (!connlist || !m_connections.keys().contains(connlist))
    return;

  QList<Connection*> &conns = m_connections[connlist];
  if (conns.contains(conn))
    return;

  conns << conn;

  connect(conn, SIGNAL(destroyed()), SLOT(removeConnection()));
  connect(conn, SIGNAL(packetReceived(MoleQueue::PacketType,
                                      MoleQueue::EndpointIdType)),
          SLOT(newPacket(MoleQueue::PacketType,MoleQueue::EndpointIdType)));

  conn->start();
}

void JsonRpc::removeConnection(ConnectionListener *connlist, Connection *conn)
{
  disconnect(0, conn);

  if (!m_connections.contains(connlist))
    return;

  QList<Connection*> &conns = m_connections[connlist];
  conns.removeOne(conn);
}

void JsonRpc::removeConnection(Connection *conn)
{
  // Find the connection listener:
  foreach (ConnectionListener *connlist, m_connections.keys()) {
    if (m_connections[connlist].contains(conn)) {
      removeConnection(connlist, conn);
      return;
    }
  }
}

void JsonRpc::removeConnection()
{
  // Use a reinterpret cast -- this is connected to a QObject::destroyed()
  // signal, and we can't qobject_cast to a Connection* after the Connection
  // destructor has run. Since this is a private slot, we can ensure that only
  // Connections will be connected to it. This pointer is never dereferenced
  // and is ignored if it can't be found in m_connections, so there are no ill
  // effects if sender() is not a Connection (other than a few wasted cycles).
  if (Connection *conn = reinterpret_cast<Connection*>(sender()))
    removeConnection(conn);
}

void JsonRpc::removeConnectionListenerInternal()
{
  // Use a reinterpret cast -- this is connected to a QObject::destroyed()
  // signal, and we can't qobject_cast to a ConnectionListener* after the
  // ConnectionListener destructor has run. Since this is a private slot, we can
  // ensure that only ConnectionListeners will be connected to it. This pointer
  // is never dereferenced and is ignored if it can't be found in m_connections,
  // so there are no ill effects if sender() is not a ConnectionListener
  // (other than a few wasted cycles).
  if (ConnectionListener *cl = reinterpret_cast<ConnectionListener*>(sender()))
    removeConnectionListener(cl);
}

void JsonRpc::newPacket(const MoleQueue::PacketType &packet,
                        const MoleQueue::EndpointIdType &endpoint)
{
  Connection *conn = qobject_cast<Connection*>(sender());
  if (!conn)
    return;

  // Parse the packet as JSON
  QJsonParseError error;
  QJsonDocument doc = QJsonDocument::fromJson(QByteArray(packet), &error);

  // Send a server error and return if there was an issue parsing the packet.
  if (error.error != QJsonParseError::NoError || doc.isNull()) {
    Message errorMessage(Message::Error, conn, endpoint);
    errorMessage.setErrorCode(-32700);
    errorMessage.setErrorMessage("Parse error");

    QJsonObject errorDataObject;
    errorDataObject.insert("QJsonParseError::error", error.error);
    errorDataObject.insert("QJsonParseError::errorString", error.errorString());
    errorDataObject.insert("QJsonParseError::offset", error.offset);
    errorDataObject.insert("bytes received", QLatin1String(packet.constData()));
    errorMessage.send();
    return;
  }

  // Pass the JSON off for further processing. Must be an array or object.
  handleJsonValue(conn, endpoint,
                  doc.isArray() ? QJsonValue(doc.array())
                                : QJsonValue(doc.object()));
}

void JsonRpc::handleJsonValue(Connection *conn, const EndpointIdType &endpoint,
                              const QJsonValue &json)
{
  // Handle batch requests recursively
  if (json.isArray()) {
    /// @todo Stage batch replies into an array before sending.
    foreach (const QJsonValue &val, json.toArray())
      handleJsonValue(conn, endpoint, val);
    return;
  }

  // Objects are RPC calls
  if (!json.isObject()) {
    Message errorMessage(Message::Error, conn, endpoint);
    errorMessage.setErrorCode(-32600);
    errorMessage.setErrorMessage("Invalid Request");

    QJsonObject errorDataObject;
    errorDataObject.insert("description", QLatin1String("Request is not a JSON "
                                                        "object."));
    errorDataObject.insert("request", json);
    errorMessage.send();
    return;
  }

  Message message(json.toObject(), conn, endpoint);
  Message errorMessage;
  if (!message.parse(errorMessage)) {
    errorMessage.send();
    return;
  }

  // Handle ping requests internally
  if (message.type() == Message::Request
      && message.method() == "internalPing") {
    Message response = message.generateResponse();
    response.setResult(QLatin1String("pong"));
    response.send();
    return;
  }

  emit messageReceived(message);
}

} // namespace MoleQueue
