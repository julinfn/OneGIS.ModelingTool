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

#ifndef OSG_TEXTURERECTANGLE
#define OSG_TEXTURERECTANGLE 1

#include <VBF_Engine/VBF_SceneGraph/VBFO_StateAttrTexture.h>

#ifndef GL_TEXTURE_RECTANGLE_NV
#define GL_TEXTURE_RECTANGLE_NV           0x84F5
#endif

#ifndef GL_TEXTURE_RECTANGLE
#define GL_TEXTURE_RECTANGLE GL_TEXTURE_RECTANGLE_NV
#endif

namespace osg {

/** Texture state class which encapsulates OpenGL texture functionality. */
class OSG_EXPORT TextureRectangle : public Texture
{

    public :

        TextureRectangle();

        TextureRectangle(Image* image);

        /** Copy constructor using CopyOp to manage deep vs shallow copy. */
        TextureRectangle(const TextureRectangle& text,const CopyOp& copyop=CopyOp::SHALLOW_COPY);

        META_StateAttribute(osg, TextureRectangle, TEXTURE);

        /** Return -1 if *this < *rhs, 0 if *this==*rhs, 1 if *this>*rhs. */
        virtual int compare(const StateAttribute& rhs) const;

        virtual GLenum getTextureTarget() const { return GL_TEXTURE_RECTANGLE; }

        /** Set the texture image. */
        void setImage(Image* image);

        /** Get the texture image. */
        Image* getImage() { return _image.get(); }

        /** Get the const texture image. */
        inline const Image* getImage() const { return _image.get(); }

        inline unsigned int& getModifiedCount(unsigned int contextID) const
        {
            // get the modified count for the current contextID.
            return _modifiedCount[contextID];
        }


        /** Set the texture image, ignoring face value as there is only one image. */
        virtual void setImage(unsigned int, Image* image) { setImage(image); }

        /** Get the texture image, ignoring face value as there is only one image. */
        virtual Image* getImage(unsigned int) { return _image.get(); }

        /** Get the const texture image, ignoring face value as there is only one image. */
        virtual const Image* getImage(unsigned int) const { return _image.get(); }

        /** Get the number of images that can be assigned to the Texture. */
        virtual unsigned int getNumImages() const { return 1; }


        /** Set the texture width and height. If width or height are zero then
          * the respective size value is calculated from the source image sizes.
        */
        inline void setTextureSize(int width, int height) const
        {
            _textureWidth = width;
            _textureHeight = height;
        }

        void setTextureWidth(int width) { _textureWidth=width; }
        void setTextureHeight(int height) { _textureHeight=height; }

        virtual int getTextureWidth() const { return _textureWidth; }
        virtual int getTextureHeight() const { return _textureHeight; }
        virtual int getTextureDepth() const { return 1; }

        class SubloadCallback : public ::CVBF_Referenced
        {
        public:
            virtual void load(const TextureRectangle&, State&) const = 0;
            virtual void subload(const TextureRectangle&, State&) const = 0;
        };

        void setSubloadCallback(SubloadCallback* cb) { _subloadCallback = cb;; }
        SubloadCallback* getSubloadCallback() { return _subloadCallback.get(); }
        const SubloadCallback* getSubloadCallback() const { return _subloadCallback.get(); }

        /** Copies pixels into a 2D texture image, as per glCopyTexImage2D.
          * Creates an OpenGL texture object from the current OpenGL background
          * framebuffer contents at position \a x, \a y with width \a width and
          * height \a height. \a width and \a height must be a power of two. */
        void copyTexImage2D(State& state, int x, int y, int width, int height );

        /** Copies a two-dimensional texture subimage, as per
          * glCopyTexSubImage2D. Updates a portion of an existing OpenGL
          * texture object from the current OpenGL background framebuffer
          * contents at position \a x, \a y with width \a width and height
          * \a height. Loads framebuffer data into the texture using offsets
          * \a xoffset and \a yoffset. \a width and \a height must be powers
          * of two. */
        void copyTexSubImage2D(State& state, int xoffset, int yoffset, int x, int y, int width, int height );

        /** On first apply (unless already compiled), create and bind the 
          * texture, subsequent apply will simply bind to texture.
        */
        virtual void apply(State& state) const;

    protected :

        virtual ~TextureRectangle();

        virtual void computeInternalFormat() const;
        void allocateMipmap(State& state) const;

        void applyTexImage_load(GLenum target, Image* image, State& state, GLsizei& inwidth, GLsizei& inheight) const;
        void applyTexImage_subload(GLenum target, Image* image, State& state, GLsizei& inwidth, GLsizei& inheight, GLint& inInternalFormat) const;

        ref_ptr<Image> _image;

        // subloaded images can have different texture and image sizes.
        mutable GLsizei _textureWidth, _textureHeight;
        
        ref_ptr<SubloadCallback> _subloadCallback;

        typedef buffered_value<unsigned int> ImageModifiedCount;
        mutable ImageModifiedCount _modifiedCount;
};

}

#endif
