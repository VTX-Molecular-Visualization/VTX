#include "ui/qt/widget/inspector/panel.hpp"
#include "ui/qt/widget_factory.hpp"
#include <app/component/chemistry/molecule.hpp>
#include <qpushbutton.h>
#include <QVBoxLayout>
#include <iostream>

namespace VTX::UI::QT::Widget::Inspector
{
	Panel::Panel( QWidget * p_parent )
	{
		_p_item		 = App::Component::Scene::SceneItemComponent();
		_inspectorGw = std::make_unique<Gateways::InspectorGw>();
	}

	void Panel::localize()
	{
		setWindowTitle( "Inspector" );
		_inspectorGw->registerResource( this );
	};

	void Panel::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		QWidget * const mainWidget = _instantiateMainWidget( QSize( 200, 0 ), QSize( 200, 0 ) );

		QVBoxLayout * const mainLayout = new QVBoxLayout( mainWidget );
		mainLayout->setContentsMargins( 0, 0, 0, 0 );

		QHBoxLayout * const _playButtonsLayout = new QHBoxLayout();

		_playButton = new QPushButton( this );
		_playButton->setIcon( QIcon( ":/sprite/trajectory_play_icon.png" ) );
		_pauseButton = new QPushButton( this );
		_pauseButton->setIcon( QIcon( ":/sprite/trajectory_pause_icon.png" ) );
		_stopButton = new QPushButton( this );
		_stopButton->setIcon( QIcon( ":/sprite/background_white_icon.png" ) );

		_playButtonsLayout->addWidget( _playButton );
		_playButtonsLayout->addWidget( _pauseButton );
		_playButtonsLayout->addWidget( _stopButton );
		mainLayout->addItem( _playButtonsLayout);

		setWidget( mainWidget );
	}

	void Panel::_setupSlots()
	{
		connect( _playButton, &QPushButton::clicked, this, &Panel::_togglePlayAction );
		connect( _pauseButton, &QPushButton::clicked, this, &Panel::_togglePauseAction );
		connect( _stopButton, &QPushButton::clicked, this, &Panel::_toggleStopAction );
	}

	void Panel::_togglePlayAction() { _inspectorGw->setPlayTrajectoryPlayer( _p_item ); }
	void Panel::_togglePauseAction() { _inspectorGw->setPauseTrajectoryPlayer( _p_item ); }
	void Panel::_toggleStopAction() { _inspectorGw->setStopTrajectoryPlayer( _p_item ); }
} // namespace VTX::UI::QT::Widget::Inspector
