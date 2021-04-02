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

#ifndef MOLEQUEUE_FILESPEC_H
#define MOLEQUEUE_FILESPEC_H

#include <qjsonobject.h>

#include <QtCore/QByteArray>
#include <QtCore/QString>

class QDir;
class QFile;

namespace MoleQueue
{
class FileSpecificationPrivate;

/**
 * @class FileSpecification filespecification.h <molequeue/filespecification.h>
 * @brief Specify files for simplifying Client-Server communication.
 * @author David C. Lonie
 *
 * The FileSpecification class contains a description of a file to facilite
 * file manipulation during RPC communication. Files are stored as either a path
 * to the local file on disk, or a filename and content string.
 */
class FileSpecification
{
public:
  /// Recognized internal formats for storing file data
  enum Format {
    /// Invalid format
    InvalidFileSpecification = -1,
    /// Single "path" member pointing to a location on the filesystem
    PathFileSpecification = 0,
    /// Filename and content strings.
    ContentsFileSpecification
  };

  /// Creates an invalid FileSpecification.
  FileSpecification();

  /// Create a FileSpecification using the members of the input QJsonObject.
  explicit FileSpecification(const QJsonObject &json);

  /// Create a FileSpecification from the input absolute filepath.
  explicit FileSpecification(const QString &path);

  /// Create a FileSpecification from the filename and content strings.
  FileSpecification(const QString &filename_, const QString &contents_);

  /// Create a FileSpecification from the specified file using the indicated
  /// format
  FileSpecification(QFile *file, Format format_ = PathFileSpecification);

  /// Copy a FileSpecification
  FileSpecification(const FileSpecification &other);

  /// Copy a FileSpecification
  FileSpecification & operator=(const FileSpecification &other);

  /// Destroy the FileSpec
  ~FileSpecification();

  /// @return The format of the FileSpec
  /// @see FileSpecification::Format
  Format format() const;

  /// @return True if the FileSpecification is formatted properly, false
  /// otherwise.
  bool isValid() const { return format() != InvalidFileSpecification; }

  /// @return The FileSpecification as a formatted JSON string.
  QByteArray toJson() const;

  /// @return The FileSpecification as a formatted JSON string.
  QJsonObject toJsonObject() const;

  /// @return Whether or not the FileSpecification refers to an existing file
  /// @note This will always be false if format() does not return
  /// ContentsFileSpecification.
  bool fileExists() const;

  /// Write contents() to a file with @a filename_ in @a dir. If @a filename
  /// is not specified, filename() will be used instead (default).
  /// @return True if the file is successfully written.
  bool writeFile(const QDir &dir, const QString &filename_ = QString()) const;

  /// @return The filename (without path) of the FileSpecification.
  QString filename() const;

  /// @return The contents of the file.
  QString contents() const;

  /// @return The filename (with path) of the FileSpecification.
  /// @note This function only makes sense if format() is PathFileSpecification.
  /// It will always return a null string otherwise.
  QString filepath() const;

  /// @return True if the filename has an extension ("file.ext"), false
  /// otherwise ("file")
  bool fileHasExtension() const;

  /// @return The filename without an extension.
  QString fileBaseName() const;

  /// @return The file extension, if any, or a null string.
  /// @see hasFileExtension
  QString fileExtension() const;

private:
  QJsonObject m_json;
};

} // namespace MoleQueue

#endif // MOLEQUEUE_FILESPEC_H
