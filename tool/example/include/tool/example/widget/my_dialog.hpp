#ifndef __VTX_TOOL_EXAMPLE_WIDGET_MY_DIALOG__
#define __VTX_TOOL_EXAMPLE_WIDGET_MY_DIALOG__

#include <QDialog>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <ui/qt/base_widget.hpp>
#include <ui/qt/widget/main_window.hpp>

namespace VTX::Tool::Example::Widget
{

	class MyDialog : public UI::QT::BaseWidget<MyDialog, QDialog>
	{
	  public:
		MyDialog() : UI::QT::BaseWidget<MyDialog, QDialog>( UI::QT::WIDGETS::get().get<UI::QT::Widget::MainWindow *>() )
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
		virtual ~MyDialog() {}
	};

} // namespace VTX::Tool::Example::Widget

#endif
