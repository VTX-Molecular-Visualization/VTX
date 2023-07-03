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

	bool									   readKeywordFile( QString filepath );
	void									   writeKeywordFile( QString newFilePath, QStringList & keywordsWithValue );
	std::vector<std::pair<QString, QString>> * getKeywords();

  private:
	std::vector<std::pair<QString, QString>> keywords;
};

#endif
