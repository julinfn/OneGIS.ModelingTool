#ifndef OSGSHADOW_VIEWDEPENDENTSHADOWTECHINIQUE
#define OSGSHADOW_VIEWDEPENDENTSHADOWTECHINIQUE 1

#include <VBF_Engine/VBF_Shadow/VBF_ShadowTechnique.h>
#include <map>
#include <VBF_Engine/VBF_Shadow/Export>

namespace osgShadow {
/**
    META_ViewDependentShadowTechniqueData macro defines initViewDependentData
    method used by derived shadow techniques to initialize their specific 
    ViewData objects. initViewDependentData will be called from 
    ViewDependentShadowTechnique base class to init derived class 
*/
#define META_ViewDependentShadowTechniqueData( ShadowTechnique, TechniqueData )\
virtual ViewDependentShadowTechnique::ViewData * initViewDependentData         \
( osgUtil::CullVisitor *cv, ViewDependentShadowTechnique::ViewData * vd )      \
{                                                                              \
    TechniqueData* td = dynamic_cast<TechniqueData*>( vd );                    \
    if ( !td ) td = new TechniqueData;                                         \
    td->init( this, cv );                                                      \
    return td;                                                                 \
}

// ViewDependentShadowTechnique	DebugShadowMap	StandardShadowMap	MinimalShadowMap	MinimalCullBoundsShadowMap
//																					    MinimalDrawBoundsShadowMap

/**
    ViewDependentShadowTechnique是所有视点相关阴影技术的基类，定义了基本的数据结构和方法
	用于管理场景中每个视图分开的阴影资源。

    View Dependent Shadow techniques are used to optimize shadow algorithms for 
    part of the scene seen on the view. If rest of the scene is behind view 
    frustum, there is no sense in computing shadows for it. Since in practice we 
    often use 3d databases extending far beyond current camera frustum View 
    Dependent Shadow approach may produce much better shadows. 

    The other goal is to provide framework for thread safe rendering of 
    the shadows. It allows to use shadows with different OSG threading models.

    Conceptually ViewDependentShadowTechnique is similar to osgSim::OverlayNode.
    Its a container class for number of ViewData (or ViewData derived) objects 
    doing actual shadow work for each of the scene views. 

    But ViewDependentShadowTechnique is intended as foundation layer for all 
    derived classes so in some way it extends osgSim::OverlayNode approach a bit.

    HOW IT WORKS:

    ViewDependendentShadowTechnique is derived from osgShadow::ShadowTechnique
    and as such overrides virtual methods of osgShadow::ShadowTechnique.
    But most of the shadow dirty work is done by ViewData objects, 
    ViewDependendentShadowTechnique::cull is the only osgShadow::ShadowTechnique
    method where ViewDependendentShadowTechnique does something significant:

    What ViewDependentShadowTechnique::cull( CullVisitor & cv ) does ? 
    It identifies View. CullVisitor ptr is used as View identificator. 
    In practice we do not check and interpret what are actual Views and SceneViews 
    set up by application. We focus on Camera and CullVisitors as a identificators 
    of views. We can safely do this because each such view will have associated 
    unique CullVisitor used to cull the scene in every frame.
       
    Based on CullVisitor ptr passed to cull method, associated Technique::ViewData 
    object is created (if neccessary) and then seleced. Then control is passed to 
    this ViewData object. So, each view has its associated unique ViewData 
    (or derived) object performing dirty work of shadow resources management and 
    shadow generation for the view.

    To support creation of classes derived from ViewDependentShadowTechnique it 
    was neccessary to provide mechanism to override ViewData and allow for 
    initialization of new derived ViewData objects. Creation and initialization 
    is performed when ViewDependendentShadowTechnique::cull gets called with
    CullVistor ptr which does not yet have associated ViewData object. When it 
    happens, virtual initViewDependentData method is called to give 
    derived techniques a chance to allocate and iniitalize its specific 
    resources as new ViewData derived instance. In practice initViewDependentData 
    in derived techniques should look the same as in base class so as a convenience
    it was defined as META_ViewDependentShadowTechnique macro. Derived techniques
    use this macro to override initViewDependentData method for their usage.

    After ViewData derived object is construted and selected, control is passed 
    to this object by call to virtual ViewData::cull method. The rest of work 
    is the done by this object. ViewDependentShadowTechnique::ViewData is intended 
    as a base class so it does nothing. In practice the rest of dirty work will 
    do new ViewData classes implemented in derived techniques.
*/
class OSGSHADOW_EXPORT ViewDependentShadowTechnique : public osgShadow::ShadowTechnique 
{
    public:
        /**
        osgShadow::ShadowTechnique equivalent methods for view dependent techniques  
        */

