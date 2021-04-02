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

#ifndef MOLEQUEUE_FILESYSTEMTOOLS_H
#define MOLEQUEUE_FILESYSTEMTOOLS_H

#include <QtCore/QString>

namespace MoleQueue {
namespace FileSystemTools {

/// Remove the directory at @a path. If @a deleteContentsOnly is true,
/// the directory itself will not be removed.
bool recursiveRemoveDirectory(const QString &path,
                              bool deleteContentsOnly = false);

/// Copy the contents of directory @a from into @a to.
bool recursiveCopyDirectory(const QString &from, const QString &to);

} // namespace FileSystemTools
} // namespace MoleQueue

#endif // MOLEQUEUE_FILESYSTEMTOOLS_H
