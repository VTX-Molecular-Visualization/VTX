#ifndef __VTX_UI_WIDGET_CUSTOM_TRAJECTORY_WIDGET__
#define __VTX_UI_WIDGET_CUSTOM_TRAJECTORY_WIDGET__

#include "model/molecule.hpp"
#include "ui/multi_data_field.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include "ui/widget/custom_widget/integer_field_slider_widget.hpp"
//#include "ui/widget/custom_widget/integer_range_widget.hpp"
#include "ui/widget/custom_widget/qt_multi_data_field.hpp"
#include <QWidget>
#include <unordered_set>

namespace VTX::UI::Widget::CustomWidget
{
	class TrajectoryWidget : public BaseManualWidget<QWidget>, public TMultiDataField<Model::Molecule>
	{
		VTX_WIDGET

	  public:
		void localize() override;

		void resetState() override;
		void updateWithNewValue( Model::Molecule & p_value ) override;

		void refreshTimer();

	  protected:
		TrajectoryWidget( QWidget * p_parent = nullptr );

		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void _displayDifferentsDataFeedback() override;

	  private:
		CustomWidget::IntegerFieldSliderWidget * _timeline = nullptr;

		QPushButton * _backToStartButton   = nullptr;
		QPushButton * _previousFrameButton = nullptr;
		QPushButton * _playButton		   = nullptr;
		QPushButton * _nextFrameButton	   = nullptr;
		QPushButton * _goToEndButton	   = nullptr;

		CustomWidget::QComboBoxMultiField * _playModeWidget = nullptr;
		CustomWidget::IntegerFieldSliderWidget * _speedWidget	 = nullptr;
		// CustomWidget::IntegerRangeWidget *	_playRangeWidget= nullptr;

		std::unordered_set<Model::Molecule *> _molecules = std::unordered_set<Model::Molecule *>();

		void _setFrameAction( const int p_frame );
		void _backToStartAction();
		void _previousFrameAction();
		void _togglePlayAction();
		void _nextFrameAction();
		void _goToEndAction();
		void _playModeChange( const int p_playMode );
		void _speedChange( const int p_speed );

		void _fillPlayModeComboBox();
	};
} // namespace VTX::UI::Widget::CustomWidget

#endif
