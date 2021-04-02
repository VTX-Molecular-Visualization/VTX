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

#ifndef MOLEQUEUE_JOBDATA_H
#define MOLEQUEUE_JOBDATA_H

#include "molequeueglobal.h"

#include "filespecification.h"

#include <qjsonobject.h>

#include <QtCore/QMetaType>
#include <QtCore/QString>

class ClientTest;
class JobManagerTest;
class ServerConnectionTest;

namespace MoleQueue
{
class Client;
class JobManager;
class Server;

/**
 * @class JobData jobdata.h <molequeue/jobdata.h>
 * @brief Internal container for job details.
 * @author David C. Lonie
 *
 * Each JobData instance stores information about a specific job. All JobData
 * objects are owned by a JobManager, which dispenses JobReferenceBase
 * subclasses (Job and JobRequest) that are used to interact with the JobData
 * members.
 *
 */
class JobData
{
public:

  JobData(JobManager *parentManager);
  JobData(const JobData &);

  /// @return The parent JobManager
  JobManager *jobManager() const { return m_jobManager; }

  /// @param newQueue name of the queue.
  void setQueue(const QString &newQueue)
  {
    if (m_queue != newQueue) {
      m_queue = newQueue;
      modified();
    }
  }

  /// @return Name of queue to use.
  QString queue() const { return m_queue; }

  /// @param newProgram Name of the program.
  void setProgram(const QString &newProgram)
  {
    if (m_program != newProgram) {
      m_program = newProgram;
      modified();
    }
  }

  /// @return Name of program to run.
  QString program() const { return m_program; }

  /// @param state Status of job
  void setJobState(JobState state)
  {
    if (m_jobState != state) {
      m_jobState = state;
      modified();
    }
  }

  /// @return Status of job
  JobState jobState() const { return m_jobState; }

  /// @param newDesc Description of job
  void setDescription(const QString &newDesc)
  {
    if (m_description != newDesc) {
      m_description = newDesc;
      modified();
    }
  }

  /// @return newDesc Description of job
  QString description() const { return m_description; }

  /// @param filespec FileSpecification describing the main input file (called
  /// by the executable)
  void setInputFile(const FileSpecification &filespec)
  {
    m_inputFile = filespec;
    modified();
  }

  /// @return FileSpecification describing the main input file (called by the
  /// executable)
  FileSpecification inputFile() const { return m_inputFile; }

  /// @param files FileSpecification objects describing additional input files
  /// to be placed in the working directory of the job prior to execution.
  void setAdditionalInputFiles(const QList<FileSpecification> & files)
  {
    m_additionalInputFiles = files;
    modified();
  }

  /// @return FileSpecification objects describing additional input files to be
  /// placed in the working directory of the job prior to execution.
  QList<FileSpecification> additionalInputFiles() const
  {
    return m_additionalInputFiles;
  }

  /// @return A reference to the additional input files list.
  QList<FileSpecification> & additionalInputFilesRef()
  {
    return m_additionalInputFiles;
  }

  /// @param path String containing a location to copy the output files to after
  /// the job completes. Ignored if empty.
  void setOutputDirectory(const QString &path)
  {
    if (m_outputDirectory != path) {
      m_outputDirectory = path;
      modified();
    }
  }

  /// @return String containing a location to copy the output files to after
  /// the job completes. Ignored if empty.
  QString outputDirectory() const { return m_outputDirectory; }

  /// @param path Temporary working directory where files are stored during job
  /// execution.
  void setLocalWorkingDirectory(const QString &path)
  {
    if (m_localWorkingDirectory != path) {
      m_localWorkingDirectory = path;
      modified();
    }
  }

  /// @return Temporary working directory where files are stored during job
  /// execution.
  QString localWorkingDirectory() const { return m_localWorkingDirectory; }

  /// @param clean If true, delete any working files on the remote server.
  /// Default: false.
  void setCleanRemoteFiles(bool clean)
  {
    if (m_cleanRemoteFiles != clean) {
      m_cleanRemoteFiles = clean;
      modified();
    }
  }

  /// @return If true, delete any working files on the remote server.
  /// Default: false.
  bool cleanRemoteFiles() const { return m_cleanRemoteFiles; }

  /// @param b If true, copies files back from remote server. Default: true
  void setRetrieveOutput(bool b)
  {
    if (m_retrieveOutput != b) {
      m_retrieveOutput = b;
      modified();
    }
  }

  /// @return If true, copies files back from remote server. Default: true
  bool retrieveOutput() const { return m_retrieveOutput; }

  /// @param b If true, the local working files are removed after job is
  /// complete. Should be used with setOutputDirectory. Default: false
  void setCleanLocalWorkingDirectory(bool b)
  {
    if (m_cleanLocalWorkingDirectory != b) {
      m_cleanLocalWorkingDirectory = b;
      modified();
    }
  }

  /// @return If true, the local working files are removed after job is
  /// complete. Should be used with setOutputDirectory. Default: false
  bool cleanLocalWorkingDirectory() const
  {
    return m_cleanLocalWorkingDirectory;
  }

  /// @param b If true, the job will not appear in the queue. Default: false
  void setHideFromGui(bool b)
  {
    if (m_hideFromGui != b) {
      m_hideFromGui = b;
      modified();
    }
  }

