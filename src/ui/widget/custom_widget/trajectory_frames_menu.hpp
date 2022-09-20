#ifndef __VTX_UI_WIDGET_CUSTOM_TRAJECTORY_FRAMES_MENU__
#define __VTX_UI_WIDGET_CUSTOM_TRAJECTORY_FRAMES_MENU__

#include "ui/ui_action/self_referenced_action.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include <QAction>
#include <QMenu>

namespace VTX
{
	namespace Model
	{
		class Selection;
		class Molecule;
	} // namespace Model

	namespace UI::Widget::CustomWidget
	{
		class TrajectoryFramesMenu : public BaseManualWidget<QMenu>
		{
			VTX_WIDGET
			Q_OBJECT

		  public:
			~TrajectoryFramesMenu() {};

			void localize() override;

			uint getFrameCount() const;

			void updateFrames( const Model::Molecule & p_molecule );
			void updateFrames( const Model::Selection & p_selection );

			void setDisplayAllFramesOption( const bool p_displayAllFramesOption );

		  signals:
			void onFrameSelected( const int p_frame );

		  protected:
			TrajectoryFramesMenu( QWidget * p_parent = nullptr );

			void _setupUi( const QString & p_name ) override;
			void _setupSlots() override;

			void _adjustFrameActions( const uint p_newFrameCount );

			void _onFrameSelected( const QAction * const p_action );

		  private:
			UIAction::SelfReferencedAction * _allFramesAction = nullptr;
			bool							 _allFramesOption = false;

			uint _frameCount = 0;
		};
	} // namespace UI::Widget::CustomWidget
} // namespace VTX
#endif
