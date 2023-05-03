#include "ui/old_ui/ui/widget/inspector/multiple_viewpoint_inspector_widget.hpp"
#include "ui/old_ui/ui/widget_factory.hpp"
#include "ui/qt/action/viewpoint.hpp"

#include <app/action/viewpoint.hpp>

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

		_actionSection = VTX::UI::WidgetFactory::get().instantiateWidget<InspectorSectionVLayout>(
			this, "inspector_item_section" );

		_actionContainer		   = new QWidget( this );
		QVBoxLayout * actionLayout = new QVBoxLayout( _actionContainer );

		_relocateButton = new QPushButton( this );
		_relocateButton->setText( "Relocate" );
		actionLayout->addWidget( _relocateButton );

		_actionSection->setBody( _actionContainer );

		_appendWidget( _transformSection );
		_appendWidget( _actionSection );

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

		connect( _relocateButton, &QPushButton::clicked, this, &MultipleViewpointWidget::_relocateAction );
	};

	void MultipleViewpointWidget::_endOfFrameRefresh( const SectionFlag & p_flag )
	{
		MultipleModelInspectorWidget::_endOfFrameRefresh( p_flag );

		_resetFieldStates( p_flag );

		const std::unordered_set<App::Component::Object3D::Viewpoint *> & targets = getTargets();

		if ( targets.size() > 0 )
		{
			const QString headerTitle
				= QString::fromStdString( "Viewpoint (" + std::to_string( targets.size() ) + ")" );
			_getHeader()->setHeaderTitle( headerTitle );

			const QPixmap * symbolPixmap = Style::IconConst::get().getModelSymbol( App::ID::Model::MODEL_VIEWPOINT );
			_getHeader()->setHeaderIcon( *symbolPixmap );

			for ( const App::Component::Object3D::Viewpoint * viewpoint : targets )
			{
				if ( bool( p_flag & SectionFlag::TRANSFORM ) )
				{
					if ( !_transformWidget->hasDifferentData() )
					{
					 App::Internal::Math::Transform viewpointTransform
							= App::Internal::Math::Transform( viewpoint->getPosition(), viewpoint->getRotation(), VEC3F_XYZ );
						_transformWidget->updateWithNewValue( viewpointTransform );
					}
				}

				if ( bool( p_flag & SectionFlag::INFOS ) )
				{
					CustomWidget::EmbeddedDataPushButton * const gotoButton
						= new CustomWidget::EmbeddedDataPushButton( this );
					const App::Core::Model::ID & id = viewpoint->getId();
					gotoButton->setData( QVariant::fromValue<App::Core::Model::ID>( id ) );

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

					_actionContainer->layout()->addWidget( gotoButton );

					_gotoButtons.emplace_back( gotoButton );
				}
			}
		}
	}

	void MultipleViewpointWidget::_onTransformChange( const App::Internal::Math::Transform & p_transform ) const
	{
		if ( !signalsBlocked() )
		{
			std::vector<App::Component::Object3D::Viewpoint *> viewpointsVector = std::vector<App::Component::Object3D::Viewpoint *>();
			viewpointsVector.reserve( getTargets().size() );

			for ( App::Component::Object3D::Viewpoint * target : getTargets() )
				viewpointsVector.emplace_back( target );

			VTX_ACTION( new App::Action::Viewpoint::Relocate( viewpointsVector, p_transform ) );
		}
	}

	void MultipleViewpointWidget::_onPositionDragged( const Vec3f & p_delta ) const
	{
		if ( !signalsBlocked() )
		{
			std::vector<App::Component::Object3D::Viewpoint *> viewpointsVector = std::vector<App::Component::Object3D::Viewpoint *>();
			viewpointsVector.reserve( getTargets().size() );

			for ( App::Component::Object3D::Viewpoint * target : getTargets() )
				viewpointsVector.emplace_back( target );

			VTX_ACTION( new App::Action::Viewpoint::Translate( viewpointsVector, p_delta ) );
		}
	}
	void MultipleViewpointWidget::_onRotationDragged( const Vec3f & p_delta ) const
	{
		if ( !signalsBlocked() )
		{
			std::unordered_set<App::Component::Object3D::Viewpoint *> viewpointsVector = std::unordered_set<App::Component::Object3D::Viewpoint *>();
			viewpointsVector.reserve( getTargets().size() );

			for ( App::Component::Object3D::Viewpoint * target : getTargets() )
				viewpointsVector.emplace( target );

			VTX_ACTION( new App::Action::Viewpoint::Rotate( viewpointsVector, p_delta ) );
		}
	}

	void MultipleViewpointWidget::_goToAction( const QVariant & p_viewpointIndex ) const
	{
		const App::Core::Model::ID &		 modelID   = p_viewpointIndex.value<App::Core::Model::ID>();
		const App::Component::Object3D::Viewpoint & viewpoint = VTX::MVC_MANAGER().getModel<App::Component::Object3D::Viewpoint>( modelID );

		VTX_ACTION( new QT::Action::Viewpoint::GoTo( viewpoint ) );
	}

	void MultipleViewpointWidget::_relocateAction() const
	{
		if ( !signalsBlocked() )
		{
			std::vector<App::Component::Object3D::Viewpoint *> viewpointsVector = std::vector<App::Component::Object3D::Viewpoint *>();
			viewpointsVector.reserve( getTargets().size() );

			for ( App::Component::Object3D::Viewpoint * target : getTargets() )
				viewpointsVector.emplace_back( target );

			VTX_ACTION( new App::Action::Viewpoint::Relocate( viewpointsVector ) );
		}
	}

	void MultipleViewpointWidget::_resetFieldStates( const SectionFlag & p_flag )
	{
		if ( bool( p_flag & SectionFlag::TRANSFORM ) )
		{
			_transformWidget->resetState();
		}

		if ( bool( p_flag & SectionFlag::INFOS ) )
		{
			while ( _gotoButtons.size() > 0 )
			{
				_actionContainer->layout()->removeWidget( _gotoButtons.back() );

				delete _gotoButtons.back();
				_gotoButtons.pop_back();
			}
		}
	}

	void MultipleViewpointWidget::localize()
	{
		_transformSection->setHeaderTitle( "Transform" );
		_actionSection->setHeaderTitle( "Action" );
		_transformSection->localize();
	}
} // namespace VTX::UI::Widget::Inspector
