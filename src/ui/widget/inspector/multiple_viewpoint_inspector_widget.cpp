#include "multiple_viewpoint_inspector_widget.hpp"
#include "action/action_manager.hpp"
#include "action/viewpoint.hpp"
#include "ui/widget_factory.hpp"

namespace VTX::UI::Widget::Inspector
{
	MultipleViewpointWidget::MultipleViewpointWidget( QWidget * p_parent ) :
		MultipleModelInspectorWidget( p_parent, ID::View::UI_INSPECTOR_ATOM ) {};

	MultipleViewpointWidget::~MultipleViewpointWidget() {}

	void MultipleViewpointWidget::_setupUi( const QString & p_name )
	{
		MultipleModelInspectorWidget::_setupUi( p_name );

		_transformSection = VTX::UI::WidgetFactory::get().instantiateWidget<InspectorSectionVLayout>(
			this, "inspector_item_section" );
		_transformWidget = UI::WidgetFactory::get().instantiateWidget<CustomWidget::TransformWidget>(
			this, "inspector_viewpoint_transform" );
		_transformSection->setBody( _transformWidget );

		_gotoButton = new CustomWidget::QPushButtonMultiField( this );

		_appendWidget( _transformSection );
		_appendWidget( _gotoButton );

		const bool oldBlockState = blockSignals( true );
		refresh();
		blockSignals( oldBlockState );
	}

	void MultipleViewpointWidget::_setupSlots()
	{
		connect( _transformWidget,
				 &CustomWidget::TransformWidget::onValueChange,
				 this,
				 &MultipleViewpointWidget::_onTransformChange );
		connect( _transformWidget,
				 &CustomWidget::TransformWidget::onPositionDragged,
				 this,
				 &MultipleViewpointWidget::_onPositionDragged );
		connect( _transformWidget,
				 &CustomWidget::TransformWidget::onRotationDragged,
				 this,
				 &MultipleViewpointWidget::_onRotationDragged );

		connect(
			_gotoButton, &CustomWidget::QPushButtonMultiField::clicked, this, &MultipleViewpointWidget::_goToAction );
	};

	void MultipleViewpointWidget::_endOfFrameRefresh( const SectionFlag & p_flag )
	{
		MultipleModelInspectorWidget::_endOfFrameRefresh( p_flag );

		_resetFieldStates( p_flag );

		const std::unordered_set<Model::Viewpoint *> & targets = _getTargets();

		if ( targets.size() > 0 )
		{
			const QString headerTitle
				= QString::fromStdString( "Viewpoint (" + std::to_string( targets.size() ) + ")" );
			_getHeader()->setHeaderTitle( headerTitle );

			const QPixmap * symbolPixmap = Style::IconConst::get().getModelSymbol( ID::Model::MODEL_ATOM );
			_getHeader()->setHeaderIcon( *symbolPixmap );

			for ( const Model::Viewpoint * viewpoint : targets )
			{
				if ( bool( p_flag & SectionFlag::INFOS ) )
				{
					if ( !_transformWidget->hasDifferentData() )
					{
						Math::Transform viewpointTransform
							= Math::Transform( viewpoint->getPosition(), viewpoint->getRotation(), VEC3F_XYZ );
						_transformWidget->updateWithNewValue( viewpointTransform );
					}
				}
			}

			_gotoButton->setEnabled( targets.size() == 1 );
		}
	}

	void MultipleViewpointWidget::_onTransformChange( const Math::Transform & p_transform ) const
	{
		if ( !signalsBlocked() )
		{
			std::vector<Model::Viewpoint *> viewpointsVector = std::vector<Model::Viewpoint *>();
			viewpointsVector.reserve( _getTargets().size() );

			for ( Model::Viewpoint * target : _getTargets() )
				viewpointsVector.emplace_back( target );

			VTX_ACTION( new Action::Viewpoint::Relocate( viewpointsVector, p_transform ) );
		}
	}

	void MultipleViewpointWidget::_onPositionDragged( const Vec3f & p_delta ) const
	{
		if ( !signalsBlocked() )
		{
			std::vector<Model::Viewpoint *> viewpointsVector = std::vector<Model::Viewpoint *>();
			viewpointsVector.reserve( _getTargets().size() );

			for ( Model::Viewpoint * target : _getTargets() )
				viewpointsVector.emplace_back( target );

			VTX_ACTION( new Action::Viewpoint::Translate( viewpointsVector, p_delta ) );
		}
	}
	void MultipleViewpointWidget::_onRotationDragged( const Vec3f & p_delta ) const
	{
		if ( !signalsBlocked() )
		{
			std::unordered_set<Model::Viewpoint *> viewpointsVector = std::unordered_set<Model::Viewpoint *>();
			viewpointsVector.reserve( _getTargets().size() );

			for ( Model::Viewpoint * target : _getTargets() )
				viewpointsVector.emplace( target );

			VTX_ACTION( new Action::Viewpoint::Rotate( viewpointsVector, p_delta ) );
		}
	}

	void MultipleViewpointWidget::_goToAction() const
	{
		if ( _getTargets().size() != 1 )
			return;

		Model::Viewpoint * viewpoint = *( _getTargets().begin() );

		VTX_ACTION( new Action::Viewpoint::GoTo( *viewpoint ) );
	}

	void MultipleViewpointWidget::_resetFieldStates( const SectionFlag & p_flag )
	{
		if ( bool( p_flag & SectionFlag::INFOS ) )
		{
			_transformWidget->resetState();
		}
	}

	void MultipleViewpointWidget::localize()
	{
		_transformSection->setHeaderTitle( "Transform" );
		_transformSection->localize();
		_gotoButton->setText( "Go to" );
	}
} // namespace VTX::UI::Widget::Inspector
