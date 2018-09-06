#ifndef OSG_STATE
#define OSG_STATE 1

#include <VBF_Engine/VBF_SceneGraph/Export>
#include <VBF_Engine/VBF_SceneGraph/StateSet>
#include <VBF_Engine/VBF_SceneGraph/VBF_ObjectMatrix.h>
//#include <VBF_Engine/VBF_SceneGraph/Uniform>
//#include <VBF_Engine/VBF_SceneGraph/VBF_BufferObject.h>
#include <VBF_Base/DeleteObserver.h>
#include <VBF_Base/VBF_Timer.h>

#include <VBF_Engine/VBF_SceneGraph/ShaderComposer>
//#include <VBF_Base/VBF_FrameStamp.h>
//#include <VBF_Engine/VBF_SceneGraph/DisplaySettings.h>
#include <Types/VBF_Polytope.h>
#include <VBF_Engine/VBF_SceneGraph/VBFO_StateAttrViewport.h>
//#include <VBF_Engine/VBF_SceneGraph/GLBeginEndAdapter>
#include <VBF_Engine/VBF_SceneGraph/ArrayDispatchers>
#include <VBF_Engine/VBF_SceneGraph/GraphicsCostEstimator>

#include <iosfwd>
//#include <vector>
//#include <map>
//#include <set>
//#include <string>




#ifndef GL_FOG_COORDINATE_ARRAY
    #ifdef GL_FOG_COORDINATE_ARRAY_EXT
        #define GL_FOG_COORDINATE_ARRAY GL_FOG_COORDINATE_ARRAY_EXT
    #else
        #define GL_FOG_COORDINATE_ARRAY 0x8457
    #endif
#endif

#ifndef GL_SECONDARY_COLOR_ARRAY
    #ifdef GL_SECONDARY_COLOR_ARRAY_EXT
        #define GL_SECONDARY_COLOR_ARRAY GL_SECONDARY_COLOR_ARRAY_EXT
    #else
        #define GL_SECONDARY_COLOR_ARRAY 0x845E
    #endif
#endif

#if !defined(GL_EXT_timer_query) && !defined(OSG_GL3_AVAILABLE)
    #ifdef _WIN32
        typedef          __int64 GLint64EXT;
        typedef unsigned __int64 GLuint64EXT;
    #else
        typedef long long int GLint64EXT;
        typedef unsigned long long int GLuint64EXT;
    #endif
#endif

namespace osg {

class GraphicsContext;


/** macro for use with osg::StateAttribute::apply methods for detecting and
  * reporting OpenGL error messages.*/
#define OSG_GL_DEBUG(message) \
    if (state.getFineGrainedErrorDetection()) \
    { \
        GLenum errorNo = glGetError(); \
        if (errorNo!=GL_NO_ERROR) \
        { \
            osg::notify(WARN)<<"Warning: detected OpenGL error '"<<gluErrorString(errorNo)<<" "<<message<<endl; \
        }\
    }




class VertexAttribAlias
{
    public:
        VertexAttribAlias(): _location(0) 
		{}

        VertexAttribAlias(const VertexAttribAlias& rhs):
					_location(rhs._location), _glName(rhs._glName), _osgName(rhs._osgName), _declaration(rhs._declaration) 
		{}

        VertexAttribAlias(GLuint location, const std::string glName, const std::string osgName, const std::string& declaration):
					 _location(location), _glName(glName), _osgName(osgName), _declaration(declaration)
		{}

        GLuint      _location;
        std::string _glName;
        std::string _osgName;
        std::string _declaration;
};

class OSG_EXPORT State : public ::CVBF_Referenced, public DeleteObserver
{
	public :
		State();

	public:
		void setGraphicsContext(GraphicsContext* context) { _graphicsContext = context; }
		GraphicsContext* getGraphicsContext() { return _graphicsContext; }
		const GraphicsContext* getGraphicsContext() const { return _graphicsContext; }

        inline void setContextID(unsigned int contextID) { _contextID=contextID; }
        inline unsigned int getContextID() const { return _contextID; }

        void setShaderCompositionEnabled(bool flag) { _shaderCompositionEnabled = flag; }
        bool getShaderCompositionEnabled() const { return _shaderCompositionEnabled; }

        /** Set the ShaderComposor object that implements shader composition.*/
        void setShaderComposer(ShaderComposer* sc) { _shaderComposer = sc; }
        ShaderComposer* getShaderComposer() { return _shaderComposer.get(); }
        const ShaderComposer* getShaderComposer() const { return _shaderComposer.get(); }



        /** Get the unform list in which to inject any uniforms that StateAttribute::apply(State&) methods provide.*/
        StateSet::UniformList& getCurrentShaderCompositionUniformList() { return _currentShaderCompositionUniformList; }

        /** Convinience method for StateAttribute:::apply(State&) methods to pass on their uniforms to osg::State so it can apply them at the appropriate point.*/
        void applyShaderCompositionUniform(const osg::Uniform* uniform, StateAttribute::OverrideValue value=StateAttribute::ON)
        {
            StateSet::RefUniformPair& up = _currentShaderCompositionUniformList[uniform->getName()];
            up.first = const_cast<Uniform*>(uniform);
            up.second = value;
        }



        void pushStateSet(const StateSet* dstate);
        void popStateSet();

         void popAllStateSets();

        void insertStateSet(unsigned int pos,const StateSet* dstate);
        void removeStateSet(unsigned int pos);

         unsigned int getStateSetStackSize() { return static_cast<unsigned int>(_stateStateStack.size()); }

        void popStateSetStackToSize(unsigned int size) { while (_stateStateStack.size()>size) popStateSet(); }

        typedef std::vector<const StateSet*> StateSetStack; 

        StateSetStack& getStateSetStack() { return _stateStateStack; }

        void captureCurrentState(StateSet& stateset) const;

        void reset(); 


        inline const Viewport* getCurrentViewport() const
        {
            return static_cast<const Viewport*>(getLastAppliedAttribute(osg::StateAttribute::VIEWPORT));
        }


        void setInitialViewMatrix(const osg::RefMatrix* matrix);

        inline const osg::Matrix& getInitialViewMatrix() const { return *_initialViewMatrix; }
        inline const osg::Matrix& getInitialInverseViewMatrix() const { return _initialInverseViewMatrix; }

        void applyProjectionMatrix(const osg::RefMatrix* matrix);

        inline const osg::Matrix& getProjectionMatrix() const { return *_projection; }
		//h00017
		void applyWorldMatrix(const osg::RefMatrix* matrix);
		inline const osg::Matrix& getWorldMatrix() const {return *_worldMat;}
		//h00017

        void applyModelViewMatrix(const osg::RefMatrix* matrix);
        void applyModelViewMatrix(const osg::Matrix&);

        const osg::Matrix& getModelViewMatrix() const { return *_modelView; }

        void setUseModelViewAndProjectionUniforms(bool flag) { _useModelViewAndProjectionUniforms = flag; }
        bool getUseModelViewAndProjectionUniforms() const { return _useModelViewAndProjectionUniforms; }

        void updateModelViewAndProjectionMatrixUniforms();

        void applyModelViewAndProjectionUniformsIfRequired();

        osg::Uniform* getModelViewMatrixUniform() { return _modelViewMatrixUniform.get(); }
        osg::Uniform* getProjectionMatrixUniform() { return _projectionMatrixUniform.get(); }
        osg::Uniform* getModelViewProjectionMatrixUniform() { return _modelViewProjectionMatrixUniform.get(); }
        osg::Uniform* getNormalMatrixUniform() { return _normalMatrixUniform.get(); }


        Polytope getViewFrustum() const;








        void setUseVertexAttributeAliasing(bool flag) { _useVertexAttributeAliasing = flag; }
        bool getUseVertexAttributeAliasing() const { return _useVertexAttributeAliasing ; }

        typedef std::vector<VertexAttribAlias> VertexAttribAliasList;

        const VertexAttribAlias& getVertexAlias() { return _vertexAlias; }
        const VertexAttribAlias& getNormalAlias() { return _normalAlias; }
        const VertexAttribAlias& getColorAlias() { return _colorAlias; }
        const VertexAttribAlias& getSecondaryColorAlias() { return _secondaryColorAlias; }
        const VertexAttribAlias& getFogCoordAlias() { return _fogCoordAlias; }
        const VertexAttribAliasList& getTexCoordAliasList() { return _texCoordAliasList; }


        const Program::AttribBindingList&  getAttributeBindingList() { return _attributeBindingList; }

        bool convertVertexShaderSourceToOsgBuiltIns(std::string& source) const;


        /** Apply stateset.*/
        void apply(const StateSet* dstate);

        /** Updates the OpenGL state so that it matches the \c StateSet at the
          * top of the stack of <tt>StateSet</tt>s maintained internally by a
          * \c State.
         */
        void apply();

        /** Apply any shader composed state.*/
        void applyShaderComposition();

        /** Set whether a particular OpenGL mode is valid in the current graphics context.
          * Use to disable OpenGL modes that are not supported by current graphics drivers/context.*/
        inline void setModeValidity(StateAttribute::GLMode mode,bool valid)
        {
            ModeStack& ms = _modeMap[mode];
            ms.valid = valid;
        }

        /** Get whether a particular OpenGL mode is valid in the current graphics context.
          * Use to disable OpenGL modes that are not supported by current graphics drivers/context.*/
        inline bool getModeValidity(StateAttribute::GLMode mode)
        {
            ModeStack& ms = _modeMap[mode];
            return ms.valid;
        }

        inline void setGlobalDefaultModeValue(StateAttribute::GLMode mode,bool enabled)
        {
            ModeStack& ms = _modeMap[mode];
            ms.global_default_value = enabled;
        }

        inline bool getGlobalDefaultModeValue(StateAttribute::GLMode mode)
        {
            return _modeMap[mode].global_default_value;
        }


        /** Apply an OpenGL mode if required. This is a wrapper around
          * \c glEnable() and \c glDisable(), that just actually calls these
          * functions if the \c enabled flag is different than the current
          * state.     */
		// 
        inline bool applyMode(StateAttribute::GLMode mode,bool enabled)
        {
            ModeStack& ms = _modeMap[mode];
            ms.changed = true;
            return applyMode(mode,enabled,ms);
        }

        inline void setGlobalDefaultTextureModeValue(unsigned int unit, StateAttribute::GLMode mode,bool enabled)
        {
            ModeMap& modeMap = getOrCreateTextureModeMap(unit);
            ModeStack& ms = modeMap[mode];
            ms.global_default_value = enabled;
        }

        inline bool getGlobalDefaultTextureModeValue(unsigned int unit, StateAttribute::GLMode mode)
        {
            ModeMap& modeMap = getOrCreateTextureModeMap(unit);
            ModeStack& ms = modeMap[mode];
            return ms.global_default_value;
        }

        inline bool applyTextureMode(unsigned int unit, StateAttribute::GLMode mode,bool enabled)
        {
            ModeMap& modeMap = getOrCreateTextureModeMap(unit);
            ModeStack& ms = modeMap[mode];
            ms.changed = true;
            return applyModeOnTexUnit(unit,mode,enabled,ms);
        }

        inline void setGlobalDefaultAttribute(const StateAttribute* attribute)
        {
            AttributeStack& as = _attributeMap[attribute->getTypeMemberPair()];
            as.global_default_attribute = attribute;
        }

        inline const StateAttribute* getGlobalDefaultAttribute(StateAttribute::Type type, unsigned int member=0)
        {
            AttributeStack& as = _attributeMap[StateAttribute::TypeMemberPair(type,member)];
            return as.global_default_attribute.get();
        }

