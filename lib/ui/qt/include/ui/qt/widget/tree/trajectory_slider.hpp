#ifndef __VTX_UI_QT_WIDGET_TRAJECTORY_SLIDER__
#define __VTX_UI_QT_WIDGET_TRAJECTORY_SLIDER__

#include <QSlider>

namespace VTX::UI::QT::Widget::Tree
{

	/**
	 * @brief QSlider subclass that paints a colored overlay on the groove
	 * to indicate which frames are currently loaded in memory.
	 */
	class TrajectorySlider : public QSlider
	{
		Q_OBJECT

	  public:
		explicit TrajectorySlider( Qt::Orientation p_orientation, QWidget * p_parent = nullptr );

		void setLoadedRange( int p_lower, int p_upper );

	  protected:
		void paintEvent( QPaintEvent * p_event ) override;

	  private:
		int _loadedLower = 0;
		int _loadedUpper = 0;
	};

} // namespace VTX::UI::QT::Widget::Tree

#endif
