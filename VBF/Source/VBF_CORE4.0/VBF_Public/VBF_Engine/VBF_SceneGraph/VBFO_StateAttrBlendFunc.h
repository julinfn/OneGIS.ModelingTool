#ifndef OSG_BLENDFUNC
#define OSG_BLENDFUNC 1

#include <VBF_Engine/VBF_SceneGraph/VBFO_StateAttribute.h>


#ifndef GL_VERSION_1_2
#define GL_CONSTANT_COLOR                 0x8001
#define GL_ONE_MINUS_CONSTANT_COLOR       0x8002
#define GL_CONSTANT_ALPHA                 0x8003
#define GL_ONE_MINUS_CONSTANT_ALPHA       0x8004
#define GL_BLEND_COLOR                    0x8005
#endif

#ifndef GL_VERSION_1_4
#define GL_BLEND_DST_RGB                  0x80C8
#define GL_BLEND_SRC_RGB                  0x80C9
#define GL_BLEND_DST_ALPHA                0x80CA
#define GL_BLEND_SRC_ALPHA                0x80CB
#endif


namespace osg {

/** Encapsulates OpenGL blend/transparency state.
 *
 *  Blending combines incoming fragment with a fragment
 *  already present in the target buffer.
 *
 *  OpenGL 1.1 supports following source and destination blending factors:
 *  GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
 *  GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA,
 *  GL_ZERO, GL_ONE.
 *
 *  Moreover, there are three source-only blending factors:
 *  GL_DST_COLOR, GL_ONE_MINUS_DST_COLOR, GL_SRC_ALPHA_SATURATE
 *  and two destination-only blending factors:
 *  GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR.
 *  OpenGL 1.4 allowed to use these five blending factors
 *  as both - source and destination blending factors.
 *
 *  Following four source and destination blending factors
 *  were added by Imaging subset of OpenGL 1.2
 *  and made mandatory by OpenGL 1.4:
 *  GL_CONSTANT_COLOR, GL_ONE_MINUS_CONSTANT_COLOR,
 *  GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA
 *
 *  OpenGL 1.4 further provides glBlendFuncSeparate
 *  (promoted from GL_EXT_blend_func_separate).
 *  It makes possible to set blending functions for RGB and Alpha separately.
 *  Before, it was possible to set just one blending function for RGBA.
 */
class OSG_EXPORT BlendFunc : public StateAttribute
{
    public :

        BlendFunc();
        
        BlendFunc(GLenum source, GLenum destination);
        BlendFunc(GLenum source, GLenum destination, GLenum source_alpha, GLenum destination_alpha);
// IE���ܴ���
        /** Copy constructor using CopyOp to manage deep vs shallow copy. */
        BlendFunc(const BlendFunc& trans,const CopyOp& copyop=CopyOp::SHALLOW_COPY):
            StateAttribute(trans,copyop),
            _source_factor(trans._source_factor),
            _destination_factor(trans._destination_factor),
            _source_factor_alpha(trans._source_factor_alpha),
            _destination_factor_alpha(trans._destination_factor_alpha) {}

        META_StateAttribute(osg, BlendFunc,BLENDFUNC);
        
        /** Return -1 if *this < *rhs, 0 if *this==*rhs, 1 if *this>*rhs. */
        virtual int compare(const StateAttribute& sa) const
        {
            // Check for equal types, then create the rhs variable
            // used by the COMPARE_StateAttribute_Parameter macros below.
            COMPARE_StateAttribute_Types(BlendFunc,sa)

            // Compare each parameter in turn against the rhs.
            COMPARE_StateAttribute_Parameter(_source_factor)
            COMPARE_StateAttribute_Parameter(_destination_factor)
            COMPARE_StateAttribute_Parameter(_source_factor_alpha)
            COMPARE_StateAttribute_Parameter(_destination_factor_alpha)

            return 0; // Passed all the above comparison macros, so must be equal.
        }

