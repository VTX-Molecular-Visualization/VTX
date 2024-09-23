#ifndef __VTX_UI_QT_DOCK_WIDGET_INSPECTOR__
#define __VTX_UI_QT_DOCK_WIDGET_INSPECTOR__

#include "ui/qt/core/base_dock_widget.hpp"
#include "app/component/render/proxy_molecule.hpp"
#include "io/reader/molecule.hpp"
#include <app/component/chemistry/trajectory.hpp>
#include <QPushButton>

namespace VTX::UI::QT::DockWidget
{

	class Inspector : public Core::BaseDockWidget<Inspector>
	{
	  public:
		Inspector( QWidget * p_parent ) : BaseDockWidget<Inspector>( "Selection", p_parent )
		{
			setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
			
			// devjla
			//QWidget * const mainWidget = _instantiateMainWidget( QSize( 200, 0 ), QSize( 200, 0 ) );
			auto * widget = new QWidget( this );

			QVBoxLayout * const layout = new QVBoxLayout( widget );
			layout->setContentsMargins( 0, 0, 0, 0 );
	
			QHBoxLayout * const _playButtonsLayout = new QHBoxLayout();
	
			auto* _playButton = new QPushButton( this );
			_playButton->setIcon( QIcon( ":/sprite/trajectory_play_icon.png" ) );
			auto* _pauseButton = new QPushButton( this );
			_pauseButton->setIcon( QIcon( ":/sprite/trajectory_pause_icon.png" ) );
			auto* _stopButton = new QPushButton( this );
			_stopButton->setIcon( QIcon( ":/sprite/background_white_icon.png" ) );
	
			_playButtonsLayout->addWidget( _playButton );
			_playButtonsLayout->addWidget( _pauseButton );
			_playButtonsLayout->addWidget( _stopButton );
			layout->addItem( _playButtonsLayout);
	
			setWidget( widget );
			
			// connect callbacks
			connect( _playButton, &QPushButton::clicked, this, [ & ]()
				{
					auto & molecule = App::MAIN_REGISTRY().findComponent<App::Component::Chemistry::Molecule>();
					if ( !molecule.hasTrajectory() )
						return;
			
					auto & trajectory = App::MAIN_REGISTRY().findComponent<VTX::App::Component::Chemistry::Trajectory>();
					//trajectory.getPlayer().play();
			
					is_playing							  = true;
					Core::Struct::Molecule moleculeStruct = Core::Struct::Molecule();
			
					std::thread writethread(
						[ & ]()
						{
							VTX_INFO( "writethread start" );
							IO::Reader::Molecule				 moleculeReader	  = IO::Reader::Molecule();
							const std::string					 moleculeName	  = "2ama_1_npt";
							const std::string					 moleculePathname = moleculeName + ".trr";
							const VTX::FilePath moleculePath = VTX::Util::Filesystem::getExecutableDir() / "data\\" / moleculePathname;
							//while (is_playing)
								moleculeReader.readFile( moleculePath, moleculeStruct );
							VTX_INFO( "writethread end" );
						}
					);
				
					/* std::thread readthreadfromlocalmolecule(
						[ & ]()
						{
							VTX_INFO( "readthreadfromlocalmolecule start" );
							VTX::Core::Struct::Frame currentFrame;
							VTX::App::Component::Render::ProxyMolecule & proxy
								= App::MAIN_REGISTRY().findComponent<App::Component::Render::ProxyMolecule>();
							while ( moleculeStruct.trajectory.getCurrentFrame( currentFrame ) )
							{
								proxy._updateAtomsPositions( currentFrame );
								//std::this_thread::sleep_for( std::chrono::milliseconds(1000));
							}
							VTX_INFO( "readthreadfromlocalmolecule end" );
						}
					); */
					VTX_INFO( "readthreadfromlocalmolecule start" );
					VTX::Core::Struct::Frame					 currentFrame;
					VTX::App::Component::Render::ProxyMolecule & proxy
						= App::MAIN_REGISTRY().findComponent<App::Component::Render::ProxyMolecule>();
					int idx(0);
					// FIXME this counter is just here to ensure we dont deadlock by reading frames for test purpose
					while ( idx < 98 )
					{
						moleculeStruct.trajectory.getCurrentFrame( currentFrame );
						proxy._updateAtomsPositions( currentFrame );
						++idx;
						// std::this_thread::sleep_for( std::chrono::milliseconds(1000));
					}
					VTX_INFO( "readthreadfromlocalmolecule end" );
					// this thread reads traj from the molecule trajectory data written when VTX is launched
					// currently problematic because no code is written to update a trajectory directly in the molecule : 
					// we are supposed to create a full Core::Struct::Molecule, write its content then std::move it into the existing molecule
					/* std::thread readthreadfromtraj(
						[ & ]()
						{
							VTX_INFO( "readthreadfromtraj start" );
							VTX::Core::Struct::Frame currentFrame;
							// VTX::App::Component::Render::ProxyMolecule &proxy =
							// App::MAIN_REGISTRY().getComponent<App::Component::Render::ProxyMolecule>(*this);
							VTX::App::Component::Render::ProxyMolecule & proxy
								= App::MAIN_REGISTRY().findComponent<App::Component::Render::ProxyMolecule>();
							while ( molecule.getTrajectory().getCurrentFrame( currentFrame ) )
							{
								// does it matter? no.. do I still write it? yes..
								size_t currentFrameIdx = trajectory.getCurrentFrame();
								proxy._updateAtomsPositions( currentFrame );
								// std::this_thread::sleep_for( std::chrono::milliseconds(1000));
							}
							VTX_INFO( "readthreadfromtraj end" );
						}
					); */
					VTX_INFO( "threads prejoin");
					writethread.join();
					//readthreadfromlocalmolecule.join();
					VTX_INFO( "threads joined");
				});

			connect( _pauseButton, &QPushButton::clicked, this, [ & ]()
				{
					is_playing = false;

					auto & molecule = App::MAIN_REGISTRY().findComponent<App::Component::Chemistry::Molecule>();
					if ( !molecule.hasTrajectory() )
						return;
			
					auto & trajectory = App::MAIN_REGISTRY().findComponent<VTX::App::Component::Chemistry::Trajectory>();
					trajectory.getPlayer().pause();
				});

			connect( _stopButton, &QPushButton::clicked, this, [ & ]()
				{
					is_playing = false;

					auto & molecule = App::MAIN_REGISTRY().findComponent<App::Component::Chemistry::Molecule>();
					if ( !molecule.hasTrajectory() )
						return;
			
					auto & trajectory = App::MAIN_REGISTRY().findComponent<VTX::App::Component::Chemistry::Trajectory>();
					trajectory.getPlayer().stop();
				});
		}

		virtual ~Inspector() {}
		
	  private:
		std::atomic_bool is_playing; // FIXME dev purpose to stop threads needs improvement
	};

} // namespace VTX::UI::QT::DockWidget

#endif
