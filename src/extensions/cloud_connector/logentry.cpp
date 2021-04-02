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

#include "logentry.h"

#include "idtypeutils.h"

#include <qjsonobject.h>

namespace MoleQueue
{

LogEntry::LogEntry(LogEntryType type, const QString &message_,
                   const IdType &moleQueueId_)
  : m_message(message_),
    m_moleQueueId(moleQueueId_),
    m_entryType(type)
{
}

LogEntry::LogEntry(const QJsonObject &json)
  : m_message(json.value("message").isString()
              ? json.value("message").toString() : QString("Invalid JSON!")),
    m_moleQueueId(toIdType(json.value("moleQueueId"))),
    m_entryType(json.value("entryType").isDouble()
                ? static_cast<LogEntryType>(
                    static_cast<int>(json.value("entryType").toDouble() + 0.5))
                : Error),
    m_timeStamp(json.value("time").isString()
                ? QDateTime::fromString(json.value("time").toString())
                : QDateTime())
{
}

LogEntry::LogEntry(const LogEntry &other)
  : m_message(other.m_message),
    m_moleQueueId(other.m_moleQueueId),
    m_entryType(other.m_entryType),
    m_timeStamp(other.m_timeStamp)
{
}

LogEntry::~LogEntry()
{
}

void LogEntry::writeSettings(QJsonObject &root) const
{
  root.insert("message", m_message);
  root.insert("moleQueueId", idTypeToJson(m_moleQueueId));
  root.insert("entryType", static_cast<double>(m_entryType));
  root.insert("time", m_timeStamp.toString());
}

void LogEntry::setTimeStamp()
{
  m_timeStamp = QDateTime::currentDateTime();
}

const QDateTime &LogEntry::timeStamp() const
{
  return m_timeStamp;
}

} // namespace MoleQueue
