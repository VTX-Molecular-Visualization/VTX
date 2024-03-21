#include "ui/qt/tool/pytx/widget/command_line_prompt.hpp"
#include "ui/qt/tool/pytx/details/include_python_binding.hpp"
#include <QHBoxLayout>
#include <QLabel>
#include <util/logger.hpp>

namespace VTX::UI::QT::Tool::PyTX::Widget
{
	CommandLinePrompt::CommandLinePrompt( QWidget * p_parent ) : BaseManualWidget( p_parent ) {}

	void CommandLinePrompt::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		setContentsMargins( 0, 0, 0, 0 );

		QLabel * const promptLabel = new QLabel( this );
		promptLabel->setText( "Command >" );

		_promptWidget = new QLineEdit( this );
		_promptWidget->setPlaceholderText( "Write command here ( e.g. select( mol_n='1AGA' )" );
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
} // namespace VTX::UI::QT::Tool::PyTX::Widget
