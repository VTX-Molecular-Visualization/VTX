/******************************************************************************

  This source file is part of the MoleQueue project.

  Copyright 2011-2012 Kitware, Inc.

  This source code is released under the New BSD License, (the "License").

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

******************************************************************************/

#include "program.h"

#include "logger.h"
#include "queue.h"
#include "queues/remote.h"
#include "queuemanager.h"
#include "server.h"

#include <qjsondocument.h>

#include <QtCore/QDebug>
#include <QtCore/QFileInfo>

namespace MoleQueue {

Program::Program(Queue *parentQueue) :
  QObject(parentQueue),
  m_queue(parentQueue),
  m_queueManager((m_queue) ? m_queue->queueManager() : NULL),
  m_server((m_queueManager) ? m_queueManager->server() : NULL),
  m_name("Program"),
  m_executable("program"),
  m_arguments(),
  m_outputFilename("$$inputFileBaseName$$.out"),
  m_launchSyntax(REDIRECT),
  m_customLaunchTemplate("")
{
}

Program::Program(const Program &other)
  : QObject(other.parent()),
    m_queue(other.m_queue),
    m_name(other.m_name),
    m_executable(other.m_executable),
    m_arguments(other.m_arguments),
    m_outputFilename(other.m_outputFilename),
    m_launchSyntax(other.m_launchSyntax),
    m_customLaunchTemplate(other.m_customLaunchTemplate)
{
}

Program::~Program()
{
}

Program &Program::operator=(const Program &other)
{
  m_queue = other.m_queue;
  m_name = other.m_name;
  m_executable = other.m_executable;
  m_arguments = other.m_arguments;
  m_outputFilename = other.m_outputFilename;
  m_launchSyntax = other.m_launchSyntax;
  m_customLaunchTemplate = other.m_customLaunchTemplate;
  return *this;
}

QString Program::queueName() const
{
  if (m_queue)
    return m_queue->name();
  else
    return "None";
}

bool Program::importSettings(const QString &fileName)
{
  if (!QFile::exists(fileName))
    return false;

  QFile stateFile(fileName);
  if (!stateFile.open(QFile::ReadOnly | QFile::Text)) {
    Logger::logError(tr("Cannot read program information from %1.")
                     .arg(fileName));
    return false;
  }
  QByteArray inputText = stateFile.readAll();
  stateFile.close();

  // Try to read existing data in
  QJsonParseError error;
  QJsonDocument doc = QJsonDocument::fromJson(inputText, &error);
  if (error.error != QJsonParseError::NoError) {
    Logger::logError(tr("Error parsing program state from %1: %2\n%3")
                     .arg(fileName)
                     .arg(tr("%1 (at offset %2)")
                          .arg(error.errorString())
                          .arg(error.offset))
                     .arg(inputText.data()));
    return false;
  }

  if (!doc.isObject()) {
    Logger::logError(tr("Error reading program state from %1: "
                        "root is not an object!\n%2")
                     .arg(fileName)
                     .arg(inputText.data()));
    return false;
  }

  return readJsonSettings(doc.object(), true);
}

bool Program::exportSettings(const QString &fileName) const
{
  QFile stateFile(fileName);
  if (!stateFile.open(QFile::ReadWrite | QFile::Text | QFile::Truncate)) {
    Logger::logError(tr("Cannot save program information for %1 in queue %2: "
                        "Cannot open file %3.").arg(name())
                     .arg(m_queue->name()).arg(fileName));
    return false;
  }

  QJsonObject root;
  if (!this->writeJsonSettings(root, true)) {
    stateFile.close();
    return false;
  }

  // Write the data back out:
  stateFile.write(QJsonDocument(root).toJson());
  stateFile.close();

  return true;
}

bool Program::writeJsonSettings(QJsonObject &json, bool exportOnly) const
{
  // No export sensitive data.
  Q_UNUSED(exportOnly)

  json.insert("executable", m_executable);
  json.insert("arguments", m_arguments);
  json.insert("outputFilename", m_outputFilename);
  json.insert("customLaunchTemplate", m_customLaunchTemplate);
  json.insert("launchSyntax", static_cast<double>(m_launchSyntax));

  return true;
}

bool Program::readJsonSettings(const QJsonObject &json, bool importOnly)
{
  // No import sensitive data.
  Q_UNUSED(importOnly)

  // Validate JSON
  if (!json.value("executable").isString() ||
      !json.value("arguments").isString() ||
      !json.value("outputFilename").isString() ||
      !json.value("customLaunchTemplate").isString() ||
      !json.value("launchSyntax").isDouble()) {
    Logger::logError(tr("Error reading program config: Invalid format:\n%1")
                     .arg(QString(QJsonDocument(json).toJson())));
    return false;
  }

  m_executable = json.value("executable").toString();
  m_arguments = json.value("arguments").toString();
  m_outputFilename = json.value("outputFilename").toString();
  m_customLaunchTemplate = json.value("customLaunchTemplate").toString();
  m_launchSyntax =
      static_cast<LaunchSyntax>(
        static_cast<int>(json.value("launchSyntax").toDouble() + 0.5));

  return true;
}

QString Program::launchTemplate() const
{
  if (m_launchSyntax == CUSTOM)
    return m_customLaunchTemplate;

  QString result = m_queue ? m_queue->launchTemplate()
                           : QString("$$programExecution$$");
  if (result.contains("$$programExecution$$")) {
    const QString progExec = Program::generateFormattedExecutionString(
          m_executable, m_arguments, m_outputFilename, m_launchSyntax);
    result.replace("$$programExecution$$", progExec);
  }
  if (QueueRemote *remoteQueue = qobject_cast<QueueRemote*>(m_queue)) {
    if (result.contains("$$remoteWorkingDir$$")) {
      const QString remoteWorkingDir = QString("%1/%2/")
          .arg(remoteQueue->workingDirectoryBase())
          .arg("$$moleQueueId$$");
      result.replace("$$remoteWorkingDir$$", remoteWorkingDir);
    }
  }

  return result;
}

QString Program::generateFormattedExecutionString(
    const QString &executable_, const QString &arguments_,
    const QString &outputFilename_, Program::LaunchSyntax syntax_)
{
  if (syntax_ == Program::CUSTOM) {
    return "";
  }

  QString execStr;
  QString command = executable_ + (arguments_.isEmpty() ? QString()
                                                        : " " + arguments_);

  switch (syntax_) {
  case Program::PLAIN:
    execStr += command;
    break;
  case Program::INPUT_ARG:
    execStr += QString("%1 $$inputFileName$$\n").arg(command);
    break;
  case Program::INPUT_ARG_NO_EXT:
  {
    execStr += QString("%1 $$inputFileBaseName$$\n").arg(command);
  }
    break;
  case Program::REDIRECT:
    execStr += QString("%1 < $$inputFileName$$ > %3\n")
        .arg(command).arg(outputFilename_);
    break;
  case Program::INPUT_ARG_OUTPUT_REDIRECT:
    execStr += QString("%1 $$inputFileName$$ > %3\n")
        .arg(command).arg(outputFilename_);
    break;
  case Program::CUSTOM:
    // Should be handled as a special case earlier.
    execStr = tr("Internal MoleQueue error: Custom syntax type not handled.\n");
    break;
  default:
    execStr = tr("Internal MoleQueue error: Unrecognized syntax type.\n");
    break;
  }

  return execStr;
}

} // End namespace
