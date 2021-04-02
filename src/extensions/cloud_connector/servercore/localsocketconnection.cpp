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

#include "localsocketconnection.h"

#include <QtCore/QDateTime>
#include <QtCore/QTimer>
#include <QtCore/QDataStream>
#include <QtNetwork/QLocalSocket>

namespace MoleQueue
{

LocalSocketConnection::LocalSocketConnection(QObject *parentObject,
                                             QLocalSocket *socket)
  : Connection(parentObject),
    m_connectionString(socket->serverName()),
    m_socket(NULL),
    m_dataStream(new QDataStream),
    m_holdRequests(true)
{
  setSocket(socket);
}

LocalSocketConnection::LocalSocketConnection(QObject *parentObject,
                                             const QString &serverName)
  : Connection(parentObject),
    m_connectionString(serverName),
    m_socket(NULL),
    m_dataStream(new QDataStream),
    m_holdRequests(true)
{
  setSocket(new QLocalSocket);
}

LocalSocketConnection::~LocalSocketConnection()
{
  // Make sure we are closed
  close();

  delete m_socket;
  m_socket = NULL;

  delete m_dataStream;
  m_dataStream = NULL;

}

void LocalSocketConnection::setSocket(QLocalSocket *socket)
{
  if (m_socket != NULL) {
    m_socket->abort();
    m_socket->disconnect(this);
    disconnect(m_socket);
    m_socket->deleteLater();
  }
  if (socket != NULL) {
    connect(socket, SIGNAL(readyRead()),
            this, SLOT(readSocket()));
    connect(socket, SIGNAL(disconnected()),
            this, SIGNAL(disconnected()));
    connect(socket, SIGNAL(destroyed()),
            this, SLOT(socketDestroyed()));
  }
  m_dataStream->setDevice(socket);
  m_dataStream->setVersion(QDataStream::Qt_4_8);
  m_socket = socket;
}

void LocalSocketConnection::readSocket()
{
  if(!m_socket->isValid())
    return;

  if (m_holdRequests)
    return;

  if (m_socket->bytesAvailable() == 0)
    return;

  PacketType packet;
  (*m_dataStream) >> packet;

  emit packetReceived(packet, EndpointIdType());

  // Check again in 50 ms if no more data is available, or immediately if there
  // is. This helps ensure that burst traffic is handled robustly.
  QTimer::singleShot(m_socket->bytesAvailable() > 0 ? 0 : 50,
                     this, SLOT(readSocket()));
}

void LocalSocketConnection::open()
{
  if (m_socket) {
    if(isOpen()) {
      qWarning() << "Socket already connected to" << m_connectionString;
      return;
    }

    m_socket->connectToServer(m_connectionString);
  }
  else {
    qWarning() << "No socket set, connection not opened.";
  }
}

void LocalSocketConnection::start()
{
  if (m_socket) {
    m_holdRequests = false;
    while (m_socket->bytesAvailable() != 0)
      readSocket();
  }
}

void LocalSocketConnection::close()
{
  if(m_socket) {
    if(m_socket->isOpen()) {
      m_socket->disconnectFromServer();
      m_socket->close();
    }
  }
}

bool  LocalSocketConnection::isOpen()
{
  return m_socket != NULL && m_socket->isOpen();
}

QString LocalSocketConnection::connectionString() const
{
  return m_connectionString;
}

bool LocalSocketConnection::send(const PacketType &packet,
                                 const EndpointIdType &endpoint)
{
  Q_UNUSED(endpoint);

  // Because of a possible bug with Qt 5.8 and 5.9 on Windows,
  // (*m_dataStream) << packet
  // sends two packets instead of one. The packets will fail to get read
  // correctly on the other side of the message. To fix this, we write the
  // message to a byte array and send it in all together as a single raw data
  // packet. If this bug gets fixed in the future, we will not need the
  // Windows section...
  // See https://bugreports.qt.io/browse/QTBUG-61097 for the bug report.
#ifdef _WIN32
  PacketType byteArray;
  QDataStream tmpStream(&byteArray, QIODevice::WriteOnly);
  tmpStream << packet;
  m_dataStream->writeRawData(byteArray, byteArray.size());
#else
  (*m_dataStream) << packet;
#endif

  return true;
}

void LocalSocketConnection::flush()
{
  m_socket->flush();
}

void LocalSocketConnection::socketDestroyed()
{
  // Set to NULL so we know we don't need to clean up
  m_socket = NULL;
  // Tell anyone listening we have been disconnected.
  emit disconnected();
}

} /* namespace MoleQueue */
