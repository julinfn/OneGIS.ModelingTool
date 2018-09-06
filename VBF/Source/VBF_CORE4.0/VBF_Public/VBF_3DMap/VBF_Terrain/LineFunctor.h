#ifndef OSGEARTH_LINEFUNCTOR
#define OSGEARTH_LINEFUNCTOR 1

#include <VBF_3DMap/VBF_Terrain/Common.h>
#include <VBF_Engine/VBF_SceneGraph/PrimitiveSet>
#include <Types/Vec2>
#include <Types/Vec3>
#include <Types/Vec4>

namespace osgEarth
{
    /**
     * This is basically the same thing as osg::TriangleFunctor, but for lines.
     */
    template<class T>
    class LineFunctor : public osg::PrimitiveFunctor, public T
    {
    public:

        LineFunctor()
        {
            _vertexArraySize=0;
            _vertexArrayPtr=0;
            _modeCache=0;
            _treatVertexDataAsTemporary=false;
        }

        virtual ~LineFunctor() {}

        void setTreatVertexDataAsTemporary(bool treatVertexDataAsTemporary) { _treatVertexDataAsTemporary=treatVertexDataAsTemporary; }
        bool getTreatVertexDataAsTemporary() const { return _treatVertexDataAsTemporary; }


        virtual void setVertexArray(unsigned int count,const osg::Vec3* vertices)
        {
            _vertexArraySize = count;
            _vertexArrayPtr = vertices;
        }

        virtual void setVertexArray(unsigned int,const osg::Vec2*) { }
        virtual void setVertexArray(unsigned int,const osg::Vec4*) { }
        virtual void setVertexArray(unsigned int,const osg::Vec2d*) { }
        virtual void setVertexArray(unsigned int,const osg::Vec3d*) { }
        virtual void setVertexArray(unsigned int,const osg::Vec4d*) { }

        virtual void drawArrays(GLenum mode,GLint first,GLsizei count)
        {
            if (_vertexArrayPtr==0 || count==0) return;

            switch(mode)
            {            
            case(GL_LINES):
                {
                    const osg::Vec3* vlast = &_vertexArrayPtr[first+count];
                    for(const osg::Vec3* vptr = &_vertexArrayPtr[first]; vptr<vlast; vptr+=2)
                        this->operator()( *(vptr), *(vptr+1), _treatVertexDataAsTemporary );
                }
                break;

            case(GL_LINE_STRIP):
                {
                    const osg::Vec3* vlast = &_vertexArrayPtr[first+count-1];
                    for(const osg::Vec3* vptr = &_vertexArrayPtr[first]; vptr<vlast; vptr++)
                        this->operator()( *(vptr), *(vptr+1), _treatVertexDataAsTemporary );
                }
                break;

            case(GL_LINE_LOOP):
                {
                    const osg::Vec3* vlast = &_vertexArrayPtr[first+count-1];
                    const osg::Vec3* vptr;
                    for(vptr = &_vertexArrayPtr[first]; vptr<vlast; vptr++)
                        this->operator()( *(vptr), *(vptr+1), _treatVertexDataAsTemporary );
                    if ( count >= 2 )
                        this->operator()( *vptr, _vertexArrayPtr[first], _treatVertexDataAsTemporary );
                }
                break;

            case(GL_TRIANGLES):
            {
                const osg::Vec3* vlast = &_vertexArrayPtr[first+count];
                for(const osg::Vec3* vptr=&_vertexArrayPtr[first];vptr<vlast;vptr+=3)
                {
                    this->operator()(*(vptr),*(vptr+1),_treatVertexDataAsTemporary);
                    this->operator()(*(vptr+1),*(vptr+2),_treatVertexDataAsTemporary);
                    this->operator()(*(vptr+2),*(vptr),_treatVertexDataAsTemporary);
                }
                break;
            }
            case(GL_TRIANGLE_STRIP):
            {
                const osg::Vec3* vptr = &_vertexArrayPtr[first];
                for(GLsizei i=2;i<count;++i,++vptr)
                {
                    if ((i%2)) { 
                        this->operator()(*(vptr),*(vptr+2),_treatVertexDataAsTemporary);
                        this->operator()(*(vptr+2),*(vptr+1),_treatVertexDataAsTemporary);
                        this->operator()(*(vptr+1),*(vptr),_treatVertexDataAsTemporary);
                    }
                    else {
                        this->operator()(*(vptr),*(vptr+1),_treatVertexDataAsTemporary);
                        this->operator()(*(vptr+1),*(vptr+2),_treatVertexDataAsTemporary);
                        this->operator()(*(vptr+2),*(vptr),_treatVertexDataAsTemporary);
                    }
                }
                break;
            }
            case(GL_QUADS):
            {
                const osg::Vec3* vptr = &_vertexArrayPtr[first];
                for(GLsizei i=3;i<count;i+=4,vptr+=4)
                {
                    this->operator()(*(vptr),*(vptr+1),_treatVertexDataAsTemporary);
                    this->operator()(*(vptr+1),*(vptr+2),_treatVertexDataAsTemporary);
                    this->operator()(*(vptr+2),*(vptr),_treatVertexDataAsTemporary);
                    this->operator()(*(vptr),*(vptr+2),_treatVertexDataAsTemporary);
                    this->operator()(*(vptr+2),*(vptr+3),_treatVertexDataAsTemporary);
                    this->operator()(*(vptr+3),*(vptr),_treatVertexDataAsTemporary);
                }
                break;
            }
            case(GL_QUAD_STRIP):
            {
                const osg::Vec3* vptr = &_vertexArrayPtr[first];
                for(GLsizei i=3;i<count;i+=2,vptr+=2)
                {
                    this->operator()(*(vptr),*(vptr+1),_treatVertexDataAsTemporary);
                    this->operator()(*(vptr+1),*(vptr+2),_treatVertexDataAsTemporary);
                    this->operator()(*(vptr+2),*(vptr),_treatVertexDataAsTemporary);
                    this->operator()(*(vptr+1),*(vptr+3),_treatVertexDataAsTemporary);
                    this->operator()(*(vptr+3),*(vptr+2),_treatVertexDataAsTemporary);
                    this->operator()(*(vptr+2),*(vptr+1),_treatVertexDataAsTemporary);
                }
                break;
            }
            case(GL_POLYGON): // treat polygons as GL_TRIANGLE_FAN
            case(GL_TRIANGLE_FAN):
            {
                const osg::Vec3* vfirst = &_vertexArrayPtr[first];
                const osg::Vec3* vptr = vfirst+1;
                for(GLsizei i=2;i<count;++i,++vptr)
                {
                    this->operator()(*(vfirst),*(vptr),_treatVertexDataAsTemporary);
                    this->operator()(*(vptr),*(vptr+1),_treatVertexDataAsTemporary);
                    this->operator()(*(vptr+1),*(vfirst),_treatVertexDataAsTemporary);
                }
                break;
            }

#if 0
            case(GL_TRIANGLES):
            case(GL_TRIANGLE_STRIP):
            case(GL_QUADS):
            case(GL_QUAD_STRIP):
            case(GL_POLYGON):
            case(GL_TRIANGLE_FAN):
#endif
            case(GL_POINTS):
            default:
                // can't be converted into to line segments.
                break;
            }
        }

