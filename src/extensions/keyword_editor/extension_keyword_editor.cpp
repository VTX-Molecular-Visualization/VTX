#include "extension_keyword_editor.hpp"
#include <QRegularExpression>
#include <QTextStream>
#include <QVariant>

ExtensionKeywordEditor::ExtensionKeywordEditor( QString filepath )
{
	keywordFile = new QFile( filepath );
	if ( !keywordFile->exists() )
	{
		// return error
	}
}

ExtensionKeywordEditor::~ExtensionKeywordEditor() {}

void ExtensionKeywordEditor::readKeywordFile()
{
	if ( keywordFile->open( QIODevice::ReadOnly ) )
	{
		QTextStream in( keywordFile );
		while ( !in.atEnd() )
		{
			QString line = in.readLine();

			// ignore lines starting with "#" corresponding to comments
			if ( line.indexOf( "#" ) == 0 )
			{
				continue;
			}

			// split lines sparated by tab/white space
			QStringList fields = line.split( QRegularExpression( "\\s" ), Qt::SkipEmptyParts );
			if ( fields.size() == 1 )
			{
				keywords.push_back( std::pair<QString, QString>( fields.at( 0 ), QString( "" ) ) );
			}
			if ( fields.size() == 2 )
			{
				keywords.push_back( std::pair<QString, QString>( fields.at( 0 ), fields.at( 1 ) ) );
			}
		}
	}
	keywordFile->close();
}
