#include "ui/qt/tool/pytx/widget/command_line_prompt.hpp"
#include "ui/qt/tool/pytx/details/include_python_binding.hpp"
#include <QCompleter>
#include <QHBoxLayout>
#include <QLabel>
#include <util/logger.hpp>

namespace VTX::UI::QT::Tool::PyTX::Widget
{
	CommandLinePrompt::CommandLinePrompt( QWidget * p_parent ) : BaseManualWidget( p_parent ) { _setupCompleter(); }

	void CommandLinePrompt::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		setContentsMargins( 0, 0, 0, 0 );

		QLabel * const promptLabel = new QLabel( this );
		promptLabel->setText( "Command >" );

		_promptWidget = new QLineEdit( this );
		_promptWidget->setPlaceholderText( "Write command here ( e.g. select( mol_n='1AGA' )" );

		_promptWidget->setCompleter( _completer );
		_promptWidget->setClearButtonEnabled( true );

		QHBoxLayout * const horizontalLayout = new QHBoxLayout( this );
		horizontalLayout->addWidget( promptLabel, 1 );
		horizontalLayout->addWidget( _promptWidget, 100 );
	}

	void CommandLinePrompt::_setupSlots()
	{
		connect( _promptWidget, &QLineEdit::returnPressed, this, &CommandLinePrompt::_launchCommand );
	};
	void CommandLinePrompt::localize() {}

	void CommandLinePrompt::_launchCommand()
	{
		if ( _promptWidget->text().isEmpty() )
			return;

		const std::string command = _promptWidget->text().toStdString();

		try
		{
			PythonBinding::INTERPRETOR().runCommand( command );
		}
		catch ( CommandException & p_e )
		{
			_promptWidget->clear();
			throw p_e;
		}

		_promptWidget->clear();
	}

	void CommandLinePrompt::_setupCompleter()
	{
		// std::vector<std::string> allCommands = PythonBinding::INTERPRETOR().getModule().commands().getFunctionList();

		QStringList strList = QStringList();

		strList.emplaceBack( "resetCamera()" );
		strList.emplaceBack( "quit()" );

		// for ( const std::string & str : allCommands )
		//{
		//	strList.emplaceBack( QString::fromStdString( str ) );
		// }

		_completer = new QCompleter( strList, this );
		_completer->setCaseSensitivity( Qt::CaseSensitivity::CaseInsensitive );
		_completer->setCompletionMode( QCompleter::CompletionMode::InlineCompletion );
	}

} // namespace VTX::UI::QT::Tool::PyTX::Widget
