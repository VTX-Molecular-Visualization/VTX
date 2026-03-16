#ifndef __VTX_TOOL_EXAMPLE_WIDGET_MY_DIALOG__
#define __VTX_TOOL_EXAMPLE_WIDGET_MY_DIALOG__

#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <ui/qt/dialog/base_dialog.hpp>

namespace VTX::Tool::Example::Widget
{

	class MyDialog : public UI::QT::Dialog::BaseDialog<MyDialog>
	{
	  public:
		MyDialog() : BaseDialog()
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
			connect( buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept );
			connect( buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject );

			connect(
				this,
				&QDialog::accepted,
				[ this ]()
				{
					// TODO: logic
				}
			);
		}
		virtual ~MyDialog() {}
	};

} // namespace VTX::Tool::Example::Widget

#endif
