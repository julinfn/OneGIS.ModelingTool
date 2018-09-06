#ifndef OSG_BLENDCOLOR
#define OSG_BLENDCOLOR 1

#include <VBF_Engine/VBF_SceneGraph/VBF_GL.h>
#include <VBF_Engine/VBF_SceneGraph/VBFO_StateAttribute.h>
#include <VBF_Base/ref_ptr>
#include <Types/Vec4>



namespace osg {

/** Encapsulates OpenGL blend/transparency state. */
class OSG_EXPORT BlendColor : public StateAttribute
{
    public :

        BlendColor();
        
        BlendColor(const osg::Vec4& constantColor);
        
        /** Copy constructor using CopyOp to manage deep vs shallow copy.*/
        BlendColor(const BlendColor& trans,const CopyOp& copyop=CopyOp::SHALLOW_COPY):
            StateAttribute(trans,copyop),
            _constantColor(trans._constantColor) {}

        META_StateAttribute(osg, BlendColor,BLENDCOLOR);
        
        /** Return -1 if *this < *rhs, 0 if *this==*rhs, 1 if *this>*rhs. */
        virtual int compare(const StateAttribute& sa) const
        {
            // Check for equal types, then create the rhs variable
            // used by the COMPARE_StateAttribute_Parameter macros below.
            COMPARE_StateAttribute_Types(BlendColor,sa)

            // Compare each parameter in turn against the rhs.
            COMPARE_StateAttribute_Parameter(_constantColor)

            return 0; // Passed all the above comparison macros, so must be equal.
        }

        virtual bool getModeUsage(StateAttribute::ModeUsage& usage) const
        {
            usage.usesMode(GL_BLEND);
            return true;
        }

        void setConstantColor(const osg::Vec4& color) { _constantColor = color; }

        inline osg::Vec4& getConstantColor() { return _constantColor; }

        inline const osg::Vec4& getConstantColor() const { return _constantColor; }

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

                void setBlendColorSupported(bool flag) { _isBlendColorSupported=flag; }
                bool isBlendColorSupported() const { return _isBlendColorSupported; }
                
                void glBlendColor(GLclampf red , GLclampf green , GLclampf blue , GLclampf alpha) const;

            protected:

                ~Extensions() {}// IE���ܴ���
                

                typedef void (GL_APIENTRY * GLBlendColorProc) (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
                bool                _isBlendColorSupported;
                GLBlendColorProc    _glBlendColor;

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

        virtual ~BlendColor();

        osg::Vec4 _constantColor;
};

}

#endif
