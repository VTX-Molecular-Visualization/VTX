#include "ui/old_ui/ui/widget/custom_widget/filename_field_widget.hpp"
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <app/internal/io/filesystem.hpp>

namespace VTX::UI::Widget::CustomWidget
{
	FilenameFieldWidget::FilenameFieldWidget( QWidget * p_parent ) : BaseManualWidget( p_parent ) {}

	void FilenameFieldWidget::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );
		setMaxLength( App::Internal::IO::Filesystem::MAX_FILE_LENGTH );

		QRegularExpressionValidator * const validator = new QRegularExpressionValidator(
			QRegularExpression( QString::fromStdString( App::Internal::IO::Filesystem::REGEX_VALID_FILENAME ) ), this );
		setValidator( validator );
	}
	void FilenameFieldWidget::_setupSlots() {}
	void FilenameFieldWidget::localize() {}

} // namespace VTX::UI::Widget::CustomWidget