        virtual bool getModeUsage(StateAttribute::ModeUsage& usage) const
        {
            usage.usesMode(GL_BLEND);
            return true;
        }

        enum BlendFuncMode {
            DST_ALPHA                = GL_DST_ALPHA,
            DST_COLOR                = GL_DST_COLOR,
            ONE                      = GL_ONE,
            ONE_MINUS_DST_ALPHA      = GL_ONE_MINUS_DST_ALPHA,
            ONE_MINUS_DST_COLOR      = GL_ONE_MINUS_DST_COLOR,
            ONE_MINUS_SRC_ALPHA      = GL_ONE_MINUS_SRC_ALPHA,
            ONE_MINUS_SRC_COLOR      = GL_ONE_MINUS_SRC_COLOR,
            SRC_ALPHA                = GL_SRC_ALPHA,
            SRC_ALPHA_SATURATE       = GL_SRC_ALPHA_SATURATE,
            SRC_COLOR                = GL_SRC_COLOR,
            CONSTANT_COLOR           = GL_CONSTANT_COLOR,
            ONE_MINUS_CONSTANT_COLOR = GL_ONE_MINUS_CONSTANT_COLOR,
            CONSTANT_ALPHA           = GL_CONSTANT_ALPHA,
            ONE_MINUS_CONSTANT_ALPHA = GL_ONE_MINUS_CONSTANT_ALPHA,
            ZERO                     = GL_ZERO
        };

        inline void setFunction( GLenum source, GLenum destination )
        {
            _source_factor = source;
            _destination_factor = destination;
            _source_factor_alpha = source;
            _destination_factor_alpha = destination;
        }

        inline void setFunction( GLenum source_rgb, GLenum destination_rgb, GLenum source_alpha, GLenum destination_alpha )
        {
            _source_factor = source_rgb;
            _destination_factor = destination_rgb;
            _source_factor_alpha = source_alpha;
            _destination_factor_alpha = destination_alpha;
        }

        void setSource(GLenum source) { _source_factor = _source_factor_alpha = source; }
        inline GLenum getSource() const { return _source_factor; }

        void setSourceRGB(GLenum source) { _source_factor = source; }
        inline GLenum getSourceRGB() const { return _source_factor; }

        void setSourceAlpha(GLenum source) { _source_factor_alpha = source; }
        inline GLenum getSourceAlpha() const { return _source_factor_alpha; }
        
        void setDestination(GLenum destination) { _destination_factor = _destination_factor_alpha = destination; }
        inline GLenum getDestination() const { return _destination_factor; }

        void setDestinationRGB(GLenum destination) { _destination_factor = destination; }
        inline GLenum getDestinationRGB() const { return _destination_factor; }

        void setDestinationAlpha(GLenum destination) { _destination_factor_alpha = destination; }
        inline GLenum getDestinationAlpha() const { return _destination_factor_alpha; }

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

            void setBlendFuncSeparateSupported(bool flag) { _isBlendFuncSeparateSupported=flag; }
            bool isBlendFuncSeparateSupported() const { return _isBlendFuncSeparateSupported; }

            void glBlendFuncSeparate(GLenum sfactorRGB,
                                     GLenum dfactorRGB,
                                     GLenum sfactorAlpha,
                                     GLenum dfactorAlpha) const;

        protected:

            ~Extensions() {}// IE���ܴ���


            typedef void (GL_APIENTRY * GLBlendFuncSeparateProc) (GLenum sfactorRGB,
                                                               GLenum dfactorRGB,
                                                               GLenum sfactorAlpha,
                                                               GLenum dfactorAlpha);
            bool                    _isBlendFuncSeparateSupported;
            GLBlendFuncSeparateProc _glBlendFuncSeparate;

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

        virtual ~BlendFunc();

        GLenum _source_factor;
        GLenum _destination_factor;
        GLenum _source_factor_alpha;
        GLenum _destination_factor_alpha;
};

}

#endif
