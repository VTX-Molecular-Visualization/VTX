#ifndef __VTX_RENDERER_CONTEXT_INCLUDE_OPENGL__
#define __VTX_RENDERER_CONTEXT_INCLUDE_OPENGL__

#ifndef VTX_RENDERER_NO_OPENGL
#include <glad/glad.h>
#else

using GLboolean	   = bool;
using GLbyte	   = char;
using GLubyte	   = unsigned char;
using GLshort	   = short;
using GLushort	   = unsigned short;
using GLint		   = int;
using GLuint	   = unsigned int;
using GLint64	   = long long int;
using GLuint64	   = unsigned long long int;
using GLfloat	   = float;
using GLsizei	   = int;
using GLenum	   = unsigned int;
using GLbitfield   = unsigned int;
using GLchar	   = char;
using GLsizeiptr   = int;
using GLvoid	   = void;
using GLintptr	   = long long int;
using GLuint64	   = unsigned long long int;

#define APIENTRY
using GLADloadproc = void * (*)( const char * );
static GLboolean			gladLoadGLLoader( GLADloadproc ) { return 0; }
static GLboolean			gladLoadGL() { return 0; }
struct gladGLversionStruct
{
	int major;
	int minor;
};
static struct gladGLversionStruct GLVersion;
constexpr int					  GLAD_GL_VERSION_4_5 = 1;

