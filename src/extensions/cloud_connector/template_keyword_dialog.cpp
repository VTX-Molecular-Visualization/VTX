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

#include "templatekeyworddialog.h"
#include "ui_templatekeyworddialog.h"

#include <QtGui/QBrush>
#include <QtGui/QTextCursor>
#include <QtGui/QTextDocument>

namespace MoleQueue {

TemplateKeywordDialog::TemplateKeywordDialog(QWidget *parent_) :
  QDialog(parent_),
  ui(new Ui::TemplateKeywordDialog)
{
  ui->setupUi(this);

  m_docHeaderBlockFormat.setAlignment(Qt::AlignHCenter);
  m_docHeaderBlockFormat.setTopMargin(-5);
  m_docHeaderCharFormat.setFontPointSize(12);

  m_helpTextBlockFormat.setAlignment(Qt::AlignJustify);
  m_helpTextBlockFormat.setTextIndent(20);

  m_keywordHeaderBlockFormat.setAlignment(Qt::AlignHCenter);
  m_keywordHeaderBlockFormat.setTopMargin(10);
  m_keywordHeaderBlockFormat.setBottomMargin(5);
  m_keywordHeaderCharFormat.setFontPointSize(10);

  m_keywordListBlockFormat.setAlignment(Qt::AlignJustify);
  m_keywordListBlockFormat.setTextIndent(-40);
  m_keywordListBlockFormat.setIndent(1);

  m_keywordCharFormat.setForeground(QBrush(Qt::blue));
  m_keywordCharFormat.setFontItalic(true);

  m_dangerousKeywordCharFormat.setForeground(QBrush(Qt::darkRed));
  m_keywordCharFormat.setFontItalic(true);

  buildKeywordLists();
  buildDocument();
}

TemplateKeywordDialog::~TemplateKeywordDialog()
{
  delete ui;
}

void TemplateKeywordDialog::buildKeywordLists()
{
  // Jobs
  m_jobKeywords.insert("$$inputFileName$$",
                       tr("Name of the current job's input file."));
  m_jobKeywords.insert("$$inputFileBaseName$$",
                       tr("Name of the current job's input file without the "
                          "file extension."));
  m_jobKeywords.insert("$$moleQueueId$$",
                       tr("MoleQueue ID number of current job."));
  m_jobKeywords.insert("$$numberOfCores$$",
                       tr("Number of processor cores requested by current "
                          "job."));
  m_jobKeywords.insert("$$maxWallTime$$",
                       tr("The maximum walltime for the current job (i.e. the "
                          "time limit before the queue will automatically "
                          "stop the job, regardless of completion state). If "
                          "the job's specified walltime is less than or equal "
                          "to zero minutes, the default "
                          "walltime (configured in the queue settings) is used."
                          " See $$$maxWallTime$$$ for a method of using the "
                          "default walltime set by the queue administrator. "
                          "Available only on remote queues."));
  m_jobKeywords.insert("$$$maxWallTime$$$",
                       tr("Same as $$maxWallTime$$, but if the job specific "
                          "walltime is not set, the entire line containing "
                          "this keyword will be removed from the final "
                          "template output. This is used to apply "
                          "the default walltime set by the queuing system's "
                          "administrator. Only available on remote queuing "
                          "systems."));
  m_jobKeywords.insert(tr("Custom"),
                       tr("Certain clients may allow custom keyword "
                          "replacements in their jobs. Consult the client "
                          "documentation to see if these are available and how "
                          "they are to be specified in the template."));

  // Queue
  m_queueKeywords.insert("$$programExecution$$",
                         tr("Used in remote queue templates to indicate where "
                            "to place program-specific executable details (e.g."
                            " where something like '[executable] < [inputfile]'"
                            " should be placed). Must only be used in a queue "
                            "configuration template (this keyword replacement "
                            "is used to generate the program specific "
                            "template)."));
}

void TemplateKeywordDialog::buildDocument()
{
  QTextCursor cur(ui->textEdit->document());
  cur.movePosition(QTextCursor::Start);
  cur.beginEditBlock();

  // Doc header
  addDocumentHeader(tr("Templates in MoleQueue"), cur);

  // Help text
  addHelpText(tr("Templates are used to specify how "
                 "programs are started on a each queue, and are "
                 "customized in two places in MoleQueue:\n"
                 "Non-local queues "
                 "(e.g. PBS, SGE, etc) use batch scripts to specify "
                 "program execution, and a template for a queue batch "
                 "script is entered in the remote queue configuration, "
                 "using the $$programExecution$$ keyword to indicate "
                 "where program-specific execution should go.\n"
                 "The program configuration dialog allows further "
                 "customization of the input template, providing a set "
                 "of common execution methods and the option to "
                 "customize them. The program-specific input template "
                 "may completely override the queue template, but will "
                 "use it as a starting point initially.\n"
                 "The following list of keywords may be used in the "
                 "input templates and are replaced by information "
                 "appropriate to a specific job. Keywords are enclosed "
                 "in '$$' or '$$$' and are case sensitive. Keywords with two "
                 "'$' symbols will be replaced by the appropriate data, while "
                 " those with three '$' have more specialized behavior ("
                 "see the maxWallTime variants for an example).\n"
                 "Any unrecognized keywords that are not replaced during script"
                 " generation will be removed and a warning printed to the "
                 "log."), cur);

  // Jobs
  addKeywordHeader(tr("Job specific keywords:"), cur);
  addKeywordMap(m_jobKeywords, cur);

  // Queue
  addKeywordHeader(tr("Queue specific keywords:"), cur);
  addKeywordMap(m_queueKeywords, cur);

  cur.endEditBlock();

  highlightKeywords();
}

void TemplateKeywordDialog::addDocumentHeader(const QString &header,
                                              QTextCursor &cur)
{
  cur.insertBlock(m_docHeaderBlockFormat);
  cur.setCharFormat(m_docHeaderCharFormat);
  cur.insertText(header);
}

void TemplateKeywordDialog::addHelpText(const QString &text, QTextCursor &cur)
{
  cur.insertBlock(m_helpTextBlockFormat);
  cur.setCharFormat(m_helpTextCharFormat);
  cur.insertText(text);
}

void TemplateKeywordDialog::addKeywordHeader(const QString &header,
                                             QTextCursor &cur)
{
  cur.insertBlock(m_keywordHeaderBlockFormat);
  cur.setCharFormat(m_keywordHeaderCharFormat);
  cur.insertText(header);
}

void TemplateKeywordDialog::addKeywordMap(const QMap<QString, QString> &map,
                                          QTextCursor &cur)
{
  foreach (const QString &keyword, map.keys()) {
    const QString &desc = map[keyword];
    cur.insertBlock(m_keywordListBlockFormat);
    cur.setCharFormat(m_keywordDescriptionCharFormat);
    cur.insertText(QString("%1: %2").arg(keyword, desc));
  }
}

void TemplateKeywordDialog::highlightKeywords()
{
  QTextDocument *doc = ui->textEdit->document();
  QTextCursor cur(doc);
  cur.movePosition(QTextCursor::Start);

  QRegExp expr("[^\\$]?\\${2,2}[^\\$\\s]+\\${2,2}[^\\$]?");

  cur = doc->find(expr, cur);
  while (!cur.isNull()) {
    cur.setCharFormat(m_keywordCharFormat);
    cur = doc->find(expr, cur);
  }

  cur.movePosition(QTextCursor::Start);

  expr.setPattern("[^\\$]?\\${3,3}[^\\$\\s]+\\${3,3}[^\\$]?");

  cur = doc->find(expr, cur);
  while (!cur.isNull()) {
    cur.setCharFormat(m_dangerousKeywordCharFormat);
    cur = doc->find(expr, cur);
  }
}

} // namespace MoleQueue
