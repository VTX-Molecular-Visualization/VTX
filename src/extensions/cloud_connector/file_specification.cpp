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

#include "filespecification.h"

#include "logger.h"

#include <qjsondocument.h>

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>

#include <string>

namespace MoleQueue
{

FileSpecification::FileSpecification()
{
}

FileSpecification::FileSpecification(const QJsonObject &json)
{
  m_json = json;
}

FileSpecification::FileSpecification(const QString &path)
{
  m_json.insert("path", path);
}

FileSpecification::FileSpecification(const QString &filename_,
                                     const QString &contents_)
{
  m_json.insert("filename", filename_);
  m_json.insert("contents", contents_);
}

FileSpecification::FileSpecification(QFile *file,
                                     FileSpecification::Format format_)
{
  switch (format_) {
  case PathFileSpecification:
    m_json.insert("path", QFileInfo(*file).absoluteFilePath());
    break;
  case ContentsFileSpecification: {
    m_json.insert("filename", QFileInfo(*file).fileName());
    if (!file->open(QFile::ReadOnly | QFile::Text)) {
      Logger::logError(Logger::tr("Error opening file for read: '%1'")
                       .arg(file->fileName()));
      return;
    }
    m_json.insert("contents", QString(file->readAll()));
    file->close();
    break;
  }
  case InvalidFileSpecification:
    Logger::logDebugMessage(Logger::tr("Cannot convert file to invalid file "
                                       "spec! (%1)").arg(file->fileName()));
    break;
  default:
    Logger::logDebugMessage(Logger::tr("Unknown filespec format (%1) for file "
                                       "'%2'").arg(format_).arg(file->fileName()));
    break;
  }
}

FileSpecification::FileSpecification(const FileSpecification &other)
{
  m_json = other.m_json;
}

FileSpecification &FileSpecification::operator=(const FileSpecification &other)
{
  m_json = other.m_json;
  return *this;
}

FileSpecification::~FileSpecification()
{
}

FileSpecification::Format FileSpecification::format() const
{
  if (m_json.contains("path")) {
    return PathFileSpecification;
  }
  else if (m_json.contains("filename") &&
           m_json.contains("contents")) {
    return ContentsFileSpecification;
  }

  return InvalidFileSpecification;
}

QByteArray FileSpecification::toJson() const
{
  return QJsonDocument(m_json).toJson();
}

QJsonObject FileSpecification::toJsonObject() const
{
  return m_json;
}

bool FileSpecification::fileExists() const
{
  if (format() == PathFileSpecification)
    return QFile::exists(m_json.value("path").toString());

  return false;
}

bool FileSpecification::writeFile(const QDir &dir, const QString &filename_) const
{
  switch (format()) {
  default:
  case InvalidFileSpecification:
    return false;
  case PathFileSpecification:
  case ContentsFileSpecification: {
    QString path = dir.absoluteFilePath(filename_.isNull() ? filename()
                                                           : filename_);
    QFile file(path);
    if (!file.open(QFile::WriteOnly | QFile::Truncate | QFile::Text))
      return false;
    file.write(contents().toLocal8Bit());
    file.close();

    return true;
  }
  }
}

QString FileSpecification::filename() const
{
  switch (format()) {
  default:
  case InvalidFileSpecification:
    Logger::logDebugMessage(Logger::tr("Cannot extract filename from invalid "
                                       "filespec\n%1").arg(QString(toJson())));
    return QString();
  case PathFileSpecification:
    return QFileInfo(m_json.value("path").toString()).fileName();
  case ContentsFileSpecification:
    return QFileInfo(m_json.value("filename").toString()).fileName();
  }
}

QString FileSpecification::contents() const
{
  switch (format()) {
  default:
  case InvalidFileSpecification:
    Logger::logWarning(Logger::tr("Cannot read contents of invalid filespec:"
                                  "\n%1").arg(QString(toJson())));
    return QString();
  case PathFileSpecification: {
    QFile file(filepath());
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
      Logger::logError(Logger::tr("Error opening file for read: '%1'")
                       .arg(file.fileName()));
      return QString();
    }
    QString result = file.readAll();
    file.close();
    return result;
  }
  case ContentsFileSpecification:
    return QString(m_json.value("contents").toString());
  }

}

QString FileSpecification::filepath() const
{
  if (format() == PathFileSpecification)
    return QFileInfo(m_json.value("path").toString()).absoluteFilePath();
  return QString();
}

bool FileSpecification::fileHasExtension() const
{
  return !QFileInfo(filename()).suffix().isEmpty();
}

QString FileSpecification::fileBaseName() const
{
  return QFileInfo(filename()).baseName();
}

QString FileSpecification::fileExtension() const
{
  return QFileInfo(filename()).suffix();
}

} // namespace MoleQueue
