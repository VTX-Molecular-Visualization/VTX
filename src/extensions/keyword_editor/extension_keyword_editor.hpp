#include <QFile>
#include <QString>
#include <QVariant>
#include <utility>
#include <vector>

class ExtensionKeywordEditor
{
  public:
	ExtensionKeywordEditor( QString filepath );
	~ExtensionKeywordEditor();

	void readKeywordFile();

  private:
	QFile *									 keywordFile;
	std::vector<std::pair<QString, QString>> keywords;
};
