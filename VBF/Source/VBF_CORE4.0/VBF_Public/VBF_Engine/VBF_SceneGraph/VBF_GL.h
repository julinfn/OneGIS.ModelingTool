#ifndef OSG_GL
#define OSG_GL 1

#include <VBF_Engine/VBF_SceneGraph/Config>
#include <VBF_Engine/VBF_SceneGraph/Export>

#if defined(OSG_GLES1_AVAILABLE)

    #ifdef __APPLE__
        #include "TargetConditionals.h"
        #include <OpenGLES/ES1/gl.h>
    #else
        #include <GLES/gl.h>
    #endif

#elif defined(OSG_GLES2_AVAILABLE)

    #ifdef __APPLE__
        #include "TargetConditionals.h"
        #include <OpenGLES/ES2/gl.h>
    #else
         #include <GLES2/gl2.h>
    #endif


#else

    #ifndef WIN32

        // Required for compatibility with glext.h sytle function definitions of 
        // OpenGL extensions, such as in src/osg/Point.cpp.
        #ifndef APIENTRY
            #define APIENTRY
        #endif

    #else // WIN32

        #if defined(__CYGWIN__) || defined(__MINGW32__)

            #ifndef APIENTRY
                 #define GLUT_APIENTRY_DEFINED
                 #define APIENTRY __stdcall
            #endif
             // XXX This is from Win32's <windef.h> 
            #ifndef CALLBACK
                #define CALLBACK __stdcall
            #endif

        #else // ! __CYGWIN__

            // Under Windows avoid including <windows.h>
            // to avoid name space pollution, but Win32's <GL/gl.h> 
            // needs APIENTRY and WINGDIAPI defined properly. 
            // XXX This is from Win32's <windef.h> 
            #ifndef APIENTRY
                #define GLUT_APIENTRY_DEFINED
                #if (_MSC_VER >= 800) || defined(_STDCALL_SUPPORTED)
                    #define WINAPI __stdcall
                    #define APIENTRY WINAPI
                #else
                    #define APIENTRY
                #endif
            #endif

             // XXX This is from Win32's <windef.h> 
            #ifndef CALLBACK
                #if (_MSC_VER >= 800) || defined(_STDCALL_SUPPORTED)
                    #define CALLBACK __stdcall
                #else
                    #define CALLBACK
                #endif
            #endif

        #endif // __CYGWIN__

        // XXX This is from Win32's <wingdi.h> and <winnt.h> 
        #ifndef WINGDIAPI
            #define GLUT_WINGDIAPI_DEFINED
            #define DECLSPEC_IMPORT __declspec(dllimport)
            #define WINGDIAPI DECLSPEC_IMPORT
        #endif
        
        // XXX This is from Win32's <ctype.h>
        #if !defined(_WCHAR_T_DEFINED) && !(defined(__GNUC__)&&((__GNUC__ == 3)||(__GNUC__ == 4)))
            typedef unsigned short wchar_t;
            #define _WCHAR_T_DEFINED
        #endif

    #endif // WIN32

    #if defined(OSG_GL3_AVAILABLE)

        #define GL3_PROTOTYPES 1
        #include <GL3/gl3.h>

    #else
        #ifndef __gl_h_
            #ifdef __APPLE__
                #include <OpenGL/gl.h>
            #else
                #include <GL/gl.h>
            #endif
        #endif
    #endif

    #ifndef GL_APIENTRY
        #define GL_APIENTRY APIENTRY
    #endif // GL_APIENTRY

#endif


#ifdef OSG_GL_MATRICES_AVAILABLE

    inline void glLoadMatrix(const float* mat) { glLoadMatrixf(static_cast<const GLfloat*>(mat)); }
    inline void glMultMatrix(const float* mat) { glMultMatrixf(static_cast<const GLfloat*>(mat)); }

    #ifdef OSG_GLES1_AVAILABLE
        inline void glLoadMatrix(const double* mat)
        {
            GLfloat flt_mat[16];
            for(unsigned int i=0;i<16;++i) flt_mat[i] = mat[i];
            glLoadMatrixf(flt_mat);
        }

        inline void glMultMatrix(const double* mat)
        {
            GLfloat flt_mat[16];
            for(unsigned int i=0;i<16;++i) flt_mat[i] = mat[i];
            glMultMatrixf(flt_mat);
        }

    #else
        inline void glLoadMatrix(const double* mat) { glLoadMatrixd(static_cast<const GLdouble*>(mat)); }
        inline void glMultMatrix(const double* mat) { glMultMatrixd(static_cast<const GLdouble*>(mat)); }
    #endif
#endif

// add defines for OpenGL targets that don't define them, just to ease compatibility across targets
#ifndef GL_DOUBLE
    #define GL_DOUBLE 0x140A
    typedef double GLdouble;
#endif

#ifndef GL_INT
    #define GL_INT 0x1404
#endif

#ifndef GL_UNSIGNED_INT
    #define GL_UNSIGNED_INT 0x1405
#endif

#ifndef GL_NONE
    // OpenGL ES1 doesn't provide GL_NONE
    #define GL_NONE 0x0
#endif

#if defined(OSG_GLES1_AVAILABLE) || defined(OSG_GLES2_AVAILABLE)
    //GLES defines (OES)
    #define GL_RGB8_OES                                             0x8051
    #define GL_RGBA8_OES                                            0x8058
#endif

#if defined(OSG_GLES1_AVAILABLE) || defined(OSG_GLES2_AVAILABLE) || defined(OSG_GL3_AVAILABLE)
    #define GL_POLYGON                         0x0009
    #define GL_QUADS                           0x0007
    #define GL_QUAD_STRIP                      0x0008
#endif

#if defined(OSG_GL3_AVAILABLE)
    #define GL_LUMINANCE                      0x1909
    #define GL_LUMINANCE_ALPHA                0x190A
#endif

#ifdef OSG_GL1_AVAILABLE
    #define OSG_GL1_FEATURES 1
#else 
    #define OSG_GL1_FEATURES 0
#endif

#ifdef OSG_GL2_AVAILABLE
    #define OSG_GL2_FEATURES 1
#else 
    #define OSG_GL2_FEATURES 0
#endif

#ifdef OSG_GL3_AVAILABLE
    #define OSG_GL3_FEATURES 1
#else 
    #define OSG_GL3_FEATURES 0
#endif

#ifdef OSG_GLES1_AVAILABLE
    #define OSG_GLES1_FEATURES 1
#else 
    #define OSG_GLES1_FEATURES 0
#endif

#ifdef OSG_GLES2_AVAILABLE
    #define OSG_GLES2_FEATURES 1
#else 
    #define OSG_GLES2_FEATURES 0
#endif


#endif  // __osgGL_h
