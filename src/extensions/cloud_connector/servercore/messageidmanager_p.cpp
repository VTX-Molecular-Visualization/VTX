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

#include "messageidmanager_p.h"

#include "message.h"

#include <cstdlib>

namespace MoleQueue {

MessageIdManager *MessageIdManager::m_instance = NULL;

MessageIdManager::MessageIdManager()
  : m_generator(0)
{
  // Clean up when program exits.
  atexit(&cleanup);
}

void MessageIdManager::init()
{
  if (!m_instance)
    m_instance = new MessageIdManager();
}

void MessageIdManager::cleanup()
{
  delete m_instance;
  m_instance = NULL;
}

MessageIdType MessageIdManager::registerMethod(const QString &method)
{
  init();
  double result = ++m_instance->m_generator;
  m_instance->m_lookup.insert(result, method);
  return MessageIdType(result);
}

QString MessageIdManager::lookupMethod(const MessageIdType &id)
{
  init();
  return id.isDouble() ? m_instance->m_lookup.take(id.toDouble()) : QString();
}

} // end namespace MoleQueue
