#ifndef OSGEARTH_NOTIFY_H
#define OSGEARTH_NOTIFY_H 1

#include <VBF_3DMap/VBF_Terrain/Export>
#include <VBF_Base/VBF_Notify.h>
#include <string>

namespace osgEarth
{
    /** set the notify level, overriding the default or the value set by
      * the environmental variable OSGNOTIFYLEVEL.
      */
    extern OSGEARTH_EXPORT void setNotifyLevel(osg::NotifySeverity severity);

    /** get the notify level. */
    extern OSGEARTH_EXPORT osg::NotifySeverity getNotifyLevel();

    /** is notification enabled, given the current setNotifyLevel() setting? */
    extern OSGEARTH_EXPORT bool isNotifyEnabled(osg::NotifySeverity severity);

    /** initialize notify level. */
    extern OSGEARTH_EXPORT bool initNotifyLevel();
  
    extern OSGEARTH_EXPORT std::ostream& notify(const osg::NotifySeverity severity);

    inline std::ostream& notify(void) { return osgEarth::notify(osg::INFO); }
}

#define OE_NOTIFY( X,Y ) if(osgEarth::isNotifyEnabled( X )) osgEarth::notify( X ) << Y
#define OE_FATAL  OE_NOTIFY(osg::FATAL,"[osgEarth]* ")
#define OE_WARN   OE_NOTIFY(osg::WARN,"[osgEarth]* ")
#define OE_NOTICE OE_NOTIFY(osg::NOTICE,"[osgEarth]  ")
#define OE_INFO   OE_NOTIFY(osg::INFO,"[osgEarth]  ")
#define OE_INFO_CONTINUE OE_NOTIFY(osg::INFO, "")
#define OE_DEBUG  OE_NOTIFY(osg::DEBUG_INFO,"[osgEarth]  ")
#define OE_NULL   if(false) osgEarth::notify(osg::ALWAYS)

#endif // OSGEARTH_NOTIFY_H
