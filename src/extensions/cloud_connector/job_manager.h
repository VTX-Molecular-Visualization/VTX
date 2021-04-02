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

#ifndef JOBMANAGER_H
#define JOBMANAGER_H

#include <QtCore/QObject>

#include "job.h"

#include <QtCore/QMap>

class QJsonObject;

class ConnectionTest;

namespace MoleQueue
{
class JobData;
class JobItemModel;
class JobReferenceBase;

/**
 * @class JobManager jobmanager.h <molequeue/jobmanager.h>
 * @brief Owns and manages JobData objects.
 * @author David C. Lonie
 *
 * The JobManager class owns all JobData objects. At least two JobManager
 * objects exist during normal operation; the Client class holds a JobManager to
 * track all jobs belonging to that client, and the Server class of the
 * MoleQueue server holds a JobManager to track all jobs that it is managing.
 */
class JobManager : public QObject
{
  Q_OBJECT
public:
  explicit JobManager(QObject *parentObject = 0);
  virtual ~JobManager();

  /// Locate jobs from the subdirectories in @a path. This function will look
  /// in all immediate subdirectories of @a path and load mqjobinfo.json files
  /// with JobData::load().
  void loadJobState(const QString &path);

  /// Sync all job state with disk.
  void syncJobState() const;

  /**
   * @name Job Management
   * Functions to add, remove, or locate jobs.
   * @{
   */

  /**
   * @return Insert a new JobData object into the JobManager's jobMap. The
   * JobData is set to default values and a Job reference to it is returned.
   */
  Job newJob();

  /**
   * @param jobState A QJsonObject describing the state of the new Job.
   * @return A new Job object, initialized to the state in @a jobState.
   * @sa Job::toJsonObject() Job::setFromJson()
   */
  Job newJob(const QJsonObject &jobState);

  /**
   * Remove the specified @a jobdata from this manager and delete it. All Job
   * objects with @a job's MoleQueue id will be invalidated.
   */
  void removeJob(JobData* jobdata);

  /**
   * Remove the job with the specified @a moleQueueId from this manager and
   * delete it.
   */
  void removeJob(IdType moleQueueId);

  /**
   * Remove the specified @a job from this manager and delete it. All Job
   * objects with @a job's MoleQueue id will be invalidated.
   */
  void removeJob(const Job &job);

  /**
   * Remove the specified @a jobs from this manager and delete them.
   */
  void removeJobs(const QList<Job> &jobsToRemove);

  /**
   * Remove the jobs with the specified @a moleQueueIds from this manager and
   * delete them.
   */
  void removeJobs(const QList<IdType> &moleQueueIds);

  /**
   * @param moleQueueId The MoleQueue Id of the requested Job.
   * @return The Job with the requested MoleQueue Id.
  * @note If no such Job exists, Job::isValid() will return false;
   */
  Job lookupJobByMoleQueueId(IdType moleQueueId) const;

  /**
   * Return a list of Job objects that have JobState @a state.
   * @param state JobState of interests
   * @return List of Job objects with JobState @a state
   */
  QList<Job> jobsWithJobState(MoleQueue::JobState state);

  /**
   * @return Number of Job objects held by this manager.
   */
  int count() const { return m_jobs.size(); }

  /**
   * Index based job look up. Use with count() to iterate over all Jobs in the
   * manager. Jobs are not sorted in any particular order.
   * @return The job with index @a i
   */
  Job jobAt(int i) const;

  /**
   * Lookup iteratible index of Job &job. Compatible with count() and jobAt().
   * @return index of @a job, or -1 if @a job is invalid.
   */
  int indexOf(const Job &job) const;

  /**
   * @return the JobItemModel for this JobManager.
   */
  JobItemModel * itemModel() const { return m_itemModel; }

  friend class JobReferenceBase;
  friend class ConnectionTest;

public slots:
  /**
   * Inform the QueueManager that the MoleQueue id of @a job has
   * changed so that it may update its internal lookup tables.
   * @param job The Job object.
   */
  void moleQueueIdChanged(const MoleQueue::Job &job);

  // End Job Management group:
  /**
   * @}
   */

  /**
   * @name Job Modification
   * Methods to change properties of jobs.
   * @{
   */

  /**
   * Set the JobState for the job with the specified MoleQueue id
   */
  void setJobState(MoleQueue::IdType jobManagerId,
                   MoleQueue::JobState newState);

  /**
   * Set the QueueId for the job with the specified MoleQueue id
   */
  void setJobQueueId(MoleQueue::IdType jobManagerIdId,
                     MoleQueue::IdType queueId);

  // End Job Modification group
  /**
   * @}
   */

signals:

  /**
   * Emitted when a job is about to be inserted. Client and MainWindow should
   * directly connect slots to this signal which will set the molequeue id and
   * local working directory.
   */
  void jobAboutToBeAdded(MoleQueue::Job job);

  /**
   * Emitted when a Job has been added to this JobManager.
   * @param job The new Job object.
   */
  void jobAdded(const MoleQueue::Job &job);

  /**
   * Emitted when a Job changes JobState.
   * @param job Job object
   * @param oldState Previous state of @a job
   * @param newState New state of @a job
   */
  void jobStateChanged(const MoleQueue::Job &job,
                       MoleQueue::JobState oldState,
                       MoleQueue::JobState newState);

  /**
   * Emitted when a Job's state changes.
   * @param job
   * @param queueId
   */
  void jobUpdated(const MoleQueue::Job &job);

  /**
   * Emitted when the @a job is about to be removed and deleted.
   */
  void jobAboutToBeRemoved(const MoleQueue::Job &job);

  /**
   * Emitted when the job with the specified @a moleQueueId has been removed
   * and deleted.
   */
  void jobRemoved(MoleQueue::IdType moleQueueId);

protected:
  /// @return The JobData with @a moleQueueId
  JobData *lookupJobDataByMoleQueueId(IdType moleQueueId) const
  {
    return m_moleQueueMap.value(moleQueueId, NULL);
  }

  /// @return Whether the address @a data is stored in m_jobs.
  bool hasJobData(const JobData *data) const
  {
    return m_jobs.contains(const_cast<JobData*>(data));
  }

  /// @param jobdata Job to insert into the internal lookup structures.
  void insertJobData(JobData *jobdata);

  /// "Master" list of JobData
  QList<JobData*> m_jobs;

  /// Item model for interacting with jobs
  JobItemModel *m_itemModel;

  /// Lookup table for MoleQueue ids
  QMap<IdType, JobData*> m_moleQueueMap;
};

}

#endif // JOBMANAGER_H