        virtual void drawElements(GLenum mode,GLsizei count,const GLubyte* indicies)
        {
            if (indicies==0 || count==0) return;

            typedef const GLubyte* IndexPointer;

            switch(mode)
            {
            case(GL_LINES):
                {
                    IndexPointer ilast = &indicies[count];
                    for(IndexPointer iptr=indicies; iptr<ilast; iptr+=2)
                        this->operator()( _vertexArrayPtr[*iptr], _vertexArrayPtr[*(iptr+1)], _treatVertexDataAsTemporary );
                }
                break;

            case(GL_LINE_STRIP):
                {
                    IndexPointer ilast = &indicies[count-1];
                    for(IndexPointer iptr=indicies; iptr<ilast; iptr++)
                        this->operator()( _vertexArrayPtr[*iptr], _vertexArrayPtr[*(iptr+1)], _treatVertexDataAsTemporary );
                }
                break;

            case(GL_LINE_LOOP):
                {
                    IndexPointer ilast = &indicies[count-1];
                    IndexPointer iptr;
                    for(iptr=indicies; iptr<ilast; iptr++)
                        this->operator()( _vertexArrayPtr[*iptr], _vertexArrayPtr[*(iptr+1)], _treatVertexDataAsTemporary );
                    if (count >= 2)
                        this->operator()( _vertexArrayPtr[*iptr], _vertexArrayPtr[indicies[0]], _treatVertexDataAsTemporary );
                }
                break;

            case(GL_TRIANGLES):
            {
                IndexPointer ilast = &indicies[count];
                for(IndexPointer  iptr=indicies;iptr<ilast;iptr+=3) {
                    this->operator()(_vertexArrayPtr[*(iptr)],_vertexArrayPtr[*(iptr+1)],_treatVertexDataAsTemporary);
                    this->operator()(_vertexArrayPtr[*(iptr+1)],_vertexArrayPtr[*(iptr+2)],_treatVertexDataAsTemporary);
                    this->operator()(_vertexArrayPtr[*(iptr+2)],_vertexArrayPtr[*(iptr)],_treatVertexDataAsTemporary);
                }
                break;
            }
            case(GL_TRIANGLE_STRIP):
            {
                IndexPointer iptr = indicies;
                for(GLsizei i=2;i<count;++i,++iptr)
                {
                    if ((i%2)) {
                        this->operator()(_vertexArrayPtr[*(iptr)],_vertexArrayPtr[*(iptr+2)],_treatVertexDataAsTemporary);
                        this->operator()(_vertexArrayPtr[*(iptr+2)],_vertexArrayPtr[*(iptr+1)],_treatVertexDataAsTemporary);
                        this->operator()(_vertexArrayPtr[*(iptr+1)],_vertexArrayPtr[*(iptr)],_treatVertexDataAsTemporary);
                    }
                    else {
                        this->operator()(_vertexArrayPtr[*(iptr)],_vertexArrayPtr[*(iptr+1)],_treatVertexDataAsTemporary);
                        this->operator()(_vertexArrayPtr[*(iptr+1)],_vertexArrayPtr[*(iptr+2)],_treatVertexDataAsTemporary);
                        this->operator()(_vertexArrayPtr[*(iptr+2)],_vertexArrayPtr[*(iptr)],_treatVertexDataAsTemporary);
                    }
                }
                break;
            }
            case(GL_QUADS):
            {
                IndexPointer iptr = indicies;
                for(GLsizei i=3;i<count;i+=4,iptr+=4)
                {
                    this->operator()(_vertexArrayPtr[*(iptr)],_vertexArrayPtr[*(iptr+1)],_treatVertexDataAsTemporary);
                    this->operator()(_vertexArrayPtr[*(iptr+1)],_vertexArrayPtr[*(iptr+2)],_treatVertexDataAsTemporary);
                    this->operator()(_vertexArrayPtr[*(iptr+2)],_vertexArrayPtr[*(iptr)],_treatVertexDataAsTemporary);

                    this->operator()(_vertexArrayPtr[*(iptr)],_vertexArrayPtr[*(iptr+2)],_treatVertexDataAsTemporary);
                    this->operator()(_vertexArrayPtr[*(iptr+2)],_vertexArrayPtr[*(iptr+3)],_treatVertexDataAsTemporary);
                    this->operator()(_vertexArrayPtr[*(iptr+3)],_vertexArrayPtr[*(iptr)],_treatVertexDataAsTemporary);
                }
                break;
            }
            case(GL_QUAD_STRIP):
            {
                IndexPointer iptr = indicies;
                for(GLsizei i=3;i<count;i+=2,iptr+=2)
                {
                    this->operator()(_vertexArrayPtr[*(iptr)],_vertexArrayPtr[*(iptr+1)],_treatVertexDataAsTemporary);
                    this->operator()(_vertexArrayPtr[*(iptr+1)],_vertexArrayPtr[*(iptr+2)],_treatVertexDataAsTemporary);
                    this->operator()(_vertexArrayPtr[*(iptr+2)],_vertexArrayPtr[*(iptr)],_treatVertexDataAsTemporary);
                    
                    this->operator()(_vertexArrayPtr[*(iptr+1)],_vertexArrayPtr[*(iptr+3)],_treatVertexDataAsTemporary);
                    this->operator()(_vertexArrayPtr[*(iptr+3)],_vertexArrayPtr[*(iptr+2)],_treatVertexDataAsTemporary);
                    this->operator()(_vertexArrayPtr[*(iptr+2)],_vertexArrayPtr[*(iptr+1)],_treatVertexDataAsTemporary);
                }
                break;
            }
            case(GL_POLYGON): // treat polygons as GL_TRIANGLE_FAN
            case(GL_TRIANGLE_FAN):
            {
                IndexPointer iptr = indicies;
                const osg::Vec3& vfirst = _vertexArrayPtr[*iptr];
                ++iptr;
                for(GLsizei i=2;i<count;++i,++iptr)
                {
                    this->operator()(vfirst,_vertexArrayPtr[*(iptr)],_treatVertexDataAsTemporary);
                    this->operator()(_vertexArrayPtr[*(iptr)],_vertexArrayPtr[*(iptr+1)],_treatVertexDataAsTemporary);
                    this->operator()(_vertexArrayPtr[*(iptr+1)],vfirst,_treatVertexDataAsTemporary);
                }
                break;
            }

            case(GL_POINTS):
            default:
                // can't be converted into to lines.
                break;
            }
        }    

