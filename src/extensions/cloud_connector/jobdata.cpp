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

#include "jobdata.h"
#include "jobmanager.h"
#include "logger.h"

#include <qjsonarray.h>
#include <qjsondocument.h>

#include <QtCore/QDir>
#include <QtCore/QFile>

namespace MoleQueue
{

JobData::JobData(JobManager *parentManager)
  : m_jobManager(parentManager),
    m_jobState(MoleQueue::None),
    m_cleanRemoteFiles(false),
    m_retrieveOutput(true),
    m_cleanLocalWorkingDirectory(false),
    m_hideFromGui(false),
    m_popupOnStateChange(false),
    m_numberOfCores(DEFAULT_NUM_CORES),
    m_maxWallTime(-1), // use default queue time
    m_moleQueueId(InvalidId),
    m_queueId(InvalidId),
    m_needsSync(true)
{
}

JobData::JobData(const MoleQueue::JobData &other)
  : m_jobManager(other.m_jobManager),
    m_queue(other.m_queue),
    m_program(other.m_program),
    m_jobState(other.m_jobState),
    m_description(other.m_description),
    m_inputFile(other.m_inputFile),
    m_additionalInputFiles(other.m_additionalInputFiles),
    m_outputDirectory(other.m_outputDirectory),
    m_localWorkingDirectory(other.m_localWorkingDirectory),
    m_cleanRemoteFiles(other.m_cleanRemoteFiles),
    m_retrieveOutput(other.m_retrieveOutput),
    m_cleanLocalWorkingDirectory(other.m_cleanLocalWorkingDirectory),
    m_hideFromGui(other.m_hideFromGui),
    m_popupOnStateChange(other.m_popupOnStateChange),
    m_numberOfCores(other.m_numberOfCores),
    m_maxWallTime(other.m_maxWallTime),
    m_moleQueueId(other.m_moleQueueId),
    m_queueId(other.m_queueId),
    m_needsSync(true)
{
}

QJsonObject JobData::toJsonObject() const
{
  QJsonObject result;

  result.insert("queue", m_queue);
  result.insert("program", m_program);
  result.insert("jobState", QLatin1String(jobStateToString(m_jobState)));
  result.insert("description", m_description);
  result.insert("inputFile", m_inputFile.toJsonObject());
  if (!m_additionalInputFiles.isEmpty()) {
    QJsonArray additionalFiles;
    foreach (const FileSpecification &spec, m_additionalInputFiles)
      additionalFiles.append(spec.toJsonObject());
    result.insert("additionalInputFiles", additionalFiles);
  }
  result.insert("outputDirectory", m_outputDirectory);
  result.insert("localWorkingDirectory", m_localWorkingDirectory);
  result.insert("cleanRemoteFiles", m_cleanRemoteFiles);
  result.insert("retrieveOutput", m_retrieveOutput);
  result.insert("cleanLocalWorkingDirectory", m_cleanLocalWorkingDirectory);
  result.insert("hideFromGui", m_hideFromGui);
  result.insert("popupOnStateChange", m_popupOnStateChange);
  result.insert("numberOfCores", m_numberOfCores);
  result.insert("maxWallTime", m_maxWallTime);
  result.insert("moleQueueId", idTypeToJson(m_moleQueueId));
  result.insert("queueId", idTypeToJson(m_queueId));
  if (!m_keywords.isEmpty()) {
    QJsonObject keywords_;
    foreach (const QString &key, m_keywords.keys())
      keywords_.insert(key, m_keywords.value(key));
    result.insert("keywords", keywords_);
  }

  return result;
}

void JobData::setFromJson(const QJsonObject &state)
{
  if (state.contains("queue"))
    m_queue = state.value("queue").toString();
  if (state.contains("program"))
    m_program = state.value("program").toString();
  if (state.contains("description"))
    m_description = state.value("description").toString();
  if (state.contains("jobState"))
    m_jobState = stringToJobState(state.value("jobState").toString());
  if (state.contains("inputFile"))
    m_inputFile = FileSpecification(state.value("inputFile").toObject());
  m_additionalInputFiles.clear();
  if (state.contains("additionalInputFiles")) {
    foreach(const QJsonValue &inputFile_,
            state.value("additionalInputFiles").toArray()) {
      m_additionalInputFiles.append(FileSpecification(inputFile_.toObject()));
    }
  }
  if (state.contains("outputDirectory"))
    m_outputDirectory = state.value("outputDirectory").toString();
  if (state.contains("localWorkingDirectory"))
    m_localWorkingDirectory = state.value("localWorkingDirectory").toString();
  if (state.contains("cleanRemoteFiles"))
    m_cleanRemoteFiles = state.value("cleanRemoteFiles").toBool();
  if (state.contains("retrieveOutput"))
    m_retrieveOutput = state.value("retrieveOutput").toBool();
  if (state.contains("cleanLocalWorkingDirectory")) {
    m_cleanLocalWorkingDirectory =
        state.value("cleanLocalWorkingDirectory").toBool();
  }
  if (state.contains("hideFromGui"))
    m_hideFromGui = state.value("hideFromGui").toBool();
  if (state.contains("popupOnStateChange"))
    m_popupOnStateChange = state.value("popupOnStateChange").toBool();
  if (state.contains("numberOfCores"))
    m_numberOfCores = static_cast<int>(state.value("numberOfCores").toDouble());
  if (state.contains("maxWallTime"))
    m_maxWallTime = static_cast<int>(state.value("maxWallTime").toDouble());
  if (state.contains("moleQueueId"))
    m_moleQueueId = toIdType(state.value("moleQueueId"));
  if (state.contains("queueId"))
    m_queueId = toIdType(state.value("queueId"));
  if (state.contains("keywords")) {
    m_keywords.clear();
    QJsonObject keywords_ = state.value("keywords").toObject();
    foreach (const QString &key, keywords_.keys())
      m_keywords.insert(key, keywords_.value(key).toString());
  }

  modified();
}

bool JobData::load(const QString &stateFilename)
{
  if (!QFile::exists(stateFilename))
    return false;

  QFile stateFile(stateFilename);
  if (!stateFile.open(QFile::ReadOnly | QFile::Text)) {
    Logger::logError(Logger::tr("Cannot read job information from %1.")
                     .arg(stateFilename));
    return false;
  }

  // Read file
  QByteArray inputText = stateFile.readAll();
  stateFile.close();

  // Parse JSON
  QJsonParseError error;
  QJsonDocument doc = QJsonDocument::fromJson(inputText, &error);
  if (error.error != QJsonParseError::NoError) {
    Logger::logError(Logger::tr("Cannot parse job state from %1: %2\n%3")
                     .arg(stateFilename)
                     .arg(Logger::tr("%1 (at offset %2)")
                          .arg(error.errorString())
                          .arg(error.offset))
                     .arg(inputText.data()));
    return false;
  }

  if (!doc.isObject()) {
    Logger::logError(Logger::tr("Error reading job state from %1: "
                                "document is not an object!\n%2")
                     .arg(stateFilename)
                     .arg(inputText.data()));
    return false;
  }

  QJsonObject jobObject = doc.object();
  if (!jobObject.contains("moleQueueId")) {
    Logger::logError(Logger::tr("Error reading job state from %1: "
                                "No moleQueueId member!\n%2")
                     .arg(stateFilename).arg(inputText.data()));
    return false;
  }

  setFromJson(jobObject);

  m_needsSync = false;

  return true;
}

bool JobData::save()
{
  QString stateFilename = m_localWorkingDirectory +
      "/mqjobinfo.json";
  QFile stateFile(stateFilename);
  if (!stateFile.open(QFile::ReadWrite | QFile::Text)) {
    Logger::logError(Logger::tr("Cannot save job information for job %1 in %2.")
                     .arg(idTypeToString(moleQueueId())).arg(stateFilename),
                     moleQueueId());
    return false;
  }

  // Try to read existing data in
  QJsonDocument doc;
  QJsonParseError error;
  QJsonObject root;
  QByteArray inputText = stateFile.readAll();
  if (!inputText.isEmpty()) {
    // Parse the file.
    doc = QJsonDocument::fromJson(inputText, &error);
    if (error.error != QJsonParseError::NoError) {
      Logger::logError(Logger::tr("Cannot parse existing state for job %1 in "
                                  "%2: %3. Job state not saved. File contents:"
                                  "\n%4")
                       .arg(idTypeToString(moleQueueId()))
                       .arg(stateFilename)
                       .arg(Logger::tr("%1 (at offset %2)")
                            .arg(error.errorString())
                            .arg(error.offset))
                       .arg(inputText.data()), moleQueueId());
      stateFile.close();
      return false;
    }

    // Verify that the JSON represents an object
    if (!doc.isObject()) {
      Logger::logError(Logger::tr("Internal error writing state for job %1 in %2:"
                                  " existing json root is not an object! Job "
                                  "state not saved.")
                       .arg(idTypeToString(moleQueueId())).arg(stateFilename),
                       moleQueueId());
      stateFile.close();
      return false;
    }
  }

  // Overlay the current job state onto the existing json:
  QJsonObject jobObject = toJsonObject();
  foreach (const QString &key, jobObject.keys())
    root.insert(key, jobObject.value(key));

  // Write the data back out:
  QByteArray outputText = QJsonDocument(root).toJson();

  stateFile.resize(0);
  stateFile.write(outputText);
  stateFile.close();

  m_needsSync = false;

  return true;
}

} // end namespace MoleQueue
