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

#include "remote.h"

#include "../filesystemtools.h"
#include "../job.h"
#include "../jobmanager.h"
#include "../logentry.h"
#include "../logger.h"
#include "../program.h"
#include "../remotequeuewidget.h"
#include "../server.h"

#include <qjsondocument.h>

#include <QtCore/QTimer>
#include <QtCore/QDebug>

#include <QtGui>

namespace MoleQueue {

QueueRemote::QueueRemote(const QString &queueName, QueueManager *parentObject)
  : Queue(queueName, parentObject),
    m_checkForPendingJobsTimerId(-1),
    m_queueUpdateInterval(DEFAULT_REMOTE_QUEUE_UPDATE_INTERVAL),
    m_defaultMaxWallTime(DEFAULT_MAX_WALLTIME)
{
  // Set remote queue check timer.
  m_checkQueueTimerId = startTimer(m_queueUpdateInterval * 60000);

  // Check for jobs to submit every 5 seconds
  m_checkForPendingJobsTimerId = startTimer(5000);
}

QueueRemote::~QueueRemote()
{
}

bool QueueRemote::writeJsonSettings(QJsonObject &json, bool exportOnly,
                                    bool includePrograms) const
{
  if (!Queue::writeJsonSettings(json, exportOnly, includePrograms))
    return false;

  if (!exportOnly)
    json.insert("workingDirectoryBase", m_workingDirectoryBase);

  json.insert("queueUpdateInterval",
              static_cast<double>(m_queueUpdateInterval));
  json.insert("defaultMaxWallTime",
              static_cast<double>(m_defaultMaxWallTime));

  return true;
}

bool QueueRemote::readJsonSettings(const QJsonObject &json, bool importOnly,
                                   bool includePrograms)
{
  // Validate JSON:
  if ((!importOnly && !json.value("workingDirectoryBase").isString()) ||
      !json.value("queueUpdateInterval").isDouble() ||
      !json.value("defaultMaxWallTime").isDouble()) {
    Logger::logError(tr("Error reading queue settings: Invalid format:\n%1")
                     .arg(QString(QJsonDocument(json).toJson())));
    return false;
  }

  if (!Queue::readJsonSettings(json, importOnly, includePrograms))
    return false;

  if (!importOnly)
    m_workingDirectoryBase = json.value("workingDirectoryBase").toString();

  m_queueUpdateInterval =
      static_cast<int>(json.value("queueUpdateInterval").toDouble() + 0.5);
  m_defaultMaxWallTime =
      static_cast<int>(json.value("defaultMaxWallTime").toDouble() + 0.5);

  return true;
}

void QueueRemote::setQueueUpdateInterval(int interval)
{
  if (interval == m_queueUpdateInterval)
    return;

  m_queueUpdateInterval = interval;

  killTimer(m_checkQueueTimerId);
  m_checkQueueTimerId = startTimer(m_queueUpdateInterval * 60000);
  requestQueueUpdate();
}

void QueueRemote::replaceKeywords(QString &launchScript,
                                  const Job &job, bool addNewline)
{
  // If a valid walltime is set, replace all occurances with the appropriate
  // string:
  int wallTime = job.maxWallTime();
  int hours = wallTime / 60;
  int minutes = wallTime % 60;
  if (wallTime > 0) {
    launchScript.replace("$$$maxWallTime$$$",
                         QString("%1:%2:00")
                         .arg(hours, 2, 10, QChar('0'))
                         .arg(minutes, 2, 10, QChar('0')));
  }
  // Otherwise, erase all lines containing the keyword
  else {
    QRegExp expr("\\n[^\\n]*\\${3,3}maxWallTime\\${3,3}[^\\n]*\\n");
    launchScript.replace(expr, "\n");
  }

  if (wallTime <= 0) {
    wallTime = defaultMaxWallTime();
    hours = wallTime / 60;
    minutes = wallTime % 60;
  }

  launchScript.replace("$$maxWallTime$$",
                       QString("%1:%2:00")
                       .arg(hours, 2, 10, QChar('0'))
                       .arg(minutes, 2, 10, QChar('0')));

  Queue::replaceKeywords(launchScript, job, addNewline);
}

bool QueueRemote::submitJob(Job job)
{
  if (job.isValid()) {
    m_pendingSubmission.append(job.moleQueueId());
    job.setJobState(MoleQueue::Accepted);
    return true;
  }
  Logger::logError(tr("Refusing to submit job to Queue '%1': Job object is "
                      "invalid.").arg(m_name), job.moleQueueId());
  return false;
}

void QueueRemote::killJob(Job job)
{
  if (!job.isValid())
    return;

  int pendingIndex = m_pendingSubmission.indexOf(job.moleQueueId());
  if (pendingIndex >= 0) {
    m_pendingSubmission.removeAt(pendingIndex);
    job.setJobState(MoleQueue::Canceled);
    return;
  }

  if (job.queue() == m_name && job.queueId() != InvalidId &&
      m_jobs.value(job.queueId()) == job.moleQueueId()) {
    m_jobs.remove(job.queueId());
    beginKillJob(job);
    return;
  }

  Logger::logWarning(tr("Queue '%1' requested to kill unknown job that belongs "
                        "to queue '%2', queue id '%3'.").arg(m_name)
                     .arg(job.queue()).arg(idTypeToString(job.queueId())),
                     job.moleQueueId());
  job.setJobState(MoleQueue::Canceled);
}

void QueueRemote::submitPendingJobs()
{
  if (m_pendingSubmission.isEmpty())
    return;

  // lookup job manager:
  JobManager *jobManager = NULL;
  if (m_server)
    jobManager = m_server->jobManager();

  if (!jobManager) {
    Logger::logError(tr("Internal error: %1\n%2").arg(Q_FUNC_INFO)
                     .arg("Cannot locate server JobManager!"));
    return;
  }

  foreach (const IdType moleQueueId, m_pendingSubmission) {
    Job job = jobManager->lookupJobByMoleQueueId(moleQueueId);
    // Kick off the submission process...
    beginJobSubmission(job);
  }

  m_pendingSubmission.clear();
}

void QueueRemote::beginJobSubmission(Job job)
{
  if (!writeInputFiles(job)) {
    Logger::logError(tr("Error while writing input files."), job.moleQueueId());
    job.setJobState(Error);
    return;
  }
  // Attempt to copy the files via scp first. Only call mkdir on the remote
  // working directory if the scp call fails.
  copyInputFilesToHost(job);
}

void QueueRemote::beginFinalizeJob(IdType queueId)
{
  IdType moleQueueId = m_jobs.value(queueId, InvalidId);
  if (moleQueueId == InvalidId)
    return;

  m_jobs.remove(queueId);

  // Lookup job
  if (!m_server)
    return;
  Job job = m_server->jobManager()->lookupJobByMoleQueueId(moleQueueId);
  if (!job.isValid())
    return;

  finalizeJobCopyFromServer(job);
}

void QueueRemote::finalizeJobCopyToCustomDestination(Job job)
{
  // Skip to next step if needed
  if (job.outputDirectory().isEmpty() ||
      job.outputDirectory() == job.localWorkingDirectory()) {
    finalizeJobCleanup(job);
    return;
  }

  // The copy function will throw errors if needed.
  if (!FileSystemTools::recursiveCopyDirectory(job.localWorkingDirectory(),
                                               job.outputDirectory())) {
    Logger::logError(tr("Cannot copy '%1' -> '%2'.")
                     .arg(job.localWorkingDirectory(),
                          job.outputDirectory()), job.moleQueueId());
    job.setJobState(MoleQueue::Error);
    return;
  }

  finalizeJobCleanup(job);
}

void QueueRemote::finalizeJobCleanup(Job job)
{
  if (job.cleanLocalWorkingDirectory())
    cleanLocalDirectory(job);

  if (job.cleanRemoteFiles())
    cleanRemoteDirectory(job);

  job.setJobState(MoleQueue::Finished);
}


void QueueRemote::jobAboutToBeRemoved(const Job &job)
{
  m_pendingSubmission.removeOne(job.moleQueueId());
  Queue::jobAboutToBeRemoved(job);
}

void QueueRemote::removeStaleJobs()
{
  if (m_server) {
    if (JobManager *jobManager = m_server->jobManager()) {
      QList<IdType> staleQueueIds;
      for (QMap<IdType, IdType>::const_iterator it = m_jobs.constBegin(),
           it_end = m_jobs.constEnd(); it != it_end; ++it) {
        if (jobManager->lookupJobByMoleQueueId(it.value()).isValid())
          continue;
        staleQueueIds << it.key();
        Logger::logError(tr("Job with MoleQueue id %1 is missing, but the Queue"
                            " '%2' is still holding a reference to it. Please "
                            "report this bug and check if the job needs to be "
                            "resubmitted.").arg(it.value()).arg(name()),
                         it.value());
      }
      foreach (IdType queueId, staleQueueIds)
        m_jobs.remove(queueId);
    }
  }
}

void QueueRemote::timerEvent(QTimerEvent *theEvent)
{
  if (theEvent->timerId() == m_checkQueueTimerId) {
    theEvent->accept();
    removeStaleJobs();
    if (!m_jobs.isEmpty())
      requestQueueUpdate();
    return;
  }
  else if (theEvent->timerId() == m_checkForPendingJobsTimerId) {
    theEvent->accept();
    submitPendingJobs();
    return;
  }

  QObject::timerEvent(theEvent);
}

} // End namespace
