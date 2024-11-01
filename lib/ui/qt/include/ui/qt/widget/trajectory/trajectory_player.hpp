#ifndef __VTX_UI_QT_WIDGET_TRAJECTORY_PLAYER__
#define __VTX_UI_QT_WIDGET_TRAJECTORY_PLAYER__

#include "app/application/system/ecs_system.hpp"
//#include <functional>
#include <ui/qt/base_widget.hpp>
#include <app/component/chemistry/trajectory.hpp>
#include <app/core/uid/uid.hpp>
#include <app/action/trajectory.hpp>
#include <QDockWidget>
#include <QPushButton>
#include <QToolButton>
#include <QSlider>
#include <QLineEdit>

namespace VTX::UI::QT::Widget
{

	class TrajectoryPlayer : public QWidget
	{
	  public:
		TrajectoryPlayer( QWidget * p_parent, const App::Core::UID::UIDRange & p_moleculeUID ) :
			QWidget( p_parent ), _moleculeUID( p_moleculeUID )
		{
			setupLayout();

			setupPlayerElts();

			addPlayerToLayout();

			connectCallbacks();
		}

		virtual ~TrajectoryPlayer() {}
		
	  private:
		void setupLayout()
		{
			_widget = new QWidget( this );
			_widget->setContentsMargins( 0, 0, 0, 0 );

			_layout = new QVBoxLayout( _widget );
			_layout->setContentsMargins( 0, 0, 0, 0 );

			_playButtonsLayout = new QHBoxLayout();
			_progressLayout	 = new QHBoxLayout();
		}

		void setupPlayerElts()
		{
			setupStop();
			setupPlayPause();
			setupProgress();
			setupFrameSelector();
			setupTmpPlayerSelector();
		}
		void setupStop()
		{
			_stopElt = new QPushButton( this );
			_stopElt->setIcon( QIcon( ":/sprite/background_white_icon.png" ) );
		}
		void setupPlayPause()
		{
			_playElt = new QToolButton( this );
			_playElt->setIcon( QIcon( ":/sprite/trajectory_play_icon.png" ) );
			_pauseElt = new QPushButton( this );
			_pauseElt->setIcon( QIcon( ":/sprite/trajectory_pause_icon.png" ) );
		}
		void setupProgress()
		{
			_progressElt = new QSlider( Qt::Horizontal, this );
			_progressElt->setTickInterval( 101 );
			_progressElt->setTickPosition( QSlider::TicksAbove );

			// FIXME should be done when traj is loaded
			//_progressElt->setTickInterval( (int)_molecule.getTrajectory().getFrameCount() );
			auto & trajectory = App::ECS_REGISTRY().getComponent<VTX::App::Component::Chemistry::Trajectory>(
				*( App::ECS_REGISTRY().findComponents<VTX::App::Component::Chemistry::Trajectory>().begin() )
			);
			trajectory.getPlayer().onFrameChange +=
				[]( size_t frame ) { VTX_INFO( "trajectory_player frame changed  = {}", frame ); };
		}
		void setupFrameSelector() {}

		void setupTmpPlayerSelector()
		{ 
			_tmpPlayerSelector = new QPushButton( this );
		}

		void addPlayerToLayout()
		{
			_layout->addItem( _playButtonsLayout );
			_layout->addItem( _progressLayout );

			_playButtonsLayout->addWidget( _playElt );
			_playButtonsLayout->addWidget( _pauseElt );
			_playButtonsLayout->addWidget( _stopElt );

			_progressLayout->addWidget( _progressElt );

			_progressLayout->addWidget( _tmpPlayerSelector );
		}

