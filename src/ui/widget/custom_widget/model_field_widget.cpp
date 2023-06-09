#include "model_field_widget.hpp"
#include "mvc/mvc_manager.hpp"
#include "ui/mime_type.hpp"
#include "ui/widget_factory.hpp"
#include <QHBoxLayout>

namespace VTX::UI::Widget::CustomWidget
{
	ModelFieldWidget::ModelFieldWidget( QWidget * p_parent ) :
		CustomWidget::ModelDropArea( p_parent ), DraggableItem( this )
	{
		_registerEvent( Event::Global::MODEL_REMOVED );
	}

	void ModelFieldWidget::receiveEvent( const Event::VTXEvent & p_event )
	{
		if ( p_event.name == Event::Global::MODEL_REMOVED )
		{
			const Event::VTXEventPtr<Model::BaseModel> & castedEvent
				= dynamic_cast<const Event::VTXEventPtr<Model::BaseModel> &>( p_event );

			if ( castedEvent.ptr == _model )
				setModel( nullptr );
		}
	}

	void ModelFieldWidget::_setupUi( const QString & p_name )
	{
		CustomWidget::ModelDropArea::_setupUi( p_name );

		setSizePolicy( QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum );
		// setMaximumHeight( 24 );

		setFrameShape( QFrame::Shape::Box );
		setFrameShadow( QFrame::Shadow::Sunken );

		_label				 = new QLabel( this );
		_modelTypeIconWidget = new QLabel( this );

		QHBoxLayout * const horizontalLayout = new QHBoxLayout( this );
		horizontalLayout->setContentsMargins( 4, 4, 4, 4 );

		horizontalLayout->addWidget( _modelTypeIconWidget );
		horizontalLayout->addWidget( _label, 10 );
		horizontalLayout->addStretch( 1000 );

		setLayout( horizontalLayout );
		refresh();
	}

	void ModelFieldWidget::_setupSlots()
	{
		CustomWidget::ModelDropArea::_setupSlots();

		connect( this, &CustomWidget::ModelDropArea::onModelDropped, this, &ModelFieldWidget::_onModelDropped );
	}

	void ModelFieldWidget::localize() { CustomWidget::ModelDropArea::localize(); };

	void ModelFieldWidget::refresh()
	{
		const Model::BaseModel * const model = getModel();

		if ( model == nullptr )
		{
			_label->setDisabled( true );
			_label->setText( _placeholder );
			_modelTypeIconWidget->setVisible( false );
		}
		else
		{
			_label->setDisabled( false );
			_label->setText( QString::fromStdString( model->getDefaultName() ) );
			_modelTypeIconWidget->setVisible( true );
			_modelTypeIconWidget->setPixmap( *Style::IconConst::get().getModelSymbol( model->getTypeId() ) );
		}

		adjustSize();
	}

	void ModelFieldWidget::setModel( Model::BaseModel * const p_model )
	{
		if ( _model != p_model )
		{
			emit onModelChanged( p_model );

			_model = p_model;
			refresh();
		}
	}

	void ModelFieldWidget::_onModelDropped( Model::BaseModel * const p_model ) { setModel( p_model ); }

	QMimeData * ModelFieldWidget::_getDataForDrag() const
	{
		if ( _model == nullptr )
			return nullptr;

		return VTX::UI::MimeType::generateMimeDataFromModel( *_model, UI::MimeType::DragSource::MODEL_FIELD );
	}

} // namespace VTX::UI::Widget::CustomWidget