constexpr GLuint GL_BOOL							   = 0x8B56;
constexpr GLuint GL_BYTE							   = 0x1400;
constexpr GLuint GL_UNSIGNED_BYTE					   = 0x1401;
constexpr GLuint GL_SHORT							   = 0x1402;
constexpr GLuint GL_UNSIGNED_SHORT					   = 0x1403;
constexpr GLuint GL_INT								   = 0x1404;
constexpr GLuint GL_UNSIGNED_INT					   = 0x1405;
constexpr GLuint GL_FLOAT							   = 0x1406;
constexpr GLuint GL_DOUBLE							   = 0x140A;
constexpr GLuint GL_INVALID_INDEX					   = 0xFFFFFFFF;
constexpr GLuint GL_FALSE							   = 0;
constexpr GLuint GL_TRUE							   = 1;
constexpr GLuint GL_ATTACHED_SHADERS				   = 0x8B85;
constexpr GLuint GL_LINK_STATUS						   = 0x8B82;
constexpr GLuint GL_COMPILE_STATUS					   = 0x8B81;
constexpr GLuint GL_VERTEX_SHADER					   = 0x8B31;
constexpr GLuint GL_FRAGMENT_SHADER					   = 0x8B30;
constexpr GLuint GL_GEOMETRY_SHADER					   = 0x8DD9;
constexpr GLuint GL_TESS_CONTROL_SHADER				   = 0x8E88;
constexpr GLuint GL_TESS_EVALUATION_SHADER			   = 0x8E87;
constexpr GLuint GL_COMPUTE_SHADER					   = 0x91B9;
constexpr GLuint GL_ARRAY_BUFFER					   = 0x8892;
constexpr GLuint GL_ELEMENT_ARRAY_BUFFER			   = 0x8893;
constexpr GLuint GL_FRAMEBUFFER						   = 0x8D40;
constexpr GLuint GL_FRAMEBUFFER_COMPLETE			   = 0x8CD5;
constexpr GLuint GL_TIMESTAMP						   = 0x8E28;
constexpr GLuint GL_QUERY_RESULT					   = 0x8866;
constexpr GLuint GL_QUERY_RESULT_AVAILABLE			   = 0x8867;
constexpr GLuint GL_TEXTURE_2D						   = 0x0DE1;
constexpr GLuint GL_TEXTURE_2D_ARRAY				   = 0x8C1A;
constexpr GLuint GL_STATIC_DRAW						   = 0x88E4;
constexpr GLuint GL_DYNAMIC_DRAW					   = 0x88E8;
constexpr GLuint GL_STREAM_DRAW						   = 0x88E0;
constexpr GLuint GL_PATCH_VERTICES					   = 0x8E72;
constexpr GLuint GL_TRIANGLES						   = 0x0004;
constexpr GLuint GL_DEBUG_OUTPUT					   = 0x92E0;
constexpr GLuint GL_DEBUG_OUTPUT_SYNCHRONOUS		   = 0x8242;
constexpr GLuint GL_DEPTH_TEST						   = 0x0B71;
constexpr GLuint GL_RGBA32F							   = 0x8814;
constexpr GLuint GL_RGBA							   = 0x1908;
constexpr GLuint GL_RGBA8							   = 0x8058;
constexpr GLuint GL_RGBA16F							   = 0x881A;
constexpr GLuint GL_RGBA32UI						   = 0x8D70;
constexpr GLuint GL_RGBA16UI						   = 0x8D76;
constexpr GLuint GL_RGBA8UI							   = 0x8D7C;
constexpr GLuint GL_RGBA32I							   = 0x8D82;
constexpr GLuint GL_RGBA16I							   = 0x8D88;
constexpr GLuint GL_RGBA8I							   = 0x8D8E;
constexpr GLuint GL_RGB32F							   = 0x8815;
constexpr GLuint GL_RGB								   = 0x1907;
constexpr GLuint GL_RGB8							   = 0x8051;
constexpr GLuint GL_RGB16F							   = 0x881B;
constexpr GLuint GL_RGB32UI							   = 0x8D71;
constexpr GLuint GL_RGB16UI							   = 0x8D77;
constexpr GLuint GL_RGB8UI							   = 0x8D7D;
constexpr GLuint GL_RGB32I							   = 0x8D83;
constexpr GLuint GL_RGB16I							   = 0x8D89;
constexpr GLuint GL_RGB8I							   = 0x8D8F;
constexpr GLuint GL_RG32F							   = 0x8230;
constexpr GLuint GL_RG								   = 0x8227;
constexpr GLuint GL_RG8								   = 0x822B;
constexpr GLuint GL_RG16F							   = 0x822F;
constexpr GLuint GL_RG32UI							   = 0x823C;
constexpr GLuint GL_RG16UI							   = 0x823A;
constexpr GLuint GL_RG8UI							   = 0x8238;
constexpr GLuint GL_RG32I							   = 0x823B;
constexpr GLuint GL_RG16I							   = 0x8239;
constexpr GLuint GL_RG8I							   = 0x8237;
constexpr GLuint GL_R32F							   = 0x822E;
constexpr GLuint GL_R								   = 0x8225;
constexpr GLuint GL_R8								   = 0x8229;
constexpr GLuint GL_R16F							   = 0x822D;
constexpr GLuint GL_R32UI							   = 0x8236;
constexpr GLuint GL_R16UI							   = 0x8234;
constexpr GLuint GL_R8UI							   = 0x8232;
constexpr GLuint GL_R32I							   = 0x8235;
constexpr GLuint GL_R16I							   = 0x8233;
constexpr GLuint GL_R8I								   = 0x8231;
constexpr GLuint GL_RG_INTEGER						   = 0x8228;
constexpr GLuint GL_REPEAT							   = 0x2901;
constexpr GLuint GL_MIRRORED_REPEAT					   = 0x8370;
constexpr GLuint GL_MIRROR_CLAMP_TO_EDGE			   = 0x8743;
constexpr GLuint GL_LINEAR							   = 0x2601;
constexpr GLuint GL_CLAMP_TO_EDGE					   = 0x812F;
constexpr GLuint GL_CLAMP_TO_BORDER					   = 0x812D;
constexpr GLuint GL_NEAREST							   = 0x2600;
constexpr GLuint GL_LINEAR_MIPMAP_LINEAR			   = 0x2703;
constexpr GLuint GL_NEAREST_MIPMAP_NEAREST			   = 0x2700;
constexpr GLuint GL_LINEAR_MIPMAP_NEAREST			   = 0x2701;
constexpr GLuint GL_NEAREST_MIPMAP_LINEAR			   = 0x2702;
constexpr GLuint GL_TEXTURE_WRAP_S					   = 0x2802;
constexpr GLuint GL_TEXTURE_WRAP_T					   = 0x2803;
constexpr GLuint GL_TEXTURE_MIN_FILTER				   = 0x2801;
constexpr GLuint GL_TEXTURE_MAG_FILTER				   = 0x2800;
constexpr GLuint GL_COLOR_ATTACHMENT0				   = 0x8CE0;
constexpr GLuint GL_COLOR_ATTACHMENT1				   = 0x8CE1;
constexpr GLuint GL_COLOR_ATTACHMENT2				   = 0x8CE2;
constexpr GLuint GL_COLOR_ATTACHMENT3				   = 0x8CE3;
constexpr GLuint GL_COLOR_ATTACHMENT4				   = 0x8CE4;
constexpr GLuint GL_COLOR_ATTACHMENT5				   = 0x8CE5;
constexpr GLuint GL_DEPTH_ATTACHMENT				   = 0x8D00;
constexpr GLuint GL_COLOR_BUFFER_BIT				   = 0x00004000;
constexpr GLuint GL_DEPTH_BUFFER_BIT				   = 0x00000100;
constexpr GLuint GL_STENCIL_BUFFER_BIT				   = 0x00000400;
constexpr GLuint GL_DEPTH_COMPONENT32F				   = 0x8CAC;
constexpr GLuint GL_DEPTH_COMPONENT32				   = 0x81A7;
constexpr GLuint GL_DEPTH_COMPONENT24				   = 0x81A6;
constexpr GLuint GL_DEPTH_COMPONENT16				   = 0x81A5;
constexpr GLuint GL_DEPTH_COMPONENT					   = 0x1902;
constexpr GLuint GL_DEPTH_STENCIL					   = 0x84F9;
constexpr GLuint GL_DRAW_FRAMEBUFFER				   = 0x8CA9;
constexpr GLuint GL_READ_FRAMEBUFFER				   = 0x8CA8;
constexpr GLuint GL_EXTENSIONS						   = 0x1F03;
constexpr GLuint GL_INFO_LOG_LENGTH					   = 0x8B84;
constexpr GLuint GL_LESS							   = 0x0201;
constexpr GLuint GL_EQUAL							   = 0x0202;
constexpr GLuint GL_GREATER							   = 0x0204;
constexpr GLuint GL_NOTEQUAL						   = 0x0205;
constexpr GLuint GL_POINTS							   = 0x0000;
constexpr GLuint GL_LINES							   = 0x0001;
constexpr GLuint GL_LINE_SMOOTH						   = 0x0B20;
constexpr GLuint GL_LINE_WIDTH						   = 0x0B21;
constexpr GLuint GL_LINE_STRIP						   = 0x0003;
constexpr GLuint GL_LINE_LOOP						   = 0x0002;
constexpr GLuint GL_TRIANGLE_STRIP					   = 0x0005;
constexpr GLuint GL_TRIANGLE_FAN					   = 0x0006;
constexpr GLuint GL_PATCHES							   = 0x000E;
constexpr GLuint GL_CULL_FACE						   = 0x0B44;
constexpr GLuint GL_FRONT							   = 0x0404;
constexpr GLuint GL_BACK							   = 0x0405;
constexpr GLuint GL_FRONT_AND_BACK					   = 0x0408;
constexpr GLuint GL_MAX_COMPUTE_WORK_GROUP_COUNT	   = 0x91BE;
constexpr GLuint GL_MAX_COMPUTE_WORK_GROUP_SIZE		   = 0x91BF;
constexpr GLuint GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS = 0x90EB;
constexpr GLuint GL_MAX_COMPUTE_SHARED_MEMORY_SIZE	   = 0x8262;
constexpr GLuint GL_MAX_COMPUTE_UNIFORM_COMPONENTS	   = 0x8263;
constexpr GLuint GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS	   = 0x91BC;
constexpr GLuint GL_MAX_COMPUTE_IMAGE_UNIFORMS		   = 0x91BD;
constexpr GLuint GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS = 0x8264;
constexpr GLuint GL_MAX_COMPUTE_ATOMIC_COUNTERS		   = 0x8265;
constexpr GLuint GL_MAX_PATCH_VERTICES				   = 0x8E7D;
constexpr GLuint GL_MAX_TESS_GEN_LEVEL				   = 0x8E7E;
constexpr GLuint GL_MAX_SHADER_STORAGE_BLOCK_SIZE	   = 0x90DE;
constexpr GLuint GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS = 0x90DD;
constexpr GLuint GL_MAX_TEXTURE_SIZE				   = 0x0D33;
constexpr GLuint GL_MAX_UNIFORM_BLOCK_SIZE			   = 0x8A30;
constexpr GLuint GL_NUM_EXTENSIONS					   = 0x821D;
constexpr GLuint GL_RENDERER						   = 0x1F01;
constexpr GLuint GL_SHADING_LANGUAGE_VERSION		   = 0x8B8C;
constexpr GLuint GL_VENDOR							   = 0x1F00;
constexpr GLuint GL_VERSION							   = 0x1F02;
constexpr GLuint GL_SHADER_STORAGE_BUFFER			   = 0x90D2;
constexpr GLuint GL_UNIFORM_BUFFER					   = 0x8A11;

