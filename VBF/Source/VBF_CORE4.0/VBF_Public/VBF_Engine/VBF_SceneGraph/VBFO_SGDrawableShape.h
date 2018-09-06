#ifndef OSG_SHAPEDRAWABLE
#define OSG_SHAPEDRAWABLE 1

#include <VBF_Engine/VBF_SceneGraph/VBFO_SGDrawable.h>
#include <Types/Vec2>
#include <Types/Vec3>
#include <Types/Vec4>
#include <VBF_Engine/VBF_SceneGraph/Array>
#include <VBF_Engine/VBF_SceneGraph/PrimitiveSet>

namespace osg {

// 网格化类：主要作用是设置预定义几何体对象的精细程度，精细程度越高，表示其细分越详细，但对于不同的几何体对象的作用是不一样的
class TessellationHints : public CVBF_Object
{
    public:
    TessellationHints():
        _TessellationMode(USE_SHAPE_DEFAULTS),
        _detailRatio(1.0f),
        _targetNumFaces(100),
        _createFrontFace(true),
        _createBackFace(false),
        _createNormals(true),
        _createTextureCoords(false),
        _createTop(true),
        _createBody(true),
        _createBottom(true) {}


    TessellationHints(const TessellationHints& tess, const CopyOp& copyop=CopyOp::SHALLOW_COPY): CVBF_Object(tess,copyop),
        _TessellationMode(tess._TessellationMode),
        _detailRatio(tess._detailRatio),
        _targetNumFaces(tess._targetNumFaces),
        _createFrontFace(tess._createFrontFace),
        _createBackFace(tess._createBackFace),
        _createNormals(tess._createNormals),
        _createTextureCoords(tess._createTextureCoords),
        _createTop(tess._createTop),
        _createBody(tess._createBody),
        _createBottom(tess._createBottom) {}

        META_Object(osg,TessellationHints);


        enum TessellationMode
        {
            USE_SHAPE_DEFAULTS,
            USE_TARGET_NUM_FACES
        };

        inline void setTessellationMode(TessellationMode mode) { _TessellationMode=mode; }
        inline TessellationMode getTessellationMode() const { return _TessellationMode; }

        inline void setDetailRatio(float ratio) { _detailRatio = ratio; }
        inline float getDetailRatio() const { return _detailRatio; }

        inline void setTargetNumFaces(unsigned int target) { _targetNumFaces=target; }
        inline unsigned int getTargetNumFaces() const { return _targetNumFaces; }

        inline void setCreateFrontFace(bool on) { _createFrontFace=on; }
        inline bool getCreateFrontFace() const { return _createFrontFace; }

        inline void setCreateBackFace(bool on) { _createBackFace=on; }
        inline bool getCreateBackFace() const { return _createBackFace; }

        inline void setCreateNormals(bool on) { _createNormals=on; }
        inline bool getCreateNormals() const { return _createNormals; }

        inline void setCreateTextureCoords(bool on) { _createTextureCoords=on; }
        inline bool getCreateTextureCoords() const { return _createTextureCoords; }

        inline void setCreateTop(bool on) { _createTop=on; }
        inline bool getCreateTop() const { return _createTop; }

        inline void setCreateBody(bool on) { _createBody=on; }
        inline bool getCreateBody() const { return _createBody; }

        inline void setCreateBottom(bool on) { _createBottom=on; }
        inline bool getCreateBottom() const { return _createBottom; }

    protected:

        ~TessellationHints() {}


        TessellationMode _TessellationMode;

        float           _detailRatio;
        unsigned int     _targetNumFaces;

        bool             _createFrontFace;
        bool             _createBackFace;
        bool             _createNormals;
        bool             _createTextureCoords;

        bool             _createTop;
        bool             _createBody;
        bool             _createBottom;

};


/** Allow the use of <tt>Shape</tt>s as <tt>Drawable</tt>s, so that they can
 *  be rendered with reduced effort. The implementation of \c ShapeDrawable is
 *  not geared to efficiency; it's better to think of it as a convenience to
 *  render <tt>Shape</tt>s easily (perhaps for test or debugging purposes) than
 *  as the right way to render basic shapes in some efficiency-critical section
 *  of code.
 */
class OSG_EXPORT ShapeDrawable : public Drawable
{
    public:

        ShapeDrawable();

        ShapeDrawable(Shape* shape, TessellationHints* hints=0);

        /** Copy constructor using CopyOp to manage deep vs shallow copy.*/
        ShapeDrawable(const ShapeDrawable& pg,const CopyOp& copyop=CopyOp::SHALLOW_COPY);

        virtual CVBF_Object* cloneType() const { return new ShapeDrawable(); }
        virtual CVBF_Object* clone(const CopyOp& copyop) const { return new ShapeDrawable(*this,copyop); }
        virtual bool isSameKindAs(const CVBF_Object* obj) const { return dynamic_cast<const ShapeDrawable*>(obj)!=NULL; }
        virtual const char* libraryName() const { return "osg"; }
        virtual const char* className() const { return "ShapeDrawable"; }

        /** Set the color of the shape.*/
        void setColor(const Vec4& color);

        /** Get the color of the shape.*/
        const Vec4& getColor() const { return _color; }

        void setTessellationHints(TessellationHints* hints);

        TessellationHints* getTessellationHints() { return _tessellationHints.get(); }
        const TessellationHints* getTessellationHints() const { return _tessellationHints.get(); }



        /** Draw ShapeDrawable directly ignoring an OpenGL display list which
          * could be attached.  This is the internal draw method which does the
          * drawing itself, and is the method to override when deriving from
          * ShapeDrawable for user-drawn objects.
        */
        virtual void drawImplementation(RenderInfo& renderInfo) const;

        /* Not all virtual overloads of these methods are overridden in this class, so
           bring the base class implementation in to avoid hiding the non-used ones. */
        using Drawable::supports;
        using Drawable::accept;

        /** Return false, osg::ShapeDrawable does not support accept(AttributeFunctor&).*/
        virtual bool supports(const AttributeFunctor&) const { return false; }

        /** Return true, osg::ShapeDrawable does support accept(Drawable::ConstAttributeFunctor&).*/
        virtual bool supports(const Drawable::ConstAttributeFunctor&) const { return true; }

        /** Accept a Drawable::ConstAttributeFunctor and call its methods to tell it about the internal attributes that this Drawable has.*/
        virtual void accept(Drawable::ConstAttributeFunctor& af) const;

        /** Return true, osg::ShapeDrawable does support accept(PrimitiveFunctor&) .*/
        virtual bool supports(const PrimitiveFunctor&) const { return true; }

        /** Accept a PrimitiveFunctor and call its methods to tell it about the internal primitives that this Drawable has.*/
        virtual void accept(PrimitiveFunctor& pf) const;

        virtual BoundingBox computeBound() const;

    protected:

        ShapeDrawable& operator = (const ShapeDrawable&) { return *this;}

        virtual ~ShapeDrawable();

        Vec4 _color;

        ref_ptr<TessellationHints> _tessellationHints;

};


}

#endif
