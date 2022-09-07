#include "model_field_widget.hpp"
#include "mvc/mvc_manager.hpp"
#include "ui/mime_type.hpp"
#include "ui/widget_factory.hpp"
#include <QHBoxLayout>

namespace VTX::UI::Widget::CustomWidget
{
	void ModelFieldWidget::_setupUi( const QString & p_name )
	{
		CustomWidget::ModelDropArea::_setupUi( p_name );

		setSizePolicy( QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding );
		setFrameShape( QFrame::Shape::Box );
		setFrameShadow( QFrame::Shadow::Sunken );

		_label				 = new QLabel( this );
		_modelTypeIconWidget = new QLabel( this );

		QHBoxLayout * const horizontalLayout = new QHBoxLayout( this );

		horizontalLayout->addWidget( _modelTypeIconWidget );
		horizontalLayout->addWidget( _label, 10 );
		horizontalLayout->addStretch( 1000 );

		setLayout( horizontalLayout );
		refresh();
	}

	void ModelFieldWidget::_setupSlots()
	{
		CustomWidget::ModelDropArea::_setupSlots();

		connect( this, &CustomWidget::ModelDropArea::onModelChanged, this, &ModelFieldWidget::_onModelDropped );
	}

	void ModelFieldWidget::localize() { CustomWidget::ModelDropArea::localize(); };

	void ModelFieldWidget::refresh()
	{
		const Model::BaseModel * const model = getModel();
		_refresh( model );
	}

	void ModelFieldWidget::_refresh( const Model::BaseModel * const p_model )
	{
		if ( p_model == nullptr )
		{
			_label->setDisabled( true );
			_label->setText( _placeholder );
			_modelTypeIconWidget->setVisible( false );
		}
		else
		{
			_label->setDisabled( false );
			_label->setText( QString::fromStdString( p_model->getDefaultName() ) );
			_modelTypeIconWidget->setVisible( true );
			_modelTypeIconWidget->setPixmap( *Style::IconConst::get().getModelSymbol( p_model->getTypeId() ) );
		}

		adjustSize();
	}

	void ModelFieldWidget::_onModelDropped( const Model::BaseModel * const p_model ) { _refresh( p_model ); }

} // namespace VTX::UI::Widget::CustomWidget
