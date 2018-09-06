#ifndef OSG_BLENDEQUATION
#define OSG_BLENDEQUATION 1

#include <VBF_Engine/VBF_SceneGraph/VBFO_StateAttribute.h>

#ifndef GL_VERSION_1_2
/* Logic Ops */
#define GL_MIN                                  0x8007
#define GL_MAX                                  0x8008
#define GL_FUNC_ADD                             0x8006
#define GL_FUNC_SUBTRACT                        0x800A
#define GL_FUNC_REVERSE_SUBTRACT                0x800B
#endif

#ifndef GL_LOGIC_OP
#define GL_LOGIC_OP                             0x0BF1
#endif

#ifndef GL_ALPHA_MIN_SGIX
#define GL_ALPHA_MIN_SGIX                       0x8320
#define GL_ALPHA_MAX_SGIX                       0x8321
#endif

namespace osg {

/** Encapsulates OpenGL BlendEquation state. */
class OSG_EXPORT BlendEquation : public StateAttribute
{
    public :

        enum Equation 
		{
            RGBA_MIN                        = GL_MIN,
            RGBA_MAX                        = GL_MAX,
            ALPHA_MIN                       = GL_ALPHA_MIN_SGIX,
            ALPHA_MAX                       = GL_ALPHA_MAX_SGIX,
            LOGIC_OP                        = GL_LOGIC_OP,
            FUNC_ADD                        = GL_FUNC_ADD,
            FUNC_SUBTRACT                   = GL_FUNC_SUBTRACT,
            FUNC_REVERSE_SUBTRACT           = GL_FUNC_REVERSE_SUBTRACT
        };

        BlendEquation();
        
        BlendEquation(Equation equation);

        BlendEquation(Equation equationRGB, Equation equationAlpha);
// IE���ܴ���
        /** Copy constructor using CopyOp to manage deep vs shallow copy. */
        BlendEquation(const BlendEquation& trans,const CopyOp& copyop=CopyOp::SHALLOW_COPY):
            StateAttribute(trans,copyop),
            _equationRGB(trans._equationRGB),
            _equationAlpha(trans._equationAlpha){}

        META_StateAttribute(osg, BlendEquation,BLENDEQUATION);
        
        /** Return -1 if *this < *rhs, 0 if *this==*rhs, 1 if *this>*rhs. */
        virtual int compare(const StateAttribute& sa) const
        {
            // Check for equal types, then create the rhs variable
            // used by the COMPARE_StateAttribute_Parameter macros below.
            COMPARE_StateAttribute_Types(BlendEquation,sa)

            // Compare each parameter in turn against the rhs.
            COMPARE_StateAttribute_Parameter(_equationRGB)
            COMPARE_StateAttribute_Parameter(_equationAlpha)

            return 0; // Passed all the above comparison macros, so must be equal.
        }

        virtual bool getModeUsage(StateAttribute::ModeUsage& usage) const
        {
            usage.usesMode(GL_BLEND);
            return true;
        }


        inline void setEquation(Equation equation) { _equationRGB = _equationAlpha = equation; }
        inline Equation getEquation() const { return _equationRGB; }

        inline void setEquationRGB(Equation equation) { _equationRGB = equation; }
        inline Equation getEquationRGB() const { return _equationRGB; }

        inline void setEquationAlpha(Equation equation) { _equationAlpha = equation; }
        inline Equation getEquationAlpha() const { return _equationAlpha; }

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

                bool isBlendEquationSupported() const { return _isBlendEquationSupported; }
                bool isBlendEquationSeparateSupported() const { return _isBlendEquationSeparateSupported; }
                bool isSGIXMinMaxSupported() const { return _isSGIXMinMaxSupported; }
                bool isLogicOpSupported() const { return _isLogicOpSupported; }
                
                void glBlendEquation(GLenum mode) const;
                void glBlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha) const;

            protected:

                ~Extensions() {}// IE���ܴ���
                
                typedef void (GL_APIENTRY * GLBlendEquationProc)(GLenum mode);
                typedef void (GL_APIENTRY * GLBlendEquationSeparateProc)(GLenum modeRGB, GLenum modeAlpha);

                bool                _isBlendEquationSupported;
                bool                _isBlendEquationSeparateSupported;
                bool                _isSGIXMinMaxSupported;
                bool                _isLogicOpSupported;
                
                GLBlendEquationProc _glBlendEquation;
                GLBlendEquationSeparateProc _glBlendEquationSeparate;
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

        virtual ~BlendEquation();


        Equation _equationRGB, _equationAlpha;
};

}

#endif
