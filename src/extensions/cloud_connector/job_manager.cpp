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

#include "jobmanager.h"

#include "job.h"
#include "jobdata.h"
#include "jobitemmodel.h"
#include "logger.h"

#include <QtCore/QDir>
#include <QtCore/QFile>

namespace MoleQueue
{

JobManager::JobManager(QObject *parentObject) :
  QObject(parentObject),
  m_itemModel(new JobItemModel(this))
{
  qRegisterMetaType<Job>("MoleQueue::Job");

  m_itemModel->setJobManager(this);

  connect(this, SIGNAL(jobStateChanged(MoleQueue::Job,MoleQueue::JobState,
                                       MoleQueue::JobState)),
          this, SIGNAL(jobUpdated(MoleQueue::Job)));
}

JobManager::~JobManager()
{
  m_moleQueueMap.clear();
  qDeleteAll(m_jobs);
  m_jobs.clear();
}

void JobManager::loadJobState(const QString &path)
{
  m_itemModel->beginResetModel();
  QDir dir(path);
  foreach (const QString &subDirName,
           dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot)) {
    QString stateFilename(QDir::cleanPath(dir.absolutePath() + "/" +
                                          subDirName + "/mqjobinfo.json"));
    if (!QFile::exists(stateFilename))
      continue;

    JobData *jobdata = new JobData(this);
    if (jobdata->load(stateFilename)) {
      m_jobs.append(jobdata);
      insertJobData(jobdata);
    }
    else {
      delete jobdata;
    }
  }

  m_itemModel->endResetModel();
}

void JobManager::syncJobState() const
{
  foreach (JobData *jobdata, m_jobs) {
    if (jobdata->needsSync())
      jobdata->save();
  }

}

Job JobManager::newJob()
{
  JobData *jobdata = new JobData(this);

  m_jobs.append(jobdata);
  emit jobAboutToBeAdded(Job(jobdata));

  insertJobData(jobdata);
  syncJobState();
  return Job(jobdata);
}

Job JobManager::newJob(const QJsonObject &jobState)
{
  JobData *jobdata = new JobData(this);
  jobdata->setFromJson(jobState);
  jobdata->setMoleQueueId(InvalidId);

  m_jobs.append(jobdata);
  emit jobAboutToBeAdded(Job(jobdata));

  insertJobData(jobdata);
  syncJobState();
  return Job(jobdata);
}

void JobManager::removeJob(JobData *jobdata)
{
  if (!jobdata || !m_jobs.contains(jobdata))
    return;

  emit jobAboutToBeRemoved(Job(jobdata));

  IdType moleQueueId = jobdata->moleQueueId();

  int jobsIndex = m_jobs.indexOf(jobdata);
  m_jobs.removeAt(jobsIndex);
  m_itemModel->removeRow(jobsIndex);
  m_moleQueueMap.remove(moleQueueId);

  // Save job state and move it so it won't get loaded next time.
  jobdata->save();
  QFile::rename(jobdata->localWorkingDirectory() + "/mqjobinfo.json",
                jobdata->localWorkingDirectory() + "/mqjobinfo-archived.json");

  delete jobdata;

  emit jobRemoved(moleQueueId);
}

void JobManager::removeJob(IdType moleQueueId)
{
  JobData *jobdata = lookupJobDataByMoleQueueId(moleQueueId);

  if (jobdata)
    removeJob(jobdata);
}

void JobManager::removeJob(const Job &job)
{
  if (job.isValid())
    removeJob(job.jobData());
}

void JobManager::removeJobs(const QList<Job> &jobsToRemove)
{
  foreach (const Job &job, jobsToRemove)
    removeJob(job);
}

void JobManager::removeJobs(const QList<IdType> &moleQueueIds)
{
  foreach(IdType moleQueueId, moleQueueIds)
    removeJob(moleQueueId);
}

Job JobManager::lookupJobByMoleQueueId(IdType moleQueueId) const
{
  return Job(lookupJobDataByMoleQueueId(moleQueueId));
}

QList<Job> JobManager::jobsWithJobState(JobState state)
{
  QList<Job> result;

  foreach (JobData *jobdata, m_jobs) {
    if (jobdata->jobState() == state)
      result << Job(jobdata);
  }

  return result;
}

Job JobManager::jobAt(int i) const
{
  if (Q_LIKELY(i >= 0 && i < m_jobs.size()))
    return Job(m_jobs.at(i));
  return Job();
}

int JobManager::indexOf(const Job &job) const
{
  JobData *jobdata = job.jobData();
  if (jobdata)
    return m_jobs.indexOf(jobdata);

  return -1;
}

void JobManager::moleQueueIdChanged(const Job &job)
{
  JobData *jobdata = job.jobData();
  if (!m_jobs.contains(jobdata))
    return;

  if (lookupJobDataByMoleQueueId(jobdata->moleQueueId()) != jobdata) {
    IdType oldMoleQueueId = m_moleQueueMap.key(jobdata, InvalidId);
    if (oldMoleQueueId != InvalidId)
      m_moleQueueMap.remove(oldMoleQueueId);
    m_moleQueueMap.insert(jobdata->moleQueueId(), jobdata);
  }
}

void JobManager::setJobState(IdType moleQueueId, JobState newState)
{
  JobData *jobdata = lookupJobDataByMoleQueueId(moleQueueId);
  if (!jobdata)
    return;

  const JobState oldState = jobdata->jobState();

  if (oldState == newState)
    return;

  jobdata->setJobState(newState);

  Logger::logNotification(tr("Job '%1' has changed status from '%2' to '%3'.")
                          .arg(jobdata->description())
                          .arg(MoleQueue::jobStateToGuiString(oldState))
                          .arg(MoleQueue::jobStateToGuiString(newState)),
                          moleQueueId);

  emit jobStateChanged(jobdata, oldState, newState);
}

void JobManager::setJobQueueId(IdType moleQueueId, IdType queueId)
{
  JobData *jobdata = lookupJobDataByMoleQueueId(moleQueueId);
  if (!jobdata)
    return;

  if (jobdata->queueId() == queueId)
    return;

  jobdata->setQueueId(queueId);

  emit jobUpdated(jobdata);
}

void JobManager::insertJobData(JobData *jobdata)
{
  if (jobdata->moleQueueId() != MoleQueue::InvalidId)
    m_moleQueueMap.insert(jobdata->moleQueueId(), jobdata);

  m_itemModel->insertRow(m_jobs.size() - 1);
  emit jobAdded(Job(jobdata));
}

} // end namespace MoleQueue
