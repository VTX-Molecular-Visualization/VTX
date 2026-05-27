#ifndef __VTX_UI_QT_TOOL_BUTTON_RESOLUTION_SELECTOR__
#define __VTX_UI_QT_TOOL_BUTTON_RESOLUTION_SELECTOR__

#include <QToolButton>

namespace VTX::UI::QT::ToolButton
{
	/**
	 * @brief Tool button to select/enter a resolution.
	 */
	class ResolutionSelector : public QToolButton
	{
		Q_OBJECT

	  public:
		/**
		 * @brief Constructor.
		 */
		ResolutionSelector( QWidget * );

		/**
		 * @brief Set the resolution text of the button.
		 */
		void setResolution( const size_t, const size_t );

	  signals:
		/**
		 * @brief User changed resolution.
		 */
		void resolutionChanged( const size_t, const size_t );

	  private:
	};

} // namespace VTX::UI::QT::ToolButton

#endif