constexpr GLuint GL_DEBUG_SOURCE_API			   = 0x8246;
constexpr GLuint GL_DEBUG_SOURCE_WINDOW_SYSTEM	   = 0x8247;
constexpr GLuint GL_DEBUG_SOURCE_SHADER_COMPILER   = 0x8248;
constexpr GLuint GL_DEBUG_SOURCE_THIRD_PARTY	   = 0x8249;
constexpr GLuint GL_DEBUG_SOURCE_APPLICATION	   = 0x824A;
constexpr GLuint GL_DEBUG_SOURCE_OTHER			   = 0x824B;
constexpr GLuint GL_DEBUG_TYPE_ERROR			   = 0x824C;
constexpr GLuint GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR = 0x824D;
constexpr GLuint GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR  = 0x824E;
constexpr GLuint GL_DEBUG_TYPE_PORTABILITY		   = 0x824F;
constexpr GLuint GL_DEBUG_TYPE_PERFORMANCE		   = 0x8250;
constexpr GLuint GL_DEBUG_TYPE_OTHER			   = 0x8251;
constexpr GLuint GL_DEBUG_TYPE_MARKER			   = 0x8268;
constexpr GLuint GL_DEBUG_TYPE_PUSH_GROUP		   = 0x8269;
constexpr GLuint GL_DEBUG_TYPE_POP_GROUP		   = 0x826A;
constexpr GLuint GL_DEBUG_SEVERITY_HIGH			   = 0x9146;
constexpr GLuint GL_DEBUG_SEVERITY_MEDIUM		   = 0x9147;
constexpr GLuint GL_DEBUG_SEVERITY_LOW			   = 0x9148;
constexpr GLuint GL_DEBUG_SEVERITY_NOTIFICATION	   = 0x826B;

