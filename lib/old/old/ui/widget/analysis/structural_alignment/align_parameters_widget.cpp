#include "align_parameters_widget.hpp"
#include "analysis/structural_alignment_method/ce_align.hpp"
#include "style.hpp"
#include "ui/widget_factory.hpp"
#include <QHBoxLayout>
#include <QVBoxLayout>

namespace VTX::UI::Widget::Analysis::StructuralAlignment
{
	AlignParametersWidget::AlignParametersWidget( QWidget * p_parent ) : BaseManualWidget( p_parent ) {}

	void AlignParametersWidget::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		QHBoxLayout * const mainLayout = new QHBoxLayout( this );
		mainLayout->setContentsMargins( 0, 0, 0, 0 );

		QVBoxLayout * const verticalLayout = new QVBoxLayout();
		verticalLayout->setContentsMargins( 0, 0, 0, 0 );

		_attributeLayout = new Layout::AttributeListLayout();

		_methodWidget = new QComboBox( this );

		for ( int i = 0; i < int( VTX::Analysis::StructuralAlignment::AlignmentMethodEnum::COUNT ); i++ )
		{
			const std::string & methodStr = VTX::Analysis::StructuralAlignment::ALIGNMENT_METHOD_STRING[ i ];
			_methodWidget->addItem( QString::fromStdString( methodStr ) );
		}

		_windowSizeWidget = WidgetFactory::get().instantiateWidget<CustomWidget::IntegerFieldSliderWidget>(
			this, "windowSizeWidget" );
		_windowSizeWidget->setMinMax( 1, 16 );

		_gapMaxWidget
			= WidgetFactory::get().instantiateWidget<CustomWidget::IntegerFieldSliderWidget>( this, "gapMaxWidget" );
		_gapMaxWidget->setMinMax( 2, 50 );

		_maxPathWidget
			= WidgetFactory::get().instantiateWidget<CustomWidget::IntegerFieldSliderWidget>( this, "maxPathWidget" );
		_maxPathWidget->setMinMax( 1, 50 );

		_d0Widget = WidgetFactory::get().instantiateWidget<CustomWidget::FloatFieldSliderWidget>( this, "d0Widget" );
		_d0Widget->setMinMax( 0.01f, 10.0f );

		_d1Widget = WidgetFactory::get().instantiateWidget<CustomWidget::FloatFieldSliderWidget>( this, "d1Widget" );
		_d1Widget->setMinMax( 0.01f, 10.0f );

		_considerWaterButton		  = new QCheckBox( this );
		_considerHiddenResiduesButton = new QCheckBox( this );

		_attributeLayout->addAttribute( _methodWidget, "Method" );

		_attributeLayout->addAttribute( _windowSizeWidget, "Window size" );
		_attributeLayout->addAttribute( _gapMaxWidget, "Gap max" );
		_attributeLayout->addAttribute( _maxPathWidget, "Max Path" );

		_attributeLayout->addAttribute( _d0Widget, "D0" );
		_attributeLayout->addAttribute( _d1Widget, "D1" );

		_attributeLayout->addAttribute( _considerWaterButton, "Consider Water" );
		_attributeLayout->addAttribute( _considerHiddenResiduesButton, "Consider Hidden Atoms" );

		verticalLayout->addLayout( _attributeLayout );
		verticalLayout->addStretch( 1000 );

		mainLayout->addLayout( verticalLayout );
		mainLayout->addStretch( 1000 );