        /** Apply an attribute if required. */
        inline bool applyAttribute(const StateAttribute* attribute)
        {
            AttributeStack& as = _attributeMap[attribute->getTypeMemberPair()];
            as.changed = true;
            return applyAttribute(attribute,as);
        }

        inline void setGlobalDefaultTextureAttribute(unsigned int unit, const StateAttribute* attribute)
        {
            AttributeMap& attributeMap = getOrCreateTextureAttributeMap(unit);
            AttributeStack& as = attributeMap[attribute->getTypeMemberPair()];
            as.global_default_attribute = attribute;
        }

        inline const StateAttribute* getGlobalDefaultTextureAttribute(unsigned int unit, StateAttribute::Type type, unsigned int member = 0)
        {
            AttributeMap& attributeMap = getOrCreateTextureAttributeMap(unit);
            AttributeStack& as = attributeMap[StateAttribute::TypeMemberPair(type,member)];
            return as.global_default_attribute.get();
        }


        inline bool applyTextureAttribute(unsigned int unit, const StateAttribute* attribute)
        {
            AttributeMap& attributeMap = getOrCreateTextureAttributeMap(unit);
            AttributeStack& as = attributeMap[attribute->getTypeMemberPair()];
            as.changed = true;
            return applyAttributeOnTexUnit(unit,attribute,as);
        }

        /** Mode has been set externally, update state to reflect this setting.*/
        void haveAppliedMode(StateAttribute::GLMode mode,StateAttribute::GLModeValue value);

        /** Mode has been set externally, therefore dirty the associated mode in osg::State
          * so it is applied on next call to osg::State::apply(..)*/
        void haveAppliedMode(StateAttribute::GLMode mode);

        /** Attribute has been applied externally, update state to reflect this setting.*/
        void haveAppliedAttribute(const StateAttribute* attribute);

        /** Attribute has been applied externally, 
          * and therefore this attribute type has been dirtied 
          * and will need to be re-applied on next osg::State.apply(..).
          * note, if you have an osg::StateAttribute which you have applied externally
          * then use the have_applied(attribute) method as this will cause the osg::State to
          * track the current state more accurately and enable lazy state updating such
          * that only changed state will be applied.*/
        void haveAppliedAttribute(StateAttribute::Type type, unsigned int member=0);

        /** Get whether the current specified mode is enabled (true) or disabled (false).*/
        bool getLastAppliedMode(StateAttribute::GLMode mode) const;

        /** Get the current specified attribute, return NULL if one has not yet been applied.*/
        const StateAttribute* getLastAppliedAttribute(StateAttribute::Type type, unsigned int member=0) const;

        /** texture Mode has been set externally, update state to reflect this setting.*/
        void haveAppliedTextureMode(unsigned int unit, StateAttribute::GLMode mode,StateAttribute::GLModeValue value);

        /** texture Mode has been set externally, therefore dirty the associated mode in osg::State
          * so it is applied on next call to osg::State::apply(..)*/
        void haveAppliedTextureMode(unsigned int unit, StateAttribute::GLMode mode);

        /** texture Attribute has been applied externally, update state to reflect this setting.*/
        void haveAppliedTextureAttribute(unsigned int unit, const StateAttribute* attribute);

        /** texture Attribute has been applied externally,
          * and therefore this attribute type has been dirtied
          * and will need to be re-applied on next osg::State.apply(..).
          * note, if you have an osg::StateAttribute which you have applied externally
          * then use the have_applied(attribute) method as this will the osg::State to
          * track the current state more accurately and enable lazy state updating such
          * that only changed state will be applied.*/
        void haveAppliedTextureAttribute(unsigned int unit, StateAttribute::Type type, unsigned int member=0);

        /** Get whether the current specified texture mode is enabled (true) or disabled (false).*/
        bool getLastAppliedTextureMode(unsigned int unit, StateAttribute::GLMode mode) const;

        /** Get the current specified texture attribute, return NULL if one has not yet been applied.*/
        const StateAttribute* getLastAppliedTextureAttribute(unsigned int unit, StateAttribute::Type type, unsigned int member=0) const;


        /** Dirty the modes previously applied in osg::State.*/
        void dirtyAllModes();

        /** Dirty the modes attributes previously applied in osg::State.*/
        void dirtyAllAttributes();

        /** disable the vertex, normal, color, tex coords, secondary color, fog coord and index arrays.*/
        void disableAllVertexArrays();

        /** dirty the vertex, normal, color, tex coords, secondary color, fog coord and index arrays.*/
        void dirtyAllVertexArrays();


        void setCurrentVertexBufferObject(osg::GLBufferObject* vbo) { _currentVBO = vbo; }
        const GLBufferObject* getCurrentVertexBufferObject() { return _currentVBO; }
        inline void bindVertexBufferObject(osg::GLBufferObject* vbo)
        {
            if (vbo == _currentVBO) return;
            if (vbo->isDirty()) vbo->compileBuffer();
            else vbo->bindBuffer();
            _currentVBO = vbo;
        }

        inline void unbindVertexBufferObject()
        {
            if (!_currentVBO) return;
            _glBindBuffer(GL_ARRAY_BUFFER_ARB,0);
            _currentVBO = 0;
        }

        void setCurrentElementBufferObject(osg::GLBufferObject* ebo) { _currentEBO = ebo; }
        const GLBufferObject* getCurrentElementBufferObject() { return _currentEBO; }

        inline void bindElementBufferObject(osg::GLBufferObject* ebo)
        {
            if (ebo == _currentEBO) return;
            if (ebo->isDirty()) ebo->compileBuffer();
            else ebo->bindBuffer();
            _currentEBO = ebo;
        }

        inline void unbindElementBufferObject()
        {
            if (!_currentEBO) return;
            _glBindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB,0);
            _currentEBO = 0;
        }

        void setCurrentPixelBufferObject(osg::GLBufferObject* pbo) { _currentPBO = pbo; }
        const GLBufferObject* getCurrentPixelBufferObject() { return _currentPBO; }

        inline void bindPixelBufferObject(osg::GLBufferObject* pbo)
        {
            if (pbo == _currentPBO) return;

            if (pbo->isDirty()) pbo->compileBuffer();
            else pbo->bindBuffer();

            _currentPBO = pbo;
        }