        virtual void drawElements(GLenum mode,GLsizei count,const GLushort* indicies)
        {
            if (indicies==0 || count==0) return;

            typedef const GLushort* IndexPointer;

            switch(mode)
            {
            case(GL_LINES):
                {
                    IndexPointer ilast = &indicies[count];
                    for(IndexPointer iptr=indicies; iptr<ilast; iptr+=2)
                        this->operator()( _vertexArrayPtr[*iptr], _vertexArrayPtr[*(iptr+1)], _treatVertexDataAsTemporary );
                }
                break;

            case(GL_LINE_STRIP):
                {
                    IndexPointer ilast = &indicies[count-1];
                    for(IndexPointer iptr=indicies; iptr<ilast; iptr++)
                        this->operator()( _vertexArrayPtr[*iptr], _vertexArrayPtr[*(iptr+1)], _treatVertexDataAsTemporary );
                }
                break;

            case(GL_LINE_LOOP):
                {
                    IndexPointer ilast = &indicies[count-1];
                    IndexPointer iptr;
                    for(iptr=indicies; iptr<ilast; iptr++)
                        this->operator()( _vertexArrayPtr[*iptr], _vertexArrayPtr[*(iptr+1)], _treatVertexDataAsTemporary );
                    if (count >= 2)
                        this->operator()( _vertexArrayPtr[*iptr], _vertexArrayPtr[indicies[0]], _treatVertexDataAsTemporary );
                }
                break;

            case(GL_TRIANGLES):
            {
                IndexPointer ilast = &indicies[count];
                for(IndexPointer  iptr=indicies;iptr<ilast;iptr+=3) {
                    this->operator()(_vertexArrayPtr[*(iptr)],_vertexArrayPtr[*(iptr+1)],_treatVertexDataAsTemporary);
                    this->operator()(_vertexArrayPtr[*(iptr+1)],_vertexArrayPtr[*(iptr+2)],_treatVertexDataAsTemporary);
                    this->operator()(_vertexArrayPtr[*(iptr+2)],_vertexArrayPtr[*(iptr)],_treatVertexDataAsTemporary);
                }
                break;
            }
            case(GL_TRIANGLE_STRIP):
            {
                IndexPointer iptr = indicies;
                for(GLsizei i=2;i<count;++i,++iptr)
                {
                    if ((i%2)) {
                        this->operator()(_vertexArrayPtr[*(iptr)],_vertexArrayPtr[*(iptr+2)],_treatVertexDataAsTemporary);
                        this->operator()(_vertexArrayPtr[*(iptr+2)],_vertexArrayPtr[*(iptr+1)],_treatVertexDataAsTemporary);
                        this->operator()(_vertexArrayPtr[*(iptr+1)],_vertexArrayPtr[*(iptr)],_treatVertexDataAsTemporary);
                    }
                    else {
                        this->operator()(_vertexArrayPtr[*(iptr)],_vertexArrayPtr[*(iptr+1)],_treatVertexDataAsTemporary);
                        this->operator()(_vertexArrayPtr[*(iptr+1)],_vertexArrayPtr[*(iptr+2)],_treatVertexDataAsTemporary);
                        this->operator()(_vertexArrayPtr[*(iptr+2)],_vertexArrayPtr[*(iptr)],_treatVertexDataAsTemporary);
                    }
                }
                break;
            }
            case(GL_QUADS):
            {
                IndexPointer iptr = indicies;
                for(GLsizei i=3;i<count;i+=4,iptr+=4)
                {
                    this->operator()(_vertexArrayPtr[*(iptr)],_vertexArrayPtr[*(iptr+1)],_treatVertexDataAsTemporary);
                    this->operator()(_vertexArrayPtr[*(iptr+1)],_vertexArrayPtr[*(iptr+2)],_treatVertexDataAsTemporary);
                    this->operator()(_vertexArrayPtr[*(iptr+2)],_vertexArrayPtr[*(iptr)],_treatVertexDataAsTemporary);

                    this->operator()(_vertexArrayPtr[*(iptr)],_vertexArrayPtr[*(iptr+2)],_treatVertexDataAsTemporary);
                    this->operator()(_vertexArrayPtr[*(iptr+2)],_vertexArrayPtr[*(iptr+3)],_treatVertexDataAsTemporary);
                    this->operator()(_vertexArrayPtr[*(iptr+3)],_vertexArrayPtr[*(iptr)],_treatVertexDataAsTemporary);
                }
                break;
            }
            case(GL_QUAD_STRIP):
            {
                IndexPointer iptr = indicies;
                for(GLsizei i=3;i<count;i+=2,iptr+=2)
                {
                    this->operator()(_vertexArrayPtr[*(iptr)],_vertexArrayPtr[*(iptr+1)],_treatVertexDataAsTemporary);
                    this->operator()(_vertexArrayPtr[*(iptr+1)],_vertexArrayPtr[*(iptr+2)],_treatVertexDataAsTemporary);
                    this->operator()(_vertexArrayPtr[*(iptr+2)],_vertexArrayPtr[*(iptr)],_treatVertexDataAsTemporary);
                    
                    this->operator()(_vertexArrayPtr[*(iptr+1)],_vertexArrayPtr[*(iptr+3)],_treatVertexDataAsTemporary);
                    this->operator()(_vertexArrayPtr[*(iptr+3)],_vertexArrayPtr[*(iptr+2)],_treatVertexDataAsTemporary);
                    this->operator()(_vertexArrayPtr[*(iptr+2)],_vertexArrayPtr[*(iptr+1)],_treatVertexDataAsTemporary);
                }
                break;
            }
            case(GL_POLYGON): // treat polygons as GL_TRIANGLE_FAN
            case(GL_TRIANGLE_FAN):
            {
                IndexPointer iptr = indicies;
                const osg::Vec3& vfirst = _vertexArrayPtr[*iptr];
                ++iptr;
                for(GLsizei i=2;i<count;++i,++iptr)
                {
                    this->operator()(vfirst,_vertexArrayPtr[*(iptr)],_treatVertexDataAsTemporary);
                    this->operator()(_vertexArrayPtr[*(iptr)],_vertexArrayPtr[*(iptr+1)],_treatVertexDataAsTemporary);
                    this->operator()(_vertexArrayPtr[*(iptr+1)],vfirst,_treatVertexDataAsTemporary);
                }
                break;
            }
            case(GL_POINTS):
            default:
                // can't be converted into to lines.
                break;
            }
        }    

