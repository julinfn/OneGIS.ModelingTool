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
#ifndef OSGEARTH_TEXTURE_COMPOSITOR_H
#define OSGEARTH_TEXTURE_COMPOSITOR_H 1

#include <VBF_3DMap/VBF_Terrain/Common.h>
#include <VBF_3DMap/VBF_Terrain/GeoData.h>
#include <VBF_Common/VBF_Utils/VBF_ThreadingUtils.h>
#include <VBF_3DMap/VBF_Terrain/VBF_ConfigOptionsTerrain.h>
#include <VBF_Engine/VBF_SceneGraph/StateSet>
#include <VBF_Engine/VBF_SceneGraph/VBFO_StateAttrProgram.h>

class CVBF_LayerTerrainImage;
class CVBF_TileKey;
struct CVBF_MapModelChange;

namespace osgEarth
{
    /**
     * Tracks the usage of texture slots and their rendering order for a map.
     *
     * A "slot" is a home for a texture layer. For example, in a multitexturing setup, a
     * slot corresponds to a texture image unit in the hardware. In a texture_array setup,
     * a slot is one "slice" of the texture array.
     *
     * A single layer can occupy more than one slot. (Specifically, this is the case when
     * LOD Blending is enabled on that layer -- the layer will require 2 slots in order to
     * blend the images together in the shader.) In this case, we refer to the first slot
     * as the "primary" slot, and any additional slots belonging to the same layer as 
     * "secondary" slots.
     */
    class OSGEARTH_EXPORT TextureLayout
    {        
    public:
        // vector of texture slots, each slot containing the UID of the image layer in that slot
        // or -1 if the slot is empty.
        typedef std::vector<UID> TextureSlotVector;

        // rendering order of texture slots- each element is an index into the TextureSlotVector.
        typedef std::vector<int> RenderOrderVector;

    public:
        TextureLayout();

        /** dtor */
        virtual ~TextureLayout() { }

        /**
         * Gets a texture slot corresponding to the layer UID. There may be more than one,
         * the "which" parameter allows you to select one in particular. You can also limit
         * the number of slots to search.
         */
        int getSlot( UID layerUID, unsigned which =0, unsigned maxSlotsToSearch =~0 ) const;

        /** Gets the render order index of the layer with the given UID */
        int getOrder( UID layerUID ) const;

        /** Gets the index of the highest populated slot. */
        int getMaxUsedSlot() const;

        /** Whether the indicated slot is available for use */
        bool isSlotAvailable( int slot ) const;

        /** Accesses a vector that maps layer UIDs to texture slots. */
        const TextureSlotVector& getTextureSlots() const { return _slots; }

        /** Access a vector that specifies the rendering order of texture slots. */
        const RenderOrderVector& getRenderOrder() const { return _order; }

        /** Returns "true" if the layout contains at least one secondary slot allocation. */
        bool containsSecondarySlots( unsigned maxSlotsToSearch = ~0) const;

        /** whether LOD blending is enabled on a layer */
        bool isBlendingEnabled( UID layerUID ) const;

    protected:
        friend class TextureCompositor;

        void applyMapModelChange( 
          const CVBF_MapModelChange& change, 
          bool reserveSeconarySlotIfNecessary,
          bool disableLodBlending );

        void setReservedSlots( const std::set<int>& reservedSlots );

    protected:
        TextureSlotVector  _slots;
        RenderOrderVector  _order;
        bool               _textureImageUnitPerSlot;
        std::set<int>      _reservedSlots;
        std::map<UID,bool> _lodBlending;

        void assignPrimarySlot( CVBF_LayerTerrainImage* layer, int index );
        void assignSecondarySlot( CVBF_LayerTerrainImage* layer );
    };

    //-----------------------------------------------------------------------

	// 纹理合成技术实现的基类
    class OSGEARTH_EXPORT TextureCompositorTechnique : public ::CVBF_Referenced
    {
    public:
        virtual bool requiresUnitTextureSpace() const =0;

        virtual bool usesShaderComposition() const =0;

        virtual bool blendingRequiresSecondarySlot() const { return false; }

        virtual bool supportsLayerUpdate() const { return false; }

        virtual void updateMasterStateSet( osg::StateSet* stateSet, const TextureLayout& layout ) const { }

        virtual GeoImage prepareImage( const GeoImage& image, const CVBF_GeoExtent& tileExtent ) const { return image; }

        virtual GeoImage prepareSecondaryImage( const GeoImage& image, const CVBF_GeoExtent& tileExtent ) const { return image; }

        virtual void applyLayerUpdate( osg::StateSet* stateSet, UID layerUID, const GeoImage& preparedImage, const CVBF_TileKey& tileKey, const TextureLayout& layout, osg::StateSet* parentStateSet ) const { }

        virtual void applyLayerRemoval( osg::StateSet* stateSet, UID layerUID ) const { }

        virtual osg::Shader* createSamplerFunction( UID layerUID, const std::string& functionName, 
                                     osg::Shader::Type type, const TextureLayout& layout ) const { return 0L; }
    };

