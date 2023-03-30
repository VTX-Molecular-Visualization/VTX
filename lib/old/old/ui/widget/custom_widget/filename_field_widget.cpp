#include "filename_field_widget.hpp"
#include <util/filesystem.hpp>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

namespace VTX::UI::Widget::CustomWidget
{
	FilenameFieldWidget::FilenameFieldWidget( QWidget * p_parent ) : BaseManualWidget( p_parent ) {}

	void FilenameFieldWidget::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );
		setMaxLength( Util::Filesystem::MAX_FILE_LENGTH );

		QRegularExpressionValidator * const validator
			= new QRegularExpressionValidator( QRegularExpression( Util::Filesystem::REGEX_VALID_FILENAME ), this );
		setValidator( validator );
	}
	void FilenameFieldWidget::_setupSlots() {}
	void FilenameFieldWidget::localize() {}

} // namespace VTX::UI::Widget::CustomWidget
