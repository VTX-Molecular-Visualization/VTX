/******************************************************************************

  This source file is part of the MoleQueue project.

  Copyright 2013 Kitware, Inc.

  This source code is released under the New BSD License, (the "License").

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

******************************************************************************/

#include "slurm.h"

#include "logger.h"

#include <QtCore/QDebug>
#include <QtCore/QStringList>

namespace MoleQueue
{

QueueSlurm::QueueSlurm(QueueManager *parentManager) :
  QueueRemoteSsh("Remote (SLURM)", parentManager)
{
  m_submissionCommand = "sbatch";
  m_killCommand = "scancel";
  m_requestQueueCommand = "squeue";
  m_launchScriptName = "job.slurm";

  m_launchTemplate =
      "#!/bin/sh\n"
      "#\n"
      "# Sample SLURM job script provided by MoleQueue.\n"
      "#\n"
      "# These commands set up your job:\n"
      "#SBATCH --job-name=\"MoleQueueJob-$$moleQueueId$$\"\n"
      "#SBATCH --time=$$maxWallTime$$\n"
      "#SBATCH --nodes=1\n"
      "#SBATCH --ntasks-per-node=$$numberOfCores$$\n"
      "\n"
      "cd $SLURM_SUBMIT_DIR\n"
      "$$programExecution$$\n";
}

QueueSlurm::~QueueSlurm()
{
}

QString QueueSlurm::generateQueueRequestCommand()
{
  QList<IdType> queueIds(m_jobs.keys());
  QStringList queueIdStringList;
  foreach (IdType id, queueIds) {
    if (id != InvalidId)
      queueIdStringList << QString::number(id);
  }

  return QString("%1 -j %2").arg(m_requestQueueCommand)
      .arg(queueIdStringList.join(","));
}

bool QueueSlurm::parseQueueId(const QString &submissionOutput, IdType *queueId)
{
  // Assuming submissionOutput is:
  // Submitted batch job <jobid>
  QRegExp parser("^Submitted batch job (\\d+)");

  int ind = parser.indexIn(submissionOutput);
  if (ind >= 0) {
    bool ok;
    *queueId = static_cast<IdType>(parser.cap(1).toInt(&ok));
    return ok;
  }
  return false;
}

bool QueueSlurm::parseQueueLine(const QString &queueListOutput,
                              IdType *queueId, JobState *state)
{
  *queueId = InvalidId;
  *state = Unknown;

  // Expecting qstat output is:
  // JOBID PARTITION     NAME     USER  ST       TIME  NODES NODELIST(REASON)
  // 4832 general-c      hello_te cdc   R       0:14      2 f16n[10-11]
  QRegExp parser("^\\s*(\\d+)"  // job-ID
                 "\\s+\\S+"     // partition
                 "\\s+\\S+"     // name
                 "\\s+\\S+"     // user
                 "\\s+(\\w+)"); // state

  int ind = parser.indexIn(queueListOutput);
  if (ind >= 0) {
    bool ok;
    *queueId = static_cast<IdType>(parser.cap(1).toInt(&ok));
    if (!ok)
      return false;

    QString stateStr(parser.cap(2).toLower());

    // JOB STATE CODES
    //
    // Jobs typically pass through several states in the course of
    // their execution.  The typical states are PENDING, RUNNING,
    // SUSPENDED, COMPLETING, and COMPLETED.  An explanation of each
    // state follows.
    //
    // CA  CANCELLED       Job  was explicitly cancelled by the user or system
    //                     administrator.  The job may or may  not  have  been
    //                     initiated.
    // CD  COMPLETED       Job has terminated all processes on all nodes.
    // CF  CONFIGURING     Job  has  been allocated resources, but are waiting
    //                     for them to become ready for use (e.g. booting).
    // CG  COMPLETING      Job is in the process of completing. Some processes
    //                     on some nodes may still be active.
    // F   FAILED          Job  terminated  with  non-zero  exit code or other
    //                     failure condition.
    // NF  NODE_FAIL       Job terminated due to failure of one or more  allo-
    //                     cated nodes.
    // PD  PENDING         Job is awaiting resource allocation.
    // R   RUNNING         Job currently has an allocation.
    // S   SUSPENDED       Job  has an allocation, but execution has been sus-
    //                     pended.
    // TO  TIMEOUT         Job terminated upon reaching its time limit.
    if (stateStr == "ca"
        || stateStr == "cd"
        || stateStr == "cg"
        || stateStr == "f"
        || stateStr == "nf"
        || stateStr == "pr"
        || stateStr == "r"
        || stateStr == "s"
        || stateStr == "to") {
      *state = MoleQueue::RunningRemote;
      return true;
    }
    else if (stateStr == "cf"
             || stateStr == "pd") {
      *state = MoleQueue::QueuedRemote;
      return true;
    }
    else {
      Logger::logWarning(tr("Unrecognized queue state '%1' in %2 queue '%3'. "
                            "Queue line:\n'%4'")
                         .arg(stateStr).arg(typeName()).arg(name())
                         .arg(queueListOutput));
      return false;
    }
  }
  return false;
}

} // End namespace
