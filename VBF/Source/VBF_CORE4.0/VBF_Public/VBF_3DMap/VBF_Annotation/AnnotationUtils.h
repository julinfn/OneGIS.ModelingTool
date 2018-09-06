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
#ifndef OSGEARTH_ANNOTATION_ANNOTATION_UTILS_H
#define OSGEARTH_ANNOTATION_ANNOTATION_UTILS_H 1

#include <VBF_3DMap/VBF_Annotation/Common>
#include <VBF_3DMap/VBF_Symbology/VBF_SymbolText.h>
#include <VBF_3DMap/VBF_Symbology/Style.h>
#include <VBF_Engine/VBF_SceneGraph/VBFO_SGNodeGroupTransformAuto.h>
#include <VBF_Engine/VBF_SceneGraph/VBFO_SGDrawable.h>
#include <VBF_Engine/VBF_SceneGraph/VBFO_SGDrawableGeometry.h>

namespace osgEarth { namespace Annotation
{
    using namespace osgEarth;
    using namespace osgEarth::Symbology;

    /**
     * Internal tools used by the annotation library.
     */
    struct OSGEARTHANNO_EXPORT AnnotationUtils
    {
        static const std::string& UNIFORM_HIGHLIGHT();

        static const std::string& UNIFORM_IS_TEXT();

        static const std::string& UNIFORM_FADE();

        static const std::string& PROGRAM_NAME();

        /**
         * Creates a drawable representing a symbolized text label in
         * pixel space.
         */
        static osg::Drawable* createTextDrawable(
            const std::string& text,
            const TextSymbol*  symbol,
            const osg::Vec3&   positionOffset );

        /**
         * Creates the basic geometry to draw an image texture-mapped to
         * a quad in pixel space.
         */
        static osg::Geometry* createImageGeometry(
            osg::Image*       image,
            const osg::Vec2s& pixelOffsets,
            unsigned          textureUnit       =0,
            double            heading = 0.0);

        /**
         * Creates a fading uniform that the decluttering engine can use
         * to adjust the alpha of annotation drawables.
         */
        static osg::Uniform* createFadeUniform();

        /**
         * Creates a boolean uniform used to indicate a hightlighted state.
         */
        static osg::Uniform* createHighlightUniform();

        /**
         * Builds a graph on top of the specified that that implements a 2-pass
         * rendering scheme for self-occluding or self-intersecting geometies that
         * would not otherwise properly blend. The scheme renders the back faces
         * first, followed by the front faces, ensuring proper blending.
         */
        static osg::IVBF_SGNode* installTwoPassAlpha( osg::IVBF_SGNode* );

        /**
         * Checks whether using a style will require transparency blending.
         */
        static bool styleRequiresAlphaBlending( const Style& style );

        /**
         * Internal - A customized AutoTransform used by the OrthoNode to
         * support intersections that are compatible with the decluttering engine
         */
        struct OrthoNodeAutoTransform : public osg::AutoTransform
        {
            void acceptCullNoTraverse( osg::CullStack* cs );

            bool okToIntersect() const { return !_firstTimeToInitEyePoint; }
        };

        // some geometry creation utilities
        static osg::Drawable* create2DQuad( const osg::BoundingBox& box, float padding, const osg::Vec4& color );

        static osg::Drawable* create2DOutline( const osg::BoundingBox& box, float padding, const osg::Vec4& color );

        static osg::IVBF_SGNode* createFullScreenQuad( const osg::Vec4& color );

        /**
         * Builds a sphere geometry.
         * @param r        Radius
         * @param color    VBFCOLOR
         * @param maxAngle Maximum angle between verts (controls tessellation)
         */
        static osg::IVBF_SGNode* createSphere( float r, const osg::Vec4& color, float maxAngle =15.0f );

        /**
         * Builds a hemisphere geometry.
         * @param r        Radius
         * @param color    VBFCOLOR
         * @param maxAngle Maximum angle between verts (controls tessellation)
         */
        static osg::IVBF_SGNode* createHemisphere( float r, const osg::Vec4& color, float maxAngle =15.0f );

        /**
         * Builds an ellipsoid geometry.
         * @param xr       X-axis radius
         * @param yr       Y-axis radius
         * @param zr       Z-axis radius
         * @param color    VBFCOLOR
         * @param maxAngle Maximum angle between verts (controls tessellation)
         */
        static osg::IVBF_SGNode* createEllipsoid( float xr, float yr, float zr, const osg::Vec4& color, float maxAngle =15.0f );

        static osg::IVBF_SGNode* createEllipsoid( 
            float xr, float yr, const osg::Vec4& color, float maxAngle =15.0f,
            float minLat =-90.0, float maxLat=90.0, float minLon=-180.0, float maxLon=180.0);

        static osg::Geometry* createEllipsoidGeometry( 
            float major, float minor, const osg::Vec4& color, float maxAngle =15.0f,
            float minLat =-90.0, float maxLat=90.0, float minLon=-180.0, float maxLon=180.0);

        struct AltitudePolicy
        {
            AltitudePolicy() : draping(false), sceneClamping(false), gpuClamping(false) { }
            bool draping;
            bool sceneClamping;
            bool gpuClamping;
        };
        static void getAltitudePolicy( const Style& style, AltitudePolicy& b );


    private:
        AnnotationUtils() { }
    };

} } // namespace osgEarth::Annotation

#endif //OSGEARTH_ANNOTATION_ANNOTATION_UTILS_H
