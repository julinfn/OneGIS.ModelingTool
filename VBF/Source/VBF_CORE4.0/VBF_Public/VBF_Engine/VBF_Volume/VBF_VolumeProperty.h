#ifndef OSGVOLUME_PROPERTY
#define OSGVOLUME_PROPERTY 1

#include <VBF_Engine/VBF_SceneGraph/TransferFunction>
#include <VBF_Engine/VBF_SceneGraph/Uniform>
#include <VBF_Engine/VBF_SceneGraph/VBFO_StateAttrAlphaFunc.h>
#include <VBF_Engine/VBF_SceneGraph/StateSet>
#include <VBF_Engine/VBF_SceneGraph/VBF_SGNodeCallback.h>
//!!!#include <VBF_Display/VBF_Manipulator/VBF_GUIEventHandler.h>

#include <VBF_Engine/VBF_Volume/Export>

namespace osgVolume {

// forward decarle
class Property;
class CompositeProperty;
class SwitchProperty;
class TransferFunctionProperty;
class ScalarProperty;
class IsoSurfaceProperty;
class MaximumIntensityProjectionProperty;
class LightingProperty;
class AlphaFuncProperty;
class SampleDensityProperty;
class SampleDensityWhenMovingProperty;
class TransparencyProperty;

class OSGVOLUME_EXPORT PropertyVisitor
{
    public:

        PropertyVisitor(bool traverseOnlyActiveChildren=true);

        virtual ~PropertyVisitor() {}

        virtual void apply(Property&) {}
        virtual void apply(CompositeProperty&);
        virtual void apply(SwitchProperty&);
        virtual void apply(TransferFunctionProperty&) {}
        virtual void apply(ScalarProperty&) {}
        virtual void apply(IsoSurfaceProperty&) {}
        virtual void apply(AlphaFuncProperty&) {}
        virtual void apply(MaximumIntensityProjectionProperty&) {}
        virtual void apply(LightingProperty&) {}
        virtual void apply(SampleDensityProperty&) {}
        virtual void apply(SampleDensityWhenMovingProperty&) {}
        virtual void apply(TransparencyProperty&) {}

        bool _traverseOnlyActiveChildren;

};


class OSGVOLUME_EXPORT Property : public osg::CVBF_Object
{
    public:

        Property();

        /** Copy constructor using CopyOp to manage deep vs shallow copy.*/
        Property(const Property&,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY);

        META_Object(osgVolume, Property);

        virtual void accept(PropertyVisitor& pv) { pv.apply(*this); }

    protected:

        virtual ~Property();
};

class OSGVOLUME_EXPORT CompositeProperty : public Property
{
    public:

        CompositeProperty();

        /** Copy constructor using CopyOp to manage deep vs shallow copy.*/
        CompositeProperty(const CompositeProperty& compositeProperty,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY);

        META_Object(osgVolume, CompositeProperty);

        virtual void accept(PropertyVisitor& pv) { pv.apply(*this); }

        void clear();

        typedef std::vector< ref_ptr<Property> > Properties;

        void setProperty(unsigned int i, Property* property) { if (i>=_properties.size()) _properties.resize(i+1); _properties[i] = property; }

        Property* getProperty(unsigned int i) { return i<_properties.size() ? _properties[i].get() : 0; }

        const Property* getProperty(unsigned int i) const { return i<_properties.size() ? _properties[i].get() : 0; }

        void addProperty(Property* property) { _properties.push_back(property); }

        void removeProperty(unsigned int i) { _properties.erase(_properties.begin()+i); }

        unsigned int getNumProperties() const { return _properties.size(); }

    protected:

        virtual ~CompositeProperty() {}


        Properties _properties;
};


class OSGVOLUME_EXPORT SwitchProperty : public CompositeProperty
{
    public:

        SwitchProperty();

        /** Copy constructor using CopyOp to manage deep vs shallow copy.*/
        SwitchProperty(const SwitchProperty& switchProperty,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY);

        META_Object(osgVolume, SwitchProperty);

        virtual void accept(PropertyVisitor& pv) { pv.apply(*this); }

        /** Set which child property is active.
          * -1 disables all children.*/
        void setActiveProperty(int i) { _activeProperty = i; }

        /** Get the active property.*/
        int getActiveProperty() const { return _activeProperty; }

    protected:

        virtual ~SwitchProperty() {}

