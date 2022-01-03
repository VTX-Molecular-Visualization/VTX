#include "multiple_viewpoint_inspector_widget.hpp"
#include "action/action_manager.hpp"
#include "action/viewpoint.hpp"
#include "ui/widget_factory.hpp"

namespace VTX::UI::Widget::Inspector
{
	MultipleViewpointWidget::MultipleViewpointWidget( QWidget * p_parent ) :
		MultipleModelInspectorWidget( p_parent, ID::View::UI_INSPECTOR_VIEWPOINT ) {};

	MultipleViewpointWidget::~MultipleViewpointWidget() {}

	void MultipleViewpointWidget::_setupUi( const QString & p_name )
	{
		MultipleModelInspectorWidget::_setupUi( p_name );

		_transformSection = VTX::UI::WidgetFactory::get().instantiateWidget<InspectorSectionVLayout>(
			this, "inspector_item_section" );
		_transformWidget = UI::WidgetFactory::get().instantiateWidget<CustomWidget::TransformWidget>(
			this, "inspector_viewpoint_transform" );
		_transformWidget->displayScale( false );
		_transformSection->setBody( _transformWidget );

		_appendWidget( _transformSection );

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
	};

	void MultipleViewpointWidget::_endOfFrameRefresh( const SectionFlag & p_flag )
	{
		MultipleModelInspectorWidget::_endOfFrameRefresh( p_flag );

		_resetFieldStates( p_flag );

		const std::unordered_set<Model::Viewpoint *> & targets = getTargets();

		if ( targets.size() > 0 )
		{
			const QString headerTitle
				= QString::fromStdString( "Viewpoint (" + std::to_string( targets.size() ) + ")" );
			_getHeader()->setHeaderTitle( headerTitle );

			const QPixmap * symbolPixmap = Style::IconConst::get().getModelSymbol( VTX::ID::Model::MODEL_VIEWPOINT );
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

				CustomWidget::EmbeddedDataPushButton * const gotoButton
					= new CustomWidget::EmbeddedDataPushButton( this );
				const Model::ID & id = viewpoint->getId();
				gotoButton->setData( QVariant::fromValue<Model::ID>( id ) );

				QString gotoButtonTxt;

				if ( targets.size() == 1 )
					gotoButtonTxt = "Go to";
				else
					gotoButtonTxt = QString::fromStdString( "Go to " + viewpoint->getDefaultName() );

				gotoButton->setText( gotoButtonTxt );

				connect( gotoButton,
						 &CustomWidget::EmbeddedDataPushButton::embeddedDataclicked,
						 this,
						 &MultipleViewpointWidget::_goToAction );

				_appendWidget( gotoButton );

				_gotoButtons.emplace_back( gotoButton );
			}
		}
	}

	void MultipleViewpointWidget::_onTransformChange( const Math::Transform & p_transform ) const
	{
		if ( !signalsBlocked() )
		{
			std::vector<Model::Viewpoint *> viewpointsVector = std::vector<Model::Viewpoint *>();
			viewpointsVector.reserve( getTargets().size() );

			for ( Model::Viewpoint * target : getTargets() )
				viewpointsVector.emplace_back( target );

			VTX_ACTION( new Action::Viewpoint::Relocate( viewpointsVector, p_transform ) );
		}
	}

	void MultipleViewpointWidget::_onPositionDragged( const Vec3f & p_delta ) const
	{
		if ( !signalsBlocked() )
		{
			std::vector<Model::Viewpoint *> viewpointsVector = std::vector<Model::Viewpoint *>();
			viewpointsVector.reserve( getTargets().size() );

			for ( Model::Viewpoint * target : getTargets() )
				viewpointsVector.emplace_back( target );

			VTX_ACTION( new Action::Viewpoint::Translate( viewpointsVector, p_delta ) );
		}
	}
	void MultipleViewpointWidget::_onRotationDragged( const Vec3f & p_delta ) const
	{
		if ( !signalsBlocked() )
		{
			std::unordered_set<Model::Viewpoint *> viewpointsVector = std::unordered_set<Model::Viewpoint *>();
			viewpointsVector.reserve( getTargets().size() );

			for ( Model::Viewpoint * target : getTargets() )
				viewpointsVector.emplace( target );

			VTX_ACTION( new Action::Viewpoint::Rotate( viewpointsVector, p_delta ) );
		}
	}

	void MultipleViewpointWidget::_goToAction( const QVariant & p_viewpointIndex ) const
	{
		const Model::ID &		 modelID   = p_viewpointIndex.value<Model::ID>();
		const Model::Viewpoint & viewpoint = MVC::MvcManager::get().getModel<Model::Viewpoint>( modelID );

		VTX_ACTION( new Action::Viewpoint::GoTo( viewpoint ) );
	}

	void MultipleViewpointWidget::_resetFieldStates( const SectionFlag & p_flag )
	{
		if ( bool( p_flag & SectionFlag::INFOS ) )
		{
			_transformWidget->resetState();

			while ( _gotoButtons.size() > 0 )
			{
				delete _gotoButtons.back();
				_gotoButtons.pop_back();
			}
		}
	}

	void MultipleViewpointWidget::localize()
	{
		_transformSection->setHeaderTitle( "Transform" );
		_transformSection->localize();
	}
} // namespace VTX::UI::Widget::Inspector