		void connectCallbacks()
		{
			// connect callbacks
			connect(
				_playElt,
				&QPushButton::clicked,
				this,
				[ & ]()
				{
					/* auto & molecule = App::ECS_REGISTRY().getComponent<App::Component::Chemistry::Molecule>(
						*( App::ECS_REGISTRY().findComponents<App::Component::Chemistry::Molecule>().begin() )
					);
					if ( !molecule.hasTrajectory() )
						return;

					auto & trajectory = App::ECS_REGISTRY().getComponent<VTX::App::Component::Chemistry::Trajectory>(
						*( App::ECS_REGISTRY().findComponents<VTX::App::Component::Chemistry::Trajectory>().begin() )
					);
					trajectory.getPlayer().play();*/

					App::ACTION_SYSTEM().execute<App::Action::Trajectory::SetPlayTrajectory>( _moleculeUID );

					is_playing								   = true;
					/*/
					VTX::Core::Struct::Molecule moleculeStruct = VTX::Core::Struct::Molecule();
					molecule.setMoleculeStruct( moleculeStruct );

					std::thread writethread(
						[]()
						{
							VTX_INFO( "writethread start" );
							auto & molecule = App::ECS_REGISTRY().getComponent<App::Component::Chemistry::Molecule>(
								*( App::ECS_REGISTRY().findComponents<App::Component::Chemistry::Molecule>().begin() )
							);
							// if ( !molecule.hasTrajectory() )
							//	return;
							auto & trajectory
								= App::ECS_REGISTRY().getComponent<VTX::App::Component::Chemistry::Trajectory>(
									*( App::ECS_REGISTRY()
										   .findComponents<VTX::App::Component::Chemistry::Trajectory>()
										   .begin() )
								);

							IO::Reader::Molecule moleculeReader	  = IO::Reader::Molecule();
							const std::string	 moleculeName	  = "2ama_1_npt";
							const std::string	 moleculePathname = moleculeName + ".trr";
							const VTX::FilePath	 moleculePath
								= VTX::Util::Filesystem::getExecutableDir() / "data\\" / moleculePathname;
							// while (is_playing)
							// moleculeReader.readFile( moleculePath, moleculeStruct );
							while ( trajectory.getPlayer().isPlaying() )
								moleculeReader.readFile( moleculePath, molecule.getMoleculeStruct() );
							VTX_INFO( "writethread end" );
						}
					);

					std::thread readthreadfromlocalmolecule(
						[]()
						{
							VTX_INFO( "readthreadfromlocalmolecule start" );
							auto & molecule = App::ECS_REGISTRY().getComponent<App::Component::Chemistry::Molecule>(
								*( App::ECS_REGISTRY().findComponents<App::Component::Chemistry::Molecule>().begin() )
							);
							// if ( !molecule.hasTrajectory() )
							//	return; no trajectory as we just created an empty traj and moved it in the molecule
							//struct...

							auto & trajectory
								= App::ECS_REGISTRY().getComponent<VTX::App::Component::Chemistry::Trajectory>(
									*( App::ECS_REGISTRY()
										   .findComponents<VTX::App::Component::Chemistry::Trajectory>()
										   .begin() )
								);
							trajectory.getPlayer().play();

							VTX::Core::Struct::Frame					 currentFrame;
							VTX::App::Component::Render::ProxyMolecule & proxy
								= App::ECS_REGISTRY().getComponent<App::Component::Render::ProxyMolecule>( *(
									App::ECS_REGISTRY().findComponents<App::Component::Render::ProxyMolecule>().begin()
								) );

							// int idx( 0 );
							// while (idx < 98)
							while ( trajectory.getPlayer().isPlaying() )
							{
								molecule.getTrajectory().getCurrentFrame( currentFrame
								); // working with real molecule actually loaded in app
								// moleculeStruct.trajectory.getCurrentFrame( currentFrame ); // working with core empty
								// molecule created in this scope proxy._updateAtomsPositions( currentFrame ); // CANNOT
								// update renderer in this thread context
								trajectory.getPlayer().StackFrame( currentFrame );
								//++idx;
								// std::this_thread::sleep_for( std::chrono::milliseconds(1000));
							}
							VTX_INFO( "readthreadfromlocalmolecule end" );
						}
					);
					VTX_INFO( "threads prejoin" );
					// writethread.join();
					// readthreadfromlocalmolecule.join();
					writethread.detach();
					readthreadfromlocalmolecule.detach();
					VTX_INFO( "threads joined" );*/
				}
			);

			connect(
				_pauseElt,
				&QPushButton::clicked,
				this,
				[ & ]()
				{
					is_playing = false;

					App::ACTION_SYSTEM().execute<App::Action::Trajectory::SetPauseTrajectory>( _moleculeUID );
				}
			);

			connect(
				_stopElt,
				&QPushButton::clicked,
				this,
				[ & ]()
				{
					is_playing = false;

					App::ACTION_SYSTEM().execute<App::Action::Trajectory::SetStopTrajectory>( _moleculeUID );
				}
			);

			connect(
				_tmpPlayerSelector,
				&QPushButton::clicked,
				this,
				[ & ]()
				{ App::ACTION_SYSTEM().execute<App::Action::Trajectory::SetCircularPlayer>( _moleculeUID );
				}
			);
		}

		std::atomic_bool is_playing; // FIXME dev purpose to stop threads needs improvement

		const App::Core::UID::UIDRange & _moleculeUID;
		QWidget		  *_widget;
		QVBoxLayout	  *_layout;
		QHBoxLayout *_playButtonsLayout;
		QHBoxLayout *_progressLayout;
		QPushButton *_stopElt;
		//QPushButton *_playPauseElt;
		//FIXME
		QToolButton *  _playElt;
		QPushButton *  _pauseElt;
		QSlider	  *_progressElt;
		QLineEdit	  *_frameSelectorElt;
		QPushButton *  _tmpPlayerSelector;
	};

} // namespace VTX::UI::QT::Widget

#endif
