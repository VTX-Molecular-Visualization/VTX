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

#include "filesystemtools.h"

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QString>

namespace MoleQueue {
namespace FileSystemTools {

bool recursiveRemoveDirectory(const QString &p, bool deleteContentsOnly)
{
  QString path = QDir::cleanPath(p);
  // Just a safety to prevent accidentally wiping /
  if (path.isEmpty() || path.simplified() == "/")
    return false;

  bool result = true;
  QDir dir;
  dir.setPath(path);

  if (dir.exists()) {
    foreach (QFileInfo info, dir.entryInfoList(
               QDir::NoDotAndDotDot | QDir::System | QDir::Hidden |
               QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {

      result = info.isDir() ? recursiveRemoveDirectory(info.absoluteFilePath())
                            : QFile::remove(info.absoluteFilePath());

      if (!result)
        return false;
    }
    // Remove the directory if needed
    if (!deleteContentsOnly)
      result = dir.rmdir(path);
  }

  if (!result)
    return false;

  return true;
}

bool recursiveCopyDirectory(const QString &from, const QString &to)
{
  bool result = true;

  QDir fromDir;
  fromDir.setPath(from);
  if (!fromDir.exists())
    return false;

  QDir toDir;
  toDir.setPath(to);
  if (!toDir.exists()) {
    if (!toDir.mkdir(toDir.absolutePath()))
      return false;
  }

  foreach (QFileInfo info, fromDir.entryInfoList(
             QDir::NoDotAndDotDot | QDir::System | QDir::Hidden |
             QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
    QString newTargetPath = QString ("%1/%2")
    .arg(toDir.absolutePath(),
         fromDir.relativeFilePath(info.absoluteFilePath()));
    if (info.isDir()) {
      result = recursiveCopyDirectory(info.absoluteFilePath(),
                                            newTargetPath);
    }
    else {
      result = QFile::copy(info.absoluteFilePath(),
                           newTargetPath);
    }

    if (!result)
      return false;
  }

  return true;
}

} // namespace FileSystemTools
} // namespace MoleQueue
