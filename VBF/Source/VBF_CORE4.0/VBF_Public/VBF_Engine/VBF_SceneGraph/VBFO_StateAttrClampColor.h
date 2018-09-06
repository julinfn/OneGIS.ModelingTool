#ifndef OSG_CLAMPCOLOR
#define OSG_CLAMPCOLOR 1

#include <VBF_Engine/VBF_SceneGraph/VBFO_StateAttribute.h>

#ifndef GL_ARB_color_buffer_float
#define GL_RGBA_FLOAT_MODE_ARB                  0x8820
#define GL_CLAMP_VERTEX_COLOR_ARB               0x891A
#define GL_CLAMP_FRAGMENT_COLOR_ARB             0x891B
#define GL_CLAMP_READ_COLOR_ARB                 0x891C
#define GL_FIXED_ONLY_ARB                       0x891D
#endif

#ifndef GL_FIXED_ONLY
#define GL_FIXED_ONLY                           GL_FIXED_ONLY_ARB
#define GL_CLAMP_VERTEX_COLOR                   GL_CLAMP_VERTEX_COLOR_ARB
#define GL_CLAMP_READ_COLOR                     GL_CLAMP_READ_COLOR_ARB
#define GL_CLAMP_FRAGMENT_COLOR                 GL_CLAMP_FRAGMENT_COLOR_ARB
#endif

#if defined(OSG_GL3_AVAILABLE)
    #define GL_CLAMP_VERTEX_COLOR             0x891A
    #define GL_CLAMP_FRAGMENT_COLOR           0x891B
#endif

namespace osg {

/** Encapsulates OpenGL ClampColor state. */
class OSG_EXPORT ClampColor : public StateAttribute
{
    public :

        ClampColor();
        
        ClampColor(GLenum vertexMode, GLenum fragmentMode, GLenum readMode);
// IE���ܴ���
        /** Copy constructor using CopyOp to manage deep vs shallow copy. */
        ClampColor(const ClampColor& rhs,const CopyOp& copyop=CopyOp::SHALLOW_COPY):
            StateAttribute(rhs,copyop),
            _clampVertexColor(rhs._clampVertexColor),
            _clampFragmentColor(rhs._clampFragmentColor),
            _clampReadColor(rhs._clampReadColor) {}

        META_StateAttribute(osg, ClampColor,CLAMPCOLOR);
        
        /** Return -1 if *this < *rhs, 0 if *this==*rhs, 1 if *this>*rhs. */
        virtual int compare(const StateAttribute& sa) const
        {
            // Check for equal types, then create the rhs variable
            // used by the COMPARE_StateAttribute_Parameter macros below.
            COMPARE_StateAttribute_Types(ClampColor,sa)

            // Compare each parameter in turn against the rhs.
            COMPARE_StateAttribute_Parameter(_clampVertexColor)
            COMPARE_StateAttribute_Parameter(_clampFragmentColor)
            COMPARE_StateAttribute_Parameter(_clampReadColor)

            return 0; // Passed all the above comparison macros, so must be equal.
        }

        void setClampVertexColor(GLenum mode) { _clampVertexColor = mode; }
        GLenum getClampVertexColor() const { return _clampVertexColor; }

        void setClampFragmentColor(GLenum mode) { _clampFragmentColor = mode; }
        GLenum getClampFragmentColor() const { return _clampFragmentColor; }

        void setClampReadColor(GLenum mode) { _clampReadColor = mode; }
        GLenum getClampReadColor() const { return _clampReadColor; }

        
        virtual void apply(State& state) const;
        /** Encapsulates queries of extension availability, obtains extension
          * function pointers, and provides convenience wrappers for
          * calling extension functions. */        
        class OSG_EXPORT Extensions : public ::CVBF_Referenced
        {
            public:
                Extensions(unsigned int contextID);

                Extensions(const Extensions& rhs);
                
                void lowestCommonDenominator(const Extensions& rhs);
                
                void setupGLExtensions(unsigned int contextID);

                void setClampColorSupported(bool flag) { _isClampColorSupported=flag; }
                bool isClampColorSupported() const { return _isClampColorSupported; }
                
                void glClampColor(GLenum target, GLenum mode) const;

            protected:

                ~Extensions() {}// IE���ܴ���
                
                typedef void (GL_APIENTRY * GLClampColorProc) (GLenum target, GLenum mode);
                bool                _isClampColorSupported;
                GLClampColorProc    _glClampColor;

        };
        
        /** Returns the Extensions object for the given context.
          * If createIfNotInitalized is true and the Extensions object doesn't
          * exist, getExtensions() creates it on the given context.
          * Returns NULL if createIfNotInitalized is false and the Extensions
          * object doesn't exist. */
        static Extensions* getExtensions(unsigned int contextID,bool createIfNotInitalized);

        /** setExtensions() allows users to override the extensions across graphics contexts.
          * Typically used when you have different extensions supported across graphics pipes,
          * but need to ensure that they all use the same low common denominator extensions. */
        static void setExtensions(unsigned int contextID,Extensions* extensions);


    protected :

        virtual ~ClampColor();

        GLenum _clampVertexColor;
        GLenum _clampFragmentColor;
        GLenum _clampReadColor;

};

}

#endif