		setLayout( mainLayout );
	}

	void AlignParametersWidget::_setupSlots()
	{
		connect( _methodWidget, &QComboBox::currentIndexChanged, this, &AlignParametersWidget::_methodHasChanged );
	}

	void AlignParametersWidget::localize() {}

	void AlignParametersWidget::applyParameter( const AlignmentParameters & p_alignParameter )
	{
		_methodWidget->setCurrentIndex( int( p_alignParameter.method ) );

		_attributeLayout->setAttributeVisibility( _d0Widget, false );
		_attributeLayout->setAttributeVisibility( _d1Widget, false );

		_windowSizeWidget->setValue( p_alignParameter.windowSize );
		_gapMaxWidget->setValue( p_alignParameter.gapMax );
		_maxPathWidget->setValue( p_alignParameter.maxPath );

		_considerWaterButton->setChecked( p_alignParameter.considerWater );
		_considerHiddenResiduesButton->setChecked( p_alignParameter.considerHiddenResidue );

		switch ( p_alignParameter.method )
		{
		case VTX::Analysis::StructuralAlignment::AlignmentMethodEnum::CEAlign:
		{
			const VTX::Analysis::StructuralAlignmentMethod::CEAlign::CustomParameters castedParameters
				= dynamic_cast<const VTX::Analysis::StructuralAlignmentMethod::CEAlign::CustomParameters &>(
					p_alignParameter );
			_attributeLayout->setAttributeVisibility( _d0Widget, true );
			_d0Widget->setValue( castedParameters.d0 );
			_attributeLayout->setAttributeVisibility( _d1Widget, true );
			_d1Widget->setValue( castedParameters.d1 );
		}
		break;

		default:
			VTX_ERROR( "Alignment method " + std::to_string( int( p_alignParameter.method ) )
					   + " not managed in AlignParametersWidget::applyParameter." );
			break;
		}
	}

	AlignParametersWidget::AlignmentParameters * AlignParametersWidget::generateParameters() const
	{
		AlignParametersWidget::AlignmentParameters * parameters = nullptr;

		VTX::Analysis::StructuralAlignment::AlignmentMethodEnum method
			= VTX::Analysis::StructuralAlignment::AlignmentMethodEnum( _methodWidget->currentIndex() );

		switch ( method )
		{
		case VTX::Analysis::StructuralAlignment::AlignmentMethodEnum::CEAlign:
			parameters = _generateCEAlignParameter();
			break;

		default:
			VTX_ERROR( "Alignment method " + std::to_string( int( method ) )
					   + " not managed in AlignParametersWidget::generateParameters." );
			break;
		}

		if ( parameters != nullptr )
		{
			parameters->windowSize = _windowSizeWidget->getValue();
			parameters->gapMax	   = _gapMaxWidget->getValue();
			parameters->maxPath	   = _maxPathWidget->getValue();
		}

		return parameters;
	}

	AlignParametersWidget::AlignmentParameters * AlignParametersWidget::_generateCEAlignParameter() const
	{
		VTX::Analysis::StructuralAlignmentMethod::CEAlign::CustomParameters * const parameters
			= new VTX::Analysis::StructuralAlignmentMethod::CEAlign::CustomParameters();

		parameters->windowSize = _windowSizeWidget->getValue();
		parameters->gapMax	   = _gapMaxWidget->getValue();
		parameters->maxPath	   = _maxPathWidget->getValue();

		parameters->considerHiddenResidue = _considerHiddenResiduesButton->checkState() == Qt::CheckState::Checked;
		parameters->considerWater		  = _considerWaterButton->checkState() == Qt::CheckState::Checked;

		parameters->d0 = _d0Widget->getValue();
		parameters->d1 = _d1Widget->getValue();

		return parameters;
	}

	void AlignParametersWidget::_methodHasChanged( const int p_methodIndex )
	{
		const VTX::Analysis::StructuralAlignment::AlignmentMethodEnum method
			= VTX::Analysis::StructuralAlignment::AlignmentMethodEnum( p_methodIndex );

		VTX::Analysis::StructuralAlignment::AlignmentParameters * const defaultParameters
			= VTX::Analysis::StructuralAlignment::instantiateDefaultParameters( method );

		applyParameter( *defaultParameters );

		delete defaultParameters;
	}

} // namespace VTX::UI::Widget::Analysis::StructuralAlignment
