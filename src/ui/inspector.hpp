#ifndef __VTX_COMPONENT_INSPECTOR__
#define __VTX_COMPONENT_INSPECTOR__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_component.hpp"
#include "localization/language.hpp"
#include "model/molecule.hpp"
#include "view/base_view.hpp"

namespace VTX
{
	namespace UI
	{
		class Inspector : public BaseComponent
		{
		  public:
			explicit Inspector( bool * const );

			virtual std::string getName() const override { return ID::UI::INSPECTOR; }

		  protected:
			virtual void _addItems() override {};
			virtual void _draw() override;

		  private:
		};
	} // namespace UI
} // namespace VTX
#endif
