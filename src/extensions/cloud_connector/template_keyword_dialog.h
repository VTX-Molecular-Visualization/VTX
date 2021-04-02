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

#ifndef MOLEQUEUE_TEMPLATEKEYWORDDIALOG_H
#define MOLEQUEUE_TEMPLATEKEYWORDDIALOG_H

#include <QtWidgets/QDialog>
#include <QtGui/QTextBlockFormat>
#include <QtGui/QTextCharFormat>
#include <QtGui/QTextCursor>

namespace Ui {
class TemplateKeywordDialog;
}

namespace MoleQueue {

/// @brief Dialog explaining how templates are used in MoleQueue.
class TemplateKeywordDialog : public QDialog
{
  Q_OBJECT

public:
  explicit TemplateKeywordDialog(QWidget *parent_ = 0);
  ~TemplateKeywordDialog();

private:
  void buildKeywordLists();
  void buildDocument();
  void addDocumentHeader(const QString &header, QTextCursor &cur);
  void addHelpText(const QString &text, QTextCursor &cur);
  void addKeywordHeader(const QString &header, QTextCursor &cur);
  void addKeywordMap(const QMap<QString, QString> &map, QTextCursor &cur);
  void highlightKeywords();

  Ui::TemplateKeywordDialog *ui;

  QTextBlockFormat m_docHeaderBlockFormat;
  QTextBlockFormat m_helpTextBlockFormat;
  QTextBlockFormat m_keywordHeaderBlockFormat;
  QTextBlockFormat m_keywordListBlockFormat;

  QTextCharFormat m_docHeaderCharFormat;
  QTextCharFormat m_helpTextCharFormat;
  QTextCharFormat m_keywordHeaderCharFormat;
  QTextCharFormat m_keywordDescriptionCharFormat;
  QTextCharFormat m_keywordCharFormat;
  QTextCharFormat m_dangerousKeywordCharFormat;

  QMap<QString, QString> m_jobKeywords;
  QMap<QString, QString> m_queueKeywords;
};


} // namespace MoleQueue
#endif // MOLEQUEUE_TEMPLATEKEYWORDDIALOG_H
