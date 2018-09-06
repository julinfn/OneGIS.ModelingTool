/* -*-c++-*- OpenSceneGraph - Copyright (C) 1998-2006 Robert Osfield 
 *
 * This library is open source and may be redistributed and/or modified under  
 * the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or 
 * (at your option) any later version.  The full license is in LICENSE file
 * included with this distribution, and on the openscenegraph.org website.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * OpenSceneGraph Public License for more details.
*/

#ifndef OSG_STENCILTWOSIDED
#define OSG_STENCILTWOSIDED 1

#include <VBF_Engine/VBF_SceneGraph/VBFO_StateAttrStencil.h>

namespace osg {

#ifndef GL_STENCIL_TEST_TWO_SIDE
#define GL_STENCIL_TEST_TWO_SIDE 0x8910
#endif

/** Provides OpenGL two sided stencil functionality, also known as separate stencil.
 *  It enables to specify different stencil function for front and back facing polygons.
 *  Two sided stenciling is used usually to eliminate the need of two rendering passes
 *  when using standard stenciling functions. See also \sa osg::Stencil.
 *
 *  Two sided stenciling is available since OpenGL 2.0. It is also supported by
 *  EXT_stencil_two_side extension especially on Nvidia cards.
 *  Another extension introduced by ATI is ATI_separate_stencil. However, ATI's extension
 *  is limited to have reference and mask value the same for both faces.
 *  ATI's extension is currently not supported by the current implementation.
 *
 *  osg::StencilTwoSided does nothing if OpenGL 2.0 or EXT_stencil_two_side are not available.
*/
class OSG_EXPORT StencilTwoSided : public StateAttribute
{
    public :


        StencilTwoSided();

        /** Copy constructor using CopyOp to manage deep vs shallow copy.*/
        StencilTwoSided(const StencilTwoSided& stencil,const CopyOp& copyop=CopyOp::SHALLOW_COPY);

        META_StateAttribute(osg, StencilTwoSided, STENCIL);

        /** return -1 if *this < *rhs, 0 if *this==*rhs, 1 if *this>*rhs.*/
        virtual int compare(const StateAttribute& sa) const;

        virtual bool getModeUsage(StateAttribute::ModeUsage& usage) const
        {
            usage.usesMode(GL_STENCIL_TEST);
            return true;
        }

        enum Face
        {
            FRONT = 0,
            BACK = 1
        };

        enum Function
        {
            NEVER = GL_NEVER,
            LESS = GL_LESS,
            EQUAL = GL_EQUAL,
            LEQUAL = GL_LEQUAL,
            GREATER = GL_GREATER,
            NOTEQUAL = GL_NOTEQUAL,
            GEQUAL = GL_GEQUAL,
            ALWAYS = GL_ALWAYS
        };

        inline void setFunction(Face face, Function func,int ref,unsigned int mask)
        {
            _func[face] = func;
            _funcRef[face] = ref;
            _funcMask[face] = mask;
        }

        inline void setFunction(Face face, Function func) { _func[face] = func; }
        inline Function getFunction(Face face) const { return _func[face]; }

        inline void setFunctionRef(Face face, int ref) { _funcRef[face]=ref; }
        inline int getFunctionRef(Face face) const { return _funcRef[face]; }

        inline void setFunctionMask(Face face, unsigned int mask) { _funcMask[face]=mask; }
        inline unsigned int getFunctionMask(Face face) const { return _funcMask[face]; }


        enum Operation
        {
            KEEP = GL_KEEP,
            ZERO = GL_ZERO,
            REPLACE = GL_REPLACE,
            INCR = GL_INCR,
            DECR = GL_DECR,
            INVERT = GL_INVERT,
            INCR_WRAP = GL_INCR_WRAP,
            DECR_WRAP = GL_DECR_WRAP
        };

        /** set the operations to apply when the various stencil and depth 
          * tests fail or pass.  First parameter is to control the operation
          * when the stencil test fails.  The second parameter is to control the
          * operation when the stencil test passes, but depth test fails. The
          * third parameter controls the operation when both the stencil test
          * and depth pass.  Ordering of parameter is the same as if using
          * glStencilOp(,,).*/
        inline void setOperation(Face face, Operation sfail, Operation zfail, Operation zpass)
        {
            _sfail[face] = sfail;
            _zfail[face] = zfail;
            _zpass[face] = zpass;
        }

        /** set the operation when the stencil test fails.*/
        inline void setStencilFailOperation(Face face, Operation sfail) { _sfail[face] = sfail; }