        int _activeProperty;
};

class OSGVOLUME_EXPORT TransferFunctionProperty : public Property
{
    public:

        TransferFunctionProperty(osg::TransferFunction* tf = 0);

        /** Copy constructor using CopyOp to manage deep vs shallow copy.*/
        TransferFunctionProperty(const TransferFunctionProperty& tfp,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY);

        META_Object(osgVolume, TransferFunctionProperty);

        virtual void accept(PropertyVisitor& pv) { pv.apply(*this); }

        /** Set the transfer function.*/
        void setTransferFunction(osg::TransferFunction* tf) { _tf = tf; }

        /** Get the transfer function.*/
        osg::TransferFunction* getTransferFunction() { return _tf.get(); }

        /** Get the const transfer function.*/
        const osg::TransferFunction* getTransferFunction() const { return _tf.get(); }

    protected:

        virtual ~TransferFunctionProperty() {}

        ref_ptr<osg::TransferFunction> _tf;
};



class OSGVOLUME_EXPORT ScalarProperty : public Property
{
    public:

        ScalarProperty(const std::string& scaleName, float value);

        ScalarProperty(const ScalarProperty& scalarProperty,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY);

        META_Object(osgVolume, ScalarProperty);

        virtual void accept(PropertyVisitor& pv) { pv.apply(*this); }

        /** Set the value.*/
        virtual void setValue(float v) { _uniform->set(v); }

        /** Get the value.*/
        float getValue() const { float v; _uniform->get(v); return v; }

        /** Get the underlying uniform.*/
        osg::Uniform* getUniform() { return _uniform.get(); }

        /** Get the underlying uniform.*/
        const osg::Uniform* getUniform() const { return _uniform.get(); }

    protected:

        virtual ~ScalarProperty() {}

        ScalarProperty();

        ref_ptr<osg::Uniform>  _uniform;
};


class OSGVOLUME_EXPORT IsoSurfaceProperty : public ScalarProperty
{
    public:

        IsoSurfaceProperty(float value=1.0);

        IsoSurfaceProperty(const IsoSurfaceProperty& isp,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY);

        META_Object(osgVolume, IsoSurfaceProperty);

        virtual void accept(PropertyVisitor& pv) { pv.apply(*this); }

    protected:

        virtual ~IsoSurfaceProperty() {}
};

class OSGVOLUME_EXPORT AlphaFuncProperty : public ScalarProperty
{
    public:

        AlphaFuncProperty(float value=1.0);

        AlphaFuncProperty(const AlphaFuncProperty& isp,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY);

        META_Object(osgVolume, AlphaFuncProperty);

        virtual void accept(PropertyVisitor& pv) { pv.apply(*this); }

        virtual void setValue(float v);

        osg::AlphaFunc* getAlphaFunc() { return _alphaFunc.get(); }

        const osg::AlphaFunc* getAlphaFunc() const { return _alphaFunc.get(); }


    protected:

        virtual ~AlphaFuncProperty() {}

        ref_ptr<osg::AlphaFunc> _alphaFunc;
};

class OSGVOLUME_EXPORT MaximumIntensityProjectionProperty : public Property
{
    public:

        MaximumIntensityProjectionProperty();

        MaximumIntensityProjectionProperty(const MaximumIntensityProjectionProperty& mipp,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY);

        META_Object(osgVolume, MaximumIntensityProjectionProperty);

        virtual void accept(PropertyVisitor& pv) { pv.apply(*this); }

    protected:

        virtual ~MaximumIntensityProjectionProperty() {}
};


class OSGVOLUME_EXPORT LightingProperty : public Property
{
    public:

        LightingProperty();

        LightingProperty(const LightingProperty& mipp,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY);

        META_Object(osgVolume, LightingProperty);

        virtual void accept(PropertyVisitor& pv) { pv.apply(*this); }

    protected:

        virtual ~LightingProperty() {}
};


/** Sample density to use when the volume is static relative to the eye point or when moving if no SampleDensityWhenMovingProperty is assigned.*/
class OSGVOLUME_EXPORT SampleDensityProperty : public ScalarProperty
{
    public:

        SampleDensityProperty(float value=1.0);

        SampleDensityProperty(const SampleDensityProperty& isp,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY);

        META_Object(osgVolume, SampleDensityProperty);

        virtual void accept(PropertyVisitor& pv) { pv.apply(*this); }

    protected:

