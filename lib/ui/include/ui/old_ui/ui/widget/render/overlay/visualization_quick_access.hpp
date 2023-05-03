#ifndef __VTX_UI_WIDGET_RENDER_OVERLAY_VISUALIZATION_QUICK_ACCESS__
#define __VTX_UI_WIDGET_RENDER_OVERLAY_VISUALIZATION_QUICK_ACCESS__

#include "base_overlay.hpp"
// #include "ui/old_ui/controller/measurement_picker.hpp"
#include "ui/old_ui/ui/multi_data_field.hpp"
#include "ui/old_ui/ui/widget/base_manual_widget.hpp"
#include <QAction>
#include <QComboBox>
#include <QFrame>
#include <QToolButton>
#include <app/application/selection/enum_selection.hpp>
#include <app/id.hpp>
#include <vector>

namespace VTX::UI::Widget::Render::Overlay
{
	class VisualizationQuickAccess : public BaseOverlay
	{
		Q_OBJECT
		VTX_WIDGET

	  private:
		template<typename T>
		class MenuItemData
		{
		  public:
			MenuItemData() : MenuItemData( T(), "", "" ) {};
			MenuItemData( const T p_data, const QString & p_name, const QString & p_iconPath ) :
				data( p_data ), name( p_name ), iconPath( p_iconPath )
			{
			}

			const T		  data;
			const QString name;
			const QString iconPath;
		};
		template<typename T>
		class MenuItemDataRef
		{
		  public:
			MenuItemDataRef() : MenuItemDataRef( T(), "", "" ) {};
			MenuItemDataRef( const T & p_data, const QString & p_name, const QString & p_iconPath ) :
				data( p_data ), name( p_name ), iconPath( p_iconPath )
			{
			}

			const T &	  data;
			const QString name;
			const QString iconPath;
		};

		inline static std::vector<MenuItemDataRef<App::VTX_ID>> CAMERA_CONTROLLERS {
			MenuItemDataRef<App::VTX_ID>( ID::Controller::TRACKBALL,
										  "Trackball",
										  ":/sprite/camera_trackball_mode.png" ),
			MenuItemDataRef<App::VTX_ID>( ID::Controller::FREEFLY, "Freefly", ":/sprite/camera_freefly_mode.png" )
		};

		inline static std::vector<MenuItemDataRef<App::VTX_ID>> PICKER_CONTROLLERS {
			MenuItemDataRef<App::VTX_ID>( ID::Controller::PICKER, "Selection", ":/sprite/picker_selection_icon.png" ),
			MenuItemDataRef<App::VTX_ID>( ID::Controller::MEASUREMENT,
										  "Measurement",
										  ":/sprite/measurement_picker_icon.png" )
		};

		inline static std::vector<MenuItemData<VTX::App::Application::Selection::GRANULARITY>> SELECTION_GRANULARITY {
			MenuItemData( VTX::App::Application::Selection::GRANULARITY::ATOM, "Pick Atom", ":/sprite/pick_atom.png" ),
			MenuItemData( VTX::App::Application::Selection::GRANULARITY::RESIDUE,
						  "Pick Residue",
						  ":/sprite/pick_residue.png" ),
			MenuItemData( VTX::App::Application::Selection::GRANULARITY::CHAIN,
						  "Pick Chain",
						  ":/sprite/pick_chain.png" ),
			MenuItemData( VTX::App::Application::Selection::GRANULARITY::MOLECULE,
						  "Pick Molecule",
						  ":/sprite/pick_molecule.png" )
		};

		// inline static std::vector<MenuItemData<Controller::MeasurementPicker::Mode>> MEASUREMENT_MODE {
		//	MenuItemData( Controller::MeasurementPicker::Mode::DISTANCE,
		//				  "Distance",
		//				  ":/sprite/measurement_distance_icon.png" ),
		//	// MenuItemData( Controller::MeasurementPicker::Mode::DISTANCE_TO_CYCLE,
		//	//			  "Distance to cycle",
		//	//			  ":/sprite/measurement_distance_to_cycle_icon.png" ),
		//	MenuItemData( Controller::MeasurementPicker::Mode::ANGLE, "Angle", ":/sprite/measurement_angle_icon.png" ),
		//	MenuItemData( Controller::MeasurementPicker::Mode::DIHEDRAL_ANGLE,
		//				  "Dihedral angle",
		//				  ":/sprite/measurement_dihedral_angle_icon.png" ),
		// };

		inline static const char * CAMERA_CONTROLLER_PROPERTY_NAME = "CAMERA_CONTROLLER";
		inline static const char * PICKER_CONTROLLER_PROPERTY_NAME = "PICKER_CONTROLLER";
		inline static const char * GRANULARITY_PROPERTY_NAME	   = "GRANULARITY";
		inline static const char * MEASUREMENT_MODE_PROPERTY_NAME  = "MEASUREMENT_MODE";

	  public:
		~VisualizationQuickAccess() = default;
		void localize() override;

		void receiveEvent( const VTX::App::Core::Event::VTXEvent & p_event ) override;

	  protected:
		VisualizationQuickAccess( QWidget * p_parent );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;
		void _refreshController();
		void _refreshPicker();
		void _refreshSelectionGranularity();
		void _refreshMeasurementMode();

	  private:
		QToolButton * _orientWidget						 = nullptr;
		QToolButton * _changeCameraControllerWidget		 = nullptr;
		QToolButton * _changePickerControllerWidget		 = nullptr;
		QToolButton * _changeSelectionGranularityWidget	 = nullptr;
		QAction *	  _changeSelectionGranularityQAction = nullptr;
		QToolButton * _changeMeasurementModeWidget		 = nullptr;
		QAction *	  _changeMeasurementModeQAction		 = nullptr;

		void _orientAction();
		void _changeCameraControllerAction( const QAction * const p_action );
		void _changePickerControllerAction( const QAction * const p_action );
		void _changeSelectionGranularityAction( const QAction * const p_action );
		void _changeMeasurementModeAction( const QAction * const p_action );
	};
} // namespace VTX::UI::Widget::Render::Overlay
#endif