        virtual void drawElements(GLenum mode,GLsizei count,const GLuint* indicies)
        {
            if (indicies==0 || count==0) return;

            typedef const GLuint* IndexPointer;

            switch(mode)
            {
            case(GL_LINES):
                {
                    IndexPointer ilast = &indicies[count];
                    for(IndexPointer iptr=indicies; iptr<ilast; iptr+=2)
                        this->operator()( _vertexArrayPtr[*iptr], _vertexArrayPtr[*(iptr+1)], _treatVertexDataAsTemporary );
                }
                break;

            case(GL_LINE_STRIP):
                {
                    IndexPointer ilast = &indicies[count-1];
                    for(IndexPointer iptr=indicies; iptr<ilast; iptr++)
                        this->operator()( _vertexArrayPtr[*iptr], _vertexArrayPtr[*(iptr+1)], _treatVertexDataAsTemporary );
                }
                break;

            case(GL_LINE_LOOP):
                {
                    IndexPointer ilast = &indicies[count-1];
                    IndexPointer iptr;
                    for(iptr=indicies; iptr<ilast; iptr++)
                        this->operator()( _vertexArrayPtr[*iptr], _vertexArrayPtr[*(iptr+1)], _treatVertexDataAsTemporary );
                    if (count >= 2)
                        this->operator()( _vertexArrayPtr[*iptr], _vertexArrayPtr[indicies[0]], _treatVertexDataAsTemporary );
                }
                break;

            case(GL_TRIANGLES):
            {
                IndexPointer ilast = &indicies[count];
                for(IndexPointer  iptr=indicies;iptr<ilast;iptr+=3) {
                    this->operator()(_vertexArrayPtr[*(iptr)],_vertexArrayPtr[*(iptr+1)],_treatVertexDataAsTemporary);
                    this->operator()(_vertexArrayPtr[*(iptr+1)],_vertexArrayPtr[*(iptr+2)],_treatVertexDataAsTemporary);
                    this->operator()(_vertexArrayPtr[*(iptr+2)],_vertexArrayPtr[*(iptr)],_treatVertexDataAsTemporary);
                }
                break;
            }
            case(GL_TRIANGLE_STRIP):
            {
                IndexPointer iptr = indicies;
                for(GLsizei i=2;i<count;++i,++iptr)
                {
                    if ((i%2)) {
                        this->operator()(_vertexArrayPtr[*(iptr)],_vertexArrayPtr[*(iptr+2)],_treatVertexDataAsTemporary);
                        this->operator()(_vertexArrayPtr[*(iptr+2)],_vertexArrayPtr[*(iptr+1)],_treatVertexDataAsTemporary);
                        this->operator()(_vertexArrayPtr[*(iptr+1)],_vertexArrayPtr[*(iptr)],_treatVertexDataAsTemporary);
                    }
                    else {
                        this->operator()(_vertexArrayPtr[*(iptr)],_vertexArrayPtr[*(iptr+1)],_treatVertexDataAsTemporary);
                        this->operator()(_vertexArrayPtr[*(iptr+1)],_vertexArrayPtr[*(iptr+2)],_treatVertexDataAsTemporary);
                        this->operator()(_vertexArrayPtr[*(iptr+2)],_vertexArrayPtr[*(iptr)],_treatVertexDataAsTemporary);
                    }
                }
                break;
            }
            case(GL_QUADS):
            {
                IndexPointer iptr = indicies;
                for(GLsizei i=3;i<count;i+=4,iptr+=4)
                {
                    this->operator()(_vertexArrayPtr[*(iptr)],_vertexArrayPtr[*(iptr+1)],_treatVertexDataAsTemporary);
                    this->operator()(_vertexArrayPtr[*(iptr+1)],_vertexArrayPtr[*(iptr+2)],_treatVertexDataAsTemporary);
                    this->operator()(_vertexArrayPtr[*(iptr+2)],_vertexArrayPtr[*(iptr)],_treatVertexDataAsTemporary);

                    this->operator()(_vertexArrayPtr[*(iptr)],_vertexArrayPtr[*(iptr+2)],_treatVertexDataAsTemporary);
                    this->operator()(_vertexArrayPtr[*(iptr+2)],_vertexArrayPtr[*(iptr+3)],_treatVertexDataAsTemporary);
                    this->operator()(_vertexArrayPtr[*(iptr+3)],_vertexArrayPtr[*(iptr)],_treatVertexDataAsTemporary);
                }
                break;
            }
            case(GL_QUAD_STRIP):
            {
                IndexPointer iptr = indicies;
                for(GLsizei i=3;i<count;i+=2,iptr+=2)
                {
                    this->operator()(_vertexArrayPtr[*(iptr)],_vertexArrayPtr[*(iptr+1)],_treatVertexDataAsTemporary);
                    this->operator()(_vertexArrayPtr[*(iptr+1)],_vertexArrayPtr[*(iptr+2)],_treatVertexDataAsTemporary);
                    this->operator()(_vertexArrayPtr[*(iptr+2)],_vertexArrayPtr[*(iptr)],_treatVertexDataAsTemporary);
                    
                    this->operator()(_vertexArrayPtr[*(iptr+1)],_vertexArrayPtr[*(iptr+3)],_treatVertexDataAsTemporary);
                    this->operator()(_vertexArrayPtr[*(iptr+3)],_vertexArrayPtr[*(iptr+2)],_treatVertexDataAsTemporary);
                    this->operator()(_vertexArrayPtr[*(iptr+2)],_vertexArrayPtr[*(iptr+1)],_treatVertexDataAsTemporary);
                }
                break;
            }
            case(GL_POLYGON): // treat polygons as GL_TRIANGLE_FAN
            case(GL_TRIANGLE_FAN):
            {
                IndexPointer iptr = indicies;
                const osg::Vec3& vfirst = _vertexArrayPtr[*iptr];
                ++iptr;
                for(GLsizei i=2;i<count;++i,++iptr)
                {
                    this->operator()(vfirst,_vertexArrayPtr[*(iptr)],_treatVertexDataAsTemporary);
                    this->operator()(_vertexArrayPtr[*(iptr)],_vertexArrayPtr[*(iptr+1)],_treatVertexDataAsTemporary);
                    this->operator()(_vertexArrayPtr[*(iptr+1)],vfirst,_treatVertexDataAsTemporary);
                }
                break;
            }
            case(GL_POINTS):
            default:
                // can't be converted into to lines.
                break;
            }
        }



        /** Note:
        * begin(..),vertex(..) & end() are convenience methods for adapting
        * non vertex array primitives to vertex array based primitives.
        * This is done to simplify the implementation of primitive functor
        * subclasses - users only need override drawArray and drawElements.
        */
        virtual void begin(GLenum mode)
        {
            _modeCache = mode;
            _vertexCache.clear();
        }