  /// @return If true, the job will not appear in the queue. Default: false
  bool hideFromGui() const { return m_hideFromGui; }

  /// @param b If true, changes in the job state will trigger a popup
  /// notification from the MoleQueue system tray icon. Default: false
  void setPopupOnStateChange(bool b)
  {
    if (m_popupOnStateChange != b) {
      m_popupOnStateChange = b;
      modified();
    }
  }

  /// @return If true, changes in the job state will trigger a popup
  /// notification from the MoleQueue system tray icon. Default: false
  bool popupOnStateChange() const { return m_popupOnStateChange; }

  /// @param num The total number of processor cores to use (if applicable).
  /// Default: 1
  void setNumberOfCores(int num)
  {
    if (m_numberOfCores != num) {
      m_numberOfCores = num;
      modified();
    }
  }

  /// @return The total number of processor cores to use (if applicable).
  /// Default: 1
  int numberOfCores() const { return m_numberOfCores; }

  /// @param minutes The maximum walltime for this job in minutes. Setting this
  /// to a value <= 0 will use the queue-specific default max walltime. Only
  /// available for remote queues. Default is -1.
  void setMaxWallTime(int minutes)
  {
    if (m_maxWallTime != minutes) {
      m_maxWallTime = minutes;
      modified();
    }
  }

  /// @return The maximum walltime for this job in minutes. Setting this to a
  /// value <= 0 will use the queue-specific default max walltime. Only
  /// available for remote queues. Default is -1.
  int maxWallTime() const { return m_maxWallTime; }

  /// @param id Internal MoleQueue identifier
  void setMoleQueueId(IdType id)
  {
    if (m_moleQueueId != id) {
      m_moleQueueId = id;
      modified();
    }
  }

  /// @return Internal MoleQueue identifier
  IdType moleQueueId() const { return m_moleQueueId; }

  /// @param id Queue Job ID
  void setQueueId(IdType id)
  {
    if (m_queueId != id) {
      m_queueId = id;
      modified();
    }
  }

  /// @return Queue Job ID
  IdType queueId() const { return m_queueId; }

  /// @return A reference to the job's keyword hash
  QHash<QString, QString> & keywordsRef() { return m_keywords; }

  /// @param keyrep The keyword replacement hash
  void setKeywords(const QHash<QString, QString> &keyrep)
  {
    if (m_keywords != keyrep) {
      m_keywords = keyrep;
      modified();
    }
  }

  /// @return The keyword replacement hash
  QHash<QString, QString> keywords() const { return m_keywords; }

  /// @return The Job's internal state as a QJsonObject
  QJsonObject toJsonObject() const;

  /// Update the Job's internal state from a QJsonObject
  void setFromJson(const QJsonObject &state);

  /// Initialize the JobData from the state in JSON file @a stateFileName
  bool load(const QString& stateFilename);

  /// Write a mqjobinfo.json file to the JobData's local working directory with
  /// the job state.
  bool save();

  /// @return true if the JobData has changed since load() or save() was called.
  bool needsSync() const { return m_needsSync; }

  /// Called when the JobData is modified.
  void modified() { m_needsSync = true; }

protected:
  /// Parent JobManager
  JobManager *m_jobManager;
  /// Name of queue to use
  QString m_queue;
  /// Name of program to run
  QString m_program;
  /// Current state of job
  JobState m_jobState;
  /// Description of job
  QString m_description;
  /// FileSpecification describing the main input file (called by the executable)
  FileSpecification m_inputFile;
  /// FileSpecification objects describing additional input files, to be placed
  /// in the working directory of the job prior to execution.
  QList<FileSpecification> m_additionalInputFiles;
  /// String containing a location to copy the output files to after the job
  /// completes. Ignored if empty.
  QString m_outputDirectory;
  /// Temporary working directory where files are stored during job execution.
  QString m_localWorkingDirectory;
  /// If true, delete any working files on the remote server. Default: false.
  bool m_cleanRemoteFiles;
  /// If true, copies files back from remote server. Default: true
  bool m_retrieveOutput;
  /// If true, the local working files are removed after job is complete. Should
  /// be used with setOutputDirectory. Default: false
  bool m_cleanLocalWorkingDirectory;
  /// If true, the job will not appear in the queue. Default: false
  bool m_hideFromGui;
  /// If true, changes in the job state will trigger a popup notification from
  /// the MoleQueue system tray icon. Default: true
  bool m_popupOnStateChange;
  /// The total number of processor cores to use (if applicable).
  /// Default: 1
  int m_numberOfCores;
  /// The maximum walltime for this job in minutes. Setting this
  /// to a value <= 0 will use the queue-specific default max walltime. Only
  /// available for remote queues. Default is -1.
  int m_maxWallTime;
  /// Internal MoleQueue identifier
  IdType m_moleQueueId;
  /// Queue Job ID
  IdType m_queueId;
  /// List of custom keyword replacements for the job's launch script
  QHash<QString, QString> m_keywords;

  /// True if the JobData has changed since load() or save() was called.
  bool m_needsSync;
};

} // end namespace MoleQueue

Q_DECLARE_METATYPE(MoleQueue::JobData*)
Q_DECLARE_METATYPE(const MoleQueue::JobData*)

#endif // JOB_H
