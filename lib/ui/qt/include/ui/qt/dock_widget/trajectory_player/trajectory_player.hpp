#ifndef __VTX_UI_QT_DOCK_WIDGET_TRAJECTORY_PLAYER__
#define __VTX_UI_QT_DOCK_WIDGET_TRAJECTORY_PLAYER__

#include <functional>
#include <ui/qt/base_widget.hpp>
#include <app/component/render/proxy_molecule.hpp>
#include <io/reader/molecule.hpp>
#include <app/component/chemistry/trajectory.hpp>
#include <QDockWidget>
#include <QPushButton>
#include <QSlider>

namespace VTX::UI::QT::DockWidget
{

	class TrajectoryPlayer : public QWidget
	{
	  public:
		TrajectoryPlayer( QWidget * p_parent, App::Component::Chemistry::Molecule &p_molecule) : QWidget( p_parent )
		{			
			// devjla
			auto * widget = new QWidget( this );
			widget->setContentsMargins( 0, 0, 0, 0 );

			QVBoxLayout * const layout = new QVBoxLayout( widget );
			layout->setContentsMargins( 0, 0, 0, 0 );
	
			QHBoxLayout * const _playButtonsLayout = new QHBoxLayout();
			QHBoxLayout * const _progressLayout	   = new QHBoxLayout();
	
			auto* _playButton = new QPushButton( this );
			_playButton->setIcon( QIcon( ":/sprite/trajectory_play_icon.png" ) );
			auto* _pauseButton = new QPushButton( this );
			_pauseButton->setIcon( QIcon( ":/sprite/trajectory_pause_icon.png" ) );
			auto* _stopButton = new QPushButton( this );
			_stopButton->setIcon( QIcon( ":/sprite/background_white_icon.png" ) );

			auto * _progressSlider = new QSlider( Qt::Horizontal, this );
			_progressSlider->setTickInterval( 101 );
			_progressSlider->setTickPosition( QSlider::TicksAbove );

			// FIXME should be done when traj is loaded
			_progressSlider->setTickInterval( (int)p_molecule.getTrajectory().getFrameCount() );
			auto & trajectory = App::MAIN_REGISTRY().findComponent<VTX::App::Component::Chemistry::Trajectory>();
			trajectory.getPlayer().onFrameChange += []( size_t frame ) { 
				VTX_INFO( "trajectory_player frame changed  = {}", frame );
			};
	
			_playButtonsLayout->addWidget( _playButton );
			_playButtonsLayout->addWidget( _pauseButton );
			_playButtonsLayout->addWidget( _stopButton );

			_progressLayout->addWidget( _progressSlider );

			layout->addItem( _playButtonsLayout);
			layout->addItem( _progressLayout );

			// connect callbacks
			connect(
				_playButton,
				&QPushButton::clicked,
				this,
				[ & ]()
				{
					auto & molecule = App::MAIN_REGISTRY().findComponent<App::Component::Chemistry::Molecule>();
					if ( !p_molecule.hasTrajectory() )
						return;
			
					auto & trajectory = App::MAIN_REGISTRY().findComponent<VTX::App::Component::Chemistry::Trajectory>();
					trajectory.getPlayer().play();
			
					is_playing							  = true;
					Core::Struct::Molecule moleculeStruct = Core::Struct::Molecule();
					p_molecule.setMoleculeStruct( moleculeStruct );
			
					std::thread writethread(
						[]()
						{
							VTX_INFO( "writethread start" );
							auto & molecule = App::MAIN_REGISTRY().findComponent<App::Component::Chemistry::Molecule>();
							//if ( !molecule.hasTrajectory() )
							//	return;
							auto & trajectory
								= App::MAIN_REGISTRY().findComponent<VTX::App::Component::Chemistry::Trajectory>();

							IO::Reader::Molecule				 moleculeReader	  = IO::Reader::Molecule();
							const std::string					 moleculeName	  = "2ama_1_npt";
							const std::string					 moleculePathname = moleculeName + ".trr";
							const VTX::FilePath moleculePath = VTX::Util::Filesystem::getExecutableDir() / "data\\" / moleculePathname;
							//while (is_playing)
								//moleculeReader.readFile( moleculePath, moleculeStruct );
							while ( trajectory.getPlayer().isPlaying() )
								moleculeReader.readFile( moleculePath, molecule.getMoleculeStruct() );
							VTX_INFO( "writethread end" );
						}
					);
				
					std::thread readthreadfromlocalmolecule(
						[]()
						{
							VTX_INFO( "readthreadfromlocalmolecule start" );
							auto & molecule = App::MAIN_REGISTRY().findComponent<App::Component::Chemistry::Molecule>();
							//if ( !molecule.hasTrajectory() )
							//	return; no trajectory as we just created an empty traj and moved it in the molecule struct...

							auto & trajectory
								= App::MAIN_REGISTRY().findComponent<VTX::App::Component::Chemistry::Trajectory>();
							trajectory.getPlayer().play();

							VTX::Core::Struct::Frame currentFrame;
							VTX::App::Component::Render::ProxyMolecule & proxy
								= App::MAIN_REGISTRY().findComponent<App::Component::Render::ProxyMolecule>();
							
							//int idx( 0 );
							//while (idx < 98)
							while ( trajectory.getPlayer().isPlaying() )
							{
								molecule.getTrajectory().getCurrentFrame( currentFrame ); // working with real molecule actually loaded in app
								//moleculeStruct.trajectory.getCurrentFrame( currentFrame ); // working with core empty molecule created in this scope
								//proxy._updateAtomsPositions( currentFrame ); // CANNOT update renderer in this thread context
								trajectory.getPlayer().StackFrame( currentFrame );
								//++idx;
								//std::this_thread::sleep_for( std::chrono::milliseconds(1000));
							}
							VTX_INFO( "readthreadfromlocalmolecule end" );
						}
					);
					VTX_INFO( "threads prejoin");
					//writethread.join();
					//readthreadfromlocalmolecule.join();
					writethread.detach();
					readthreadfromlocalmolecule.detach();
					VTX_INFO( "threads joined");
				});

			connect( _pauseButton, &QPushButton::clicked, this, [ & ]()
				{
					is_playing = false;

					//auto & molecule = App::MAIN_REGISTRY().findComponent<App::Component::Chemistry::Molecule>();
					if ( !p_molecule.hasTrajectory() )
						return;
			
					auto & trajectory = App::MAIN_REGISTRY().findComponent<VTX::App::Component::Chemistry::Trajectory>();
					trajectory.getPlayer().pause();
				});

			connect( _stopButton, &QPushButton::clicked, this, [ & ]()
				{
					is_playing = false;

					//auto & molecule = App::MAIN_REGISTRY().findComponent<App::Component::Chemistry::Molecule>();
					if ( !p_molecule.hasTrajectory() )
						return;
			
					auto & trajectory = App::MAIN_REGISTRY().findComponent<VTX::App::Component::Chemistry::Trajectory>();
					trajectory.getPlayer().stop();
				});
		}

		virtual ~TrajectoryPlayer() {}
		
	  private:
		std::atomic_bool is_playing; // FIXME dev purpose to stop threads needs improvement
	};

} // namespace VTX::UI::QT::DockWidget

#endif