        virtual void vertex(const osg::Vec2& vert) { _vertexCache.push_back(osg::Vec3(vert[0],vert[1],0.0f)); }
        virtual void vertex(const osg::Vec3& vert) { _vertexCache.push_back(vert); }
        virtual void vertex(const osg::Vec4& vert) { _vertexCache.push_back(osg::Vec3(vert[0],vert[1],vert[2])/vert[3]); }
        virtual void vertex(float x,float y) { _vertexCache.push_back(osg::Vec3(x,y,0.0f)); }
        virtual void vertex(float x,float y,float z) { _vertexCache.push_back(osg::Vec3(x,y,z)); }
        virtual void vertex(float x,float y,float z,float w) { _vertexCache.push_back(osg::Vec3(x,y,z)/w); }
        virtual void end()
        {
            if (!_vertexCache.empty())
            {
                setVertexArray(_vertexCache.size(),&_vertexCache.front());
                _treatVertexDataAsTemporary = true;
                drawArrays(_modeCache,0,_vertexCache.size());
            }
        }

    protected:


        unsigned int        _vertexArraySize;
        const osg::Vec3*         _vertexArrayPtr;

        GLenum              _modeCache;
        std::vector<osg::Vec3>   _vertexCache;
        bool                _treatVertexDataAsTemporary;
    };



    
    template<typename T>
    class SimpleIndexFunctor : public osg::PrimitiveIndexFunctor, public T
    {
    public:

        virtual void setVertexArray(unsigned int,const osg::Vec2*) { }
        virtual void setVertexArray(unsigned int ,const osg::Vec3* ) { }
        virtual void setVertexArray(unsigned int,const osg::Vec4* ) { }
        virtual void setVertexArray(unsigned int,const osg::Vec2d*) { }
        virtual void setVertexArray(unsigned int ,const osg::Vec3d* ) { }
        virtual void setVertexArray(unsigned int,const osg::Vec4d* ) { }

        virtual void begin(GLenum mode)
        {
            _modeCache = mode;
            _indexCache.clear();
        }

        virtual void vertex(unsigned int vert)
        {
            _indexCache.push_back(vert);
        }

        virtual void end()
        {
            if (!_indexCache.empty())
            {
                drawElements(_modeCache,_indexCache.size(),&_indexCache.front());
            }
        }


        // simulate triangles
        void operator()(GLuint i0, GLuint i1, GLuint i2)
        {
            T::operator()(i0);
            T::operator()(i1);
            T::operator()(i2);
        }
        void operator()(GLushort i0, GLushort i1, GLushort i2)
        {
            T::operator()(i0);
            T::operator()(i1);
            T::operator()(i2);
        }
        void operator()(GLubyte i0, GLubyte i1, GLubyte i2)
        {
            T::operator()(i0);
            T::operator()(i1);
            T::operator()(i2);
        }

        // simulate lines
        void operator()(GLuint i0, GLuint i1)
        {
            T::operator()(i0);
            T::operator()(i1);
        }
        void operator()(GLushort i0, GLushort i1)
        {
            T::operator()(i0);
            T::operator()(i1);
        }
        void operator()(GLubyte i0, GLubyte i1)
        {
            T::operator()(i0);
            T::operator()(i1);
        }



        virtual void drawArrays(GLenum mode,GLint first,GLsizei count)
        {
            switch(mode)
            {
                case(GL_TRIANGLES):
                {
                    unsigned int pos=first;
                    for(GLsizei i=2;i<count;i+=3,pos+=3)
                    {
                        this->operator()(pos,pos+1,pos+2);
                    }
                    break;
                }
                case(GL_TRIANGLE_STRIP):
                 {
                    unsigned int pos=first;
                    for(GLsizei i=2;i<count;++i,++pos)
                    {
                        if ((i%2)) this->operator()(pos,pos+2,pos+1);
                        else       this->operator()(pos,pos+1,pos+2);
                    }
                    break;
                }
                case(GL_QUADS):
                {
                    unsigned int pos=first;
                    for(GLsizei i=3;i<count;i+=4,pos+=4)
                    {
                        this->operator()(pos,pos+1,pos+2);
                        this->operator()(pos,pos+2,pos+3);
                    }
                    break;
                }
                case(GL_QUAD_STRIP):
                {
                    unsigned int pos=first;
                    for(GLsizei i=3;i<count;i+=2,pos+=2)
                    {
                        this->operator()(pos,pos+1,pos+2);
                        this->operator()(pos+1,pos+3,pos+2);
                    }
                    break;
                }
                case(GL_POLYGON): // treat polygons as GL_TRIANGLE_FAN
                case(GL_TRIANGLE_FAN):
                {
                    unsigned int pos=first+1;
                    for(GLsizei i=2;i<count;++i,++pos)
                    {
                        this->operator()(first,pos,pos+1);
                    }
                    break;
                }
                case(GL_LINES):
                {
                    unsigned pos=first;
                    for(GLsizei i=1;i<count;i+=2,pos+=2)
                    {
                        //this->operator()(pos,pos+1);
                        this->operator()(pos,pos+1);
                    }
                    break;
                }
                case(GL_LINE_STRIP):
                {
                    unsigned pos=first;
                    for(GLsizei i=1;i<count;i+=1,pos+=1)
                    {
                        this->operator()(pos,pos+1);
                    }
                    break;
                }
                case(GL_LINE_LOOP):
                {
                    unsigned pos=first;
                    for(GLsizei i=1; i<count; i+=1, pos+=1)
                    {
                        this->operator()(pos,pos+1);
                    }
                    if (count > 0 )
                    {
                        this->operator()(pos,first);
                    }
                    break;
                }
                case(GL_POINTS):
                default:
                    // can't be converted into to triangles.
                    break;
            }
        }

        virtual void drawElements(GLenum mode,GLsizei count,const GLubyte* indices)
        {
            if (indices==0 || count==0) return;

            typedef GLubyte Index;
            typedef const Index* IndexPointer;

            switch(mode)
            {
                case(GL_TRIANGLES):
                {
                    IndexPointer ilast = &indices[count];
                    for(IndexPointer  iptr=indices;iptr<ilast;iptr+=3)
                        this->operator()(*iptr,*(iptr+1),*(iptr+2));
                    break;
                }
                case(GL_TRIANGLE_STRIP):
                {
                    IndexPointer iptr = indices;
                    for(GLsizei i=2;i<count;++i,++iptr)
                    {
                        if ((i%2)) this->operator()(*(iptr),*(iptr+2),*(iptr+1));
                        else       this->operator()(*(iptr),*(iptr+1),*(iptr+2));
                    }
                    break;
                }
                case(GL_QUADS):
                {
                    IndexPointer iptr = indices;
                    for(GLsizei i=3;i<count;i+=4,iptr+=4)
                    {
                        this->operator()(*(iptr),*(iptr+1),*(iptr+2));
                        this->operator()(*(iptr),*(iptr+2),*(iptr+3));
                    }
                    break;
                }
                case(GL_QUAD_STRIP):
                {
                    IndexPointer iptr = indices;
                    for(GLsizei i=3;i<count;i+=2,iptr+=2)
                    {
                        this->operator()(*(iptr),*(iptr+1),*(iptr+2));
                        this->operator()(*(iptr+1),*(iptr+3),*(iptr+2));
                    }
                    break;
                }
                case(GL_POLYGON): // treat polygons as GL_TRIANGLE_FAN
                case(GL_TRIANGLE_FAN):
                {
                    IndexPointer iptr = indices;
                    Index first = *iptr;
                    ++iptr;
                    for(GLsizei i=2;i<count;++i,++iptr)
                    {
                        this->operator()(first,*(iptr),*(iptr+1));
                    }
                    break;
                }
                case(GL_LINES):
                {
                    IndexPointer iptr = indices;
                    Index first = *iptr;
                    for(GLsizei i=1;i<count;i+=2,iptr+=2)
                    {
                        this->operator()(*iptr,*(iptr+1));
                    }
                    break;
                }
                case(GL_LINE_STRIP):
                {
                    IndexPointer iptr = indices;
                    Index first = *iptr;
                    for(GLsizei i=1;i<count;i+=1,++iptr)
                    {
                        this->operator()(*iptr,*(iptr+1));
                    }
                    break;
                }
                case(GL_LINE_LOOP):
                {
                    IndexPointer iptr = indices;
                    Index first = *iptr;
                    for(GLsizei i=1;i<count;i+=1,++iptr)
                    {
                        this->operator()(*iptr,*(iptr+1));
                    }
                    this->operator()(*iptr,first);
                    break;
                }

                case(GL_POINTS):
                default:
                    // can't be converted into to triangles.
                    break;
            }
        }    

