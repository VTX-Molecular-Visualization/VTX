#include "app/old/render/renderer/gl/framebuffer.hpp"
#include <util/exceptions.hpp>
#include <util/logger.hpp>

namespace VTX::App::Old::Render::Renderer::GL
{
	// TODO reimplement this without Qt (OpenGL embedded by Qt)
	void Framebuffer::_checkStatus() const
	{
		//// Error messages come from http://docs.gl/gl4/glCheckFramebufferStatus.
		// const GLenum status = _gl->glCheckNamedFramebufferStatus( _id, GLenum( _target ) );
		// if ( status != GL_FRAMEBUFFER_COMPLETE )
		//{
		//	switch ( status )
		//	{
		//	case GL_FRAMEBUFFER_UNDEFINED:
		//		VTX_WARNING(
		//			"The specified framebuffer is the default read or draw framebuffer, but the default framebuffer "
		//			"does not exist." );
		//		break;
		//	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		//		VTX_WARNING( "Any of the framebuffer attachment points are framebuffer incomplete." );
		//		break;
		//	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		//		VTX_WARNING( "The framebuffer does not have at least one image attached to it." );
		//		break;
		//	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
		//		VTX_WARNING(
		//			"The value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for any color attachment point(s) "
		//			"named by GL_DRAW_BUFFERi." );
		//		break;
		//	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
		//		VTX_WARNING(
		//			"GL_READ_BUFFER is not GL_NONE and the value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE "
		//			"for the color attachment point named by GL_READ_BUFFER." );
		//		break;
		//	case GL_FRAMEBUFFER_UNSUPPORTED:
		//		VTX_WARNING(
		//			"The combination of internal formats of the attached images violates an implementation-dependent "
		//			"set of restrictions." );
		//		break;
		//	case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
		//		VTX_WARNING(
		//			"The value of GL_RENDERBUFFER_SAMPLES is not the same for all attached renderbuffers; the value "
		//			"of GL_TEXTURE_SAMPLES is the not same for all attached textures; or, the attached images are a "
		//			"mix of renderbuffers and textures, the value of GL_RENDERBUFFER_SAMPLES does not match the value "
		//			"of GL_TEXTURE_SAMPLES.\n"
		//			"OR:\n"
		//			"The value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not the same for all attached textures; or, "
		//			"the attached images are a mix of renderbuffers and textures, the value of "
		//			"GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not GL_TRUE for all attached textures." );
		//		break;
		//	case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
		//		VTX_WARNING(
		//			"Any framebuffer attachment is layered, and any populated attachment is not layered, or all "
		//			"populated color attachments are not from textures of the same target." );
		//		break;
		//	default: VTX_WARNING( "Unknown framebuffer error" ); break;
		//	}
		//}
	}
} // namespace VTX::App::Old::Render::Renderer::GL
