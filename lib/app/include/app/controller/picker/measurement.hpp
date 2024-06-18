// TODO Move this controller in dedicated tool

// #ifndef __VTX_UI_QT_CONTROLLER_MEASUREMENT_PICKER__
// #define __VTX_UI_QT_CONTROLLER_MEASUREMENT_PICKER__
//
// #include "base_keyboard_controller.hpp"
// #include "base_mouse_controller.hpp"
// #include "ui/id.hpp"
// #include <app/old/component/chemistry/_fwd.hpp>
// #include <app/old/core/model/base_model.hpp>
// #include <vector>
//
// namespace VTX
//{
//	namespace Model::Measurement
//	{
//		class MeasureInProgress;
//	} // namespace Model::Measurement
//
//	namespace UI::QT::Controller
//	{
//		class MeasurementPicker : public BaseMouseController, public BaseKeyboardController
//		{
//		  public:
//			enum class Mode : int
//			{
//				DISTANCE,
//				DISTANCE_TO_CYCLE,
//				ANGLE,
//				DIHEDRAL_ANGLE
//			};
//
//		  public:
//			MeasurementPicker();
//			~MeasurementPicker();
//
//			inline App::Old::VTX_ID getTargetWidget() override { return UI::ID::Input::RENDER_WIDGET; }
//
//			const Mode & getCurrentMode() const { return _currentMode; }
//			void		 setCurrentMode( const Mode & p_mode );
//
//			void update( const float & p_deltaTime ) override;
//
//			Model::Measurement::MeasureInProgress & getMeasureInProgressModel() { return *_currentMeasureModel; }
//
//		  protected:
//			void _onMouseLeftClick( const uint p_x, const uint p_y ) override;
//			void _onMouseLeftDoubleClick( const uint p_x, const uint p_y ) override;
//			void _onMouseRightClick( const uint p_x, const uint p_y ) override;
//			void _handleMouseMotionEvent( const QMouseEvent & p_event ) override;
//
//			void receiveEvent( const QKeyEvent & p_event ) override;
//
//		  private:
//			bool _canCreateLabel() const;
//			void _createLabel();
//
//			Mode _currentMode = Mode::DISTANCE;
//
//			Model::Measurement::MeasureInProgress * _currentMeasureModel = nullptr;
//		};
//	} // namespace UI::QT::Controller
//
// } // namespace VTX
// #endif