        virtual void drawElements(GLenum mode,GLsizei count,const GLushort* indices)
        {
            if (indices==0 || count==0) return;

            typedef GLushort Index;
            typedef const Index* IndexPointer;
        

            switch(mode)
            {
                case(GL_TRIANGLES):
                {
                    IndexPointer ilast = &indices[count];
                    for(IndexPointer  iptr=indices;iptr<ilast;iptr+=3)
                        this->operator()(*iptr,*(iptr+1),*(iptr+2));
                    break;
                }
                case(GL_TRIANGLE_STRIP):
                {
                    IndexPointer iptr = indices;
                    for(GLsizei i=2;i<count;++i,++iptr)
                    {
                        if ((i%2)) this->operator()(*(iptr),*(iptr+2),*(iptr+1));
                        else       this->operator()(*(iptr),*(iptr+1),*(iptr+2));
                    }
                    break;
                }
                case(GL_QUADS):
                {
                    IndexPointer iptr = indices;
                    for(GLsizei i=3;i<count;i+=4,iptr+=4)
                    {
                        this->operator()(*(iptr),*(iptr+1),*(iptr+2));
                        this->operator()(*(iptr),*(iptr+2),*(iptr+3));
                    }
                    break;
                }
                case(GL_QUAD_STRIP):
                {
                    IndexPointer iptr = indices;
                    for(GLsizei i=3;i<count;i+=2,iptr+=2)
                    {
                        this->operator()(*(iptr),*(iptr+1),*(iptr+2));
                        this->operator()(*(iptr+1),*(iptr+3),*(iptr+2));
                    }
                    break;
                }
                case(GL_POLYGON): // treat polygons as GL_TRIANGLE_FAN
                case(GL_TRIANGLE_FAN):
                {
                    IndexPointer iptr = indices;
                    Index first = *iptr;
                    ++iptr;
                    for(GLsizei i=2;i<count;++i,++iptr)
                    {
                        this->operator()(first,*(iptr),*(iptr+1));
                    }
                    break;
                }
                case(GL_LINES):
                {
                    IndexPointer iptr = indices;
                    Index first = *iptr;
                    for(GLsizei i=1;i<count;i+=2,iptr+=2)
                    {
                        this->operator()(*iptr,*(iptr+1));
                    }
                    break;
                }
                case(GL_LINE_STRIP):
                {
                    IndexPointer iptr = indices;
                    Index first = *iptr;
                    for(GLsizei i=1;i<count;i+=1,++iptr)
                    {
                        this->operator()(*iptr,*(iptr+1));
                    }
                    break;
                }
                case(GL_LINE_LOOP):
                {
                    IndexPointer iptr = indices;
                    Index first = *iptr;
                    for(GLsizei i=1;i<count;i+=1,++iptr)
                    {
                        this->operator()(*iptr,*(iptr+1));
                    }
                    this->operator()(*iptr,first);
                    break;
                }

                case(GL_POINTS):
                default:
                    // can't be converted into to triangles.
                    break;
            }
        }    

        virtual void drawElements(GLenum mode,GLsizei count,const GLuint* indices)
        {
            if (indices==0 || count==0) return;
        
            typedef GLuint Index;
            typedef const Index* IndexPointer;
        

            switch(mode)
            {
                case(GL_TRIANGLES):
                {
                    IndexPointer ilast = &indices[count];
                    for(IndexPointer  iptr=indices;iptr<ilast;iptr+=3)
                        this->operator()(*iptr,*(iptr+1),*(iptr+2));
                    break;
                }
                case(GL_TRIANGLE_STRIP):
                {
                    IndexPointer iptr = indices;
                    for(GLsizei i=2;i<count;++i,++iptr)
                    {
                        if ((i%2)) this->operator()(*(iptr),*(iptr+2),*(iptr+1));
                        else       this->operator()(*(iptr),*(iptr+1),*(iptr+2));
                    }
                    break;
                }
                case(GL_QUADS):
                {
                    IndexPointer iptr = indices;
                    for(GLsizei i=3;i<count;i+=4,iptr+=4)
                    {
                        this->operator()(*(iptr),*(iptr+1),*(iptr+2));
                        this->operator()(*(iptr),*(iptr+2),*(iptr+3));
                    }
                    break;
                }
                case(GL_QUAD_STRIP):
                {
                    IndexPointer iptr = indices;
                    for(GLsizei i=3;i<count;i+=2,iptr+=2)
                    {
                        this->operator()(*(iptr),*(iptr+1),*(iptr+2));
                        this->operator()(*(iptr+1),*(iptr+3),*(iptr+2));
                    }
                    break;
                }
                case(GL_POLYGON): // treat polygons as GL_TRIANGLE_FAN
                case(GL_TRIANGLE_FAN):
                {
                    IndexPointer iptr = indices;
                    Index first = *iptr;
                    ++iptr;
                    for(GLsizei i=2;i<count;++i,++iptr)
                    {
                        this->operator()(first,*(iptr),*(iptr+1));
                    }
                    break;
                }
                case(GL_LINES):
                {
                    IndexPointer iptr = indices;
                    Index first = *iptr;
                    for(GLsizei i=1;i<count;i+=2,iptr+=2)
                    {
                        this->operator()(*iptr,*(iptr+1));
                    }
                    break;
                }
                case(GL_LINE_STRIP):
                case(GL_POINTS):
                {
                    IndexPointer iptr = indices;
                    Index first = *iptr;
                    for(GLsizei i=1;i<count;i+=1,++iptr)
                    {
                        this->operator()(*iptr,*(iptr+1));
                    }
                    break;
                }
                case(GL_LINE_LOOP):
                {
                    IndexPointer iptr = indices;
                    Index first = *iptr;
                    for(GLsizei i=1;i<count;i+=1,++iptr)
                    {
                        this->operator()(*iptr,*(iptr+1));
                    }
                    this->operator()(*iptr,first);
                    break;
                }

                default:
                    // can't be converted into to triangles.
                    break;
            }
        }    

