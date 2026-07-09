#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_PREPARATION_TIMELINE__
#define __VTX_TOOL_TOOLS_MDPREP_UI_PREPARATION_TIMELINE__

#include <string>
#include <tool/mdprep/gateway/shared.hpp>
#include <ui/qt/dock_widget/base_dock_widget.hpp>
#include <util/event_hub.hpp>
#include <vector>

class QPushButton;

namespace VTX::Tool::Mdprep::ui
{
	class StepNode;

	/**
	 * @brief Dock widget presenting the preparation job as a vertical, top-to-bottom timeline.
	 *
	 * Each node maps to a preparation step. Nodes are highlighted according to their state (pending, running, done,
	 * failed) and can be expanded to display a static description of the step as well as the gromacs stdout/stderr once
	 * the step is finished. A button at the bottom opens the packed system in the OS file explorer; it stays disabled
	 * until the pack step succeeds.
	 *
	 * The panel listens to the preparation events (fired on the main thread) and updates itself accordingly.
	 */
	class PreparationTimeline : public UI::QT::DockWidget::BaseDockWidget<PreparationTimeline>
	{
	  public:
		inline static const QSize PREFERRED_SIZE { 420, 720 };

		explicit PreparationTimeline( QWidget * p_parent );

		/**
		 * @brief Bring every node back to its pending state for a fresh preparation run.
		 */
		void reset() noexcept;

	  private:
		std::vector<StepNode *> _nodes;
		StepNode *				_packNode	   = nullptr;
		QPushButton *			_openFolderBtn = nullptr;
		std::string				_packedPath;

		Util::EventHub::ScopedConnection _stepStartedConnection;
		Util::EventHub::ScopedConnection _stepFinishedConnection;
		Util::EventHub::ScopedConnection _systemPackedConnection;

		void _setupUi() noexcept;

		void _onStepStarted( const Gateway::PreparationStepStarted & ) noexcept;
		void _onStepFinished( const Gateway::PreparationStepFinished & ) noexcept;
		void _onSystemPacked( const Gateway::SystemPacked & ) noexcept;
	};

	/**
	 * @brief Create (once) the preparation timeline dock, tabified next to the mdprep panel, then show, raise and reset
	 * it for a new run.
	 */
	void showPreparationTimeline() noexcept;

} // namespace VTX::Tool::Mdprep::ui

#endif