        inline void unbindPixelBufferObject()
        {
            if (!_currentPBO) return;

            _glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB,0);
            _currentPBO = 0;
        }

        typedef std::vector<GLushort> IndicesGLushort;
        IndicesGLushort _quadIndicesGLushort[4];

        typedef std::vector<GLuint> IndicesGLuint;
        IndicesGLuint _quadIndicesGLuint[4];

        void drawQuads(GLint first, GLsizei count, GLsizei primCount=0);

        inline void glDrawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei primcount)
        {
            if (primcount>=1 && _glDrawArraysInstanced!=0) _glDrawArraysInstanced(mode, first, count, primcount);
            else glDrawArrays(mode, first, count);
        }

        inline void glDrawElementsInstanced(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices, GLsizei primcount )
        {
            if (primcount>=1 && _glDrawElementsInstanced!=0) _glDrawElementsInstanced(mode, count, type, indices, primcount);
            else glDrawElements(mode, count, type, indices);
        }


        inline void Vertex(float x, float y, float z, float w=1.0f)
        {
        #if defined(OSG_GL_VERTEX_FUNCS_AVAILABLE) && !defined(OSG_GLES1_AVAILABLE)
            if (_useVertexAttributeAliasing) _glVertexAttrib4f( _vertexAlias._location, x,y,z,w);
            else glVertex4f(x,y,z,w);
        #else
            _glVertexAttrib4f( _vertexAlias._location, x,y,z,w);
        #endif
        }

        inline void Color(float r, float g, float b, float a=1.0f)
        {
        #ifdef OSG_GL_VERTEX_FUNCS_AVAILABLE
            if (_useVertexAttributeAliasing) _glVertexAttrib4f( _colorAlias._location, r,g,b,a);
            else glColor4f(r,g,b,a);
        #else
            _glVertexAttrib4f( _colorAlias._location, r,g,b,a);
        #endif
        }

        void Normal(float x, float y, float z)
        {
        #ifdef OSG_GL_VERTEX_FUNCS_AVAILABLE
            if (_useVertexAttributeAliasing) _glVertexAttrib4f( _normalAlias._location, x,y,z,0.0);
            else glNormal3f(x,y,z);
        #else
            _glVertexAttrib4f( _normalAlias._location, x,y,z,0.0);
        #endif
        }

        void TexCoord(float x, float y=0.0f, float z=0.0f, float w=1.0f)
        {
        #if !defined(OSG_GLES1_AVAILABLE)
            #ifdef OSG_GL_VERTEX_FUNCS_AVAILABLE
                if (_useVertexAttributeAliasing) _glVertexAttrib4f( _texCoordAliasList[0]._location, x,y,z,w);
                else glTexCoord4f(x,y,z,w);
            #else
                _glVertexAttrib4f( _texCoordAliasList[0]._location, x,y,z,w);
            #endif
        #endif
        }

        void MultiTexCoord(unsigned int unit, float x, float y=0.0f, float z=0.0f, float w=1.0f)
        {
        #if !defined(OSG_GLES1_AVAILABLE)
            #ifdef OSG_GL_VERTEX_FUNCS_AVAILABLE
                if (_useVertexAttributeAliasing) _glVertexAttrib4f( _texCoordAliasList[unit]._location, x,y,z,w);
                else _glMultiTexCoord4f(GL_TEXTURE0+unit,x,y,z,w);
            #else
                _glVertexAttrib4f( _texCoordAliasList[unit]._location, x,y,z,w);
            #endif
        #endif
        }

        void VerteAttrib(unsigned int location, float x, float y=0.0f, float z=0.0f, float w=0.0f)
        {
            _glVertexAttrib4f( location, x,y,z,w);
        }


        /** Mark all the vertex attributes as being disabled but leave the disabling till a later call to applyDisablingOfVertexAttributes.*/
        void lazyDisablingOfVertexAttributes();

        /** Disable all the vertex attributes that have been marked as to be disabled.*/
        void applyDisablingOfVertexAttributes();

        /** Wrapper around glInterleavedArrays(..).
          * also resets the internal array points and modes within osg::State to keep the other
          * vertex array operations consistent. */
        void setInterleavedArrays( GLenum format, GLsizei stride, const GLvoid* pointer);

        /** Set the vertex pointer using an osg::Array, and manage any VBO that are required.*/
        inline void setVertexPointer(const Array* array)
        {
            if (array)
            {
                GLBufferObject* vbo = array->getOrCreateGLBufferObject(_contextID);
                if (vbo)
                {
                    bindVertexBufferObject(vbo);
                    setVertexPointer(array->getDataSize(),array->getDataType(),0,(const GLvoid *)(vbo->getOffset(array->getBufferIndex())));
                }
                else
                {
                    unbindVertexBufferObject();
                    setVertexPointer(array->getDataSize(),array->getDataType(),0,array->getDataPointer());
                }
            }
        }

        /** wrapper around glEnableClientState(GL_VERTEX_ARRAY);glVertexPointer(..);
          * note, only updates values that change.*/
        inline void setVertexPointer( GLint size, GLenum type,
                                      GLsizei stride, const GLvoid *ptr )
        {
        #ifdef OSG_GL_VERTEX_ARRAY_FUNCS_AVAILABLE
            if (_useVertexAttributeAliasing)
            {
                setVertexAttribPointer(_vertexAlias._location, size, type, GL_FALSE, stride, ptr);
            }
            else
            {
                if (!_vertexArray._enabled || _vertexArray.m_bDirty)
                {
                    _vertexArray._enabled = true;
                    glEnableClientState(GL_VERTEX_ARRAY);
                }
                //if (_vertexArray._pointer!=ptr || _vertexArray.m_bDirty)
                {
                    _vertexArray._pointer=ptr;
                    glVertexPointer( size, type, stride, ptr );
                }
                _vertexArray._lazy_disable = false;
                _vertexArray.m_bDirty = false;
            }
        #else
            setVertexAttribPointer(_vertexAlias._location, size, type, GL_FALSE, stride, ptr);
        #endif
        }

        /** wrapper around glDisableClientState(GL_VERTEX_ARRAY).
          * note, only updates values that change.*/
        inline void disableVertexPointer()
        {
        #ifdef OSG_GL_VERTEX_ARRAY_FUNCS_AVAILABLE
            if (_useVertexAttributeAliasing)
            {
                disableVertexAttribPointer(_vertexAlias._location);
            }
            else
            {
                if (_vertexArray._enabled || _vertexArray.m_bDirty)
                {
                    _vertexArray._lazy_disable = false;
                    _vertexArray._enabled = false;
                    _vertexArray.m_bDirty = false;
                    glDisableClientState(GL_VERTEX_ARRAY);
                }
            }
        #else
            disableVertexAttribPointer(_vertexAlias._location);
        #endif
        }

        inline void dirtyVertexPointer()
        {
        #ifdef OSG_GL_VERTEX_ARRAY_FUNCS_AVAILABLE
            if (_useVertexAttributeAliasing)
            {
                dirtyVertexAttribPointer(_vertexAlias._location);
            }
            else
            {
                _vertexArray._pointer = 0;
                _vertexArray.m_bDirty = true;
            }
        #else
            dirtyVertexAttribPointer(_vertexAlias._location);
        #endif
        }


        /** Set the normal pointer using an osg::Array, and manage any VBO that are required.*/
        inline void setNormalPointer(const Array* array)
        {
            if (array)
            {
                GLBufferObject* vbo = array->getOrCreateGLBufferObject(_contextID);
                if (vbo)
                {
                    bindVertexBufferObject(vbo);
                    setNormalPointer(array->getDataType(),0,(const GLvoid *)(vbo->getOffset(array->getBufferIndex())));
                }
                else
                {
                    unbindVertexBufferObject();
                    setNormalPointer(array->getDataType(),0,array->getDataPointer());
                }
            }
        }

        /** wrapper around glEnableClientState(GL_NORMAL_ARRAY);glNormalPointer(..);
          * note, only updates values that change.*/
        inline void setNormalPointer( GLenum type, GLsizei stride, const GLvoid *ptr )
        {
        #ifdef OSG_GL_VERTEX_ARRAY_FUNCS_AVAILABLE
            if (_useVertexAttributeAliasing)
            {
                setVertexAttribPointer(_normalAlias._location, 3, type, GL_FALSE, stride, ptr);
            }
            else
            {
                if (!_normalArray._enabled || _normalArray.m_bDirty)
                {
                    _normalArray._enabled = true;
                    glEnableClientState(GL_NORMAL_ARRAY);
                }
                //if (_normalArray._pointer!=ptr || _normalArray.m_bDirty)
                {
                    _normalArray._pointer=ptr;
                    glNormalPointer( type, stride, ptr );
                }
                _normalArray._lazy_disable = false;
                _normalArray.m_bDirty = false;
            }
        #else
            setVertexAttribPointer(_normalAlias._location, 3, type, GL_FALSE, stride, ptr);
        #endif
        }

        /** wrapper around glDisableClientState(GL_NORMAL_ARRAY);
          * note, only updates values that change.*/
        inline void disableNormalPointer()
        {
        #ifdef OSG_GL_VERTEX_ARRAY_FUNCS_AVAILABLE
            if (_useVertexAttributeAliasing)
            {
                disableVertexAttribPointer(_normalAlias._location);
            }
            else
            {
                if (_normalArray._enabled || _normalArray.m_bDirty)
                {
                    _normalArray._lazy_disable = false;
                    _normalArray._enabled = false;
                    _normalArray.m_bDirty = false;
                    glDisableClientState(GL_NORMAL_ARRAY);
                }
            }
        #else
            disableVertexAttribPointer(_normalAlias._location);
        #endif
        }

        inline void dirtyNormalPointer()
        {
        #ifdef OSG_GL_VERTEX_ARRAY_FUNCS_AVAILABLE
            if (_useVertexAttributeAliasing)
            {
                dirtyVertexAttribPointer(_normalAlias._location);
            }
            else
            {
                _normalArray._pointer = 0;
                _normalArray.m_bDirty = true;
            }
        #else
            dirtyVertexAttribPointer(_normalAlias._location);
        #endif
        }

        /** Set the color pointer using an osg::Array, and manage any VBO that are required.*/
        inline void setColorPointer(const Array* array)
        {
            if (array)
            {
                GLBufferObject* vbo = array->getOrCreateGLBufferObject(_contextID);
                if (vbo)
                {
                    bindVertexBufferObject(vbo);
                    setColorPointer(array->getDataSize(),array->getDataType(),0,(const GLvoid *)(vbo->getOffset(array->getBufferIndex())));
                }
                else
                {
                    unbindVertexBufferObject();
                    setColorPointer(array->getDataSize(),array->getDataType(),0,array->getDataPointer());
                }
            }
        }


        /** wrapper around glEnableClientState(GL_COLOR_ARRAY);glColorPointer(..);
          * note, only updates values that change.*/
        inline void setColorPointer( GLint size, GLenum type,
                                     GLsizei stride, const GLvoid *ptr )
        {
        #ifdef OSG_GL_VERTEX_ARRAY_FUNCS_AVAILABLE
            if (_useVertexAttributeAliasing)
            {
                setVertexAttribPointer(_colorAlias._location, size, type, GL_FALSE, stride, ptr);
            }
            else
            {
                if (!_colorArray._enabled || _colorArray.m_bDirty)
                {
                    _colorArray._enabled = true;
                    glEnableClientState(GL_COLOR_ARRAY);
                }
                //if (_colorArray._pointer!=ptr || _colorArray.m_bDirty)
                {
                    _colorArray._pointer=ptr;
                    glColorPointer( size, type, stride, ptr );
                }
                _colorArray._lazy_disable = false;
                _colorArray.m_bDirty = false;
            }
        #else
            setVertexAttribPointer(_colorAlias._location, size, type, GL_FALSE, stride, ptr);
        #endif
        }

        /** wrapper around glDisableClientState(GL_COLOR_ARRAY);
          * note, only updates values that change.*/
        inline void disableColorPointer()
        {
        #ifdef OSG_GL_VERTEX_ARRAY_FUNCS_AVAILABLE
            if (_useVertexAttributeAliasing)
            {
                disableVertexAttribPointer(_colorAlias._location);
            }
            else
            {
                if (_colorArray._enabled || _colorArray.m_bDirty)
                {
                    _colorArray._lazy_disable = false;
                    _colorArray._enabled = false;
                    _colorArray.m_bDirty = false;
                    glDisableClientState(GL_COLOR_ARRAY);
                }
            }
        #else
            disableVertexAttribPointer(_colorAlias._location);
        #endif
        }

        inline void dirtyColorPointer()
        {
        #ifdef OSG_GL_VERTEX_ARRAY_FUNCS_AVAILABLE
            if (_useVertexAttributeAliasing)
            {
                dirtyVertexAttribPointer(_colorAlias._location);
            }
            else
            {
                _colorArray._pointer = 0;
                _colorArray.m_bDirty = true;
            }
        #else
            dirtyVertexAttribPointer(_colorAlias._location);
        #endif
        }


        inline bool isSecondaryColorSupported() const { return _isSecondaryColorSupportResolved?_isSecondaryColorSupported:computeSecondaryColorSupported(); }


        /** Set the secondary color pointer using an osg::Array, and manage any VBO that are required.*/
        inline void setSecondaryColorPointer(const Array* array)
        {
            if (array)
            {
                GLBufferObject* vbo = array->getOrCreateGLBufferObject(_contextID);
                if (vbo)
                {
                    bindVertexBufferObject(vbo);
                    setSecondaryColorPointer(array->getDataSize(),array->getDataType(),0,(const GLvoid *)(vbo->getOffset(array->getBufferIndex())));
                }
                else
                {
                    unbindVertexBufferObject();
                    setSecondaryColorPointer(array->getDataSize(),array->getDataType(),0,array->getDataPointer());
                }
            }
        }

        /** wrapper around glEnableClientState(GL_SECONDARY_COLOR_ARRAY);glSecondayColorPointer(..);
          * note, only updates values that change.*/
        void setSecondaryColorPointer( GLint size, GLenum type, GLsizei stride, const GLvoid *ptr );

        /** wrapper around glDisableClientState(GL_SECONDARY_COLOR_ARRAY);
          * note, only updates values that change.*/
        inline void disableSecondaryColorPointer()
        {
        #ifdef OSG_GL_VERTEX_ARRAY_FUNCS_AVAILABLE
            if (_useVertexAttributeAliasing)
            {
                disableVertexAttribPointer(_secondaryColorAlias._location);
            }
            else
            {
                if (_secondaryColorArray._enabled || _secondaryColorArray.m_bDirty)
                {
                    _secondaryColorArray._lazy_disable = false;
                    _secondaryColorArray._enabled = false;
                    _secondaryColorArray.m_bDirty = false;
                    if (isSecondaryColorSupported()) glDisableClientState(GL_SECONDARY_COLOR_ARRAY);
                }
            }
        #else
            disableVertexAttribPointer(_secondaryColorAlias._location);
        #endif
        }

        inline void dirtySecondaryColorPointer()
        {
        #ifdef OSG_GL_VERTEX_ARRAY_FUNCS_AVAILABLE
            if (_useVertexAttributeAliasing)
            {
                dirtyVertexAttribPointer(_secondaryColorAlias._location);
            }
            else
            {
                _secondaryColorArray._pointer = 0;
                _secondaryColorArray.m_bDirty = true;
            }
        #else
            dirtyVertexAttribPointer(_secondaryColorAlias._location);
        #endif
        }

        inline bool isFogCoordSupported() const { return _isFogCoordSupportResolved?_isFogCoordSupported:computeFogCoordSupported(); }


        /** Set the fog coord pointer using an osg::Array, and manage any VBO that are required.*/
        inline void setFogCoordPointer(const Array* array)
        {
            if (array)
            {
                GLBufferObject* vbo = array->getOrCreateGLBufferObject(_contextID);
                if (vbo)
                {
                    bindVertexBufferObject(vbo);
                    setFogCoordPointer(array->getDataType(),0,(const GLvoid *)(vbo->getOffset(array->getBufferIndex())));
                }
                else
                {
                    unbindVertexBufferObject();
                    setFogCoordPointer(array->getDataType(),0,array->getDataPointer());
                }
            }
        }


        /** wrapper around glEnableClientState(GL_FOG_COORDINATE_ARRAY);glFogCoordPointer(..);
          * note, only updates values that change.*/
        void setFogCoordPointer( GLenum type, GLsizei stride, const GLvoid *ptr );

        /** wrapper around glDisableClientState(GL_FOG_COORDINATE_ARRAY);
          * note, only updates values that change.*/
        inline void disableFogCoordPointer()
        {
        #ifdef OSG_GL_VERTEX_ARRAY_FUNCS_AVAILABLE
            if (_useVertexAttributeAliasing)
            {
                disableVertexAttribPointer(_fogCoordAlias._location);
            }
            else
            {
                if (_fogArray._enabled || _fogArray.m_bDirty)
                {
                    _fogArray._lazy_disable = false;
                    _fogArray._enabled = false;
                    _fogArray.m_bDirty = false;
                    if (isFogCoordSupported()) glDisableClientState(GL_FOG_COORDINATE_ARRAY);
                }
            }
        #else
            disableVertexAttribPointer(_fogCoordAlias._location);
        #endif
        }

        inline void dirtyFogCoordPointer()
        {
        #ifdef OSG_GL_VERTEX_ARRAY_FUNCS_AVAILABLE
            if (_useVertexAttributeAliasing)
            {
                dirtyVertexAttribPointer(_fogCoordAlias._location);
            }
            else
            {
                _fogArray._pointer = 0;
                _fogArray.m_bDirty = true;
            }
        #else
            dirtyVertexAttribPointer(_fogCoordAlias._location);
        #endif
        }



        /** Set the tex coord pointer using an osg::Array, and manage any VBO that are required.*/
        inline void setTexCoordPointer(unsigned int unit, const Array* array)
        {
            if (array)
            {
                GLBufferObject* vbo = array->getOrCreateGLBufferObject(_contextID);
                if (vbo)
                {
                    bindVertexBufferObject(vbo);
                    setTexCoordPointer(unit, array->getDataSize(),array->getDataType(),0, (const GLvoid *)(vbo->getOffset(array->getBufferIndex())));
                }
                else
                {
                    unbindVertexBufferObject();
                    setTexCoordPointer(unit, array->getDataSize(),array->getDataType(),0,array->getDataPointer());
                }
            }
        }

        /** wrapper around glEnableClientState(GL_TEXTURE_COORD_ARRAY);glTexCoordPointer(..);
          * note, only updates values that change.*/
        inline void setTexCoordPointer( unsigned int unit,
                                        GLint size, GLenum type,
                                        GLsizei stride, const GLvoid *ptr )
        {
        #ifdef OSG_GL_VERTEX_ARRAY_FUNCS_AVAILABLE
            if (_useVertexAttributeAliasing)
            {
                setVertexAttribPointer(_texCoordAliasList[unit]._location, size, type, GL_FALSE, stride, ptr);
            }
            else
            {
                if (setClientActiveTextureUnit(unit))
                {
                    if ( unit >= _texCoordArrayList.size()) _texCoordArrayList.resize(unit+1);
                    EnabledArrayPair& eap = _texCoordArrayList[unit];

                    if (!eap._enabled || eap.m_bDirty)
                    {
                        eap._enabled = true;
                        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                    }
                    //if (eap._pointer!=ptr || eap.m_bDirty)
                    {
                        glTexCoordPointer( size, type, stride, ptr );
                        eap._pointer = ptr;
                    }
                    eap._lazy_disable = false;
                    eap.m_bDirty = false;
                }
            }
        #else
            setVertexAttribPointer(_texCoordAliasList[unit]._location, size, type, GL_FALSE, stride, ptr);
        #endif
        }

        /** wrapper around glDisableClientState(GL_TEXTURE_COORD_ARRAY);
          * note, only updates values that change.*/
        inline void disableTexCoordPointer( unsigned int unit )
        {
        #ifdef OSG_GL_VERTEX_ARRAY_FUNCS_AVAILABLE
            if (_useVertexAttributeAliasing)
            {
                disableVertexAttribPointer(_texCoordAliasList[unit]._location);
            }
            else
            {
                if (setClientActiveTextureUnit(unit))
                {
                    if ( unit >= _texCoordArrayList.size()) _texCoordArrayList.resize(unit+1);
                    EnabledArrayPair& eap = _texCoordArrayList[unit];

                    if (eap._enabled || eap.m_bDirty)
                    {
                        eap._lazy_disable = false;
                        eap._enabled = false;
                        eap.m_bDirty = false;
                        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
                    }
                }
            }
        #else
            disableVertexAttribPointer(_texCoordAliasList[unit]._location);
        #endif
        }

        inline void dirtyTexCoordPointer( unsigned int unit )
        {
        #ifdef OSG_GL_VERTEX_ARRAY_FUNCS_AVAILABLE
            if (_useVertexAttributeAliasing)
            {
                dirtyVertexAttribPointer(_texCoordAliasList[unit]._location);
            }
            else
            {
                if ( unit >= _texCoordArrayList.size()) return; // _texCoordArrayList.resize(unit+1);
                EnabledArrayPair& eap = _texCoordArrayList[unit];
                eap._pointer = 0;
                eap.m_bDirty = true;
            }
        #else
            dirtyVertexAttribPointer(_texCoordAliasList[unit]._location);
        #endif
        }


        inline void disableTexCoordPointersAboveAndIncluding( unsigned int unit )
        {
        #ifdef OSG_GL_VERTEX_ARRAY_FUNCS_AVAILABLE
            if (_useVertexAttributeAliasing)
            {
                disableVertexAttribPointersAboveAndIncluding(_texCoordAliasList[unit]._location);
            }
            else
            {
                while (unit<_texCoordArrayList.size())
                {
                    EnabledArrayPair& eap = _texCoordArrayList[unit];
                    if (eap._enabled || eap.m_bDirty)
                    {
                        if (setClientActiveTextureUnit(unit))
                        {
                            eap._lazy_disable = false;
                            eap._enabled = false;
                            eap.m_bDirty = false;
                            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
                        }
                    }
                    ++unit;
                }
            }
        #else
            disableVertexAttribPointersAboveAndIncluding(_texCoordAliasList[unit]._location);
        #endif
        }

        inline void dirtyTexCoordPointersAboveAndIncluding( unsigned int unit )
        {
        #ifdef OSG_GL_VERTEX_ARRAY_FUNCS_AVAILABLE
            if (_useVertexAttributeAliasing)
            {
                dirtyVertexAttribPointersAboveAndIncluding(_texCoordAliasList[unit]._location);
            }
            else
            {
                while (unit<_texCoordArrayList.size())
                {
                    EnabledArrayPair& eap = _texCoordArrayList[unit];
                    eap._pointer = 0;
                    eap.m_bDirty = true;
                    ++unit;
                }
            }
        #else
            dirtyVertexAttribPointersAboveAndIncluding(_texCoordAliasList[unit]._location);
        #endif
        }


        /** Set the current texture unit, return true if selected,
          * false if selection failed such as when multi texturing is not supported.
          * note, only updates values that change.*/
        inline bool setActiveTextureUnit( unsigned int unit );
        
        /** Get the current texture unit.*/
        unsigned int getActiveTextureUnit() const { return _currentActiveTextureUnit; }

        /** Set the current tex coord array texture unit, return true if selected,
          * false if selection failed such as when multi texturing is not supported.
          * note, only updates values that change.*/
        bool setClientActiveTextureUnit( unsigned int unit );

        /** Get the current tex coord array texture unit.*/
        unsigned int getClientActiveTextureUnit() const { return _currentClientActiveTextureUnit; }

        /** Set the vertex attrib pointer using an osg::Array, and manage any VBO that are required.*/
        inline void setVertexAttribPointer(unsigned int unit, const Array* array, GLboolean normalized)
        {
            if (array)
            {
                GLBufferObject* vbo = array->getOrCreateGLBufferObject(_contextID);
                if (vbo)
                {
                    bindVertexBufferObject(vbo);
                    setVertexAttribPointer(unit, array->getDataSize(),array->getDataType(),normalized,0,(const GLvoid *)(vbo->getOffset(array->getBufferIndex())));
                }
                else
                {
                    unbindVertexBufferObject();
                    setVertexAttribPointer(unit, array->getDataSize(),array->getDataType(),normalized,0,array->getDataPointer());
                }
            }
        }

        /** wrapper around glEnableVertexAttribArrayARB(index);glVertexAttribPointerARB(..);
          * note, only updates values that change.*/
        void setVertexAttribPointer( unsigned int index,
                                     GLint size, GLenum type, GLboolean normalized,
                                     GLsizei stride, const GLvoid *ptr );

        /** wrapper around DisableVertexAttribArrayARB(index);
          * note, only updates values that change.*/
        void disableVertexAttribPointer( unsigned int index );

        void disableVertexAttribPointersAboveAndIncluding( unsigned int index );

        inline void dirtyVertexAttribPointer( unsigned int index )
        {
            if (index<_vertexAttribArrayList.size())
            {
                EnabledArrayPair& eap = _vertexAttribArrayList[index];
                eap._pointer = 0;
                eap.m_bDirty = true;
            }
        }

        inline void dirtyVertexAttribPointersAboveAndIncluding( unsigned int index )
        {
            while (index<_vertexAttribArrayList.size())
            {
                EnabledArrayPair& eap = _vertexAttribArrayList[index];
                eap._pointer = 0;
                eap.m_bDirty = true;
                ++index;
            }
        }

        bool isVertexBufferObjectSupported() const { return _isVertexBufferObjectSupportResolved?_isVertexBufferObjectSupported:computeVertexBufferObjectSupported(); }


        inline void setLastAppliedProgramObject(const Program::PerContextProgram* program)
        {
            if (_lastAppliedProgramObject!=program)
            {
                _lastAppliedProgramObject = program;
                if (program && _appliedProgramObjectSet.count(program)==0)
                {
                    _appliedProgramObjectSet.insert(program);
                    program->addObserver(this);
                }
            }
        }
        inline const Program::PerContextProgram* getLastAppliedProgramObject() const { return _lastAppliedProgramObject; }

        inline GLint getUniformLocation( unsigned int uniformNameID ) const { return _lastAppliedProgramObject ? _lastAppliedProgramObject->getUniformLocation(uniformNameID) : -1; }
        /**
          * Alternative version of getUniformLocation( unsigned int uniformNameID )
          * retrofited into OSG for backward compatibility with osgCal,
          * after uniform ids were refactored from std::strings to GLints in OSG version 2.9.10.
          * 
          * Drawbacks: This method is not particularly fast. It has to access mutexed static 
          * map of uniform ids. So don't overuse it or your app performance will suffer.
        */
        inline GLint getUniformLocation( const std::string & uniformName ) const { return _lastAppliedProgramObject ? _lastAppliedProgramObject->getUniformLocation(uniformName) : -1; }
        inline GLint getAttribLocation( const std::string& name ) const { return _lastAppliedProgramObject ? _lastAppliedProgramObject->getAttribLocation(name) : -1; }

        typedef std::pair<const StateAttribute*,StateAttribute::OverrideValue>  AttributePair;
        typedef std::vector<AttributePair>                                      AttributeVec;

        AttributeVec& getAttributeVec( const osg::StateAttribute* attribute )
        {
                AttributeStack& as = _attributeMap[ attribute->getTypeMemberPair() ];
                return as.attributeVec;
        }

        /** Set the frame stamp for the current frame.*/
        inline void setFrameStamp(::CVBF_FrameStamp* fs) { _frameStamp = fs; }

        /** Get the frame stamp for the current frame.*/
        inline ::CVBF_FrameStamp* getFrameStamp() { return _frameStamp.get(); }

        /** Get the const frame stamp for the current frame.*/
        inline const ::CVBF_FrameStamp* getFrameStamp() const { return _frameStamp.get(); }


        /** Set the DisplaySettings. Note, nothing is applied, the visual settings are just
          * used in the State object to pass the current visual settings to Drawables
          * during rendering. */
        inline void setDisplaySettings(DisplaySettings* vs) { _displaySettings = vs; }

        /** Get the DisplaySettings */
        inline const DisplaySettings* getDisplaySettings() const { return _displaySettings.get(); }



        /** Set flag for early termination of the draw traversal.*/
        void setAbortRenderingPtr(bool* abortPtr) { _abortRenderingPtr = abortPtr; }

        /** Get flag for early termination of the draw traversal,
          * if true steps should be taken to complete rendering early.*/
        bool getAbortRendering() const { return _abortRenderingPtr!=0?(*_abortRenderingPtr):false; }


        struct DynamicObjectRenderingCompletedCallback : public ::CVBF_Referenced
        {
            virtual void completed(osg::State*) = 0;
        };

        /** Set the callback to be called when the dynamic object count hits 0.*/
        void setDynamicObjectRenderingCompletedCallback(DynamicObjectRenderingCompletedCallback* cb){ _completeDynamicObjectRenderingCallback = cb; }

        /** Get the callback to be called when the dynamic object count hits 0.*/
        DynamicObjectRenderingCompletedCallback* getDynamicObjectRenderingCompletedCallback() { return _completeDynamicObjectRenderingCallback.get(); }

        /** Set the number of dynamic objects that will be rendered in this graphics context this frame.*/
        void setDynamicObjectCount(unsigned int count, bool callCallbackOnZero = false)
        {
            if (_dynamicObjectCount != count)
            {
                _dynamicObjectCount = count;
                if (_dynamicObjectCount==0 && callCallbackOnZero && _completeDynamicObjectRenderingCallback.valid())
                {
                    _completeDynamicObjectRenderingCallback->completed(this);
                }
            }
        }

        /** Get the number of dynamic objects that will be rendered in this graphics context this frame.*/
        unsigned int getDynamicObjectCount() const { return _dynamicObjectCount; }
        
        inline void decrementDynamicObjectCount()
        {
            --_dynamicObjectCount;
            if (_dynamicObjectCount==0 && _completeDynamicObjectRenderingCallback.valid())
            {
                _completeDynamicObjectRenderingCallback->completed(this);
            }
        }

        void setMaxTexturePoolSize(unsigned int size);
        unsigned int getMaxTexturePoolSize() const { return _maxTexturePoolSize; }

        void setMaxBufferObjectPoolSize(unsigned int size);
        unsigned int getMaxBufferObjectPoolSize() const { return _maxBufferObjectPoolSize; }


        enum CheckForGLErrors
        {
            /** NEVER_CHECK_GL_ERRORS hints that OpenGL need not be checked for, this
                is the fastest option since checking for errors does incurr a small overhead.*/
            NEVER_CHECK_GL_ERRORS,
            /** ONCE_PER_FRAME means that OpenGl errors will be checked for once per
                frame, the overhead is still small, but at least OpenGL errors that are occurring
                will be caught, the reporting isn't fine grained enough for debugging purposes.*/
            ONCE_PER_FRAME,
            /** ONCE_PER_ATTRIBUTE means that OpenGL errors will be checked for after
                every attribute is applied, allow errors to be directly associated with
                particular operations which makes debugging much easier.*/
            ONCE_PER_ATTRIBUTE
        };

        /** Set whether and how often OpenGL errors should be checked for.*/
        void setCheckForGLErrors(CheckForGLErrors check) { _checkGLErrors = check; }

        /** Get whether and how often OpenGL errors should be checked for.*/
        CheckForGLErrors getCheckForGLErrors() const { return _checkGLErrors; }

        bool checkGLErrors(const char* str) const;
        bool checkGLErrors(StateAttribute::GLMode mode) const;
        bool checkGLErrors(const StateAttribute* attribute) const;

        /** print out the internal details of osg::State - useful for debugging.*/
        void print(std::ostream& fout) const;

        /** Initialize extension used by osg:::State.*/
        void initializeExtensionProcs();

        virtual void objectDeleted(void* object);

        /** Get the GL adapter object used to map OpenGL 1.0 glBegin/glEnd usage to vertex arrays.*/
        inline GLBeginEndAdapter& getGLBeginEndAdapter() { return _glBeginEndAdapter; }

        /** Get the helper class for dispatching osg::Arrays as OpenGL attribute data.*/
        inline ArrayDispatchers& getArrayDispatchers() { return _arrayDispatchers; }


        /** Set the helper class that provides applications with estimate on how much different graphics operations will cost.*/
        inline void setGraphicsCostEstimator(GraphicsCostEstimator* gce) { _graphicsCostEstimator = gce; }

        /** Get the helper class that provides applications with estimate on how much different graphics operations will cost.*/
        inline GraphicsCostEstimator* getGraphicsCostEstimator() { return _graphicsCostEstimator.get(); }

        /** Get the cont helper class that provides applications with estimate on how much different graphics operations will cost.*/
        inline const GraphicsCostEstimator* getGraphicsCostEstimator() const { return _graphicsCostEstimator.get(); }



        /** Support for synchronizing the system time and the timestamp
         * counter available with ARB_timer_query. Note that State
         * doesn't update these values itself.
         */
        CVBF_Timer_t getStartTick() const { return _startTick; }
        void setStartTick(CVBF_Timer_t tick) { _startTick = tick; }
        CVBF_Timer_t getGpuTick() const { return _gpuTick; }

        double getGpuTime() const
        {
            return CVBF_Timer::instance()->delta_s(_startTick, _gpuTick);
        }
        GLuint64EXT getGpuTimestamp() const { return _gpuTimestamp; }

        void setGpuTimestamp(CVBF_Timer_t tick, GLuint64EXT timestamp)
        {
            _gpuTick = tick;
            _gpuTimestamp = timestamp;
        }
        int getTimestampBits() const { return _timestampBits; }
        void setTimestampBits(int bits) { _timestampBits = bits; }

        /** called by the GraphicsContext just before GraphicsContext::swapBuffersImplementation().*/
        virtual void frameCompleted();

    protected:

        virtual ~State();

        GraphicsContext*            _graphicsContext;
        unsigned int                _contextID;

        bool                            _shaderCompositionEnabled;
        bool                            _shaderCompositionDirty;
        ref_ptr<ShaderComposer>    _shaderComposer;
        osg::Program*                   _currentShaderCompositionProgram;
        StateSet::UniformList           _currentShaderCompositionUniformList;

        ref_ptr< ::CVBF_FrameStamp >         _frameStamp;

        ref_ptr<const RefMatrix>    _identity;
        ref_ptr<const RefMatrix>    _initialViewMatrix;
        ref_ptr<const RefMatrix>    _projection;
        ref_ptr<const RefMatrix>    _modelView;
        ref_ptr<RefMatrix>          _modelViewCache;

        bool                        _useModelViewAndProjectionUniforms;
        ref_ptr<Uniform>            _modelViewMatrixUniform;
        ref_ptr<Uniform>            _projectionMatrixUniform;
        ref_ptr<Uniform>            _modelViewProjectionMatrixUniform;
        ref_ptr<Uniform>            _normalMatrixUniform;

        Matrix                      _initialInverseViewMatrix;

        ref_ptr<DisplaySettings>    _displaySettings;

        bool*                       _abortRenderingPtr;
        CheckForGLErrors            _checkGLErrors;


        bool                        _useVertexAttributeAliasing;
        VertexAttribAlias           _vertexAlias;
        VertexAttribAlias           _normalAlias;
        VertexAttribAlias           _colorAlias;
        VertexAttribAlias           _secondaryColorAlias;
        VertexAttribAlias           _fogCoordAlias;
        VertexAttribAliasList       _texCoordAliasList;

        Program::AttribBindingList  _attributeBindingList;

        void setUpVertexAttribAlias(VertexAttribAlias& alias, GLuint location, const std::string glName, const std::string osgName, const std::string& declaration);


        struct ModeStack
        {
            typedef std::vector<StateAttribute::GLModeValue> ValueVec;

            ModeStack()
            {
                valid = true;
                changed = false;
                last_applied_value = false;
                global_default_value = false;
            }

            void print(std::ostream& fout) const;

            bool        valid;
            bool        changed;
            bool        last_applied_value;
            bool        global_default_value;
            ValueVec    valueVec;
        };

        struct AttributeStack
        {
            AttributeStack()
            {
                changed = false;
                last_applied_attribute = 0L;
                last_applied_shadercomponent = 0L;
                global_default_attribute = 0L;
                
            }

            void print(std::ostream& fout) const;

            /** apply an attribute if required, passing in attribute and appropriate attribute stack */
            bool                    changed;
            const StateAttribute*   last_applied_attribute;
            const ShaderComponent*  last_applied_shadercomponent;
            ref_ptr<const StateAttribute> global_default_attribute;
            AttributeVec            attributeVec;
        };


        struct UniformStack
        {
            typedef std::pair<const Uniform*,StateAttribute::OverrideValue>         UniformPair;
            typedef std::vector<UniformPair>                                        UniformVec;

            UniformStack() {}

            void print(std::ostream& fout) const;

            UniformVec              uniformVec;
        };


		//h00017
		ref_ptr<const RefMatrix>    _worldMat;
		//h00017
        /** Apply an OpenGL mode if required, passing in mode, enable flag and
          * appropriate mode stack. This is a wrapper around \c glEnable() and
          * \c glDisable(), that just actually calls these functions if the
          * \c enabled flag is different than the current state.
          * @return \c true if the state was actually changed. \c false
          *         otherwise. Notice that a \c false return does not indicate
          *         an error, it just means that the mode was already set to the
          *         same value as the \c enabled parameter.
        */
        inline bool applyMode(StateAttribute::GLMode mode,bool enabled,ModeStack& ms)
        {
            if (ms.valid && ms.last_applied_value != enabled)
            {
                ms.last_applied_value = enabled;

                if (enabled) glEnable(mode);
                else glDisable(mode);

                if (_checkGLErrors==ONCE_PER_ATTRIBUTE) checkGLErrors(mode);

                return true;
            }
            else
                return false;
        }

        inline bool applyModeOnTexUnit(unsigned int unit,StateAttribute::GLMode mode,bool enabled,ModeStack& ms)
        {
            if (ms.valid && ms.last_applied_value != enabled)
            {
                if (setActiveTextureUnit(unit))
                {
                    ms.last_applied_value = enabled;

                    if (enabled) glEnable(mode);
                    else glDisable(mode);

                    if (_checkGLErrors==ONCE_PER_ATTRIBUTE) checkGLErrors(mode);

                    return true;
                }
                else
                    return false;
            }
            else
                return false;
        }

        /** apply an attribute if required, passing in attribute and appropriate attribute stack */
        inline bool applyAttribute(const StateAttribute* attribute,AttributeStack& as)
        {
            if (as.last_applied_attribute != attribute)
            {
                if (!as.global_default_attribute.valid()) as.global_default_attribute = dynamic_cast<StateAttribute*>(attribute->cloneType());

                as.last_applied_attribute = attribute;
                attribute->apply(*this);

                const ShaderComponent* sc = attribute->getShaderComponent();
                if (as.last_applied_shadercomponent != sc)
                {
                    as.last_applied_shadercomponent = sc;
                    _shaderCompositionDirty = true;
                }

                if (_checkGLErrors==ONCE_PER_ATTRIBUTE) checkGLErrors(attribute);

                return true;
            }
            else
                return false;
        }

        inline bool applyAttributeOnTexUnit(unsigned int unit,const StateAttribute* attribute,AttributeStack& as)
        {
            if (as.last_applied_attribute != attribute)
            {
                if (setActiveTextureUnit(unit))
                {
                    if (!as.global_default_attribute.valid()) as.global_default_attribute = dynamic_cast<StateAttribute*>(attribute->cloneType());

                    as.last_applied_attribute = attribute;
                    attribute->apply(*this);

                    const ShaderComponent* sc = attribute->getShaderComponent();
                    if (as.last_applied_shadercomponent != sc)
                    {
                        as.last_applied_shadercomponent = sc;
                        _shaderCompositionDirty = true;
                    }

                    if (_checkGLErrors==ONCE_PER_ATTRIBUTE) checkGLErrors(attribute);

                    return true;
                }
                else
                    return false;
            }
            else
                return false;
        }


        inline bool applyGlobalDefaultAttribute(AttributeStack& as)
        {
            if (as.last_applied_attribute != as.global_default_attribute.get())
            {
                as.last_applied_attribute = as.global_default_attribute.get();
                if (as.global_default_attribute.valid())
                {
                    as.global_default_attribute->apply(*this);
                    const ShaderComponent* sc = as.global_default_attribute->getShaderComponent();
                    if (as.last_applied_shadercomponent != sc)
                    {
                        as.last_applied_shadercomponent = sc;
                        _shaderCompositionDirty = true;
                    }

                    if (_checkGLErrors==ONCE_PER_ATTRIBUTE) checkGLErrors(as.global_default_attribute.get());
                }
                return true;
            }
            else
                return false;
        }

        inline bool applyGlobalDefaultAttributeOnTexUnit(unsigned int unit,AttributeStack& as)
        {
            if (as.last_applied_attribute != as.global_default_attribute.get())
            {
                if (setActiveTextureUnit(unit))
                {
                    as.last_applied_attribute = as.global_default_attribute.get();
                    if (as.global_default_attribute.valid())
                    {
                        as.global_default_attribute->apply(*this);
                        const ShaderComponent* sc = as.global_default_attribute->getShaderComponent();
                        if (as.last_applied_shadercomponent != sc)
                        {
                            as.last_applied_shadercomponent = sc;
                            _shaderCompositionDirty = true;
                        }
                        if (_checkGLErrors==ONCE_PER_ATTRIBUTE) checkGLErrors(as.global_default_attribute.get());
                    }
                    return true;
                }
                else
                    return false;
            }
            else
                return false;
        }


        typedef std::map<StateAttribute::GLMode,ModeStack>              ModeMap;
        typedef std::vector<ModeMap>                                    TextureModeMapList;

        typedef std::map<StateAttribute::TypeMemberPair,AttributeStack> AttributeMap;
        typedef std::vector<AttributeMap>                               TextureAttributeMapList;

        typedef std::map<std::string,UniformStack>                      UniformMap;

        typedef std::vector<ref_ptr<const Matrix> >                     MatrixStack;

        typedef std::set<const Program::PerContextProgram* >            AppliedProgramObjectSet;

   
		ModeMap                                                         _modeMap;
        AttributeMap                                                    _attributeMap;



        UniformMap                                                      _uniformMap;

        TextureModeMapList                                              _textureModeMapList;
        TextureAttributeMapList                                         _textureAttributeMapList;

        AppliedProgramObjectSet                                         _appliedProgramObjectSet;
        const Program::PerContextProgram*                               _lastAppliedProgramObject;

        StateSetStack                                                   _stateStateStack;

        unsigned int                                                    _maxTexturePoolSize;
        unsigned int                                                    _maxBufferObjectPoolSize;


        struct EnabledArrayPair
        {
            EnabledArrayPair():_lazy_disable(false),m_bDirty(true),_enabled(false),_normalized(0),_pointer(0) {}
            EnabledArrayPair(const EnabledArrayPair& eap):_lazy_disable(eap._lazy_disable),m_bDirty(eap.m_bDirty), _enabled(eap._enabled),_normalized(eap._normalized),_pointer(eap._pointer) {}
            EnabledArrayPair& operator = (const EnabledArrayPair& eap) { _lazy_disable = eap._lazy_disable;m_bDirty=eap.m_bDirty; _enabled=eap._enabled; _normalized=eap._normalized;_pointer=eap._pointer; return *this; }

            bool            _lazy_disable;
            bool            m_bDirty;
            bool            _enabled;
            GLboolean       _normalized;
            const GLvoid*   _pointer;
        };

        typedef std::vector<EnabledArrayPair>                   EnabledTexCoordArrayList;
        typedef std::vector<EnabledArrayPair>                   EnabledVertexAttribArrayList;

        EnabledArrayPair                _vertexArray;
        EnabledArrayPair                _normalArray;
        EnabledArrayPair                _colorArray;
        EnabledArrayPair                _secondaryColorArray;
        EnabledArrayPair                _fogArray;
        EnabledTexCoordArrayList        _texCoordArrayList;
        EnabledVertexAttribArrayList    _vertexAttribArrayList;

        unsigned int                    _currentActiveTextureUnit;
        unsigned int                    _currentClientActiveTextureUnit;
        GLBufferObject*                 _currentVBO;
        GLBufferObject*                 _currentEBO;
        GLBufferObject*                 _currentPBO;


        inline ModeMap& getOrCreateTextureModeMap(unsigned int unit)
        {
            if (unit>=_textureModeMapList.size()) _textureModeMapList.resize(unit+1);
            return _textureModeMapList[unit];
        }


        inline AttributeMap& getOrCreateTextureAttributeMap(unsigned int unit)
        {
            if (unit>=_textureAttributeMapList.size()) _textureAttributeMapList.resize(unit+1);
            return _textureAttributeMapList[unit];
        }

        inline void pushModeList(ModeMap& modeMap,const StateSet::ModeList& modeList);
        inline void pushAttributeList(AttributeMap& attributeMap,const StateSet::AttributeList& attributeList);
        inline void pushUniformList(UniformMap& uniformMap,const StateSet::UniformList& uniformList);

        inline void popModeList(ModeMap& modeMap,const StateSet::ModeList& modeList);
        inline void popAttributeList(AttributeMap& attributeMap,const StateSet::AttributeList& attributeList);
        inline void popUniformList(UniformMap& uniformMap,const StateSet::UniformList& uniformList);

        inline void applyModeList(ModeMap& modeMap,const StateSet::ModeList& modeList);
        inline void applyAttributeList(AttributeMap& attributeMap,const StateSet::AttributeList& attributeList);
        inline void applyUniformList(UniformMap& uniformMap,const StateSet::UniformList& uniformList);

        inline void applyModeMap(ModeMap& modeMap);
        inline void applyAttributeMap(AttributeMap& attributeMap);
        inline void applyUniformMap(UniformMap& uniformMap);

        inline void applyModeListOnTexUnit(unsigned int unit,ModeMap& modeMap,const StateSet::ModeList& modeList);
        inline void applyAttributeListOnTexUnit(unsigned int unit,AttributeMap& attributeMap,const StateSet::AttributeList& attributeList);

        inline void applyModeMapOnTexUnit(unsigned int unit,ModeMap& modeMap);
        inline void applyAttributeMapOnTexUnit(unsigned int unit,AttributeMap& attributeMap);

        void haveAppliedMode(ModeMap& modeMap,StateAttribute::GLMode mode,StateAttribute::GLModeValue value);
        void haveAppliedMode(ModeMap& modeMap,StateAttribute::GLMode mode);
        void haveAppliedAttribute(AttributeMap& attributeMap,const StateAttribute* attribute);
        void haveAppliedAttribute(AttributeMap& attributeMap,StateAttribute::Type type, unsigned int member);
        bool getLastAppliedMode(const ModeMap& modeMap,StateAttribute::GLMode mode) const;
        const StateAttribute* getLastAppliedAttribute(const AttributeMap& attributeMap,StateAttribute::Type type, unsigned int member) const;

        void loadModelViewMatrix();


        mutable bool _isSecondaryColorSupportResolved;
        mutable bool _isSecondaryColorSupported;
        bool computeSecondaryColorSupported() const;

        mutable bool _isFogCoordSupportResolved;
        mutable bool _isFogCoordSupported;
        bool computeFogCoordSupported() const;

        mutable bool _isVertexBufferObjectSupportResolved;
        mutable bool _isVertexBufferObjectSupported;
        bool computeVertexBufferObjectSupported() const;

        typedef void (GL_APIENTRY * ActiveTextureProc) (GLenum texture);
        typedef void (GL_APIENTRY * FogCoordPointerProc) (GLenum type, GLsizei stride, const GLvoid *pointer);
        typedef void (GL_APIENTRY * SecondaryColorPointerProc) (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
        typedef void (GL_APIENTRY * MultiTexCoord4fProc) (GLenum target, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
        typedef void (GL_APIENTRY * VertexAttrib4fProc)(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
        typedef void (GL_APIENTRY * VertexAttrib4fvProc)(GLuint index, const GLfloat *v);
        typedef void (GL_APIENTRY * VertexAttribPointerProc) (unsigned int, GLint, GLenum, GLboolean normalized, GLsizei stride, const GLvoid *pointer);
        typedef void (GL_APIENTRY * EnableVertexAttribProc) (unsigned int);
        typedef void (GL_APIENTRY * DisableVertexAttribProc) (unsigned int);
        typedef void (GL_APIENTRY * BindBufferProc) (GLenum target, GLuint buffer);

        typedef void (GL_APIENTRY * DrawArraysInstancedProc)( GLenum mode, GLint first, GLsizei count, GLsizei primcount );
        typedef void (GL_APIENTRY * DrawElementsInstancedProc)( GLenum mode, GLsizei count, GLenum type, const GLvoid *indices, GLsizei primcount );

        bool                        _extensionProcsInitialized;
        GLint                       _glMaxTextureCoords;
        GLint                       _glMaxTextureUnits;
        ActiveTextureProc           _glClientActiveTexture;
        ActiveTextureProc           _glActiveTexture;
        MultiTexCoord4fProc         _glMultiTexCoord4f;
        VertexAttrib4fProc          _glVertexAttrib4f;
        VertexAttrib4fvProc         _glVertexAttrib4fv;
        FogCoordPointerProc         _glFogCoordPointer;
        SecondaryColorPointerProc   _glSecondaryColorPointer;
        VertexAttribPointerProc     _glVertexAttribPointer;
        EnableVertexAttribProc      _glEnableVertexAttribArray;
        DisableVertexAttribProc     _glDisableVertexAttribArray;
        BindBufferProc              _glBindBuffer;
        DrawArraysInstancedProc     _glDrawArraysInstanced;
        DrawElementsInstancedProc   _glDrawElementsInstanced;

        unsigned int                                            _dynamicObjectCount;
        ref_ptr<DynamicObjectRenderingCompletedCallback>   _completeDynamicObjectRenderingCallback;

        GLBeginEndAdapter           _glBeginEndAdapter;
        ArrayDispatchers            _arrayDispatchers;

        ref_ptr<GraphicsCostEstimator> _graphicsCostEstimator;
        
        CVBF_Timer_t                      _startTick;
        CVBF_Timer_t                      _gpuTick;
        GLuint64EXT                 _gpuTimestamp;
        int                         _timestampBits;
};

inline void State::pushModeList(ModeMap& modeMap,const StateSet::ModeList& modeList)
{
    for(StateSet::ModeList::const_iterator mitr=modeList.begin();
        mitr!=modeList.end();
        ++mitr)
    {
        // get the mode stack for incoming GLmode {mitr->first}.
        ModeStack& ms = modeMap[mitr->first];
        if (ms.valueVec.empty())
        {
            // first pair so simply push incoming pair to back.
            ms.valueVec.push_back(mitr->second);
        }
        else if ((ms.valueVec.back() & StateAttribute::OVERRIDE) && !(mitr->second & StateAttribute::PROTECTED)) // check the existing override flag
        {
            // push existing back since override keeps the previous value.
            ms.valueVec.push_back(ms.valueVec.back());
        }
        else
        {
            // no override on so simply push incoming pair to back.
            ms.valueVec.push_back(mitr->second);
        }
        ms.changed = true;
    }
}

inline void State::pushAttributeList(AttributeMap& attributeMap,const StateSet::AttributeList& attributeList)
{
    for(StateSet::AttributeList::const_iterator aitr=attributeList.begin();
        aitr!=attributeList.end();
        ++aitr)
    {
        // get the attribute stack for incoming type {aitr->first}.
        AttributeStack& as = attributeMap[aitr->first];
        if (as.attributeVec.empty())
        {
            // first pair so simply push incoming pair to back.
            as.attributeVec.push_back(
                AttributePair(aitr->second.first.get(),aitr->second.second));
        }
        else if ((as.attributeVec.back().second & StateAttribute::OVERRIDE) && !(aitr->second.second & StateAttribute::PROTECTED)) // check the existing override flag
        {
            // push existing back since override keeps the previous value.
            as.attributeVec.push_back(as.attributeVec.back());
        }
        else
        {
            // no override on so simply push incoming pair to back.
            as.attributeVec.push_back(
                AttributePair(aitr->second.first.get(),aitr->second.second));
        }
        as.changed = true;
    }
}


inline void State::pushUniformList(UniformMap& uniformMap,const StateSet::UniformList& uniformList)
{
    for(StateSet::UniformList::const_iterator aitr=uniformList.begin();
        aitr!=uniformList.end();
        ++aitr)
    {
        // get the attribute stack for incoming type {aitr->first}.
        UniformStack& us = uniformMap[aitr->first];
        if (us.uniformVec.empty())
        {
            // first pair so simply push incoming pair to back.
            us.uniformVec.push_back(
                UniformStack::UniformPair(aitr->second.first.get(),aitr->second.second));
        }
        else if ((us.uniformVec.back().second & StateAttribute::OVERRIDE) && !(aitr->second.second & StateAttribute::PROTECTED)) // check the existing override flag
        {
            // push existing back since override keeps the previous value.
            us.uniformVec.push_back(us.uniformVec.back());
        }
        else
        {
            // no override on so simply push incoming pair to back.
            us.uniformVec.push_back(
                UniformStack::UniformPair(aitr->second.first.get(),aitr->second.second));
        }
    }
}

inline void State::popModeList(ModeMap& modeMap,const StateSet::ModeList& modeList)
{
    for(StateSet::ModeList::const_iterator mitr=modeList.begin();
        mitr!=modeList.end();
        ++mitr)
    {
        // get the mode stack for incoming GLmode {mitr->first}.
        ModeStack& ms = modeMap[mitr->first];
        if (!ms.valueVec.empty())
        {
            ms.valueVec.pop_back();
        }
        ms.changed = true;
    }
}

inline void State::popAttributeList(AttributeMap& attributeMap,const StateSet::AttributeList& attributeList)
{
    for(StateSet::AttributeList::const_iterator aitr=attributeList.begin();
        aitr!=attributeList.end();
        ++aitr)
    {
        // get the attribute stack for incoming type {aitr->first}.
        AttributeStack& as = attributeMap[aitr->first];
        if (!as.attributeVec.empty())
        {
            as.attributeVec.pop_back();
        }
        as.changed = true;
    }
}

inline void State::popUniformList(UniformMap& uniformMap,const StateSet::UniformList& uniformList)
{
    for(StateSet::UniformList::const_iterator aitr=uniformList.begin();
        aitr!=uniformList.end();
        ++aitr)
    {
        // get the attribute stack for incoming type {aitr->first}.
        UniformStack& us = uniformMap[aitr->first];
        if (!us.uniformVec.empty())
        {
            us.uniformVec.pop_back();
        }
    }
}

inline void State::applyModeList(ModeMap& modeMap,const StateSet::ModeList& modeList)
{
    StateSet::ModeList::const_iterator ds_mitr = modeList.begin();
    ModeMap::iterator this_mitr=modeMap.begin();

    while (this_mitr!=modeMap.end() && ds_mitr!=modeList.end())
    {
        if (this_mitr->first<ds_mitr->first)
        {

            // note GLMode = this_mitr->first
            ModeStack& ms = this_mitr->second;
            if (ms.changed)
            {
                ms.changed = false;
                if (!ms.valueVec.empty())
                {
                    bool new_value = ms.valueVec.back() & StateAttribute::ON;
                    applyMode(this_mitr->first,new_value,ms);
                }
                else
                {
                    // assume default of disabled.
                    applyMode(this_mitr->first,ms.global_default_value,ms);

                }

            }

            ++this_mitr;

        }
        else if (ds_mitr->first<this_mitr->first)
        {

            // ds_mitr->first is a new mode, therefore
            // need to insert a new mode entry for ds_mistr->first.
            ModeStack& ms = modeMap[ds_mitr->first];

            bool new_value = ds_mitr->second & StateAttribute::ON;
            applyMode(ds_mitr->first,new_value,ms);

            // will need to disable this mode on next apply so set it to changed.
            ms.changed = true;

            ++ds_mitr;

        }
        else
        {
            // this_mitr & ds_mitr refer to the same mode, check the override
            // if any otherwise just apply the incoming mode.

            ModeStack& ms = this_mitr->second;

            if (!ms.valueVec.empty() && (ms.valueVec.back() & StateAttribute::OVERRIDE) && !(ds_mitr->second & StateAttribute::PROTECTED))
            {
                // override is on, just treat as a normal apply on modes.

                if (ms.changed)
                {
                    ms.changed = false;
                    bool new_value = ms.valueVec.back() & StateAttribute::ON;
                    applyMode(this_mitr->first,new_value,ms);

                }
            }
            else
            {
                // no override on or no previous entry, therefore consider incoming mode.
                bool new_value = ds_mitr->second & StateAttribute::ON;
                if (applyMode(ds_mitr->first,new_value,ms))
                {
                    ms.changed = true;
                }
            }

            ++this_mitr;
            ++ds_mitr;
        }
    }

    // iterator over the remaining state modes to apply any previous changes.
    for(;
        this_mitr!=modeMap.end();
        ++this_mitr)
    {
        // note GLMode = this_mitr->first
        ModeStack& ms = this_mitr->second;
        if (ms.changed)
        {
            ms.changed = false;
            if (!ms.valueVec.empty())
            {
                bool new_value = ms.valueVec.back() & StateAttribute::ON;
                applyMode(this_mitr->first,new_value,ms);
            }
            else
            {
                // assume default of disabled.
                applyMode(this_mitr->first,ms.global_default_value,ms);

            }

        }
    }

    // iterator over the remaining incoming modes to apply any new mode.
    for(;
        ds_mitr!=modeList.end();
        ++ds_mitr)
    {
        ModeStack& ms = modeMap[ds_mitr->first];

        bool new_value = ds_mitr->second & StateAttribute::ON;
        applyMode(ds_mitr->first,new_value,ms);

        // will need to disable this mode on next apply so set it to changed.
        ms.changed = true;
    }
}

inline void State::applyModeListOnTexUnit(unsigned int unit,ModeMap& modeMap,const StateSet::ModeList& modeList)
{
    StateSet::ModeList::const_iterator ds_mitr = modeList.begin();
    ModeMap::iterator this_mitr=modeMap.begin();

    while (this_mitr!=modeMap.end() && ds_mitr!=modeList.end())
    {
        if (this_mitr->first<ds_mitr->first)
        {

            // note GLMode = this_mitr->first
            ModeStack& ms = this_mitr->second;
            if (ms.changed)
            {
                ms.changed = false;
                if (!ms.valueVec.empty())
                {
                    bool new_value = ms.valueVec.back() & StateAttribute::ON;
                    applyModeOnTexUnit(unit,this_mitr->first,new_value,ms);
                }
                else
                {
                    // assume default of disabled.
                    applyModeOnTexUnit(unit,this_mitr->first,ms.global_default_value,ms);

                }

            }

            ++this_mitr;

        }
        else if (ds_mitr->first<this_mitr->first)
        {

            // ds_mitr->first is a new mode, therefore
            // need to insert a new mode entry for ds_mistr->first.
            ModeStack& ms = modeMap[ds_mitr->first];

            bool new_value = ds_mitr->second & StateAttribute::ON;
            applyModeOnTexUnit(unit,ds_mitr->first,new_value,ms);

            // will need to disable this mode on next apply so set it to changed.
            ms.changed = true;

            ++ds_mitr;

        }
        else
        {
            // this_mitr & ds_mitr refer to the same mode, check the override
            // if any otherwise just apply the incoming mode.

            ModeStack& ms = this_mitr->second;

            if (!ms.valueVec.empty() && (ms.valueVec.back() & StateAttribute::OVERRIDE) && !(ds_mitr->second & StateAttribute::PROTECTED))
            {
                // override is on, just treat as a normal apply on modes.

                if (ms.changed)
                {
                    ms.changed = false;
                    bool new_value = ms.valueVec.back() & StateAttribute::ON;
                    applyModeOnTexUnit(unit,this_mitr->first,new_value,ms);

                }
            }
            else
            {
                // no override on or no previous entry, therefore consider incoming mode.
                bool new_value = ds_mitr->second & StateAttribute::ON;
                if (applyModeOnTexUnit(unit,ds_mitr->first,new_value,ms))
                {
                    ms.changed = true;
                }
            }

            ++this_mitr;
            ++ds_mitr;
        }
    }

    // iterator over the remaining state modes to apply any previous changes.
    for(;
        this_mitr!=modeMap.end();
        ++this_mitr)
    {
        // note GLMode = this_mitr->first
        ModeStack& ms = this_mitr->second;
        if (ms.changed)
        {
            ms.changed = false;
            if (!ms.valueVec.empty())
            {
                bool new_value = ms.valueVec.back() & StateAttribute::ON;
                applyModeOnTexUnit(unit,this_mitr->first,new_value,ms);
            }
            else
            {
                // assume default of disabled.
                applyModeOnTexUnit(unit,this_mitr->first,ms.global_default_value,ms);

            }

        }
    }

    // iterator over the remaining incoming modes to apply any new mode.
    for(;
        ds_mitr!=modeList.end();
        ++ds_mitr)
    {
        ModeStack& ms = modeMap[ds_mitr->first];

        bool new_value = ds_mitr->second & StateAttribute::ON;
        applyModeOnTexUnit(unit,ds_mitr->first,new_value,ms);

        // will need to disable this mode on next apply so set it to changed.
        ms.changed = true;
    }
}

inline void State::applyAttributeList(AttributeMap& attributeMap,const StateSet::AttributeList& attributeList)
{
    StateSet::AttributeList::const_iterator ds_aitr=attributeList.begin();

    AttributeMap::iterator this_aitr=attributeMap.begin();

    while (this_aitr!=attributeMap.end() && ds_aitr!=attributeList.end())
    {
        if (this_aitr->first<ds_aitr->first)
        {

            // note attribute type = this_aitr->first
            AttributeStack& as = this_aitr->second;
            if (as.changed)
            {
                as.changed = false;
                if (!as.attributeVec.empty())
                {
                    const StateAttribute* new_attr = as.attributeVec.back().first;
                    applyAttribute(new_attr,as);
                }
                else
                {
                    applyGlobalDefaultAttribute(as);
                }
            }

            ++this_aitr;

        }
        else if (ds_aitr->first<this_aitr->first)
        {

            // ds_aitr->first is a new attribute, therefore
            // need to insert a new attribute entry for ds_aitr->first.
            AttributeStack& as = attributeMap[ds_aitr->first];

            const StateAttribute* new_attr = ds_aitr->second.first.get();
            applyAttribute(new_attr,as);

            as.changed = true;

            ++ds_aitr;

        }
        else
        {
            // this_mitr & ds_mitr refer to the same attribute, check the override
            // if any otherwise just apply the incoming attribute

            AttributeStack& as = this_aitr->second;

            if (!as.attributeVec.empty() && (as.attributeVec.back().second & StateAttribute::OVERRIDE) && !(ds_aitr->second.second & StateAttribute::PROTECTED))
            {
                // override is on, just treat as a normal apply on attribute.

                if (as.changed)
                {
                    as.changed = false;
                    const StateAttribute* new_attr = as.attributeVec.back().first;
                    applyAttribute(new_attr,as);
                }
            }
            else
            {
                // no override on or no previous entry, therefore consider incoming attribute.
                const StateAttribute* new_attr = ds_aitr->second.first.get();
                if (applyAttribute(new_attr,as))
                {
                    as.changed = true;
                }
            }

            ++this_aitr;
            ++ds_aitr;
        }
    }

    // iterator over the remaining state attributes to apply any previous changes.
    for(; this_aitr!=attributeMap.end(); ++this_aitr)
    {
        // note attribute type = this_aitr->first
        AttributeStack& as = this_aitr->second;
        if (as.changed)
        {
            as.changed = false;
            if (!as.attributeVec.empty())
            {
                const StateAttribute* new_attr = as.attributeVec.back().first;
                applyAttribute(new_attr,as);
            }
            else
            {
                applyGlobalDefaultAttribute(as);
            }
        }
    }

    // iterator over the remaining incoming attribute to apply any new attribute.
    for(;
        ds_aitr!=attributeList.end();
        ++ds_aitr)
    {
        // ds_aitr->first is a new attribute, therefore
        // need to insert a new attribute entry for ds_aitr->first.
        AttributeStack& as = attributeMap[ds_aitr->first];

        const StateAttribute* new_attr = ds_aitr->second.first.get();
        applyAttribute(new_attr,as);

        // will need to update this attribute on next apply so set it to changed.
        as.changed = true;
    }

}

inline void State::applyAttributeListOnTexUnit(unsigned int unit,AttributeMap& attributeMap,const StateSet::AttributeList& attributeList)
{
    StateSet::AttributeList::const_iterator ds_aitr=attributeList.begin();

    AttributeMap::iterator this_aitr=attributeMap.begin();

    while (this_aitr!=attributeMap.end() && ds_aitr!=attributeList.end())
    {
        if (this_aitr->first<ds_aitr->first)
        {

            // note attribute type = this_aitr->first
            AttributeStack& as = this_aitr->second;
            if (as.changed)
            {
                as.changed = false;
                if (!as.attributeVec.empty())
                {
                    const StateAttribute* new_attr = as.attributeVec.back().first;
                    applyAttributeOnTexUnit(unit,new_attr,as);
                }
                else
                {
                    applyGlobalDefaultAttributeOnTexUnit(unit,as);
                }
            }

            ++this_aitr;

        }
        else if (ds_aitr->first<this_aitr->first)
        {

            // ds_aitr->first is a new attribute, therefore
            // need to insert a new attribute entry for ds_aitr->first.
            AttributeStack& as = attributeMap[ds_aitr->first];

            const StateAttribute* new_attr = ds_aitr->second.first.get();
            applyAttributeOnTexUnit(unit,new_attr,as);

            as.changed = true;

            ++ds_aitr;

        }
        else
        {
            // this_mitr & ds_mitr refer to the same attribute, check the override
            // if any otherwise just apply the incoming attribute

            AttributeStack& as = this_aitr->second;

            if (!as.attributeVec.empty() && (as.attributeVec.back().second & StateAttribute::OVERRIDE) && !(ds_aitr->second.second & StateAttribute::PROTECTED))
            {
                // override is on, just treat as a normal apply on attribute.

                if (as.changed)
                {
                    as.changed = false;
                    const StateAttribute* new_attr = as.attributeVec.back().first;
                    applyAttributeOnTexUnit(unit,new_attr,as);
                }
            }
            else
            {
                // no override on or no previous entry, therefore consider incoming attribute.
                const StateAttribute* new_attr = ds_aitr->second.first.get();
                if (applyAttributeOnTexUnit(unit,new_attr,as))
                {
                    as.changed = true;
                }
            }

            ++this_aitr;
            ++ds_aitr;
        }
    }

    // iterator over the remaining state attributes to apply any previous changes.
    for(;
        this_aitr!=attributeMap.end();
        ++this_aitr)
    {
        // note attribute type = this_aitr->first
        AttributeStack& as = this_aitr->second;
        if (as.changed)
        {
            as.changed = false;
            if (!as.attributeVec.empty())
            {
                const StateAttribute* new_attr = as.attributeVec.back().first;
                applyAttributeOnTexUnit(unit,new_attr,as);
            }
            else
            {
                applyGlobalDefaultAttributeOnTexUnit(unit,as);
            }
        }
    }

    // iterator over the remaining incoming attribute to apply any new attribute.
    for(;
        ds_aitr!=attributeList.end();
        ++ds_aitr)
    {
        // ds_aitr->first is a new attribute, therefore
        // need to insert a new attribute entry for ds_aitr->first.
        AttributeStack& as = attributeMap[ds_aitr->first];

        const StateAttribute* new_attr = ds_aitr->second.first.get();
        applyAttributeOnTexUnit(unit,new_attr,as);

        // will need to update this attribute on next apply so set it to changed.
        as.changed = true;
    }

}

inline void State::applyUniformList(UniformMap& uniformMap,const StateSet::UniformList& uniformList)
{
    if (!_lastAppliedProgramObject) return;

    StateSet::UniformList::const_iterator ds_aitr=uniformList.begin();

    UniformMap::iterator this_aitr=uniformMap.begin();

    while (this_aitr!=uniformMap.end() && ds_aitr!=uniformList.end())
    {
        if (this_aitr->first<ds_aitr->first)
        {
            // note attribute type = this_aitr->first
            UniformStack& as = this_aitr->second;
            if (!as.uniformVec.empty())
            {
                _lastAppliedProgramObject->apply(*as.uniformVec.back().first);
            }

            ++this_aitr;

        }
        else if (ds_aitr->first<this_aitr->first)
        {
            _lastAppliedProgramObject->apply(*(ds_aitr->second.first.get()));

            ++ds_aitr;
        }
        else
        {
            // this_mitr & ds_mitr refer to the same attribute, check the override
            // if any otherwise just apply the incoming attribute

            UniformStack& as = this_aitr->second;

            if (!as.uniformVec.empty() && (as.uniformVec.back().second & StateAttribute::OVERRIDE) && !(ds_aitr->second.second & StateAttribute::PROTECTED))
            {
                // override is on, just treat as a normal apply on uniform.
                _lastAppliedProgramObject->apply(*as.uniformVec.back().first);
            }
            else
            {
                // no override on or no previous entry, therefore consider incoming attribute.
                _lastAppliedProgramObject->apply(*(ds_aitr->second.first.get()));
            }

            ++this_aitr;
            ++ds_aitr;
        }
    }

    // iterator over the remaining state attributes to apply any previous changes.
    for(; this_aitr!=uniformMap.end(); ++this_aitr)
    {
        // note attribute type = this_aitr->first
        UniformStack& as = this_aitr->second;
        if (!as.uniformVec.empty())
        {
            _lastAppliedProgramObject->apply(*as.uniformVec.back().first);
        }
    }

    // iterator over the remaining incoming attribute to apply any new attribute.
    for(; ds_aitr!=uniformList.end(); ++ds_aitr)
    {
        _lastAppliedProgramObject->apply(*(ds_aitr->second.first.get()));
    }

}

inline void State::applyModeMap(ModeMap& modeMap)
{
    for(ModeMap::iterator mitr=modeMap.begin(); mitr!=modeMap.end(); ++mitr)
    {
        // note GLMode = mitr->first
        ModeStack& ms = mitr->second;
        if (ms.changed)
        {
            ms.changed = false;
            if (!ms.valueVec.empty())
            {
                bool new_value = ms.valueVec.back() & StateAttribute::ON;
                applyMode(mitr->first,new_value,ms);
            }
            else
            {
                // assume default of disabled.
                applyMode(mitr->first,ms.global_default_value,ms);
            }

        }
    }
}

inline void State::applyModeMapOnTexUnit(unsigned int unit,ModeMap& modeMap)
{
    for(ModeMap::iterator mitr=modeMap.begin(); mitr!=modeMap.end(); ++mitr)
    {
        // note GLMode = mitr->first
        ModeStack& ms = mitr->second;
        if (ms.changed)
        {
            ms.changed = false;
            if (!ms.valueVec.empty())
            {
                bool new_value = ms.valueVec.back() & StateAttribute::ON;
                applyModeOnTexUnit(unit,mitr->first,new_value,ms);
            }
            else
            {
                // assume default of disabled.
                applyModeOnTexUnit(unit,mitr->first,ms.global_default_value,ms);
            }

        }
    }
}

inline void State::applyAttributeMap(AttributeMap& attributeMap)
{
    for(AttributeMap::iterator aitr=attributeMap.begin(); aitr!=attributeMap.end(); ++aitr)
    {
        AttributeStack& as = aitr->second;
        if (as.changed)
        {
            as.changed = false;
            if (!as.attributeVec.empty())
            {
                const StateAttribute* new_attr = as.attributeVec.back().first;
                applyAttribute(new_attr,as);
            }
            else
            {
                applyGlobalDefaultAttribute(as);
            }

        }
    }
}

inline void State::applyAttributeMapOnTexUnit(unsigned int unit,AttributeMap& attributeMap)
{
    for(AttributeMap::iterator aitr=attributeMap.begin(); aitr!=attributeMap.end(); ++aitr)
    {
        AttributeStack& as = aitr->second;
        if (as.changed)
        {
            as.changed = false;
            if (!as.attributeVec.empty())
            {
                const StateAttribute* new_attr = as.attributeVec.back().first;
                applyAttributeOnTexUnit(unit,new_attr,as);
            }
            else
            {
                applyGlobalDefaultAttributeOnTexUnit(unit,as);
            }
        }
    }
}

inline void State::applyUniformMap(UniformMap& uniformMap)
{
    if (!_lastAppliedProgramObject) return;

    for(UniformMap::iterator aitr=uniformMap.begin(); aitr!=uniformMap.end(); ++aitr)
    {
        UniformStack& as = aitr->second;
        if (!as.uniformVec.empty())
        {
            _lastAppliedProgramObject->apply(*as.uniformVec.back().first);
        }
    }
}

inline bool State::setActiveTextureUnit( unsigned int unit )
{
	if (unit==_currentActiveTextureUnit) return true;
   
	if (_glActiveTexture && unit < (unsigned int)(maximum(_glMaxTextureCoords,_glMaxTextureUnits)) )
	{
		_glActiveTexture(GL_TEXTURE0+unit);
		_currentActiveTextureUnit = unit;
		return true;
	}
	else return unit==0;
	
}

}

#endif
