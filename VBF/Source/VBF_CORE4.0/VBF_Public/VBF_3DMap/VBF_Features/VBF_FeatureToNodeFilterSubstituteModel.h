/* -*-c++-*- */
/* osgEarth - Dynamic map generation toolkit for OpenSceneGraph
 * Copyright 2008-2013 Pelican Mapping
 * http://osgearth.org
 *
 * osgEarth is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef OSGEARTHFEATURES_SUBSTITUTE_MODEL_FILTER_H
#define OSGEARTHFEATURES_SUBSTITUTE_MODEL_FILTER_H 1

#include <VBF_3DMap/VBF_Features/Common>
#include <VBF_3DMap/VBF_Features/Feature.h>
#include <VBF_3DMap/VBF_Features/VBF_FeatureFilter.h>
#include <VBF_3DMap/VBF_Symbology/Style.h>
#include <VBF_3DMap/VBF_Symbology/VBF_ResourceMarker.h>
#include <VBF_Common/VBF_Utils/VBF_CacheLRU.h>

namespace osgEarth { namespace Features
{
    using namespace osgEarth;
    using namespace osgEarth::Symbology;

    /**
     * Feature filter that will substitute external nodes for feature geometry.
     *
     * TODO:
     *  - line sub (interval, density scattering)
     *  - polygon sub (interval, density scattering)
     *  - density-based scattering for points
     *  - clustering (optional) at various levels
     *  - proper ECEF normal computation per feature
     *  - terrain clamping of the localization point
     *  - automatic height offset based on minimum Z of model bbox
     *  - predicate based model selection (scripting)
     *  - instancing (maybe)
     *  - texture collection and sharing (session based) when clustering
     */
    class OSGEARTHFEATURES_EXPORT SubstituteModelFilter : public FeaturesToNodeFilter
    {
    public:
        static bool isSupported() { return true; }

    public:
        SubstituteModelFilter( const Style& style =Style() );
        virtual ~SubstituteModelFilter() { }

        /** Whether to cluster all the model instances into a single geode. Default is false. */
        void setClustering( bool value ) { _cluster = value; }
        bool getClustering() const { return _cluster; }

        /** Whether to convert model instances to use "DrawInstanced" instead of transforms. Default is false */
        void setUseDrawInstanced( bool value ) { _useDrawInstanced = value; }
        bool getUseDrawInstanced() const { return _useDrawInstanced; }

        /** Whether to merge marker geometries into geodes */
        void setMergeGeometry( bool value ) { _merge = value; }
        bool getMergeGeometry() const { return _merge; }

        void setFeatureNameExpr( const StringExpression& expr ) { _featureNameExpr = expr; }
        const StringExpression& getFeatureNameExpr() const { return _featureNameExpr; }

    public:
        virtual osg::IVBF_SGNode* push( FeatureList& input, FilterContext& context );

    protected:
        Style                         _style;
        bool                          _cluster;
        bool                          _useDrawInstanced;
        bool                          _merge;
        StringExpression              _featureNameExpr;
        ref_ptr<ResourceLibrary> _resourceLib;
        bool                          _normalScalingRequired;

        typedef CVBF_CacheLRU<URI, ref_ptr<InstanceResource> > InstanceCache;
        InstanceCache _instanceCache;
        
        bool process(const FeatureList& features, const InstanceSymbol* symbol, Session* session, osg::IVBF_SGNodeGroup* ap, FilterContext& context );
        bool cluster(const FeatureList& features, const InstanceSymbol* symbol, Session* session, osg::IVBF_SGNodeGroup* ap, FilterContext& context );
        
        InstanceResource* findResource( const URI& instanceURI, const InstanceSymbol* symbol, FilterContext& context, std::set<URI>& missing ) ;
    };

} } // namespace osgEarth::Features

#endif // OSGEARTHFEATURES_SUBSTITUTE_MODEL_FILTER_H
