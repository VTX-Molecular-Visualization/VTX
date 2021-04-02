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

#include "jobreferencebase.h"

#include "jobdata.h"
#include "jobmanager.h"

namespace MoleQueue {

JobReferenceBase::JobReferenceBase(JobData *jobdata)
  : m_jobData(jobdata),
    m_jobManager( Q_LIKELY(jobdata != NULL) ? jobdata->jobManager() : NULL),
    m_moleQueueId( Q_LIKELY(jobdata != NULL) ? jobdata->moleQueueId() :
                                               MoleQueue::InvalidId )

{
}

JobReferenceBase::JobReferenceBase(JobManager *jobManager, IdType moleQueueId)
  : m_jobData(jobManager->lookupJobDataByMoleQueueId(moleQueueId)),
    m_jobManager(jobManager),
    m_moleQueueId(moleQueueId)
{
}

JobReferenceBase::~JobReferenceBase()
{
}

bool JobReferenceBase::isValid() const
{
  if (m_jobData) {
    // If we have a molequeue id, validate the job data using the faster QMap
    // lookup, O(log(n))
    if (m_moleQueueId != InvalidId) {
      JobData *ref = m_jobManager->lookupJobDataByMoleQueueId(m_moleQueueId);
      if (ref) {
        if (ref == m_jobData)
          return true;

        qWarning() << "Job with molequeue id" << m_moleQueueId << "maps to a "
                      "different job than expected.\nExpected:\n"
                   << m_jobData << "\nLookup returned:\n"
                   << ref;
        return false;
      }
    }
    // If the cached molequeue id is invalid, do the slow QList lookup, O(n):
    if (m_jobManager->hasJobData(m_jobData)) {
      // m_jobData is still valid. Try to update our cached molequeueid:
      if (m_jobData->moleQueueId() != InvalidId)
        m_moleQueueId = m_jobData->moleQueueId();
      return true;
    }
    // m_jobData is gone...
    m_jobData = NULL;
    return false;
  }
  // If m_jobData is NULL, the reference is invalid
  return false;
}

} // namespace MoleQueue