        GLenum               _modeCache;
        std::vector<GLuint>  _indexCache;
    };




    
    template<typename T>
    class LineIndexFunctor : public osg::PrimitiveIndexFunctor, public T
    {
    public:


        virtual void setVertexArray(unsigned int,const osg::Vec2*) 
        {
        }

        virtual void setVertexArray(unsigned int ,const osg::Vec3* )
        {
        }

        virtual void setVertexArray(unsigned int,const osg::Vec4* ) 
        {
        }

        virtual void setVertexArray(unsigned int,const osg::Vec2d*) 
        {
        }

        virtual void setVertexArray(unsigned int ,const osg::Vec3d* )
        {
        }

        virtual void setVertexArray(unsigned int,const osg::Vec4d* ) 
        {
        }

        virtual void begin(GLenum mode)
        {
            _modeCache = mode;
            _indexCache.clear();
        }

        virtual void vertex(unsigned int vert)
        {
            _indexCache.push_back(vert);
        }

        virtual void end()
        {
            if (!_indexCache.empty())
            {
                drawElements(_modeCache,_indexCache.size(),&_indexCache.front());
            }
        }

        // simulate triangles
        void operator()(GLuint i0, GLuint i1, GLuint i2)
        {
            T::line(i0,i1);
            T::line(i1,i2);
            T::line(i2,i1);
        }
        void operator()(GLushort i0, GLushort i1, GLushort i2)
        {
            T::line(i0,i1);
            T::line(i1,i2);
            T::line(i2,i1);
        }
        void operator()(GLubyte i0, GLubyte i1, GLubyte i2)
        {
            T::line(i0,i1);
            T::line(i1,i2);
            T::line(i2,i1);
        }


        virtual void drawArrays(GLenum mode,GLint first,GLsizei count)
        {
            switch(mode)
            {
                case(GL_TRIANGLES):
                {
                    unsigned int pos=first;
                    for(GLsizei i=2;i<count;i+=3,pos+=3)
                    {
                        this->operator()(pos,pos+1,pos+2);
                    }
                    break;
                }
                case(GL_TRIANGLE_STRIP):
                 {
                    unsigned int pos=first;
                    for(GLsizei i=2;i<count;++i,++pos)
                    {
                        if ((i%2)) this->operator()(pos,pos+2,pos+1);
                        else       this->operator()(pos,pos+1,pos+2);
                    }
                    break;
                }
                case(GL_QUADS):
                {
                    unsigned int pos=first;
                    for(GLsizei i=3;i<count;i+=4,pos+=4)
                    {
                        this->operator()(pos,pos+1,pos+2);
                        this->operator()(pos,pos+2,pos+3);
                    }
                    break;
                }
                case(GL_QUAD_STRIP):
                {
                    unsigned int pos=first;
                    for(GLsizei i=3;i<count;i+=2,pos+=2)
                    {
                        this->operator()(pos,pos+1,pos+2);
                        this->operator()(pos+1,pos+3,pos+2);
                    }
                    break;
                }
                case(GL_POLYGON): // treat polygons as GL_TRIANGLE_FAN
                case(GL_TRIANGLE_FAN):
                {
                    unsigned int pos=first+1;
                    for(GLsizei i=2;i<count;++i,++pos)
                    {
                        this->operator()(first,pos,pos+1);
                    }
                    break;
                }
                case(GL_LINES):
                {
                    unsigned pos=first;
                    for(GLsizei i=1;i<count;i+=2,pos+=2)
                    {
                        //this->operator()(pos,pos+1);
                        T::line(pos,pos+1);
                    }
                    break;
                }
                case(GL_LINE_STRIP):
                {
                    unsigned pos=first;
                    for(GLsizei i=1;i<count;i+=1,pos+=1)
                    {
                        T::line(pos,pos+1);
                    }
                    break;
                }
                case(GL_LINE_LOOP):
                {
                    unsigned pos=first;
                    for(GLsizei i=1; i<count; i+=1, pos+=1)
                    {
                        T::line(pos,pos+1);
                    }
                    if (count > 0 )
                    {
                        T::line(pos,first);
                    }
                    break;
                }
                case(GL_POINTS):
                default:
                    // can't be converted into to triangles.
                    break;
            }
        }

        virtual void drawElements(GLenum mode,GLsizei count,const GLubyte* indices)
        {
            if (indices==0 || count==0) return;

            typedef GLubyte Index;
            typedef const Index* IndexPointer;

            switch(mode)
            {
                case(GL_TRIANGLES):
                {
                    IndexPointer ilast = &indices[count];
                    for(IndexPointer  iptr=indices;iptr<ilast;iptr+=3)
                        this->operator()(*iptr,*(iptr+1),*(iptr+2));
                    break;
                }
                case(GL_TRIANGLE_STRIP):
                {
                    IndexPointer iptr = indices;
                    for(GLsizei i=2;i<count;++i,++iptr)
                    {
                        if ((i%2)) this->operator()(*(iptr),*(iptr+2),*(iptr+1));
                        else       this->operator()(*(iptr),*(iptr+1),*(iptr+2));
                    }
                    break;
                }
                case(GL_QUADS):
                {
                    IndexPointer iptr = indices;
                    for(GLsizei i=3;i<count;i+=4,iptr+=4)
                    {
                        this->operator()(*(iptr),*(iptr+1),*(iptr+2));
                        this->operator()(*(iptr),*(iptr+2),*(iptr+3));
                    }
                    break;
                }
                case(GL_QUAD_STRIP):
                {
                    IndexPointer iptr = indices;
                    for(GLsizei i=3;i<count;i+=2,iptr+=2)
                    {
                        this->operator()(*(iptr),*(iptr+1),*(iptr+2));
                        this->operator()(*(iptr+1),*(iptr+3),*(iptr+2));
                    }
                    break;
                }
                case(GL_POLYGON): // treat polygons as GL_TRIANGLE_FAN
                case(GL_TRIANGLE_FAN):
                {
                    IndexPointer iptr = indices;
                    Index first = *iptr;
                    ++iptr;
                    for(GLsizei i=2;i<count;++i,++iptr)
                    {
                        this->operator()(first,*(iptr),*(iptr+1));
                    }
                    break;
                }
                case(GL_LINES):
                {
                    IndexPointer iptr = indices;
                    Index first = *iptr;
                    for(GLsizei i=1;i<count;i+=2,iptr+=2)
                    {
                        T::line(*iptr,*(iptr+1));
                    }
                    break;
                }
                case(GL_LINE_STRIP):
                {
                    IndexPointer iptr = indices;
                    Index first = *iptr;
                    for(GLsizei i=1;i<count;i+=1,++iptr)
                    {
                        T::line(*iptr,*(iptr+1));
                    }
                    break;
                }
                case(GL_LINE_LOOP):
                {
                    IndexPointer iptr = indices;
                    Index first = *iptr;
                    for(GLsizei i=1;i<count;i+=1,++iptr)
                    {
                        T::line(*iptr,*(iptr+1));
                    }
                    T::line(*iptr,first);
                    break;
                }

                case(GL_POINTS):
                default:
                    // can't be converted into to triangles.
                    break;
            }
        }    

