#ifndef __VTX_CONTROLLER_MEASUREMENT_PICKER__
#define __VTX_CONTROLLER_MEASUREMENT_PICKER__

#include "base_keyboard_controller.hpp"
#include "base_mouse_controller.hpp"
#include "model/base_model.hpp"
#include <vector>

namespace VTX
{
	namespace Model
	{
		class Atom;
		class Residue;
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

			MeasurementPicker();

			inline ID::VTX_ID getTargetWidget() override { return ID::UI::Input::RENDER_WIDGET; }
			void			  update( const float & p_deltaTime ) override;

			const Mode & getCurrentMode() const { return _currentMode; }
			void		 setCurrentMode( const Mode & p_mode );

		  protected:
			void _onMouseLeftClick( const uint p_x, const uint p_y ) override;
			void _onMouseLeftDoubleClick( const uint p_x, const uint p_y ) override;
			void _onMouseRightClick( const uint p_x, const uint p_y ) override;

		  private:
			void _updateDisplay();

			Vec2i _lastClickedIds;

			Mode				   _currentMode = Mode::DISTANCE;
			std::vector<Model::ID> atoms		= std::vector<Model::ID>();
		};
	} // namespace Controller

} // namespace VTX
#endif
