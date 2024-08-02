#ifndef __VTX_UI_QT_INSPECTOR_WIDGET__
#define __VTX_UI_QT_INSPECTOR_WIDGET__

#include "ui/qt/qt_panel.hpp"
#include "ui/qt/gateways/inspector_gw.hpp"

namespace VTX::UI::QT::Widget::Inspector
{
	class Panel : public QtDockablePanel
	{
		NEW_ARCHI_VTX_WIDGET

	  public:
		void localize() override;

		void setPItem( const App::Component::Scene::SceneItemComponent & p_item ) { _p_item = p_item; }

	  protected:
		Panel( QWidget * p_parent = nullptr );
		~Panel() = default;

		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

	  private:
		QPushButton * _playButton = nullptr;
		QPushButton * _pauseButton = nullptr;
		QPushButton * _stopButton = nullptr;
		std::unique_ptr<VTX::UI::QT::Gateways::InspectorGw> _inspectorGw;
		// meh
		App::Component::Scene::SceneItemComponent			_p_item;

		void _togglePlayAction();
		void _togglePauseAction();
		void _toggleStopAction();
	};
} // namespace VTX::UI::QT::Widget::Inspector

#endif