        virtual void drawElements(GLenum mode,GLsizei count,const GLushort* indices)
        {
            if (indices==0 || count==0) return;

            typedef GLushort Index;
            typedef const Index* IndexPointer;
        

            switch(mode)
            {
                case(GL_TRIANGLES):
                {
                    IndexPointer ilast = &indices[count];
                    for(IndexPointer  iptr=indices;iptr<ilast;iptr+=3)
                        this->operator()(*iptr,*(iptr+1),*(iptr+2));
                    break;
                }
                case(GL_TRIANGLE_STRIP):
                {
                    IndexPointer iptr = indices;
                    for(GLsizei i=2;i<count;++i,++iptr)
                    {
                        if ((i%2)) this->operator()(*(iptr),*(iptr+2),*(iptr+1));
                        else       this->operator()(*(iptr),*(iptr+1),*(iptr+2));
                    }
                    break;
                }
                case(GL_QUADS):
                {
                    IndexPointer iptr = indices;
                    for(GLsizei i=3;i<count;i+=4,iptr+=4)
                    {
                        this->operator()(*(iptr),*(iptr+1),*(iptr+2));
                        this->operator()(*(iptr),*(iptr+2),*(iptr+3));
                    }
                    break;
                }
                case(GL_QUAD_STRIP):
                {
                    IndexPointer iptr = indices;
                    for(GLsizei i=3;i<count;i+=2,iptr+=2)
                    {
                        this->operator()(*(iptr),*(iptr+1),*(iptr+2));
                        this->operator()(*(iptr+1),*(iptr+3),*(iptr+2));
                    }
                    break;
                }
                case(GL_POLYGON): // treat polygons as GL_TRIANGLE_FAN
                case(GL_TRIANGLE_FAN):
                {
                    IndexPointer iptr = indices;
                    Index first = *iptr;
                    ++iptr;
                    for(GLsizei i=2;i<count;++i,++iptr)
                    {
                        this->operator()(first,*(iptr),*(iptr+1));
                    }
                    break;
                }
                case(GL_LINES):
                {
                    IndexPointer iptr = indices;
                    Index first = *iptr;
                    for(GLsizei i=1;i<count;i+=2,iptr+=2)
                    {
                        T::line(*iptr,*(iptr+1));
                    }
                    break;
                }
                case(GL_LINE_STRIP):
                {
                    IndexPointer iptr = indices;
                    Index first = *iptr;
                    for(GLsizei i=1;i<count;i+=1,++iptr)
                    {
                        T::line(*iptr,*(iptr+1));
                    }
                    break;
                }
                case(GL_LINE_LOOP):
                {
                    IndexPointer iptr = indices;
                    Index first = *iptr;
                    for(GLsizei i=1;i<count;i+=1,++iptr)
                    {
                        T::line(*iptr,*(iptr+1));
                    }
                    T::line(*iptr,first);
                    break;
                }

                case(GL_POINTS):
                default:
                    // can't be converted into to triangles.
                    break;
            }
        }    

        virtual void drawElements(GLenum mode,GLsizei count,const GLuint* indices)
        {
            if (indices==0 || count==0) return;
        
            typedef GLuint Index;
            typedef const Index* IndexPointer;
        

            switch(mode)
            {
                case(GL_TRIANGLES):
                {
                    IndexPointer ilast = &indices[count];
                    for(IndexPointer  iptr=indices;iptr<ilast;iptr+=3)
                        this->operator()(*iptr,*(iptr+1),*(iptr+2));
                    break;
                }
                case(GL_TRIANGLE_STRIP):
                {
                    IndexPointer iptr = indices;
                    for(GLsizei i=2;i<count;++i,++iptr)
                    {
                        if ((i%2)) this->operator()(*(iptr),*(iptr+2),*(iptr+1));
                        else       this->operator()(*(iptr),*(iptr+1),*(iptr+2));
                    }
                    break;
                }
                case(GL_QUADS):
                {
                    IndexPointer iptr = indices;
                    for(GLsizei i=3;i<count;i+=4,iptr+=4)
                    {
                        this->operator()(*(iptr),*(iptr+1),*(iptr+2));
                        this->operator()(*(iptr),*(iptr+2),*(iptr+3));
                    }
                    break;
                }
                case(GL_QUAD_STRIP):
                {
                    IndexPointer iptr = indices;
                    for(GLsizei i=3;i<count;i+=2,iptr+=2)
                    {
                        this->operator()(*(iptr),*(iptr+1),*(iptr+2));
                        this->operator()(*(iptr+1),*(iptr+3),*(iptr+2));
                    }
                    break;
                }
                case(GL_POLYGON): // treat polygons as GL_TRIANGLE_FAN
                case(GL_TRIANGLE_FAN):
                {
                    IndexPointer iptr = indices;
                    Index first = *iptr;
                    ++iptr;
                    for(GLsizei i=2;i<count;++i,++iptr)
                    {
                        this->operator()(first,*(iptr),*(iptr+1));
                    }
                    break;
                }
                case(GL_LINES):
                {
                    IndexPointer iptr = indices;
                    Index first = *iptr;
                    for(GLsizei i=1;i<count;i+=2,iptr+=2)
                    {
                        T::line(*iptr,*(iptr+1));
                    }
                    break;
                }
                case(GL_LINE_STRIP):
                case(GL_POINTS):
                {
                    IndexPointer iptr = indices;
                    Index first = *iptr;
                    for(GLsizei i=1;i<count;i+=1,++iptr)
                    {
                        T::line(*iptr,*(iptr+1));
                    }
                    break;
                }
                case(GL_LINE_LOOP):
                {
                    IndexPointer iptr = indices;
                    Index first = *iptr;
                    for(GLsizei i=1;i<count;i+=1,++iptr)
                    {
                        T::line(*iptr,*(iptr+1));
                    }
                    T::line(*iptr,first);
                    break;
                }

                default:
                    // can't be converted into to triangles.
                    break;
            }
        }    

        GLenum               _modeCache;
        std::vector<GLuint>  _indexCache;
    };

} // namespace osgEarth


#endif // OSGEARTH_LINEFUNCTOR

