#include "ui/old_ui/ui/widget/custom_widget/trajectory_widget.hpp"
#include "ui/old_ui/ui/widget_factory.hpp"
#include "ui/old_ui/util/ui.hpp"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <app/old/action/molecule.hpp>
#include <app/old/component/chemistry/enum_trajectory.hpp>
#include <app/old/application/setting.hpp>

namespace VTX::UI::Widget::CustomWidget
{
	TrajectoryWidget::TrajectoryWidget( QWidget * p_parent ) :
		BaseManualWidget<QWidget>( p_parent ), TMultiDataField<App::Old::Component::Chemistry::Molecule>()
	{
	}

	void TrajectoryWidget::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		QGridLayout * const _mainLayout		   = new QGridLayout( this );
		QHBoxLayout * const _playButtonsLayout = new QHBoxLayout();

		_timeline = WidgetFactory::get().instantiateWidget<CustomWidget::IntegerFieldSliderWidget>(
			this, "trajectory_timeline" );
		_timeline->setSingleStep( 1 );
		_timeline->setMin( 0 );

		_backToStartButton = new QPushButton( this );
		_backToStartButton->setIcon( QIcon( ":/sprite/trajectory_first_frame_icon.png" ) );
		_previousFrameButton = new QPushButton( this );
		_previousFrameButton->setIcon( QIcon( ":/sprite/trajectory_previous_frame_icon.png" ) );
		_playButton = new QPushButton( this );
		_playButton->setIcon( QIcon( ":/sprite/trajectory_play_icon.png" ) );
		_nextFrameButton = new QPushButton( this );
		_nextFrameButton->setIcon( QIcon( ":/sprite/trajectory_next_frame_icon.png" ) );
		_goToEndButton = new QPushButton( this );
		_goToEndButton->setIcon( QIcon( ":/sprite/trajectory_last_frame_icon.png" ) );

		QLabel * playModeLabel = new QLabel( this );
		playModeLabel->setText( "Play mode" );
		_playModeWidget = new CustomWidget::QComboBoxMultiField( this );
		Util::UI::filterEventOnWidget( _playModeWidget, QEvent::Type::Wheel );

		_fillPlayModeComboBox();

		QLabel * speedLabel = new QLabel( this );
		speedLabel->setText( "Speed" );
		_speedWidget = WidgetFactory::get().instantiateWidget<CustomWidget::IntegerFieldSliderWidget>(
			this, "trajectory_speed" );
		_speedWidget->setMinMax( VTX::App::Old::Application::Setting::MIN_TRAJECTORY_SPEED, VTX::App::Old::Application::Setting::MAX_TRAJECTORY_SPEED );

		// QLabel * playRangeLabel = new QLabel( this );
		// playRangeLabel->setText( "Play range" );
		//_playRangeWidget
		//	= WidgetFactory::get().instantiateWidget<CustomWidget::IntegerRangeWidget>( this, "trajectory_play_range" );

		_playButtonsLayout->addWidget( _backToStartButton );
		_playButtonsLayout->addWidget( _previousFrameButton );
		_playButtonsLayout->addWidget( _playButton );
		_playButtonsLayout->addWidget( _nextFrameButton );
		_playButtonsLayout->addWidget( _goToEndButton );

