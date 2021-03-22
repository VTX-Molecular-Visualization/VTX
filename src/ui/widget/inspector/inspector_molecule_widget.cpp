#include "inspector_molecule_widget.hpp"
#include "action/action_manager.hpp"
#include "action/transformable.hpp"
#include "style.hpp"
#include "ui/widget_factory.hpp"
#include <QBoxLayout>
#include <QFont>
#include <QGridLayout>
#include <QLabel>
#include <QPixmap>

namespace VTX::UI::Widget::Inspector
{
	InspectorMoleculeWidget::~InspectorMoleculeWidget() {}

	void InspectorMoleculeWidget::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );
		setContentsMargins( 0, 0, 0, 0 );

		QVBoxLayout * mainLayout = new QVBoxLayout( this );
		mainLayout->setContentsMargins( 0, 0, 0, 0 );
		mainLayout->setSpacing( 1 );

		_mainWidget
			= WidgetFactory::get().instantiateWidget<CustomWidget::CollapsingHeaderWidget>( this, "inspector_item" );
		_mainWidget->setContentsMargins( 0, 0, 0, 0 );
		_mainWidget->setHeaderHeight( Style::INSPECTOR_HEADER_HEIGHT );

		QWidget * mainContent = new QWidget();
		mainContent->setContentsMargins( 0, 0, 0, 0 );
		QVBoxLayout * contentLayout = new QVBoxLayout( mainContent );
		contentLayout->setContentsMargins( 0, 0, 0, 0 );

		_transformSection = WidgetFactory::get().instantiateWidget<InspectorSection>( this, "inspector_item_section" );
		_transformWidget  = WidgetFactory::get().instantiateWidget<CustomWidget::TransformWidget>(
			 this, "inspector_molecule_transform" );
		_transformSection->appendField( "", _transformWidget );

		_infoSection   = WidgetFactory::get().instantiateWidget<InspectorSection>( this, "inspector_item_section" );
		_fullnameLabel = new QLabel( this );
		_fullnameLabel->setWordWrap( true );
		_infoSection->appendField( "Full Name", _fullnameLabel );

		_nbChainsLabel = new QLabel( this );
		_nbChainsLabel->setWordWrap( true );
		_infoSection->appendField( "Nb Chains", _nbChainsLabel );

		_nbResiduesLabel = new QLabel( this );
		_nbResiduesLabel->setWordWrap( true );
		_infoSection->appendField( "Nb Residues", _nbResiduesLabel );

		_nbAtomsLabel = new QLabel( this );
		_nbAtomsLabel->setWordWrap( true );
		_infoSection->appendField( "Nb Atoms", _nbAtomsLabel );

		contentLayout->addWidget( _transformSection );
		contentLayout->addWidget( _infoSection );
		contentLayout->addStretch( 1000 );

		_mainWidget->setBody( mainContent );
		mainLayout->addWidget( _mainWidget );
	}

	void InspectorMoleculeWidget::_setupSlots()
	{
		connect( _transformWidget,
				 QOverload<const Math::Transform &>::of( &CustomWidget::TransformWidget::onValueChange ),
				 this,
				 &InspectorMoleculeWidget::_onTransformChange );
	};

	void InspectorMoleculeWidget::refresh()
	{
		if ( _model == nullptr )
			return;

		_mainWidget->setHeaderTitle( QString::fromStdString( _model->getDefaultName() ) );
		const QPixmap * symbolPixmap = Style::IconConst::get().getModelSymbol( _model->getTypeId() );
		_mainWidget->setHeaderIcon( *symbolPixmap );

		_transformWidget->setData( _model->getTransform() );

		_fullnameLabel->setText( QString::fromStdString( _model->getName() ) );
		_nbChainsLabel->setText( QString::fromStdString( std::to_string( _model->getChainCount() ) ) );
		_nbResiduesLabel->setText( QString::fromStdString( std::to_string( _model->getResidueCount() ) ) );
		_nbAtomsLabel->setText( QString::fromStdString( std::to_string( _model->getAtomCount() ) ) );
	}

	void InspectorMoleculeWidget::localize()
	{
		_transformSection->setHeaderTitle( "Transform" );
		_infoSection->setHeaderTitle( "Infos" );
		_infoSection->localize();
	}

	void InspectorMoleculeWidget::_onTransformChange( const Math::Transform & _transform )
	{
		VTX_ACTION( new VTX::Action::Transformable::ApplyTransform( *_model, _transform ) );
	}

	void InspectorMoleculeWidget::setModelEnableFromCheckBox( const int checkboxState )
	{
		_model->setEnable( checkboxState > 0 );
	}

} // namespace VTX::UI::Widget::Inspector