static void glEnable( GLenum ) {}
static void glDisable( GLenum ) {}
static void glClearColor( GLfloat, GLfloat, GLfloat, GLfloat ) {}
static void glClear( GLbitfield ) {}
static void glViewport( GLint, GLint, GLsizei, GLsizei ) {}
static void
glDebugMessageCallback( void ( * )( GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar *, const void * ), const void * )
{
}
static void			   glDepthFunc( GLenum ) {}
static void			   glLineWidth( GLfloat ) {}
static const GLubyte * glGetString( GLenum ) { return nullptr; }
static const GLubyte * glGetStringi( GLenum, GLuint ) { return nullptr; }
static void			   glGetIntegerv( GLenum, GLint * ) {}
static void			   glGetIntegeri_v( GLenum, GLuint, GLint * ) {}
static void			   glGetFloatv( GLenum, GLfloat * ) {}
static void			   glGetError() {}
static void			   glFinish() {}
static void			   glFlush() {}
static void			   glPatchParameteri( GLenum, GLint ) {}

static GLboolean glIsProgram( GLuint ) { return 0; }
static GLboolean glIsShader( GLuint ) { return 0; }
static GLuint	 glCreateProgram() { return 0; }
static void		 glAttachShader( GLuint, GLuint ) {}
static void		 glLinkProgram( GLuint ) {}
static void		 glUseProgram( GLuint ) {}
static void		 glDetachShader( GLuint, GLuint ) {}
static void		 glDeleteProgram( GLuint ) {}
static void		 glDeleteShader( GLuint ) {}
static void		 glShaderSource( GLuint, GLsizei, const GLchar **, const GLint * ) {}
static void		 glCompileShader( GLuint ) {}
static GLuint	 glCreateShader( GLenum ) { return 0; }
static void		 glGetAttachedShaders( GLuint, GLsizei, GLsizei *, GLuint * ) {}
static GLint	 glGetUniformLocation( GLuint, const GLchar * ) { return -1; }
static void		 glGetProgramiv( GLuint, GLenum, GLint * ) {}
static void		 glGetProgramInfoLog( GLuint, GLsizei, GLsizei *, GLchar * ) {}
static void		 glGetShaderInfoLog( GLuint, GLsizei, GLsizei *, GLchar * ) {}
static void		 glGetShaderiv( GLuint, GLenum, GLint * ) {}
static void		 glUniform1ui( GLint, GLuint ) {}
static void		 glUniform1i( GLint, GLint ) {}
static void		 glUniform1f( GLint, GLfloat ) {}
static void		 glUniform2i( GLint, GLint, GLint ) {}
static void		 glUniform2f( GLint, GLfloat, GLfloat ) {}
static void		 glUniform2fv( GLint, GLsizei, const GLfloat * ) {}
static void		 glUniform2iv( GLint, GLsizei, const GLint * ) {}
static void		 glUniform3i( GLint, GLint, GLint, GLint ) {}
static void		 glUniform3f( GLint, GLfloat, GLfloat, GLfloat ) {}
static void		 glUniform3iv( GLint, GLsizei, const GLint * ) {}
static void		 glUniform3uiv( GLint, GLsizei, const GLuint * ) {}
static void		 glUniform3fv( GLint, GLsizei, const GLfloat * ) {}
static void		 glUniform4i( GLint, GLint, GLint, GLint, GLint ) {}
static void		 glUniform4f( GLint, GLfloat, GLfloat, GLfloat, GLfloat ) {}
static void		 glUniform4iv( GLint, GLsizei, const GLint * ) {}
static void		 glUniform4fv( GLint, GLsizei, const GLfloat * ) {}
static void		 glUniformMatrix4fv( GLint, GLsizei, GLboolean, const GLfloat * ) {}

