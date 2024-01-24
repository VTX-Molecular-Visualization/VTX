// #ifndef __VTX_UI_QT_CONTROLLER_CAMERA_ANIMATOR__
// #define __VTX_UI_QT_CONTROLLER_CAMERA_ANIMATOR__
//
// #include <QWidget>
// #include <app/component/render/camera.hpp>
// #include <util/maths/aabb.hpp>
// #include <util/types.hpp>
//
// namespace VTX::UI::QT::Controller
//{
//	class CameraAnimator
//	{
//	  public:
//		explicit CameraAnimator( VTX::App::Component::Render::Camera & p_camera );
//		virtual ~CameraAnimator() = default;
//
//		void setTarget( const QWidget * const p_widget );
//
//		inline const bool	 isOrienting() const { return _isOrienting; }
//		inline const Vec3f & getOrientStartingPosition() const { return _orientStartingPosition; }
//		inline const Vec3f & getOrientTargetPosition() const { return _orientTargetPosition; }
//
//		void update( const float & p_deltaTime ) override;
//
//		virtual void reset() { _isOrienting = false; }
//
//		virtual void orient( const Util::Math::AABB & p_aabb )
//		{
//			_orientTime = 0.f;
//			_computeOrientPositions( p_aabb );
//		}
//		virtual void orient( const Vec3f & p_position, const Quatf & p_orientation )
//		{
//			_orientTime = 0.f;
//			_computeOrientPositions( p_position, p_orientation );
//		}
//
//	  protected:
//		const float ORIENT_DURATION	 = 0.5f;
//		const float ORIENT_THRESHOLD = 1e-4f;
//
//		App::Component::Render::Camera & _camera;
//		const QWidget *					 _widgetTarget = nullptr;
//
//		bool  _isOrienting			  = false;
//		float _orientTime			  = 0.f;
//		Vec3f _orientStartingPosition = VEC3F_ZERO;
//		Vec3f _orientTargetPosition	  = VEC3F_ZERO;
//		Quatf _orientStartingRotation = QUATF_ID;
//		Quatf _orientTargetRotation	  = QUATF_ID;
//
//		virtual void _updateInputs( const float & )													  = 0;
//		virtual void _computeOrientPositions( const Util::Math::AABB & p_aabb )						  = 0;
//		virtual void _computeOrientPositions( const Vec3f & p_position, const Quatf & p_orientation ) = 0;
//		virtual void _updateOrient( const float & )													  = 0;
//	};
// } // namespace VTX::UI::QT::Controller
// #endif