        /** get the operation when the stencil test fails.*/
        inline Operation getStencilFailOperation(Face face) const { return _sfail[face]; }

        /** set the operation when the stencil test passes but the depth test fails.*/
        inline void setStencilPassAndDepthFailOperation(Face face, Operation zfail) { _zfail[face]=zfail; }

        /** get the operation when the stencil test passes but the depth test fails.*/
        inline Operation getStencilPassAndDepthFailOperation(Face face) const { return _zfail[face]; }

        /** set the operation when both the stencil test and the depth test pass.*/
        inline void setStencilPassAndDepthPassOperation(Face face, Operation zpass) { _zpass[face]=zpass; }

        /** get the operation when both the stencil test and the depth test pass.*/
        inline Operation getStencilPassAndDepthPassOperation(Face face) const { return _zpass[face]; }


        inline void setWriteMask(Face face, unsigned int mask) { _writeMask[face] = mask; }

        inline unsigned int getWriteMask(Face face) const { return _writeMask[face]; }


        virtual void apply(State& state) const;

    public:

        /** Extensions class which encapsulates the querying of extensions and
          * associated function pointers, and provide convenience wrappers to 
          * check for the extensions or use the associated functions.
        */
        class OSG_EXPORT Extensions : public ::CVBF_Referenced
        {
            public:
                Extensions(unsigned int contextID);

                Extensions(const Extensions& rhs);

                void lowestCommonDenominator(const Extensions& rhs);

                void setupGLExtensions(unsigned int contextID);

                void setStencilTwoSidedSupported(bool flag) { _isStencilTwoSidedSupported=flag; }
                bool isStencilTwoSidedSupported() const { return _isStencilTwoSidedSupported; }
                void setOpenGL20Supported(bool flag) { _isOpenGL20Supported=flag; }
                bool isOpenGL20Supported() const { return _isOpenGL20Supported; }
                void setSeparateStencilSupported(bool flag) { _isSeparateStencilSupported=flag; }
                bool isSeparateStencilSupported() const { return _isSeparateStencilSupported; }

                void glActiveStencilFace(GLenum face) const;
                void glStencilOpSeparate(GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass) const;
                void glStencilMaskSeparate(GLenum face, GLuint mask) const;
                void glStencilFuncSeparate(GLenum face, GLenum func, GLint ref, GLuint mask) const;
                void glStencilFuncSeparateATI(GLenum frontfunc, GLenum backfunc, GLint ref, GLuint mask) const;

            protected:

                ~Extensions() {}// IE���ܴ���

                bool _isStencilTwoSidedSupported;
                bool _isOpenGL20Supported;
                bool _isSeparateStencilSupported;

                typedef void (GL_APIENTRY * ActiveStencilFaceProc) (GLenum);
                typedef void (GL_APIENTRY * StencilOpSeparate) (GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass);
                typedef void (GL_APIENTRY * StencilMaskSeparate) (GLenum face, GLuint mask);
                typedef void (GL_APIENTRY * StencilFuncSeparate) (GLenum face, GLenum func, GLint ref, GLuint mask);
                typedef void (GL_APIENTRY * StencilFuncSeparateATI) (GLenum frontfunc, GLenum backfunc, GLint ref, GLuint mask);

                ActiveStencilFaceProc _glActiveStencilFace;
                StencilOpSeparate _glStencilOpSeparate;
                StencilMaskSeparate _glStencilMaskSeparate;
                StencilFuncSeparate _glStencilFuncSeparate;
                StencilFuncSeparate _glStencilFuncSeparateATI;
        };

        /** Function to call to get the extension of a specified context.
          * If the Extension object for that context has not yet been created
          * and the 'createIfNotInitalized' flag been set to false then returns NULL.
          * If 'createIfNotInitalized' is true then the Extensions object is
          * automatically created. However, in this case the extension object
          * will only be created with the graphics context associated with ContextID.
        */
        static Extensions* getExtensions(unsigned int contextID,bool createIfNotInitalized);

        /** The setExtensions method allows users to override the extensions across graphics contexts.
          * Typically used when you have different extensions supported across graphics pipes
          * but need to ensure that they all use the same low common denominator extensions.
        */
        static void setExtensions(unsigned int contextID,Extensions* extensions);


    protected:

        virtual ~StencilTwoSided();

        Function            _func[2];
        int                 _funcRef[2];
        unsigned int        _funcMask[2];

        Operation           _sfail[2];
        Operation           _zfail[2];
        Operation           _zpass[2];

        unsigned int        _writeMask[2];

};

}

#endif
