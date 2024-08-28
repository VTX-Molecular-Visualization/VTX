#ifndef __VTX_TOOL_EXAMPLE_WIDGET_DIALOG__
#define __VTX_TOOL_EXAMPLE_WIDGET_DIALOG__

#include <QDialog>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <qt/base_widget.hpp>

namespace VTX::Tool::Example::Widget
{

	class Dialog : public UI::QT::BaseWidget<Dialog, QDialog>
	{
	  public:
		Dialog()
		{
			setWindowTitle( "Example tool dialog" );
			setMinimumSize( 300, 100 );

			// Layout.
			QVBoxLayout * layout = new QVBoxLayout( this );

			// Add other widgets.
			//

			// Buttons.
			QDialogButtonBox * buttonBox = new QDialogButtonBox(
				QDialogButtonBox::StandardButton::Cancel | QDialogButtonBox::StandardButton::Open, this
			);

			layout->addWidget( buttonBox );
			setLayout( layout );

			// Callbacks.
			connect(
				buttonBox->button( QDialogButtonBox::StandardButton::Open ),
				&QPushButton::clicked,
				[ this ]()
				{
					// Implement logic.
					//

					close();
				}
			);
			connect(
				buttonBox->button( QDialogButtonBox::StandardButton::Cancel ),
				&QPushButton::clicked,
				[ this ]() { close(); }
			);
		}
		virtual ~Dialog() {}
	};

} // namespace VTX::Tool::Example::Widget

#endif
