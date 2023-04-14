#ifndef __VTX_CONTROLLER_MEASUREMENT_PICKER__
#define __VTX_CONTROLLER_MEASUREMENT_PICKER__

#include "base_keyboard_controller.hpp"
#include "base_mouse_controller.hpp"
#include <app/old_app/model/base_model.hpp>

namespace VTX
{
	namespace Model
	{
		class Atom;
		class Residue;

		namespace Measurement
		{
			class MeasureInProgress;
		}
	} // namespace Model

	namespace Controller
	{
		class MeasurementPicker : public BaseMouseController, public BaseKeyboardController
		{
		  public:
			enum class Mode : int
			{
				DISTANCE,
				DISTANCE_TO_CYCLE,
				ANGLE,
				DIHEDRAL_ANGLE
			};

		  public:
			MeasurementPicker();
			~MeasurementPicker();

			inline ID::VTX_ID getTargetWidget() override { return ID::UI::Input::RENDER_WIDGET; }

			const Mode & getCurrentMode() const { return _currentMode; }
			void		 setCurrentMode( const Mode & p_mode );

			void update( const float & p_deltaTime ) override;

			Model::Measurement::MeasureInProgress & getMeasureInProgressModel() { return *_currentMeasureModel; }

		  protected:
			void _onMouseLeftClick( const uint p_x, const uint p_y ) override;
			void _onMouseLeftDoubleClick( const uint p_x, const uint p_y ) override;
			void _onMouseRightClick( const uint p_x, const uint p_y ) override;
			void _handleMouseMotionEvent( const QMouseEvent & p_event ) override;

			void receiveEvent( const QKeyEvent & p_event ) override;

		  private:
			bool _canCreateLabel() const;
			void _createLabel();

			Mode _currentMode = Mode::DISTANCE;

			Model::Measurement::MeasureInProgress * _currentMeasureModel = nullptr;
		};
	} // namespace Controller

} // namespace VTX
#endif