static GLboolean glIsQuery( GLuint ) { return 0; }
static void		 glCreateQueries( GLenum, GLsizei, GLuint * ) {}
static void		 glDeleteQueries( GLsizei, const GLuint * ) {}
static void		 glQueryCounter( GLuint, GLenum ) {}
static void		 glGetQueryObjectiv( GLuint, GLenum, GLint * ) {}
static void		 glGetQueryObjectui64v( GLuint, GLenum, GLuint64 * ) {}

static GLboolean glIsBuffer( GLuint ) { return 0; }
static void		 glCreateBuffers( GLsizei, GLuint * ) {}
static void		 glDeleteBuffers( GLsizei, const GLuint * ) {}
static void		 glBindBuffer( GLenum, GLuint ) {}
static void		 glBindBufferBase( GLenum, GLuint, GLuint ) {}
static void		 glBufferData( GLenum, GLsizeiptr, const GLvoid *, GLenum ) {}
static void		 glNamedBufferData( GLuint, GLsizeiptr, const GLvoid *, GLenum ) {}
static void		 glNamedBufferStorage( GLuint, GLsizeiptr, const GLvoid *, GLbitfield ) {}
static void		 glBufferSubData( GLenum, GLintptr, GLsizeiptr, const GLvoid * ) {}
static void		 glNamedBufferSubData( GLuint, GLintptr, GLsizeiptr, const GLvoid * ) {}
static void *	 glMapNamedBuffer( GLuint, GLenum ) { return nullptr; }
static void		 glUnmapNamedBuffer( GLuint ) {}
static void *	 glMapNamedBufferRange( GLuint, GLintptr, GLsizeiptr, GLbitfield ) { return nullptr; }

