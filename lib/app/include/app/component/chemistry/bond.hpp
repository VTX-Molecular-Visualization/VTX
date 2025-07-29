#ifndef __VTX_APP_COMPONENT_CHEMISTRY_BOND__
#define __VTX_APP_COMPONENT_CHEMISTRY_BOND__

#include "_fwd.hpp"
#include "app/core/ecs/base_component.hpp"
#include <util/constants.hpp>
#include <util/types.hpp>

namespace VTX::App::Component::Chemistry
{
	class Bond : public Core::ECS::BaseComponent
	{
	  public:
		Bond() {}
		Bond( System * const p_systemPtr ) : _systemPtr( p_systemPtr ) {}
		Bond( System * const p_systemPtr, const Index p_index ) : _systemPtr( p_systemPtr ), _index( p_index ) {}

		const System * const getSystemPtr() const { return _systemPtr; }
		System * const		 getSystemPtr() { return _systemPtr; }

		Index getIndexFirstAtom() const;
		void  setIndexFirstAtom( const Index p_atomIndex );

		Index getIndexSecondAtom() const;
		void  setIndexSecondAtom( const Index p_atomIndex );

		bool isVisible() const;
		void setVisible( const bool p_visible );

	  private:
		System * _systemPtr = nullptr;
		Index	 _index		= INVALID_INDEX;
	};

} // namespace VTX::App::Component::Chemistry
#endif
