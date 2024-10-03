#include "ui/qt/dialog/export_image.hpp"
#include "ui/qt/application.hpp"
#include <QDialogButtonBox>

namespace VTX::UI::QT::Dialog
{

	ExportImage::ExportImage() : BaseWidget<ExportImage, QDialog>( APP_QT::getMainWindow() )
	{
		setWindowTitle( "Export image" );
		// TODO: try size policy?
		setMinimumSize( 300, 100 );

		// Resolution.
		QLabel * labelResolution = new QLabel( "Resolution", this );
		_comboBoxResolution		 = new QComboBox( this );
		_comboBoxResolution->setInsertPolicy( QComboBox::InsertPolicy::NoInsert );

		// Buttons.
		QDialogButtonBox * buttonBox = new QDialogButtonBox(
			QDialogButtonBox::StandardButton::Cancel | QDialogButtonBox::StandardButton::Save, this
		);
	}

} // namespace VTX::UI::QT::Dialog
