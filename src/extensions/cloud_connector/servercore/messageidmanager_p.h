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

#ifndef MOLEQUEUE_MESSAGEIDMANAGER_P_H
#define MOLEQUEUE_MESSAGEIDMANAGER_P_H

#include "servercoreglobal.h"

#include <QtCore/QMap>
#include <QtCore/QString>

namespace MoleQueue {

/**
 * @brief The MessageIdManager class provides a static lookup table that is used
 * to identify replies to JSON-RPC requests.
 * @author David C. Lonie
 */
class MessageIdManager
{
public:
  /**
   * @brief registerMethod Request a new message id that is assocated with @a
   * method. The new id and method will be registered in the lookup table.
   * @return The assigned message id.
   */
  static MessageIdType registerMethod(const QString &method);

  /**
   * @brief lookupMethod Determine the method assocated with the @a id.
   * @note This removes the id from the internal lookup table.
   * @return The method assocated with the given id.
   */
  static QString lookupMethod(const MessageIdType &id);

private:
  MessageIdManager();
  static void init();
  static void cleanup();

  static MessageIdManager *m_instance;
  QMap<double, QString> m_lookup;
  double m_generator;
};

}

#endif // MOLEQUEUE_MESSAGEIDMANAGER_P_H
