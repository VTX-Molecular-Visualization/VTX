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

#ifndef MOLEQUEUE_LOGWINDOW_H
#define MOLEQUEUE_LOGWINDOW_H

#include <QtWidgets/QDialog>

#include "molequeueglobal.h"

class QSpinBox;
class QTextBlockFormat;
class QTextCharFormat;
class QTextEdit;

namespace Ui {
class LogWindow;
}

namespace MoleQueue
{
class LogEntry;

/// Window that displays log contents
class LogWindow : public QDialog
{
  Q_OBJECT

public:
  /// If moleQueueId is set to something other than InvalidId, this window will
  /// filter its contents to only the entries related to the specified job.
  LogWindow(QWidget *theParent = 0, IdType moleQueueId = InvalidId);
  ~LogWindow();

signals:
  void aboutToClose();

protected:
  void changeEvent(QEvent *e);
  void closeEvent(QCloseEvent *);
  void hideEvent(QHideEvent *);
  void showEvent(QShowEvent *);

private slots:
  void addLogEntry(const MoleQueue::LogEntry &);
  void clearLog();
  void changeMaxEntries();

private:
  void createUi();
  void setupFormats();
  void initializeLogText();

  Ui::LogWindow *ui;

  QTextEdit *m_log;
  QSpinBox *m_maxEntries;

  QTextBlockFormat *m_logEntryBlockFormat;
  QTextCharFormat *m_timeStampFormat;
  QTextCharFormat *m_debugMessageFormat;
  QTextCharFormat *m_notificationFormat;
  QTextCharFormat *m_warningFormat;
  QTextCharFormat *m_errorFormat;
  QTextCharFormat *m_moleQueueIdFormat;
  QTextCharFormat *m_messageFormat;

  IdType m_moleQueueId;
};


} // namespace MoleQueue

#endif // MOLEQUEUE_LOGWINDOW_H
