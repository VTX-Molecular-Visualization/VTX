#ifndef KEYWORDEDITOR_H
#define KEYWORDEDITOR_H

#include <QFile>
#include <QString>
#include <QVariant>
#include <utility>
#include <vector>

class ExtensionKeywordEditor
{
  public:
	ExtensionKeywordEditor();
	~ExtensionKeywordEditor();

	void readKeywordFile( QString filepath );
	void writeKeywordFile( QString newFilePath );

  private:
	std::vector<std::pair<QString, QString>> keywords;

	friend class ExtensionKeywordEditorGUI;
};

#endif