    //-----------------------------------------------------------------------

	// 工具类，用于合成纹理数据（在地形引擎中使用）
    class OSGEARTH_EXPORT TextureCompositor : public ::CVBF_Referenced
    {
    public:

        TextureCompositor( const CVBF_OptionsTerrain& options );
        virtual ~TextureCompositor() { }

		// 设置自定义的纹理合成技术
        void setTechnique( TextureCompositorTechnique* tech );

		// 获取合成器实际选择的技术的类型，可能和请求的技术类型不一样，是因为需要验证系统的能力并根据需要自动回落到（"fall back"）较少的技术
        const CVBF_OptionsTerrain::CompositingTechnique& getTechnique() const { return _tech; }

        /**
         * Applies a map model change action to the texture layout contained within this compositor.
         * You should call this any time the map model change happens.
         */
        void applyMapModelChange( const CVBF_MapModelChange& change );

        /**
         * Returns true if the compositor implementation supports the ability to update an individual
         * image layer (via prepareLayerUpdate and applyLayerUpdate).
         */
        bool supportsLayerUpdate() const;

        /**
         * Prepares a new image for incorporation into the texture composite. Usually you will call this 
         * method when you are updating a single layer AFTER having originally created the stateset
         * (via createStateSet). It is safe to call this method from any thread. You can thereafter take
         * the result and pass it to updateLayer. These two methods are separated so that you can call
         * this one from any thread (it is guaranteed to be thread-safe) and then use updateLayer to 
         * updaet a live scene graph if necessary.
         */
        GeoImage prepareImage( const GeoImage& image, const CVBF_GeoExtent& tileExtent ) const;

        /**
         * Like prepareImage, but prepares it for use as a secondary texture (for LOD blending).
         */
        GeoImage prepareSecondaryImage( const GeoImage& image, const CVBF_GeoExtent& tileExtent ) const;

        /**
         * Updates a stateset's texture composition with an image. Typically this will be the image
         * returned from prepareImage(), but it doesn't have to be. Note: if the stateset is live in
         * the scene graph, be sure to only call this method from UPDATE trav.
         */
        void applyLayerUpdate(
            osg::StateSet*  stateSet,
            UID             layerUID,
            const GeoImage& preparedImage,
            const CVBF_TileKey&  tileKey,
            osg::StateSet*  parentStateSet ) const;

        /**
         * Updates a stateset's texture composition based on the information that a layer has been
         * removed.
         */
        void applyLayerRemoval( osg::StateSet* stateSet, UID layerUID ) const;

        /**
         * Assigns a texture coordinate array to a geometry, putting it in the proper
         * texture slot according to the compositor's layout.
         */
        void assignTexCoordArray( osg::Geometry* geom, UID layerUID, osg::Vec2Array* texCoords ) const;

        /**
         * Whether the texture composition technique uses a single, unit (0..1) texture coordinate
         * space (true), or a separate texture coordinate space per layer (false). This provides a
         * hint to whomever is generating the texture coordinates as to what they need to do to 
         * support the current technique.
         */
        bool requiresUnitTextureSpace() const;

        /**
         * Whether the composition technique uses GLSL shader composition.
         */
        bool usesShaderComposition() const;

        /**
         * Updates a state set with attributes required to support the technique.
         * Call this whenever the composition of the image layer stack changes in order to update
         * applicable global uniforms, texture attributes, and so forth.
         */
        void updateMasterStateSet( osg::StateSet* stateSet ) const;

        /**
         * Requests a texture image unit that is not in use, and marks is as reserved. You can release
         * the reserved texture image unit by calling releaseTextureImageUnit().
         */
        bool reserveTextureImageUnit( int& out_unit );

        /**
         * Releases a reserved texture image unit previously returned by reserveTextureImageUnit.
         */
        void releaseTextureImageUnit( int unit );

        /**
         * Gets the rendering order of the layer with the specified UID.
         */
        int getRenderOrder( UID layerUID ) const;

        /**
         * Creates a shader component that implements texture sampling for the layer with 
         * the specified layer UID. You can use this is a custom shader to sample texels from
         * one of osgEarth's CVBF_Map image layers.
         *
         *    vec4 function_name()
         */
        osg::Shader* createSamplerFunction( UID layerUID, const std::string& functionName, osg::Shader::Type type ) const;

		// 获取地形选项
        CVBF_OptionsTerrain& getOptions() { return _options;}

    private:
        void init();

        OpenThreads::Mutex                       _initMutex;
        CVBF_OptionsTerrain::CompositingTechnique     _tech;
        CVBF_OptionsTerrain                           _options;
        bool                                     _forceTech;
        ref_ptr<osg::Program>               _program;
        ref_ptr<TextureCompositorTechnique> _impl;

        TextureLayout _layout;
        CVBF_ReadWriteMutex _layoutMutex;

        std::set<int> _reservedUnits;
    };
}

#endif // OSGEARTH_TEXTURE_COMPOSITOR_H
