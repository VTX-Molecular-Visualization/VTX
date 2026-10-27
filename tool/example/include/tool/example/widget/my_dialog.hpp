#ifndef VTX_TOOL_EXAMPLE_WIDGET_MY_DIALOG_HPP
#define VTX_TOOL_EXAMPLE_WIDGET_MY_DIALOG_HPP

#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <ui/qt/dialog/base_dialog.hpp>
#include <ui/qt/helper.hpp>

namespace VTX::Tool::Example::Widget
{

	class MyDialog : public UI::QT::Dialog::BaseDialog<MyDialog>
	{
	  public:
		MyDialog()
		{
			setWindowTitle( "Example tool dialog" );
			setMinimumSize( 300, 100 );

			// Layout.
			auto * layout = UI::QT::Helper::makeQObject<QVBoxLayout>( this );

			// Add other widgets.
			//

			// Buttons.
			auto * buttonBox = UI::QT::Helper::makeQObject<QDialogButtonBox>(
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
				[]()
				{
					// TODO: logic
				}
			);
		}

		~MyDialog() override = default;
	};

} // namespace VTX::Tool::Example::Widget

#endif
