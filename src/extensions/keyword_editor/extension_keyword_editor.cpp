#include "extension_keyword_editor.hpp"
#include <QRegularExpression>
#include <QTextStream>
#include <QVariant>

ExtensionKeywordEditor::ExtensionKeywordEditor() {}

ExtensionKeywordEditor::~ExtensionKeywordEditor() {}

bool ExtensionKeywordEditor::readKeywordFile( QString filepath )
{
	QFile keywordFile( filepath );
	if ( !keywordFile.exists() )
	{
		// return error
		return false;
	}

	if ( keywordFile.open( QIODevice::ReadOnly ) )
	{
		QTextStream in( &keywordFile );
		while ( !in.atEnd() )
		{
			QString line = in.readLine();

			// ignore lines starting with "#" corresponding to comments
			if ( line.indexOf( "#" ) == 0 || line.isEmpty() )
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
		keywordFile.close();
		return true;
	}
	else
	{
		return false;
	}
}

void ExtensionKeywordEditor::writeKeywordFile( QString newFilePath, QStringList & keywordsWithValue )
{
	QFile keywordFile( newFilePath );
	if ( !keywordFile.exists() )
	{
		// return error
	}

	if ( keywordFile.open( QIODevice::ReadWrite ) )
	{
		QTextStream stream( &keywordFile );

		for ( int keyword = 0; keyword < keywordsWithValue.size(); keyword++ )
		{
			stream << keywordsWithValue.at( keyword ) << Qt::endl;
		}

		keywordFile.close();
	}
}

std::vector<std::pair<QString, QString>> * ExtensionKeywordEditor::getKeywords()
{
	if ( keywords.size() != 0 )
	{
		return &keywords;
	}
	else
	{
		return nullptr;
	}
}
