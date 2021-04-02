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

#ifndef MOLEQUEUE_JOB_H
#define MOLEQUEUE_JOB_H

#include "jobreferencebase.h"

#include "filespecification.h"

#include <qjsonobject.h>

#include <QtCore/QMetaType>
#include <QtCore/QString>

namespace MoleQueue
{
class JobManager;

/**
 * @class Job job.h <molequeue/job.h>
 * @brief Server-side interface to JobData properties.
 * @author David C. Lonie
 *
 * The Job class provides a lightweight interface to a specific instance of
 * JobData. Since JobData contains dynamic information that changes during its
 * lifetime, the Job interface forwards requests to a JobData instance, which
 * ensures that all references to job information are synced throughout the
 * application.
 *
 * The Job class also ensures that JobData is modified in a consistent way. For
 * example, calling the Job::setQueueId method will cause JobManager to emit the
 * JobManager::jobQueueIdChanged signal, so that listeners (e.g. GUI elements)
 * can be made aware of the change.
 *
 * The JobReferenceBase class holds and validates a pointer to a JobData
 * instance and will detect when the associated JobData object is removed from
 * the JobManager (such as when the user deletes a job from the job table). To
 * check the validity of the JobData pointer, use Job::isValid(), defined in
 * JobReferenceBase.
 *
 * To serialize a collection of valid Job objects as references (e.g., to
 * maintain state in a Queue implementation between sessions), store the
 * identifier returned from Job::moleQueueId() in the data store, and then
 * deserialize the Job using JobManager::lookupJobByMoleQueueId.
 *
 * Full serialization of all Job details can be performed by saving and
 * restoring the JobData state via the Job::hash() and Job::setFromHash()
 * methods. However, this should not need to be performed outside of the
 * JobManager's serialization methods.
 */
class Job : public JobReferenceBase
{
public:
  /// Construct a new Job object with the specified JobData
  Job(JobData *jobdata = NULL);

  /// Construct a new Job object for the job with the MoleQueueId
  /// in the indicated JobManager
  Job(JobManager *jobManager, IdType mQId);

  /// Construct a new Job object with the same JobData as @a other.
  Job(const JobReferenceBase &other);

  ~Job();

  /// @return The JobData's internal state as a QJsonObject
  QJsonObject toJsonObject() const;

  /// Update the JobData's internal state from a QJsonObject
  void setFromJson(const QJsonObject &state);

  /// @param newQueue name of the queue.
  void setQueue(const QString &newQueue);

  /// @return Name of queue to use.
  QString queue() const;

  /// @param newProgram Name of the program.
  void setProgram(const QString &newProgram);

  /// @return Name of program to run.
  QString program() const;

  /// Set the current JobState for the job. Calling this function with a
  /// different JobState will cause the JobManager::jobStateChanged signal
  /// to be emitted.
  /// @param state Status of job
  void setJobState(JobState state);

  /// @return Status of job
  JobState jobState() const;

  /// @param newDesc Description of job
  void setDescription(const QString &newDesc);

  /// @return newDesc Description of job
  QString description() const;

  /// @param filespec FileSpecification describing the main input file (called
  /// by the executable)
  void setInputFile(const FileSpecification &spec);

  /// @return FileSpecification describing the main input file (called by the
  /// executable)
  FileSpecification inputFile() const;

  /// @param files FileSpecification objects describing additional input files
  /// to be placed in the working directory of the job prior to execution.
  void setAdditionalInputFiles(const QList<FileSpecification> & files);

  /// @return FileSpecification objects describing additional input files to be
  /// placed in the working directory of the job prior to execution.
  QList<FileSpecification> additionalInputFiles() const;

  /// @a param spec FileSpecification describing an input file to append to the
  /// additional input file list.
  void addInputFile(const FileSpecification &spec);

  /**
   * Set the output directory for the job.
   * If empty, the Server will set it to the temporary working directory once
   * the job is accepted. Otherwise, the output files will be copied to the
   * specified location when the job completes.
   */
  void setOutputDirectory(const QString &path);