        virtual ~SampleDensityProperty() {}
};

/** Sample density to use when the volume is moving relative to the eye point.*/
class OSGVOLUME_EXPORT SampleDensityWhenMovingProperty : public ScalarProperty
{
    public:

        SampleDensityWhenMovingProperty(float value=1.0);

        SampleDensityWhenMovingProperty(const SampleDensityWhenMovingProperty& isp,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY);

        META_Object(osgVolume, SampleDensityWhenMovingProperty);

        virtual void accept(PropertyVisitor& pv) { pv.apply(*this); }

    protected:

        virtual ~SampleDensityWhenMovingProperty() {}
};

class OSGVOLUME_EXPORT TransparencyProperty : public ScalarProperty
{
    public:

        TransparencyProperty(float value=1.0);

        TransparencyProperty(const TransparencyProperty& isp,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY);

        META_Object(osgVolume, TransparencyProperty);

        virtual void accept(PropertyVisitor& pv) { pv.apply(*this); }

    protected:

        virtual ~TransparencyProperty() {}
};


class OSGVOLUME_EXPORT CollectPropertiesVisitor : public osgVolume::PropertyVisitor
{
    public:

        CollectPropertiesVisitor(bool traverseOnlyActiveChildren=true);

        virtual void apply(Property&);
        virtual void apply(TransferFunctionProperty&);
        virtual void apply(ScalarProperty&);
        virtual void apply(IsoSurfaceProperty& iso);
        virtual void apply(AlphaFuncProperty& af);
        virtual void apply(MaximumIntensityProjectionProperty& mip);
        virtual void apply(LightingProperty& lp);
        virtual void apply(SampleDensityProperty& sdp);
        virtual void apply(SampleDensityWhenMovingProperty& sdp);
        virtual void apply(TransparencyProperty& tp);

        ref_ptr<TransferFunctionProperty>              _tfProperty;
        ref_ptr<IsoSurfaceProperty>                    _isoProperty;
        ref_ptr<AlphaFuncProperty>                     _afProperty;
        ref_ptr<MaximumIntensityProjectionProperty>    _mipProperty;
        ref_ptr<LightingProperty>                      _lightingProperty;
        ref_ptr<SampleDensityProperty>                 _sampleDensityProperty;
        ref_ptr<SampleDensityWhenMovingProperty>       _sampleDensityWhenMovingProperty;
        ref_ptr<TransparencyProperty>                  _transparencyProperty;

};

//!!!���ڷ���class OSGVOLUME_EXPORT PropertyAdjustmentCallback : public osgGA::GUIEventHandler, public osg::StateSet::Callback
class OSGVOLUME_EXPORT PropertyAdjustmentCallback : public osg::NodeCallback,public osg::StateSet::Callback
{
    public:

        PropertyAdjustmentCallback();

        PropertyAdjustmentCallback(const PropertyAdjustmentCallback&,const osg::CopyOp&);

        META_Object(osgVolume,PropertyAdjustmentCallback);

        void setKeyEventCycleForward(int key) { _cyleForwardKey = key; }
        int getKeyEventCycleForward() const { return _cyleForwardKey; }

        void setKeyEventCycleBackward(int key) { _cyleBackwardKey = key; }
        int getKeyEventCycleBackward() const { return _cyleBackwardKey; }

        void setKeyEventActivatesTransparencyAdjustment(int key) { _transparencyKey = key; }
        int getKeyEventActivatesTransparencyAdjustment() const { return _transparencyKey; }

        void setKeyEventActivatesSampleDensityAdjustment(int key) { _sampleDensityKey = key; }
        int getKeyEventActivatesSampleDensityAdjustment() const { return _sampleDensityKey; }

        void setKeyEventActivatesAlphaFuncAdjustment(int key) { _alphaFuncKey = key; }
        int getKeyEventActivatesAlphaFuncAdjustment() const { return _alphaFuncKey; }

//!!!       virtual bool handle(const osgGA::GUIEventAdapter& ea,::IVBF_GUIActionAdapter&, osg::CVBF_Object* object, osg::NodeVisitor*);

        int     _cyleForwardKey;
        int     _cyleBackwardKey;
        int     _transparencyKey;
        int     _alphaFuncKey;
        int     _sampleDensityKey;

        bool    _updateTransparency;
        bool    _updateAlphaCutOff;
        bool    _updateSampleDensity;
};

}

#endif
