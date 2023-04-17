#include "old_tool/ui/widget/custom_widget/citations_window.hpp"
#include <QClipboard>

CitationsWindowWidget::CitationsWindowWidget( QWidget * p_parent ) : QDialog( p_parent, Qt::WindowCloseButtonHint )
{
	ui.setupUi( this );
}

void CitationsWindowWidget::onCopyCitationsToClipboardPressed()
{
	QClipboard * clipboard = QGuiApplication::clipboard();
	clipboard->setText( ui.textBrowserCitations->toPlainText() );
}
