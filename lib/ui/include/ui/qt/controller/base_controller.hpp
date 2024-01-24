#ifndef __VTX_UI_QT_CONTROLLER_BASE_CONTROLLER__
#define __VTX_UI_QT_CONTROLLER_BASE_CONTROLLER__

#include <app/core/collectionable.hpp>
#include <memory>
#include <string>
#include <util/hashing.hpp>
#include <utility>

namespace VTX::UI::QT::Controller
{
	class BaseController : public App::Core::Collectionable
	{
	  public:
		BaseController()		  = default;
		virtual ~BaseController() = default;

		virtual void update( const float & p_deltaTime ) {};

		bool		 isActive() const { return _isActive; }
		virtual void setActive( const bool p_active ) { _isActive = p_active; }

		virtual Util::Hashing::Hash				getHashedCollectionID() const = 0;
		virtual std::unique_ptr<BaseController> clone() const				  = 0;

	  private:
		bool _isActive = true;
	};
} // namespace VTX::UI::QT::Controller
#endif