        ViewDependentShadowTechnique( void );
        ViewDependentShadowTechnique( const ViewDependentShadowTechnique& vdst, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY );


        /** Declaration of standard OSG object methods */
        META_Object( osgShadow, ViewDependentShadowTechnique );

        /** Dirty view data bits and force update of view data resources */
        virtual void dirty();

        /** Initialize the ShadowedScene and some data structures.*/
        virtual void init();

        /** Run the update traversal of the ShadowedScene and update any local cached data structures.*/
        virtual void update(osg::NodeVisitor& nv);

        /** Run the cull traversal of the ShadowedScene and set up the rendering for this ShadowTechnique.*/
        virtual void cull(osgUtil::CullVisitor& cv);

        /** Clean scene graph from any shadow technique specific nodes, state and drawables.*/
        virtual void cleanSceneGraph();

        /** Traverse shadow scene graph.*/
        virtual void traverse(osg::NodeVisitor& nv);

    protected:
        /** Classic protected OSG destructor */
        ~ViewDependentShadowTechnique( void );

        /** 
        Base container class for view dependent shadow resources. 
        Techniques based on ViewDependentShadowTechnique will usually define
        similar struct and derive it from ViewData to contain their specufic resources.
        */
        struct OSGSHADOW_EXPORT ViewData: public ::CVBF_Referenced
        {
            virtual const char* className() const { return "ViewData"; }

            /**
            Method called upon ViewData instance to initialize internal variables
            */
            virtual void init ( ViewDependentShadowTechnique *st, osgUtil::CullVisitor *cv );

            /**
            Method called by ViewDependentShadowTechnique to allow ViewData 
            do the hard work computing shadows for its associated view
            */
            virtual void cull();

            /** 
            Dirty is called by parent ViewDependentShadowTechnique to force 
            update of resources after some of them were modified in parent technique
            */
            virtual void dirty( bool flag );

            /**
            Simple constructor zeroing all variables.
            */
            ViewData(): m_bDirty( true ), _cv( NULL ), _st( NULL ) { };

            /**
            Mutex used to guard m_bDirty flag from override in case when parent technique calls 
            dirty() simultaneously with ViewData while it is updating resources inside init method.
            */
            OpenThreads::Mutex                                _mutex;

            /**
            Dirty flag tells this instance to update its resources 
            */
            bool                                              m_bDirty;

            /** 
            View's CullVisitor associated with this ViewData instance 
            */
            observer_ptr< osgUtil::CullVisitor >         _cv;

            /**
            Parent ViewDependentShadowTechnique 
            */
            observer_ptr< ViewDependentShadowTechnique > _st;

        };

        /** 
        Map of view dependent data per view cull visitor (CVs are used as indices) 
        ViewDependentShadowTechnique uses this map to find VieData for each cull vitior
        */

        typedef std::map< ref_ptr< osgUtil::CullVisitor >, ref_ptr< ViewData > > ViewDataMap;

        ViewDataMap    _viewDataMap;


        /**
        Mutex used to serialize accesses to ViewDataMap
        */
        OpenThreads::Mutex   _viewDataMapMutex;

        /** Return view dependent data for the cull visitor */
        virtual ViewDependentShadowTechnique::ViewData * getViewDependentData( osgUtil::CullVisitor * cv );

        /** Define view dependent data for the cull visitor */
        virtual void setViewDependentData( osgUtil::CullVisitor * cv, ViewDependentShadowTechnique::ViewData * data );

        /** 
        Declare standard initViewDependentData method. 
        */
        META_ViewDependentShadowTechniqueData( ViewDependentShadowTechnique, ViewData )
};


} // namespace osgShadow

#endif