		_mainLayout->addWidget( _timeline, 0, 0, 1, 2 );
		_mainLayout->addItem( _playButtonsLayout, 1, 0, 1, 2 );
		_mainLayout->addWidget( playModeLabel, 2, 0 );
		_mainLayout->addWidget( _playModeWidget, 2, 1 );
		_mainLayout->addWidget( speedLabel, 3, 0 );
		_mainLayout->addWidget( _speedWidget, 3, 1 );
		//_mainLayout->addWidget( playRangeLabel, 4, 0 );
		//_mainLayout->addWidget( _playRangeWidget, 4, 1 );
	}
	void TrajectoryWidget::_setupSlots()
	{
		connect( _timeline,
				 &CustomWidget::IntegerFieldSliderWidget::onValueChange,
				 this,
				 &TrajectoryWidget::_setFrameAction );

		connect( _backToStartButton, &QPushButton::clicked, this, &TrajectoryWidget::_backToStartAction );
		connect( _previousFrameButton, &QPushButton::clicked, this, &TrajectoryWidget::_previousFrameAction );
		connect( _playButton, &QPushButton::clicked, this, &TrajectoryWidget::_togglePlayAction );
		connect( _nextFrameButton, &QPushButton::clicked, this, &TrajectoryWidget::_nextFrameAction );
		connect( _goToEndButton, &QPushButton::clicked, this, &TrajectoryWidget::_goToEndAction );

		connect( _playModeWidget,
				 QOverload<int>::of( &QComboBoxMultiField::currentIndexChanged ),
				 this,
				 &TrajectoryWidget::_playModeChange );

		connect( _speedWidget,
				 &CustomWidget::IntegerFieldSliderWidget::onValueChange,
				 this,
				 &TrajectoryWidget::_speedChange );
	}

	void TrajectoryWidget::refreshTimer()
	{
		_timeline->resetState();

		for ( const App::Old::Component::Chemistry::Molecule * const molecule : _molecules )
		{
			_timeline->updateWithNewValue( molecule->getFrame() );
		}
	}

	void TrajectoryWidget::resetState()
	{
		_molecules.clear();
		_timeline->resetState();
		const bool oldBlockSignals = _timeline->blockSignals( true );
		_timeline->setMax( 0 );
		_timeline->blockSignals( oldBlockSignals );

		_playButton->setIcon( QIcon( ":/sprite/trajectory_play_icon.png" ) );

		_playModeWidget->resetState();
		_speedWidget->resetState();
		//_playRangeWidget->resetState();
	}
	void TrajectoryWidget::updateWithNewValue( App::Old::Component::Chemistry::Molecule & p_molecule )
	{
		_molecules.emplace( &p_molecule );

		const uint currentMax = uint( _timeline->getMax() );
		if ( ( p_molecule.getFrameCount() - 1 ) > currentMax )
		{
			_timeline->setMinMax( 0, p_molecule.getFrameCount() - 1 );

			int pageStep = p_molecule.getFrameCount() / 10;
			pageStep	 = pageStep < 1 ? 1 : pageStep;
			_timeline->setPageStep( pageStep );
		}

		if ( p_molecule.isPlaying() )
			_playButton->setIcon( QIcon( ":/sprite/trajectory_pause_icon.png" ) );

		_timeline->updateWithNewValue( p_molecule.getFrame() );
		_playModeWidget->updateWithNewValue( int( p_molecule.getPlayMode() ) );
		_speedWidget->updateWithNewValue( p_molecule.getFPS() );
		//	_playRangeWidget->resetState();
	}

	void TrajectoryWidget::_displayDifferentsDataFeedback() {}

	void TrajectoryWidget::localize() {}

	void TrajectoryWidget::_setFrameAction( const int p_frame )
	{
		VTX_ACTION( new App::Old::Action::Molecule::ChangeFrame( _molecules, p_frame, true ) );
	}

	void TrajectoryWidget::_backToStartAction()
	{
		VTX_ACTION( new App::Old::Action::Molecule::ChangeFrame( _molecules, 0, true ) );
	}
	void TrajectoryWidget::_previousFrameAction()
	{
		VTX_ACTION( new App::Old::Action::Molecule::PreviousFrame( _molecules, true ) );
	}
	void TrajectoryWidget::_togglePlayAction()
	{
		// if one is playing => set to pause else play all
		bool setPlay = true;

		for ( const App::Old::Component::Chemistry::Molecule * const molecule : _molecules )
			setPlay = setPlay && !molecule->isPlaying();

		VTX_ACTION( new App::Old::Action::Molecule::ChangeIsPlaying( _molecules, setPlay ) );
	}
	void TrajectoryWidget::_nextFrameAction()
	{
		VTX_ACTION( new App::Old::Action::Molecule::NextFrame( _molecules, true ) );
	}
	void TrajectoryWidget::_goToEndAction()
	{
		VTX_ACTION( new App::Old::Action::Molecule::ChangeFrame( _molecules, INT32_MAX, true ) );
	}

	void TrajectoryWidget::_playModeChange( const int p_playMode )
	{
		const App::Old::Component::Chemistry::PlayMode playmode	 = App::Old::Component::Chemistry::PlayMode( p_playMode );
		bool									  dataChange = false;

		for ( const App::Old::Component::Chemistry::Molecule * const molecule : _molecules )
		{
			if ( molecule->getPlayMode() != playmode )
			{
				dataChange = true;
				break;
			}
		}

		if ( dataChange )
			VTX_ACTION( new App::Old::Action::Molecule::ChangePlayMode( _molecules, playmode ) );
	}
	void TrajectoryWidget::_speedChange( const int p_speed )
	{
		bool dataChange = false;

		for ( const App::Old::Component::Chemistry::Molecule * const molecule : _molecules )
		{
			if ( molecule->getFPS() != p_speed )
			{
				dataChange = true;
				break;
			}
		}

		if ( dataChange )
			VTX_ACTION( new App::Old::Action::Molecule::ChangeFPS( _molecules, p_speed ) );
	}

	void TrajectoryWidget::_fillPlayModeComboBox()
	{
		for ( const std::string & str : App::Old::Component::Chemistry::PLAY_MODE_STRING )
		{
			_playModeWidget->addItem( QString::fromStdString( str ) );
		}
	}

} // namespace VTX::UI::Widget::CustomWidget
