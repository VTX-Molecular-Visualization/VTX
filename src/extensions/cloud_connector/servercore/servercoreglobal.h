/******************************************************************************

  This source file is part of the MoleQueue project.

  Copyright 2013 Kitware, Inc.

  This source code is released under the New BSD License, (the "License").

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

******************************************************************************/

#ifndef MOLEQUEUE_SERVERCORE_SERVERCOREGLOBAL_H
#define MOLEQUEUE_SERVERCORE_SERVERCOREGLOBAL_H

#include <QtCore/QJsonValue>
#include <QtCore/QByteArray>

namespace MoleQueue
{

/// Type for Endpoint identifiers
typedef QByteArray EndpointIdType;

/// Type for Message identifiers (JSON-RPC ids)
typedef QJsonValue MessageIdType;

/// Type for RPC packets
typedef QByteArray PacketType;

}

#endif // MOLEQUEUE_SERVERCORE_SERVERCOREGLOBAL_H