  /// @return String containing a location to copy the output files to after
  /// the job completes. Ignored if empty.
  QString outputDirectory() const;

  /**
   * @param path Temporary working directory where files are stored during job
   * execution
   * @warning This is set internally by MoleQueue, do not modify.
   */
  void setLocalWorkingDirectory(const QString &path);

  /// @return Temporary working directory where files are stored during job
  /// execution.
  QString localWorkingDirectory() const;

  /// @param clean If true, delete any working files on the remote server.
  /// Default: false.
  void setCleanRemoteFiles(bool clean);

  /// @return If true, delete any working files on the remote server.
  /// Default: false.
  bool cleanRemoteFiles() const;

  /// @param b If true, copies files back from remote server. Default: true
  void setRetrieveOutput(bool b);

  /// @return If true, copies files back from remote server. Default: true
  bool retrieveOutput() const;

  /// @param b If true, the local working files are removed after job is
  /// complete. Should be used with setOutputDirectory. Default: false
  void setCleanLocalWorkingDirectory(bool b);

  /// @return If true, the local working files are removed after job is
  /// complete. Should be used with setOutputDirectory. Default: false
  bool cleanLocalWorkingDirectory() const;

  /// @param b If true, the job will not appear in the MoleQueue user interface
  /// by default. Useful for automated batch jobs.
  void setHideFromGui(bool b);

  /// @return If true, the job will not appear in the queue. Default: false
  bool hideFromGui() const;

  /// @param b If true, changes in the job state will trigger a popup
  /// notification from the MoleQueue system tray icon. Default: false
  void setPopupOnStateChange(bool b);

  /// @return If true, changes in the job state will trigger a popup
  /// notification from the MoleQueue system tray icon. Default: false
  bool popupOnStateChange() const;

  /// @param num The total number of processor cores to use (if applicable).
  /// Default: 1
  void setNumberOfCores(int num);

  /// @return The total number of processor cores to use (if applicable).
  /// Default: 1
  int numberOfCores() const;

  /// @param minutes The maximum walltime for this job in minutes. Setting this
  /// to a value <= 0 will use the queue-specific default max walltime. Only
  /// available for remote queues. Default is -1.
  void setMaxWallTime(int minutes);

  /// @return The maximum walltime for this job in minutes. Setting this to a
  /// value <= 0 will use the queue-specific default max walltime. Only
  /// available for remote queues. Default is -1.
  int maxWallTime() const;

  /// @param id The new MoleQueue id for this job.
  /// @warning Do not call this function except in Server or Client as a
  ///   response to the JobManager::jobAboutToBeAdded signal.
  void setMoleQueueId(IdType id);

  /// @return Internal MoleQueue identifier
  IdType moleQueueId() const;

  /// Set the job's queue id. Calling this function will cause the
  // JobManager::jobQueueIdChanged signal to be emitted.
  /// @param id Queue Job ID.
  void setQueueId(IdType id);

  /// @return Queue Job ID
  IdType queueId() const;

  /// @param keyrep The keyword replacement hash for this job.
  void setKeywords(const QHash<QString, QString> &keyrep);

  /// @return The keyword replacement hash for this job.
  QHash<QString, QString> keywords() const;

  /// Add a keyword / replacement pair for this job.
  void setKeywordReplacement(const QString &keyword,
                             const QString &replacement);

  /// @return True if the @a keyword has a replacement.
  bool hasKeywordReplacement(const QString &keyword) const;

  /// @return The replacement string for the @a keyword.
  QString lookupKeywordReplacement(const QString &keyword) const;

  /// Apply the replacements in the keywords() hash to the @a script.
  /// @note Do not call this directly, use Queue::replaceKeywords instead.
  void replaceKeywords(QString &launchScript) const;
};

} // end namespace MoleQueue

Q_DECLARE_METATYPE(MoleQueue::Job)

#endif // MOLEQUEUE_JOB_H
