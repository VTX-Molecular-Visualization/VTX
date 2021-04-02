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

#ifndef MOLEQUEUE_JOBREFERENCEBASE_H
#define MOLEQUEUE_JOBREFERENCEBASE_H

#include "molequeueglobal.h"
#include "idtypeutils.h"

#include <QtCore/QDebug>

namespace MoleQueue
{
class JobData;
class JobManager;

/**
 * @class JobReferenceBase jobreferencebase.h <molequeue/jobreferencebase.h>
 * @brief Base class for lightweight interfaces to JobData objects.
 * @author David C. Lonie
 *
 * JobData objects, owned by JobManager, each contain data pertaining to a
 * specific job running a Program on a Queue. JobData contains several dynamic
 * properties that change during it's lifetime, e.g. Queue id and JobState. To
 * avoid having out-of-date references in the MoleQueue application, subclasses
 * of JobReferenceBase provide a convenient interface for obtaining and
 * modifying job properties.
 *
 * JobReferenceBase validates the pointer to the JobData object it represents by
 * querying the JobManager. The validity of the JobData pointer can be checked
 * with isValid(), which will return false if the JobData has been removed from
 * the JobManager. Subclasses of JobReferenceBase, Job on the Server and
 * JobRequest on the Client, will forward requests to the JobData. Certain
 * methods may cause signals to be emitted from JobManager; these cases will be
 * noted in the method documentation.
 */
class JobReferenceBase
{
public:
  /// Construct a new JobReferenceBase with the specified JobData
  explicit JobReferenceBase(JobData *jobdata = NULL);

  /// Construct a new JobReferenceBase for the job with the MoleQueueId
  /// in the indicated JobManager
  JobReferenceBase(JobManager *jobManager, IdType moleQueueId);

  /// Construct a new JobReferenceBase with the same JobData as @a other.
  JobReferenceBase(const JobReferenceBase &other)
    : m_jobData(other.m_jobData),m_jobManager(other.m_jobManager),
      m_moleQueueId(other.m_moleQueueId) {}

  virtual ~JobReferenceBase();

  /// Returns true if both JobReferenceBases are valid and refer to the same
  /// JobData instance.
  bool operator==(const JobReferenceBase &other) const
  {
    return isValid() && other.isValid() && m_jobData == other.m_jobData;
  }

  /// @return true if the guarded JobData pointer is valid, false otherwise.
  bool isValid() const;

  friend class JobManager;

protected:
  JobData * jobData() const { return m_jobData; }

  /// Print a warning with debugging info and return false if isValid() returns
  /// false.
  bool warnIfInvalid() const
  {
    if (Q_LIKELY(isValid()))
      return true;

    qWarning() << "Invalid reference to job with MoleQueue id "
               << idTypeToString(m_moleQueueId)
               << " accessed!";
    return false;
  }

  /// May be set to NULL during validation
  mutable JobData* m_jobData;
  JobManager *m_jobManager;
  /// Used to speed up lookups and validation
  mutable IdType m_moleQueueId;
};

} // namespace MoleQueue

#endif // MOLEQUEUE_JOBREFERENCEBASE_H