static GLboolean glIsFramebuffer( GLuint ) { return 0; }
static void		 glCreateFramebuffers( GLsizei, GLuint * ) {}
static void		 glBindFramebuffer( GLenum, GLuint ) {}
static GLenum	 glCheckFramebufferStatus( GLenum ) { return 0; }
static void		 glDeleteFramebuffers( GLsizei, const GLuint * ) {}
static void		 glNamedFramebufferTexture( GLuint, GLenum, GLuint, GLint ) {}
static void		 glNamedFramebufferDrawBuffers( GLuint, GLsizei, const GLenum * ) {}
static void		 glNamedFramebufferReadBuffer( GLuint, GLenum ) {}
static void		 glReadPixels( GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, void * ) {}
static void		 glReadnPixels( GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, GLsizei, void * ) {}

static GLboolean glIsVertexArray( GLuint ) { return 0; }
static void		 glBindVertexArray( GLuint ) {}
static void		 glCreateVertexArrays( GLsizei, GLuint * ) {}
static void		 glDeleteVertexArrays( GLsizei, const GLuint * ) {}
static void		 glVertexArrayAttribBinding( GLuint, GLuint, GLuint ) {}
static void		 glDrawArrays( GLenum, GLint, GLsizei ) {}
static void		 glDrawElements( GLenum, GLsizei, GLenum, const void * ) {}
static void		 glEnableVertexArrayAttrib( GLuint, GLuint ) {}
static void		 glVertexArrayVertexBuffer( GLuint, GLuint, GLuint, GLintptr, GLsizei ) {}
static void		 glVertexArrayAttribFormat( GLuint, GLuint, GLint, GLenum, GLboolean, GLuint ) {}
static void		 glVertexArrayAttribIFormat( GLuint, GLuint, GLint, GLenum, GLuint ) {}
static void		 glVertexArrayAttribLFormat( GLuint, GLuint, GLint, GLenum, GLuint ) {}
static void		 glVertexArrayElementBuffer( GLuint, GLuint ) {}
static void		 glMultiDrawArrays( GLenum, const GLint *, const GLsizei *, GLsizei ) {}
static void		 glMultiDrawElements( GLenum, const GLsizei *, GLenum, const void * const *, GLsizei ) {}

static GLboolean glIsTexture( GLuint ) { return 0; }
static void		 glCreateTextures( GLenum, GLsizei, GLuint * ) {}
static void		 glDeleteTextures( GLsizei, const GLuint * ) {}
static void		 glBindTexture( GLenum, GLuint ) {}
static void		 glBindTextureUnit( GLuint, GLuint ) {}
static void		 glClearTexImage( GLuint, GLint, GLenum, GLenum, const void * ) {}
static void		 glGetTextureImage( GLuint, GLint, GLenum, GLenum, GLsizei, void * ) {}
static void		 glTextureParameteri( GLuint, GLenum, GLint ) {}
static void		 glTextureStorage2D( GLuint, GLsizei, GLenum, GLsizei, GLsizei ) {}
static void		 glTextureSubImage2D( GLuint, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const void * ) {}

#endif

#endif