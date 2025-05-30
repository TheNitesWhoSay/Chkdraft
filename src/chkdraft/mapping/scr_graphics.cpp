#include "scr_graphics.h"
#include <chkdraft/ui/main_windows/gui_map.h>
#include <cross_cut/logger.h>
#include <rarecpp/json.h>
#include <chrono>
#include <cstring>
#include <Windows.h>
#include "memory_tiered_tasks.h"
#include <glad/gl.h>
#include "gl/base.h"
#include <glm/vec3.hpp>

extern Logger logger;

template <size_t PaletteStartIndex, size_t RemappedLength>
void remapPalette(gl::Palette & palette, void* colorMapping, Chk::PlayerColor playerColor, Chk::PlayerColor & prevMappedColor)
{
    if ( playerColor != prevMappedColor )
    {
        prevMappedColor = playerColor;
        std::memcpy(&palette[PaletteStartIndex], colorMapping, RemappedLength*sizeof(palette[0]));
        palette.update();
    }
}

// ScopedPaletteRemap stores what was in the palette and remaps it, when going out of scope, restores palette & clears prevMappedColor
template <size_t PaletteStartIndex, size_t RemappedLength>
class ScopedPaletteRemap
{
    std::optional<gl::Palette> & palette;
    Chk::PlayerColor & prevMappedColor;
    std::uint32_t remapped[RemappedLength] {};

public:
    ScopedPaletteRemap(std::optional<gl::Palette> & palette, void* colorMapping, Chk::PlayerColor & prevMappedColor) : palette(palette), prevMappedColor(prevMappedColor)
    {
        if ( palette )
        {
            std::memcpy(remapped, &(*palette)[PaletteStartIndex], sizeof(remapped));
            std::memcpy(&(*palette)[PaletteStartIndex], colorMapping, sizeof(remapped));
            palette->update();
        }
    }

    ~ScopedPaletteRemap()
    {
        if ( palette )
        {
            prevMappedColor = Chk::PlayerColor::None;
            std::memcpy(&(*palette)[PaletteStartIndex], remapped, sizeof(remapped));
            palette->update();
        }
    }
};

// ScopedPaletteRestore stores what was in the palette previously, when going out of scope, restores palette & clears prevMappedColor
template <size_t PaletteStartIndex, size_t RemappedLength>
class ScopedPaletteRestore
{
    std::optional<gl::Palette> & palette;
    Chk::PlayerColor & prevMappedColor;
    std::uint32_t remapped[RemappedLength] {};

public:
    ScopedPaletteRestore(std::optional<gl::Palette> & palette, Chk::PlayerColor & prevMappedColor) : palette(palette), prevMappedColor(prevMappedColor)
    {
        if ( palette )
            std::memcpy(remapped, &(*palette)[PaletteStartIndex], sizeof(remapped));
    }

    ~ScopedPaletteRestore()
    {
        if ( palette )
        {
            prevMappedColor = Chk::PlayerColor::None;
            std::memcpy(&(*palette)[PaletteStartIndex], remapped, sizeof(remapped));
            palette->update();
        }
    }
};

void Scr::GraphicsData::Shaders::loadClassic()
{
    if ( !simplePaletteShader.hasShaders() )
        simplePaletteShader.load();

    if ( !solidColorShader.hasShaders() )
        solidColorShader.load();
}

void Scr::GraphicsData::Shaders::load(ArchiveCluster & archiveCluster)
{
    if ( waterShader.hasShaders() )
        return; // Already loaded remastered

    std::unordered_map<std::string, std::vector<u8>> includableShaders {};
    auto loadIncludableShader = [&includableShaders](ArchiveCluster & archiveCluster, const std::string & filePath) {
        includableShaders.emplace(getSystemFileName(filePath), fragmentShaderStringFromDatFile(archiveCluster, filePath));
    };

    auto preprocessor = [&includableShaders](std::vector<u8> & shader) {
        constexpr std::string_view includePrefix = "#include <";
        constexpr std::string_view includeSuffix = ">";
        auto shaderView = std::string_view((const char*)shader.data(), shader.size());
        auto prefixPos = shaderView.find(includePrefix);
        if ( prefixPos != std::string_view::npos )
        {
            auto it = shader.begin();
            std::vector<u8> processed {};
            processed.reserve(shader.size());
            do
            {
                auto suffixPos = shaderView.find(includeSuffix, prefixPos+includePrefix.size());
                if ( suffixPos == std::string::npos )
                    throw std::runtime_error("Closing > not found for include statement!");
                            
                auto includedFileNameView = shaderView.substr(prefixPos+includePrefix.size(), suffixPos-(prefixPos+includePrefix.size()));
                auto includedFileName = std::string(includedFileNameView.data(), includedFileNameView.size());
                auto includedFile = includableShaders.find(includedFileName);
                if ( includedFile == includableShaders.end() )
                    throw std::runtime_error("Could not find included shader: " + includedFileName);
                            
                processed.insert(processed.end(), it, shader.begin()+prefixPos);
                processed.insert(processed.end(), includedFile->second.begin(), includedFile->second.end());
                it = shader.begin()+suffixPos+includeSuffix.size();
                prefixPos = shaderView.find(includePrefix, suffixPos);
            }
            while ( prefixPos != std::string_view::npos );

            processed.insert(processed.end(), it, shader.end());
            ensureVersioned(processed);
            processed.shrink_to_fit();
            //logger.info() << "Processed\n\n" << shader.data() << "\n\n\ninto\n\n\n" << processed.data() << "\n\n\n";
            shader.swap(processed);
        }
        else
        {
            ensureVersioned(shader);
            shader.shrink_to_fit();
        }
    };
                
    loadIncludableShader(archiveCluster, "ShadersGLSL\\util_scale.glsl");

    // heat_distortion.glsl -> HeatShader
    //    - includes util_scale.glsl (no dependencies)
    //heatShader.load(archiveCluster, preprocessor);

    // water.glsl -> WaterShader
    //    - includes util_scale.glsl
    waterShader.load(archiveCluster, preprocessor);

    includableShaders.clear(); // Done with util_scale.glsl

    // palette_color_frag.glsl -> PaletteShader (no dependencies)
    paletteShader.load(archiveCluster, preprocessor);
                
    loadIncludableShader(archiveCluster, "ShadersGLSL\\util_hallucinate.glsl");
    loadIncludableShader(archiveCluster, "ShadersGLSL\\util_team_color.glsl");
    // sprite_frag.glsl -> SpriteShader
    //    - includes util_hallucinate.glsl (no dependencies)
    //    - includes util_team_color.glsl (no dependencies)
    spriteShader.load(archiveCluster, preprocessor);

    includableShaders.clear(); // Done with util_hallucinate.glsl, util_team_color.glsl

    // textured_frag.glsl -> TileShader (no dependencies)
    tileShader.load(archiveCluster, preprocessor);

    // sprite_solid_frag.glsl -> SelectionShader (no dependencies)
    selectionShader.load(archiveCluster, preprocessor);

    simpleShader.load();

    if ( !solidColorShader.hasShaders() )
        solidColorShader.load();
}

void Scr::GraphicsData::loadSkin(Skin & skin, ArchiveCluster & archiveCluster, VisualQuality visualQuality, ByteBuffer & fileData)
{
    auto path = std::filesystem::path(visualQuality.prefix) / "anim" / skin / "skin.json";
    if ( archiveCluster.getFile(path.string(), fileData) )
    {
        SkinData skinData {};
        {
            std::stringstream ssfile((const char*)fileData.data());
            ssfile >> Json::in(skinData);
        }

        for ( auto image : skinData.imageList )
            skin.imageUsesSkinTexture[image] = 1;
    }
}

Scr::GraphicsData::DDS_HEADER::Format Scr::GraphicsData::DDS_HEADER::getFormat()
{
    if ( fileType != DDS )
        throw std::runtime_error("Error: Invalid header");

    switch ( pixelFormat.fourCharacterCode )
    {
        case DXT1: return Format {GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, 8};
        case DXT3: return Format {GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, 16};
        case DXT5: return Format {GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, 16};
        default: throw std::runtime_error("Unknown format: " + std::string((const char*)&pixelFormat.fourCharacterCode, 4));
    }
}

gl::Texture Scr::GraphicsData::loadDdsTexture(u8* data, gl::ContextSemaphore* contextSemaphore)
{
    DDS_HEADER* header = (DDS_HEADER*)data;
    GLint mipMapCount = GLint(header->mipMapCount == 0 ? 1 : header->mipMapCount);
    //mipMapCount = 1; // If this is set it disables mipMap loading, TODO: Check if mipmaps are ever used
    DDS_HEADER::Format format = header->getFormat();

    auto contextLock = gl::ContextSemaphore::aquire_unique_context_lock(contextSemaphore);
    gl::Texture texture {};
    texture.genTexture();
    texture.bind();
    if ( mipMapCount > 1 )
    {
        texture.setMipmapLevelRange(0, mipMapCount-1);
        texture.setMinMagFilters(GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST);
    }
    else // No mipMaps
        texture.setMinMagFilters(GL_NEAREST);

    size_t offset = sizeof(DDS_HEADER);
    size_t width = header->width;
    size_t height = header->height;

    for ( GLint i=0; i<mipMapCount; i++ )
    {
        if ( width == 0 || height == 0 )
        {
            mipMapCount--; // Discard empty mipmaps
            continue;
        }
        size_t size = ((width+3)/4) * ((height+3)/4) * format.blockSize;
        texture.loadCompressedImage2D({
            .data = &data[offset],
            .imageSize = GLsizei(size),
            .width = GLsizei(width),
            .height = GLsizei(height),
            .level = i,
            .internalformat = format.id
        });
        offset += size;

        // Next mipMap is half the size the previous
        width /= 2;
        height /= 2;
    }
    texture.setMipmapLevelRange(0, mipMapCount-1);
    gl::Texture::bindDefault();

    return texture;
}

void Scr::GraphicsData::loadDdsTextureSubImage(u8* data, gl::Texture & texture, GLint xOffset, GLint yOffset, GLsizei width, GLsizei height)
{
    DDS_HEADER* header = (DDS_HEADER*)data;
    DDS_HEADER::Format format = header->getFormat();
    size_t offset = sizeof(DDS_HEADER);
    size_t size = size_t((header->width+3)/4) * size_t((header->height+3)/4) * size_t(format.blockSize);
    texture.loadCompressedSubImage2D(gl::Texture::CompressedSubImage2D{
        .data = &data[offset],
        .imageSize = GLsizei(size),
        .xoffset = xOffset,
        .yoffset = yOffset,
        .width = width,
        .height = height,
        .level = 0,
        .format = format.id
    });
}

gl::Texture Scr::GraphicsData::loadBmpLuminanceTexture(u8* data, GLsizei width, GLsizei height, gl::ContextSemaphore* contextSemaphore)
{
    if ( *((u32*)data) != Animation::MainSd::BMP )
        throw std::logic_error("Invalid BMP file type!");
            
    auto contextLock = gl::ContextSemaphore::aquire_unique_context_lock(contextSemaphore);
    gl::Texture texture {};
    texture.genTexture();

    texture.bind();
    texture.setMinMagFilters(GL_NEAREST);
    texture.loadImage2D({
        .data = &data[4],
        .width = width,
        .height = height,
        .internalformat = GL_RED,
        .format = GL_RED
    });
    gl::Texture::bindDefault();

    return texture;
}

std::shared_ptr<Scr::Animation> Scr::GraphicsData::loadSdAnim(Sc::Data & scData, u8* data, u32 index, gl::ContextSemaphore* contextSemaphore)
{
    auto & grp = scData.sprites.getGrp(scData.sprites.getImage(index).grpFile).get();

    Animation::Header* header = (Animation::Header*)data;
    u32 entryOffset = *((u32*)&data[sizeof(Animation::Header) + index*sizeof(u32)]);
    Animation::MainSd::Entry* entry = (Animation::MainSd::Entry*)&data[entryOffset];

    if ( entry->frames == 0 )
        throw std::logic_error("Animation had no frames!");

    auto animation = std::make_shared<Animation>();
    animation->totalFrames = entry->frames;
    animation->width = grp.grpWidth;
    animation->height = grp.grpHeight;
    u32 textureWidth = entry->images[0].textureWidth;
    u32 textureHeight = entry->images[0].textureHeight;
    animation->xScale = 1.0f/textureWidth;
    animation->yScale = 1.0f/textureHeight;

    for ( size_t i = 0; i < size_t(header->layers); i++ )
    {
        if ( entry->images[i].offset != 0 )
        {
            if ( entry->images[i].textureWidth != textureWidth || entry->images[i].textureHeight != textureHeight )
                throw std::logic_error("Texture size mismatch");
            else if ( std::string_view("diffuse") == (const char*)header->layernames[i] )
                animation->diffuse = std::move(loadDdsTexture(&data[entry->images[i].offset], contextSemaphore));
            else if ( std::string_view("teamcolor") == (const char*)header->layernames[i] )
            {
                switch ( *((u32*)(&data[entry->images[i].offset])) )
                {
                    case DDS_HEADER::DDS:
                        animation->teamColor = std::move(loadDdsTexture(&data[entry->images[i].offset], contextSemaphore));
                        break;
                    case Animation::MainSd::BMP:
                        animation->teamColor = std::move(loadBmpLuminanceTexture(
                            &data[entry->images[i].offset], GLuint(entry->images[i].textureWidth), GLuint(entry->images[i].textureHeight)));
                        break;
                    default:
                        throw std::logic_error("Unrecognized image format!");
                        break;
                }
            }
        }
    }
    animation->frames.assign(entry->frames, {});
    std::memcpy(&animation->frames[0], &data[entry->offset], sizeof(Animation::Frame) * entry->frames);
    return animation;
}
        
std::shared_ptr<Scr::Animation> Scr::GraphicsData::loadHdAnim(bool halfAnim, ArchiveCluster & archiveCluster, const std::filesystem::path & path, ByteBuffer & fileData, gl::ContextSemaphore* contextSemaphore)
{
    if ( archiveCluster.getFile(path.string(), fileData) )
    {
        Animation::Header* header = (Animation::Header*)fileData.data();
        Animation::MainSd::Entry* entry = (Animation::MainSd::Entry*)&fileData[sizeof(Animation::Header)];

        if ( header->fileType != Animation::Header::ANIM )
            throw std::logic_error("Invalid header " + std::to_string(header->fileType));
        else if ( header->version == 0x0101 )
            throw std::logic_error("loadAnimHD called for SD animation file!");
        else if ( entry->frames == 0 )
            throw std::logic_error("Animation had no frames!");

        auto animation = std::make_shared<Animation>();
        animation->totalFrames = entry->frames;
        animation->width = entry->grpWidth;
        animation->height = entry->grpHeight;
        u32 textureWidth = entry->images[0].textureWidth;
        u32 textureHeight = entry->images[0].textureHeight;
        animation->xScale = halfAnim ? .5f/textureWidth : 1.0f/textureWidth;
        animation->yScale = halfAnim ? .5f/textureHeight : 1.0f/textureHeight;

        for ( size_t i = 0; i < size_t(header->layers); i++ )
        {
            if ( entry->images[i].offset != 0 )
            {
                if ( entry->images[i].textureWidth != textureWidth || entry->images[i].textureHeight != textureHeight )
                    throw std::logic_error("Texture size mismatch");
                else if ( std::string_view("diffuse") == (const char*)header->layernames[i] )
                    animation->diffuse = std::move(loadDdsTexture(&fileData[entry->images[i].offset], contextSemaphore));
                else if ( std::string_view("teamcolor") == (const char*)header->layernames[i] )
                    animation->teamColor = std::move(loadDdsTexture(&fileData[entry->images[i].offset], contextSemaphore));
            }
        }

        animation->frames.assign(entry->frames, {});
        std::memcpy(&animation->frames[0], &fileData[entry->offset], sizeof(Animation::Frame) * entry->frames);

        return animation;
    }
    return nullptr;
}

bool Scr::GraphicsData::loadGrp(ArchiveCluster & archiveCluster, const std::filesystem::path & path, Grp & grp, ByteBuffer & fileData, bool framedTex, bool mergedTex)
{
    if ( archiveCluster.getFile(path.string(), fileData) )
    {
        Grp::Header* header = (Grp::Header*)fileData.data();
        grp.frames = header->frames;
        if ( header->frames == 0 )
            throw std::logic_error("GRP had no frames!");
        else if ( header->containsDdsFiles() )
        {
            size_t offset = sizeof(Grp::Header);
            if ( framedTex )
                grp.texture.reserve(header->frames);

            for ( size_t i = 0; i < size_t(header->frames) && offset < size_t(header->fileSize); i++ )
            {
                Grp::File* file = (Grp::File*)&fileData[offset];
                offset += sizeof(Grp::File);

                if ( i == 0 )
                {
                    grp.width = file->width;
                    grp.height = file->height;

                    if ( mergedTex )
                    {
                        DDS_HEADER* header = (DDS_HEADER*)&fileData[offset];
                        if ( header->fileType != DDS_HEADER::DDS )
                            throw std::runtime_error("Error: Invalid header");
                                
                        auto format = header->getFormat();
                        grp.mergedTexture.genTexture();
                        grp.mergedTexture.bind();
                        grp.mergedTexture.setMinMagFilters(GL_NEAREST);
                        GLsizei size = GLsizei(128 * 128 * ((header->width+3)/4) * ((header->height+3)/4) * format.blockSize);
                        grp.mergedTexture.loadCompressedImage2D({
                            .data = NULL,
                            .imageSize = GLsizei(size),
                            .width = GLsizei(header->width*128),
                            .height = GLsizei(header->height*128),
                            .level = 0,
                            .internalformat = format.id,
                        });
                    }
                }

                if ( file->width != grp.width || file->height != grp.height )
                    throw std::logic_error("Texture size mismatch");
                else if ( mergedTex )
                {
                    if ( framedTex )
                        grp.mergedTexture.bind();

                    loadDdsTextureSubImage(&fileData[offset], grp.mergedTexture,
                        GLint(i%128)*GLint(grp.width), GLint(i/128)*GLint(grp.height), grp.width, grp.height);
                }

                if ( framedTex && file->size != 0 )
                    grp.texture.push_back(std::move(loadDdsTexture(&fileData[offset])));

                offset += size_t(file->size);
            }
        }
        else // contains paletted bitmap image
        {
            Grp::Bmp* bmp = (Grp::Bmp*)&fileData[sizeof(Grp::Header)];
            size_t bmpSize = size_t(bmp->width) * size_t(bmp->height);
            grp.palette = std::make_optional<gl::Palette>();
            grp.width = bmp->width;
            grp.height = bmp->height;
            grp.palette->tex = {};
            std::memcpy(grp.palette->colors, bmp->palette, sizeof(u32)*256);
            grp.palette->update();

            size_t grpFrameDataSize = fileData.size() - sizeof(Grp::Header) - sizeof(Grp::Bmp);
            GLuint grpFrameDataBuffer = 0;
            grp.texture = gl::Texture::genTextures(header->frames);

            if ( mergedTex )
            {
                grp.mergedTexture.genTexture();
                grp.mergedTexture.bind();
                grp.mergedTexture.setMinMagFilters(GL_NEAREST);
                grp.mergedTexture.loadImage2D({
                    .data = NULL,
                    .width = GLsizei(bmp->width)*128,
                    .height = GLsizei(bmp->height)*128,
                    .internalformat = GL_RED,
                    .format = GL_RED
                });
            }

            glGenBuffers(1, &grpFrameDataBuffer);
            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, grpFrameDataBuffer);
            glBufferData(GL_PIXEL_UNPACK_BUFFER, grpFrameDataSize, &fileData[sizeof(Grp::Header) + sizeof(Grp::Bmp)], GL_STATIC_DRAW);

            size_t offset = 0;
            for ( size_t i = 0; i < size_t(header->frames) && offset + sizeof(Grp::Header) + sizeof(Grp::Bmp) < size_t(header->fileSize); i++ )
            {
                if ( framedTex )
                {
                    grp.texture[i].bind();
                    grp.texture[i].setMinMagFilters(GL_NEAREST);
                    grp.texture[i].loadImage2D({
                        .data = (void*)offset,
                        .width = bmp->width,
                        .height = bmp->height,
                        .internalformat = GL_RED,
                        .format = GL_RED
                    });
                }
                else
                {
                    grp.mergedTexture.loadSubImage2D({
                        .pixels = (void*)offset,
                        .xoffset = GLint(i%128)*GLint(bmp->width),
                        .yoffset = GLint(i/128)*GLint(bmp->height),
                        .width = GLsizei(bmp->width),
                        .height = GLsizei(bmp->height),
                        .format = GL_RED
                    });
                }
                            
                offset += bmpSize;
            }
            gl::Texture::bindDefault();
            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
            glDeleteBuffers(1, &grpFrameDataBuffer);
        }
        return true;
    }
    return false;
}

Scr::SpkGrp Scr::GraphicsData::loadStarGrp(ArchiveCluster & archiveCluster, const std::filesystem::path & path, ByteBuffer & fileData) // An individual starGrp is a texture
{
    if ( archiveCluster.getFile(path.string(), fileData) )
    {
        Grp::Header* header = (Grp::Header*)fileData.data();
        if ( header->frames == 0 )
            throw std::logic_error("StarGrp had no frames!");

        SpkGrp stars {};
        stars.texture.reserve(header->frames);

        stars.layers = header->frames;
        u32 offset = sizeof(Grp::Header);

        for ( size_t i=0; i<size_t(header->frames) && offset<header->fileSize; ++i )
        {
            Grp::File* file = (Grp::File*)(&fileData[offset]);
            if ( file->size == 0 )
                throw std::runtime_error("Invalid texture!");

            stars.texture.push_back(std::move(loadDdsTexture(&fileData[offset + sizeof(Grp::File)])));
            stars.texture[i].xScale = 1.0f/file->width;
            stars.texture[i].yScale = 1.0f/file->height;
            offset += sizeof(Grp::File) + file->size;
        }

        return stars;
    }
    else
        throw std::runtime_error("Failed to get stars \"" + path.string() + "\" from archives");
}

void Scr::GraphicsData::loadMaskedTiles(ArchiveCluster & archiveCluster, Sc::Terrain::Tileset tileset, Grp & tilesetGrp, Grp & tileMasks, std::vector<u16> & maskIds)
{
    GLint savedViewport[4] {};
    glGetIntegerv(GL_VIEWPORT, savedViewport);

    // For masked tiles, place red channel in alpha channel of tiles
    glDisable(GL_BLEND);

    GLuint vao = 0;
    glBindVertexArray(vao);

    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
            
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); // Position
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(GLfloat)));
    glEnableVertexAttribArray(1); // Texture Coord

    GLuint framebuffer = 0;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    // Note: attaching a render-buffer here *might* improve performance if needed

    tileMasks.mergedTexture.genTexture();
    tileMasks.mergedTexture.bind();
    tileMasks.mergedTexture.setMinMagFilters(GL_NEAREST);
    tileMasks.mergedTexture.loadImage2D(gl::Texture::Image2D {
        .data = NULL,
        .width = GLsizei(128*tilesetGrp.width),
        .height = GLsizei(128*tilesetGrp.height),
        .level = 0,
        .internalformat = GL_RGBA,
        .format = GL_RGBA,
        .type = GL_UNSIGNED_BYTE
    });

    std::vector<GLfloat> blackMaskedData {};
    GLfloat blackMaskedPixel[4] { 0.0f, 0.0f, 0.0f, 1.0f };
    for ( size_t i=0; i<size_t(tilesetGrp.width)*size_t(tilesetGrp.height); ++i )
        blackMaskedData.insert(blackMaskedData.end(), blackMaskedPixel, blackMaskedPixel+std::size(blackMaskedPixel));
    gl::Texture blackMask {};
    blackMask.genTexture();
    blackMask.bind();
    blackMask.setMinMagFilters(GL_NEAREST);
    blackMask.loadImage2D(gl::Texture::Image2D {
        .data = &blackMaskedData[0],
        .width = GLsizei(tilesetGrp.width),
        .height = GLsizei(tilesetGrp.height),
        .level = 0,
        .internalformat = GL_RGBA,
        .format = GL_RGBA,
        .type = GL_UNSIGNED_BYTE
    });
    blackMaskedData.clear();

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, tileMasks.mergedTexture.getId(), 0);
    GLenum drawBuffers[] { GL_NONE, GL_COLOR_ATTACHMENT1 }; // effect_mask includes a seemingly deprecated render target at location 0
    glDrawBuffers(2, &drawBuffers[0]);

    glViewport(0, 0, 128*tilesetGrp.width, 128*tilesetGrp.height);

    GLfloat posToNdc[4][4] { // Converts 2D game coordinates (0 to tex width/height) to NDCs which range (-1 to 1)
        { 2.0f/(128*tilesetGrp.width),   0, 0, 0 }, // x = 2x/width
        {  0, 2.0f/(128*tilesetGrp.height), 0, 0 }, // y = 2y/height
        {  0, 0,                            1, 0 },
        { -1, -1,                           0, 1 } // x = x-1, y = y-1
    };

    Shaders::EffectMask effectMask {};
    effectMask.load(archiveCluster);
    effectMask.use();
    effectMask.spriteTex.setSlot(0); //setSpriteTex(0);
    effectMask.sampleTex.setSlot(1); //effectMask.setSampleTex(1);
    effectMask.posToNdc.setMat4(&posToNdc[0][0]);

    for ( u32 i=0; i<65536; ++i )
    {
        u16 mask = maskIds[i];
        if ( mask != 0xFFFF ) // tile has mask
        {
            u32 left = i%128 * tilesetGrp.width;
            u32 top = i/128 * tilesetGrp.height;
            u32 right = left + tilesetGrp.width;
            u32 bottom = top + tilesetGrp.height;
            GLfloat verticies2[] {
                GLfloat(left), GLfloat(top), 0.f, 0.f,
                GLfloat(right), GLfloat(top), 1.f, 0.f,
                GLfloat(left), GLfloat(bottom), 0.f, 1.f,
                GLfloat(left), GLfloat(bottom), 0.f, 1.f,
                GLfloat(right), GLfloat(bottom), 1.f, 1.f,
                GLfloat(right), GLfloat(top), 1.f, 0.f
            };
            tilesetGrp.texture[i].bindToSlot(GL_TEXTURE0);
            tileMasks.texture[mask].bindToSlot(GL_TEXTURE1);
            glBufferData(GL_ARRAY_BUFFER, sizeof(verticies2), &verticies2[0], GL_DYNAMIC_DRAW);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        else
        {
            u32 left = i%128 * tilesetGrp.width;
            u32 top = i/128 * tilesetGrp.height;
            u32 right = left + tilesetGrp.width;
            u32 bottom = top + tilesetGrp.height;
            GLfloat verticies2[] {
                GLfloat(left), GLfloat(top), GLfloat(left)/(128*tilesetGrp.width), GLfloat(top)/(128*tilesetGrp.height),// 0.f, 0.f,
                GLfloat(right), GLfloat(top), GLfloat(right)/(128*tilesetGrp.width), GLfloat(top)/(128*tilesetGrp.height),// 1.f, 0.f,
                GLfloat(left), GLfloat(bottom), GLfloat(left)/(128*tilesetGrp.width), GLfloat(bottom)/(128*tilesetGrp.height),// 0.f, 1.f,
                GLfloat(left), GLfloat(bottom), GLfloat(left)/(128*tilesetGrp.width), GLfloat(bottom)/(128*tilesetGrp.height),// 0.f, 1.f,
                GLfloat(right), GLfloat(bottom), GLfloat(right)/(128*tilesetGrp.width), GLfloat(bottom)/(128*tilesetGrp.height),// 1.f, 1.f,
                GLfloat(right), GLfloat(top), GLfloat(right)/(128*tilesetGrp.width), GLfloat(top)/(128*tilesetGrp.height)//, 1.f, 0.f
            };
            tilesetGrp.mergedTexture.bindToSlot(GL_TEXTURE0);
            blackMask.bindToSlot(GL_TEXTURE1);
            glBufferData(GL_ARRAY_BUFFER, sizeof(verticies2), &verticies2[0], GL_DYNAMIC_DRAW);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
    }

    glFlush();

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, 0, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    gl::Texture::releaseSlots(GL_TEXTURE1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glViewport(savedViewport[0], savedViewport[1], savedViewport[2], savedViewport[3]);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
        
void Scr::GraphicsData::loadTileMasks(ArchiveCluster & archiveCluster, std::filesystem::path path, std::vector<u16> & maskIds)
{
    auto tmskFile = archiveCluster.getFile(path.string());
    TileMask::File* file = (TileMask::File*)tmskFile->data();
    std::vector<TileMask::Entry> tmsk(size_t(file->count), TileMask::Entry{});
    std::memcpy(&tmsk[0], file->tileMasks, sizeof(TileMask::Entry)*file->count);
    maskIds.assign(65536, 0xFFFF);
    for ( size_t i=0; i<tmsk.size(); ++i )
    {
        if ( tmsk[i].maskIndex != 0xFFFF )
            maskIds[tmsk[i].vr4Index] = tmsk[i].maskIndex;
    }
}

void Scr::GraphicsData::Data::Skin::Tileset::load(ArchiveCluster & archiveCluster, const LoadSettings & loadSettings, ByteBuffer & fileData)
{
    constexpr std::string_view tilesetNames[8] { "badlands", "platform", "install", "ashworld", "jungle", "desert", "ice", "twilight" };
    std::string tilesetName = std::string(tilesetNames[size_t(loadSettings.tileset) % size_t(Sc::Terrain::NumTilesets)]);
    auto visualQuality = loadSettings.visualQuality;
    auto skinName = Scr::Skin::skinNames[size_t(loadSettings.skinId)];
    std::filesystem::path tilesetPath {};
    if ( loadSettings.skinId != Scr::Skin::Id::Remastered && visualQuality > VisualQuality::SD )
        tilesetPath = std::filesystem::path(visualQuality.prefix) / skinName / "tileset" / (tilesetName + ".dds.vr4");
    else
        tilesetPath = std::filesystem::path(visualQuality.prefix) / "tileset" / (tilesetName + ".dds.vr4");

    if ( visualQuality > VisualQuality::SD && // HD tileset effects for badlands, ashworld, jungle, desert, ice, twilight
        (Sc::Terrain::Tileset)loadSettings.tileset != Sc::Terrain::Tileset::SpacePlatform &&
        (Sc::Terrain::Tileset)loadSettings.tileset != Sc::Terrain::Tileset::Installation )
    {
        auto path = std::filesystem::path(visualQuality.prefix) / skinName / "tileset" / (tilesetName + ".tmsk");
        loadTileMasks(archiveCluster, path, maskIds);

        path = std::filesystem::path(visualQuality.prefix) / skinName / "tileset" / (tilesetName + "_mask.dds.grp");
        loadGrp(archiveCluster, path, tileMask, fileData, true, false);
        loadGrp(archiveCluster, tilesetPath, tilesetGrp, fileData, true, true); // Loads the merged tex and (temporarily) the individual tiles
        loadMaskedTiles(archiveCluster, loadSettings.tileset, tilesetGrp, tileMask, maskIds);
        tileMask.texture.clear(); // Individual tiles/tile-masks only used for loading masked tiles and can be cleared after
        tilesetGrp.texture.clear();
    }
    else
        loadGrp(archiveCluster, tilesetPath, tilesetGrp, fileData, false, true);
}

void Scr::GraphicsData::Data::Skin::loadClassicStars(Sc::Data & scData)
{
    if ( spk == nullptr )
    {
        spk = std::make_shared<SpkData>();
        for ( const auto & layer : scData.spk.layerStars )
        {
            auto & starLayer = spk->classicLayers.emplace_back();
            for ( const auto & star : layer )
            {
                u16 alignedWidth = (star.bitmap->width+3)/4*4;
                u16 alignedHeight = (star.bitmap->height+3)/4*4;
                auto & classicStar = starLayer.emplace_back(Scr::ClassicStar {
                    .xc = star.xc,
                    .yc = star.yc,
                    .width = star.bitmap->width,
                    .height = star.bitmap->height,
                    .texWidth = alignedWidth,
                    .texHeight = alignedHeight
                });
                classicStar.tex.genTexture();
                classicStar.tex.bind();
                classicStar.tex.setMinMagFilters(GL_NEAREST);
                classicStar.tex.setTextureWrap(GL_CLAMP_TO_EDGE);
                if ( star.bitmap->width % 4 == 0 && star.bitmap->height % 4 == 0 ) // Dimensions already 4-byte aligned
                {
                    classicStar.tex.loadImage2D({
                        .data = &star.bitmap->data[0],
                        .width = int(star.bitmap->width),
                        .height = int(star.bitmap->height),
                        .level = 0,
                        .internalformat = GL_R8UI,
                        .format = GL_RED_INTEGER,
                        .type = GL_UNSIGNED_BYTE
                    });
                }
                else // Need to align the data prior to loading
                {
                    std::vector<u8> bitmap(size_t(alignedWidth)*size_t(alignedHeight), u8(0));
                    if ( star.bitmap->width % 4 == 0 ) // Width already 4-byte aligned
                    {
                        std::memcpy(&bitmap[0], &star.bitmap->data[0], size_t(star.bitmap->width)*size_t(star.bitmap->height));
                        classicStar.tex.loadImage2D({
                            .data = &bitmap[0],
                            .width = int(alignedWidth),
                            .height = int(alignedHeight),
                            .level = 0,
                            .internalformat = GL_R8UI,
                            .format = GL_RED_INTEGER,
                            .type = GL_UNSIGNED_BYTE
                        });
                    }
                    else // Width and height unaligned
                    {
                        for ( size_t y=0; y<size_t(star.bitmap->height); ++y )
                            std::memcpy(&bitmap[y*size_t(alignedWidth)], &star.bitmap->data[y*size_t(star.bitmap->width)], size_t(star.bitmap->width));

                        classicStar.tex.loadImage2D({
                            .data = &bitmap[0],
                            .width = int(alignedWidth),
                            .height = int(alignedHeight),
                            .level = 0,
                            .internalformat = GL_R8UI,
                            .format = GL_RED_INTEGER,
                            .type = GL_UNSIGNED_BYTE
                        });
                    }
                }
                gl::Texture::bindDefault();
            }
        }
    }
}

void Scr::GraphicsData::Data::Skin::loadStars(ArchiveCluster & archiveCluster, std::filesystem::path texPrefix, ByteBuffer & fileData)
{
    if ( spk == nullptr )
    {
        spk = std::make_shared<SpkData>();
        spk->stars = std::move(loadStarGrp(archiveCluster, (texPrefix / "parallax" / "star.dds.grp").string(), fileData));

        if ( auto file = archiveCluster.getFile((texPrefix / "parallax" / "star.spk").string()) )
            spk->load(std::move(*file));
        else
            logger.error() << "Error loading star.spk\n";
    }
}

void Scr::GraphicsData::Data::Skin::loadClassicTiles(Sc::Data & scData, const LoadSettings & loadSettings)
{
    auto tilesetIndex = size_t(loadSettings.tileset) % size_t(Sc::Terrain::NumTilesets);
    this->tiles[tilesetIndex] = std::make_shared<Scr::GraphicsData::Data::Skin::Tileset>();
    // Populate tileTextureData
    constexpr size_t width = 32*128;
    std::vector<u8> tileTextureData(size_t(width*4096), u8(0)); // 4096x4096 palette indexes (128x128 tiles)
    auto tiles = scData.terrain.get(loadSettings.tileset);
    for ( u32 i=0; i<65536; ++i )
    {
        size_t groupIndex = Sc::Terrain::Tiles::getGroupIndex(u16(i));
        if ( groupIndex < tiles.tileGroups.size() )
        {
            const Sc::Terrain::TileGroup & tileGroup = tiles.tileGroups[groupIndex];
            const u16 megaTileIndex = tileGroup.megaTileIndex[tiles.getGroupMemberIndex(u16(i))];
            u32 left = megaTileIndex%128 * 32;
            u32 top = megaTileIndex/128 * 32;
            const Sc::Terrain::TileGraphicsEx & tileGraphics = tiles.tileGraphics[megaTileIndex];
            for ( size_t yMiniTile = 0; yMiniTile < 4; yMiniTile++ )
            {
                s64 yMiniOffset = top + yMiniTile*8;
                for ( size_t xMiniTile = 0; xMiniTile < 4; xMiniTile++ )
                {
                    const Sc::Terrain::TileGraphicsEx::MiniTileGraphics & miniTileGraphics = tileGraphics.miniTileGraphics[yMiniTile][xMiniTile];
                    bool flipped = miniTileGraphics.isFlipped();
                    size_t vr4Index = size_t(miniTileGraphics.vr4Index());
                
                    s64 xMiniOffset = left + xMiniTile*8;
                    for ( s64 yMiniPixel = yMiniOffset < 0 ? -yMiniOffset : 0; yMiniPixel < 8; yMiniPixel++ )
                    {
                        for ( s64 xMiniPixel = xMiniOffset < 0 ? -xMiniOffset : 0; xMiniPixel < 8; xMiniPixel++ )
                        {
                            const Sc::Terrain::MiniTilePixels & miniTilePixels = tiles.miniTilePixels[vr4Index];
                            const u8 & wpeIndex = miniTilePixels.wpeIndex[yMiniPixel][flipped ? 7-xMiniPixel : xMiniPixel];
                            tileTextureData[size_t((yMiniOffset+yMiniPixel)*width + (xMiniOffset+xMiniPixel))] = wpeIndex;
                        }
                    }
                }
            }
        }
    }

    // Load tileTextureData into a GRP
    auto & tilesetGrp = this->tiles[tilesetIndex]->tilesetGrp;
    tilesetGrp.width = 32;
    tilesetGrp.height = 32;
    tilesetGrp.frames = 1;
    auto & texture = tilesetGrp.mergedTexture;
    texture.genTexture();
    texture.bind();
    texture.setMinMagFilters(GL_NEAREST);
    texture.loadImage2D(gl::Texture::Image2D {
        .data = &tileTextureData[0],
        .width = GLsizei(128*tilesetGrp.width),
        .height = GLsizei(128*tilesetGrp.height),
        .level = 0,
        .internalformat = GL_R8UI,
        .format = GL_RED_INTEGER,
        .type = GL_UNSIGNED_BYTE
    });
    tileTextureData.clear();
    gl::Texture::bindDefault();

    // Load palette
    tilesetGrp.palette.emplace();
    std::memcpy(&(tilesetGrp.palette.value())[0], &tiles.staticSystemColorPalette[0], 4*256);
    for ( size_t i=0; i<256; ++i )
        std::swap(((u8*)&tilesetGrp.palette.value()[i])[0], (((u8*)&tilesetGrp.palette.value()[i])[2])); // red-blue swap

    tilesetGrp.palette->update();
}

void Scr::GraphicsData::Data::Skin::loadTiles(ArchiveCluster & archiveCluster, const LoadSettings & loadSettings, ByteBuffer & fileData)
{
    auto tilesetIndex = size_t(loadSettings.tileset) % size_t(Sc::Terrain::NumTilesets);
    if ( tiles[tilesetIndex] == nullptr )
    {
        tiles[tilesetIndex] = std::make_shared<Tileset>();
        tiles[tilesetIndex]->load(archiveCluster, loadSettings, fileData);
    }
}

void Scr::GraphicsData::Data::Skin::loadClassicImages(Sc::Data & scData)
{
    classicImages = std::make_shared<std::vector<std::shared_ptr<Scr::ClassicGrp>>>();
    classicImages->assign(999, nullptr);
    std::vector<u8> bitmap(size_t(60480), u8(0));

    for ( size_t i=0; i<999; ++i )
    {
        auto & imageDat = scData.sprites.getImage(i);
        auto & grp = scData.sprites.getGrp(imageDat.grpFile);
        auto & grpFile = grp.get();
        size_t numFrames = size_t(grpFile.numFrames);
        if ( numFrames > 0 )
        {
            auto classicImage = std::make_shared<Scr::ClassicGrp>();
            (*classicImages)[i] = classicImage;
            classicImage->frames.reserve(numFrames);
            classicImage->grpWidth = grpFile.grpWidth;
            classicImage->grpHeight = grpFile.grpHeight;
            for ( size_t frameIndex=0; frameIndex<numFrames; ++frameIndex )
            {
                classicImage->frames.emplace_back();
                const Sc::Sprite::GrpFrameHeader & grpFrameHeader = grpFile.frameHeaders[frameIndex];
                auto & frame = classicImage->frames[frameIndex];
                s64 frameWidth = s64(grpFrameHeader.frameWidth);
                s64 frameHeight = s64(grpFrameHeader.frameHeight);
                if ( frameWidth == 0 || frameHeight == 0 ) // A dimension is zero, nothing to draw
                    continue;

                s64 bitmapWidth = (frameWidth+3)/4*4;
                s64 bitmapHeight = (frameHeight+3)/4*4;
                frame.frameWidth = grpFrameHeader.frameWidth;
                frame.frameHeight = grpFrameHeader.frameHeight;
                frame.texWidth = u32(bitmapWidth);
                frame.texHeight = u32(bitmapHeight);
                frame.xOffset = grpFrameHeader.xOffset;
                frame.yOffset = grpFrameHeader.yOffset;
        
                size_t frameOffset = size_t(grpFrameHeader.frameOffset);
                const Sc::Sprite::GrpFrame & grpFrame = (const Sc::Sprite::GrpFrame &)((u8*)&grpFile)[frameOffset];
                for ( s64 row=0; row < frameHeight; row++ )
                {
                    size_t rowOffset = size_t(grpFrame.rowOffsets[row]);
                    const Sc::Sprite::PixelRow & grpPixelRow = (const Sc::Sprite::PixelRow &)((u8*)&grpFile)[frameOffset+rowOffset];
                    const s64 rowStart = row*bitmapWidth;
                    s64 currPixelIndex = rowStart;
                    size_t pixelLineOffset = 0;
                    auto addition = size_t(row*bitmapWidth);
                    auto currPixel = &bitmap[0]+addition; // Start from the left-most pixel of this row of the frame
                    auto rowEnd = row == bitmapHeight-1 ? &bitmap[0]+bitmapWidth*bitmapHeight : &bitmap[0]+size_t(currPixelIndex+frameWidth);

                    while ( currPixel < rowEnd )
                    {
                        const Sc::Sprite::PixelLine & pixelLine = (const Sc::Sprite::PixelLine &)((u8*)&grpPixelRow)[pixelLineOffset];
                        s64 lineLength = s64(pixelLine.lineLength());
                        if ( std::distance(currPixel, rowEnd) < lineLength )
                            lineLength = rowEnd - currPixel;
                    
                        if ( pixelLine.isSpeckled() )
                        {
                            for ( s64 linePixel=0; linePixel<lineLength; linePixel++, ++currPixel ) // For every pixel in the line
                                *currPixel = pixelLine.paletteIndex[linePixel]; // Place color from palette index specified in the array at current pixel
                        }
                        else // Solid or transparent
                        {
                            if ( pixelLine.isSolidLine() )
                                std::fill_n(currPixel, lineLength, pixelLine.paletteIndex[0]); // Place single color across the entire line

                            currPixel += size_t(lineLength);
                        }
                        pixelLineOffset += pixelLine.sizeInBytes();
                    }
                }
                
                frame.tex.genTexture();
                frame.tex.bind();
                frame.tex.setMinMagFilters(GL_NEAREST);
                frame.tex.loadImage2D({
                    .data = &bitmap[0],
                    .width = int(bitmapWidth),
                    .height = int(bitmapHeight),
                    .level = 0,
                    .internalformat = GL_R8UI,
                    .format = GL_RED_INTEGER,
                    .type = GL_UNSIGNED_BYTE
                });
                gl::Texture::bindDefault();

                std::fill(&bitmap[0], &bitmap[0] + bitmapWidth * bitmapHeight, 0);
            }
        }
    }
}

void Scr::GraphicsData::Data::Skin::loadImages(Sc::Data & scData, ArchiveCluster & archiveCluster, std::filesystem::path texPrefix, const LoadSettings & loadSettings, ByteBuffer & fileData, gl::ContextSemaphore* contextSemaphore)
{
    bool halfAnims = loadSettings.visualQuality.halfAnims;
    if ( this->images == nullptr )
        this->images = std::make_shared<std::vector<std::shared_ptr<Animation>>>(size_t(999), nullptr);

    Scr::Skin skin {};
    skin.skinName = Scr::Skin::skinNames[size_t(loadSettings.skinId)];
    loadSkin(skin, archiveCluster, loadSettings.visualQuality, fileData);

    std::filesystem::path animPrefix = texPrefix / "anim";
    std::filesystem::path skinAnimPrefix = animPrefix / skin;
    auto sdDataFile = archiveCluster.getFile("SD/mainSD.anim");
    u8* sdData = sdDataFile->data();
    if ( sdData == nullptr )
        throw std::logic_error("Error loading mainSD.anim");

    Animation::Header* header = (Animation::Header*)sdData;
    u32* entryOffsets = (u32*)(&sdData[sizeof(Animation::Header)]);

    if ( header->fileType != Animation::Header::ANIM || header->version != 0x0101 )
        throw std::logic_error("Invalid mainSD.anim header");
    else if ( loadSettings.visualQuality == VisualQuality::SD || contextSemaphore == nullptr ) // SD (not reading additional casc files) or no context semaphore
    {
        for ( u32 i=0; i<header->entries; i++ )
        {
            Animation::MainSd::Entry* entry = (Animation::MainSd::Entry*)&sdData[entryOffsets[i]];
            if ( entry->frames == 0 )
            {
                Animation::MainSd::Reference* ref = (Animation::MainSd::Reference*)(entry);
                if ( ref->referencedImage > i || (*images)[ref->referencedImage] == nullptr )
                    throw std::logic_error("Referenced anim was not yet loaded!");
                else
                    (*images)[i] = (*images)[ref->referencedImage];
            }
            else if ( loadSettings.visualQuality == VisualQuality::SD )
                (*images)[i] = loadSdAnim(scData, sdData, i);
            else if ( skin.imageUsesSkinTexture[i] != 0 )
                (*images)[i] = loadHdAnim(halfAnims, archiveCluster, skinAnimPrefix / (std::string("main_") + (to_zero_padded_str(3, i) + ".anim")), fileData);
            else
                (*images)[i] = loadHdAnim(halfAnims, archiveCluster, animPrefix / (std::string("main_") + (to_zero_padded_str(3, i) + ".anim")), fileData);
        }
    }
    else // Have a context semaphore and loading VisualQuality::HD2/HD which reads ~1000 casc files, use multithreading for up to ~70% speedup
    {
        struct Task
        {
            bool halfAnim;
            ArchiveCluster* archiveCluster;
            std::filesystem::path path;
            std::shared_ptr<Animation>* anim;
            gl::ContextSemaphore* contextSemaphore;

            size_t requiredMemory = 0;

            void perform(ByteBuffer & memory) { *anim = GraphicsData::loadHdAnim(halfAnim, *archiveCluster, path.string(), memory, contextSemaphore); }
        };

        MemoryTieredTasks<Task> memoryTieredTasks {std::min((unsigned int)12, std::thread::hardware_concurrency()), {
            {.range = {.min=           0, .max= 16*1024*1024}, .totalBuffers = 12}, // Twelve 16MB buffers
            {.range = {.min=16*1024*1024, .max= 42*1024*1024}, .totalBuffers =  2}, // Two 42MB buffers
            {.range = {.min=42*1024*1024, .max=120*1024*1024}, .totalBuffers =  1}  // One 120MB buffer
        }};

        memoryTieredTasks.run_tasks([&](){
            contextSemaphore->releaseContext();
            for ( u32 i=0; i<header->entries; i++ )
            {
                Animation::MainSd::Entry* entry = (Animation::MainSd::Entry*)&sdData[entryOffsets[i]];
                if ( entry->frames > 0 )
                {
                    if ( loadSettings.visualQuality == VisualQuality::SD )
                        (*images)[i] = loadSdAnim(scData, sdData, i);
                    else
                    {
                        std::filesystem::path path {};
                        if ( skin.imageUsesSkinTexture[i] != 0 )
                            path = skinAnimPrefix / (std::string("main_") + (to_zero_padded_str(3, i) + ".anim"));
                        else
                            path = animPrefix / (std::string("main_") + (to_zero_padded_str(3, i) + ".anim"));

                        memoryTieredTasks.add_task({
                            .halfAnim = loadSettings.visualQuality.halfAnims,
                            .archiveCluster = &archiveCluster,
                            .path = path,
                            .anim = &(*images)[i],
                            .contextSemaphore = contextSemaphore,
                            .requiredMemory = archiveCluster.getFileSize(path.string())
                        });
                    }
                }
            }
        });
        contextSemaphore->claimContext();

        for ( u32 i=0; i<header->entries; i++ )
        {
            Animation::MainSd::Entry* entry = (Animation::MainSd::Entry*)&sdData[entryOffsets[i]];
            if ( entry->frames == 0 )
            {
                Animation::MainSd::Reference* ref = (Animation::MainSd::Reference*)(entry);
                if ( ref->referencedImage > i || (*images)[ref->referencedImage] == nullptr )
                    throw std::logic_error("Referenced anim was not yet loaded!");
                else
                    (*images)[i] = (*images)[ref->referencedImage];
            }
        }
    }
}

void Scr::GraphicsData::Data::loadWaterNormals(ArchiveCluster & archiveCluster, std::filesystem::path texPrefix, ByteBuffer & fileData)
{
    for ( size_t i=0; i<std::size(waterNormal); ++i )
    {
        if ( waterNormal[i] == nullptr )
        {
            waterNormal[i] = std::make_shared<Grp>();
            auto path = texPrefix / "effect" / ("water_normal_" + std::to_string(i+1) + ".dds.grp");
            if ( !loadGrp(archiveCluster, path, *waterNormal[i], fileData, true, false) )
            {
                logger.error("Failed to load \"" + path.string() + "\"");
                waterNormal[i] = nullptr;
            }
        }
    }
}

void Scr::GraphicsData::unload(const LoadSettings & loadSettings)
{
    auto visualQualityIndex = loadSettings.visualQuality.index();
    if ( visualQualityData[visualQualityIndex] != nullptr )
    {
        if ( visualQualityData[visualQualityIndex].use_count() == 1 ) // Visual quality unused
            visualQualityData[visualQualityIndex] = nullptr;
        else // Visual quality still used, check individual component usage
        {
            auto skinIndex = size_t(loadSettings.skinId);
            Data & data = *visualQualityData[visualQualityIndex];
            if ( data.skin[skinIndex] != nullptr )
            {
                if ( data.skin[skinIndex].use_count() == 1 ) // Skin unused
                    data.skin[skinIndex] = nullptr;
                else // Skin still used, check individual component usage
                {
                    auto & skin = *data.skin[skinIndex];
                    if ( skin.spk != nullptr && skin.spk.use_count() == 1 ) // Spk unused
                        skin.spk = nullptr;

                    auto tilesetIndex = size_t(loadSettings.tileset) % Sc::Terrain::NumTilesets;
                    if ( skin.tiles[tilesetIndex] != nullptr && skin.tiles[tilesetIndex].use_count() == 1 ) // Tiles unused
                        skin.tiles[tilesetIndex] = nullptr;

                    if ( skin.images != nullptr && skin.images.use_count() == 1 ) // Images unused
                        skin.images = nullptr;
                }

                for ( size_t i=0; i<std::size(data.waterNormal); ++i )
                {
                    if ( data.waterNormal[i] != nullptr && data.waterNormal[i].use_count() == 1 ) // Water normal unused
                        data.waterNormal[i] = nullptr;
                }
            }
        }
    }

    if ( shaders != nullptr && shaders.use_count() == 1 ) // Shaders unused
        shaders = nullptr;
}

bool Scr::GraphicsData::isLoaded(const LoadSettings & loadSettings)
{
    bool isRemastered = loadSettings.skinId != Scr::Skin::Id::Classic;

    if ( shaders == nullptr )
        return false;

    auto visualQualityIndex = loadSettings.visualQuality.index();
    if ( visualQualityData[visualQualityIndex] == nullptr )
        return false;

    auto & data = *(visualQualityData[visualQualityIndex]);
    if ( isRemastered && Sc::Terrain::hasWater(loadSettings.tileset) && loadSettings.visualQuality > VisualQuality::SD &&
        (data.waterNormal[0] == nullptr || data.waterNormal[1] == nullptr) )
    {
        return false;
    }

    auto skinIndex = size_t(loadSettings.skinId);
    if ( skinIndex >= Scr::Skin::total )
        throw std::logic_error("Invalid skinId");
    else if ( data.skin[skinIndex] == nullptr )
        return false;

    auto & skin = *(data.skin[skinIndex]);
    if ( loadSettings.showStars() && skin.spk == nullptr )
        return false;

    auto tilesetIndex = size_t(loadSettings.tileset) % Sc::Terrain::NumTilesets;
    if ( skin.tiles[tilesetIndex] == nullptr )
        return false;

    return isRemastered ? skin.images != nullptr : skin.classicImages != nullptr;
}

std::shared_ptr<Scr::GraphicsData::RenderData> Scr::GraphicsData::load(Sc::Data & scData, ArchiveCluster & archiveCluster, const LoadSettings & loadSettings, ByteBuffer & fileData)
{
    bool isRemastered = loadSettings.skinId != Scr::Skin::Id::Classic;

    if ( shaders == nullptr )
        shaders = std::make_shared<Shaders>();

    if ( isRemastered )
        shaders->load(archiveCluster);
    else
        shaders->loadClassic();

    auto visualQualityIndex = loadSettings.visualQuality.index();
    if ( visualQualityData[visualQualityIndex] == nullptr )
        visualQualityData[visualQualityIndex] = std::make_shared<Data>();

    auto & data = *(visualQualityData[visualQualityIndex]);
    std::filesystem::path visualQualityPrefix = std::filesystem::path(loadSettings.visualQuality.prefix);
    if ( isRemastered && Sc::Terrain::hasWater(loadSettings.tileset) && loadSettings.visualQuality > VisualQuality::SD )
        data.loadWaterNormals(archiveCluster, visualQualityPrefix, fileData);

    auto skinIndex = size_t(loadSettings.skinId);
    Scr::Skin skinDescriptor { .skinName = std::string(Scr::Skin::skinNames[skinIndex]) };
    if ( skinIndex >= Scr::Skin::total )
        throw std::logic_error("Invalid skinId");
    else if ( data.skin[skinIndex] == nullptr )
        data.skin[skinIndex] = std::make_shared<Data::Skin>();

    auto & skin = *(data.skin[skinIndex]);
    if ( skin.spk == nullptr && loadSettings.showStars() )
    {
        if ( isRemastered )
            skin.loadStars(archiveCluster, visualQualityPrefix / skinDescriptor, fileData);
        else
            skin.loadClassicStars(scData);
    }
    
    auto tilesetIndex = size_t(loadSettings.tileset) % Sc::Terrain::NumTilesets;
    if ( skin.tiles[tilesetIndex] == nullptr )
    {
        if ( isRemastered )
            skin.loadTiles(archiveCluster, loadSettings, fileData);
        else
            skin.loadClassicTiles(scData, loadSettings);
    }
    
    if ( isRemastered && skin.images == nullptr )
        skin.loadImages(scData, archiveCluster, visualQualityPrefix, loadSettings, fileData, openGlContextSemaphore);
    else if ( !isRemastered && skin.classicImages == nullptr )
        skin.loadClassicImages(scData);

    std::shared_ptr<RenderData> renderData = std::make_shared<RenderData>();
    renderData->shaders = this->shaders;
    renderData->spk = skin.spk;
    renderData->tiles = skin.tiles[loadSettings.tileset];
    renderData->images = skin.images;
    renderData->classicImages = skin.classicImages;
    renderData->waterNormal[0] = data.waterNormal[0];
    renderData->waterNormal[1] = data.waterNormal[1];
    return renderData;
}

u32 Scr::MapGraphics::getPlayerColor(u8 player)
{
    return (u32 &)(scData.tunit.rgbaPalette[8*size_t(player)]);
}

size_t Scr::MapGraphics::getImageId(Sc::Unit::Type unitType)
{
    u32 flingyId = u32(scData.units.getUnit(Sc::Unit::Type(unitType >= 228 ? 0 : unitType)).graphics);
    u32 spriteId = u32(scData.units.getFlingy(flingyId >= 209 ? 0 : flingyId).sprite);
    return scData.sprites.getSprite(spriteId >= 517 ? 0 : spriteId).imageFile;
}

size_t Scr::MapGraphics::getImageId(Sc::Sprite::Type spriteType)
{
    return scData.sprites.getSprite(spriteType >= 517 ? 0 : spriteType).imageFile;
}

size_t Scr::MapGraphics::getImageId(const Chk::Unit & unit)
{
    return getImageId(unit.type);
}

size_t Scr::MapGraphics::getImageId(Sc::Sprite::Type spriteType, bool isDrawnAsSprite)
{
    return isDrawnAsSprite ?
        getImageId(spriteType) :
        getImageId(Sc::Unit::Type(spriteType));
}

size_t Scr::MapGraphics::getImageId(const Chk::Sprite & sprite)
{
    return sprite.isDrawnAsSprite() ?
        getImageId(sprite.type) :
        getImageId(Sc::Unit::Type(sprite.type));
}

Scr::MapGraphics::UnitInfo Scr::MapGraphics::getUnitInfo(Sc::Unit::Type unitType, bool attached, bool lifted)
{
    auto getPreferredFrame = [&](u16 unitId) -> size_t {
        switch ( Sc::Unit::Type(unitId) )
        {
            case Sc::Unit::Type::TerranSiegeTank_SiegeMode:
            case Sc::Unit::Type::EdmundDuke_SiegeMode: return 5;
            case Sc::Unit::Type::SiegeTankTurret_SiegeMode:
            case Sc::Unit::Type::DukeTurretType2: return 0x55;
            case Sc::Unit::Type::TerranGoliath:
            case Sc::Unit::Type::GoliathTurret:
            case Sc::Unit::Type::AlanSchezar_Goliath:
            case Sc::Unit::Type::AlanTurret: return 0x77;
            default: return 0;
        }
    };
    auto getLiftedFrame = [&](u16 unitId) -> size_t {
        switch ( Sc::Unit::Type(unitId) )
        {
            case Sc::Unit::Type::InfestedCommandCenter:
            case Sc::Unit::Type::TerranBarracks:
            case Sc::Unit::Type::TerranCommandCenter:
            case Sc::Unit::Type::TerranEngineeringBay: return 4;
            case Sc::Unit::Type::TerranFactory:
            case Sc::Unit::Type::TerranScienceFacility: return 5;
            case Sc::Unit::Type::TerranStarport: return 3;
            default: return 0;
        }
    };
    auto getSubUnitOffset = [&](u16 unitId) {
        switch ( Sc::Unit::Type(unitId) )
        {
            case Sc::Unit::Type::SiegeTankTurret_SiegeMode:
            case Sc::Unit::Type::DukeTurretType2: return point{8, 16};
            default: return point{0, 0};
        }
    };
    auto getOverlay = [&](u16 unitId) -> size_t {
        switch ( Sc::Unit::Type(unitId) )
        {
            case Sc::Unit::Type::InfestedCommandCenter: return 101;
            default: return std::numeric_limits<size_t>::max();
        }
    };
    auto getAttachedOverlay = [&](u16 unitId) -> size_t {
        switch ( Sc::Unit::Type(unitId) )
        {
            case Sc::Unit::Type::TerranComsatStation: return 272;
            case Sc::Unit::Type::TerranControlTower: return 282;
            case Sc::Unit::Type::TerranCovertOps: return 289;
            case Sc::Unit::Type::TerranMachineShop: return 294;
            case Sc::Unit::Type::TerranNuclearSilo: return 313;
            case Sc::Unit::Type::TerranPhysicsLab: return 302;
            default: return std::numeric_limits<size_t>::max();
        }
    };
    auto getAttachedOverlayPreferredFrame = [&](size_t unitId) -> size_t {
        switch ( Sc::Unit::Type(unitId) )
        {
            case Sc::Unit::Type::TerranComsatStation: return 3;
            case Sc::Unit::Type::TerranControlTower: return 4;
            case Sc::Unit::Type::TerranCovertOps: return 4;
            case Sc::Unit::Type::TerranMachineShop: return 3;
            case Sc::Unit::Type::TerranNuclearSilo: return 4;
            case Sc::Unit::Type::TerranPhysicsLab: return 4;
            default: return std::numeric_limits<size_t>::max();
        }
    };

    auto & unitDat = scData.units.getUnit(Sc::Unit::Type(unitType >= 228 ? 0 : unitType));
    size_t subUnitImageId = unitDat.subunit1 == 228 ? std::numeric_limits<size_t>::max() :
        scData.sprites.getSprite(scData.units.getFlingy(scData.units.getUnit(unitDat.subunit1).graphics).sprite).imageFile;
    u32 flingyId = u32(scData.units.getUnit(Sc::Unit::Type(unitType >= 228 ? 0 : unitType)).graphics);
    u32 spriteId = u32(scData.units.getFlingy(flingyId >= 209 ? 0 : flingyId).sprite);
    auto & sprite = scData.sprites.getSprite(spriteId >= 517 ? 0 : spriteId);
    return UnitInfo {
        .imageId = sprite.imageFile > 999 ? 0 : size_t(sprite.imageFile),
        .imageFrame = lifted ? getLiftedFrame(unitType) : getPreferredFrame(unitType),
        .subUnitImageId = subUnitImageId,
        .subUnitOffset = getSubUnitOffset(unitDat.subunit1),
        .subUnitFrame = getPreferredFrame(unitDat.subunit1),
        .overlayImageId = getOverlay(unitType),
        .attachOverlayImageId = attached ? getAttachedOverlay(unitType) : std::numeric_limits<size_t>::max(),
        .attachFrame = attached ? getAttachedOverlayPreferredFrame(unitType) : std::numeric_limits<size_t>::max()
    };
}

Scr::MapGraphics::SelectInfo Scr::MapGraphics::getSelInfo(Sc::Unit::Type unitType)
{
    u32 flingyId = u32(scData.units.getUnit(Sc::Unit::Type(unitType >= 228 ? 0 : unitType)).graphics);
    u32 spriteId = u32(scData.units.getFlingy(flingyId >= 209 ? 0 : flingyId).sprite);
    auto & sprite = scData.sprites.getSprite(spriteId >= 517 ? 0 : spriteId);
    u32 selectionImageId = sprite.selectionCircleImage+561;
    return SelectInfo {
        .imageId = selectionImageId > 999 ? 570 : selectionImageId,
        .yOffset = sprite.selectionCircleOffset
    };
}

Scr::MapGraphics::SelectInfo Scr::MapGraphics::getSelInfo(Sc::Sprite::Type spriteType, bool isDrawnAsSprite)
{
    auto & sprite = scData.sprites.getSprite(spriteType >= 517 ? 0 : spriteType);
    u32 selectionImageId = sprite.selectionCircleImage+561;
    return SelectInfo {
        .imageId = selectionImageId > 999 ? 570 : selectionImageId,
        .yOffset = sprite.selectionCircleOffset
    };
}

Scr::Animation & Scr::MapGraphics::getImage(size_t imageId)
{
    return *((*renderDat->images)[imageId]);
}

Scr::Animation & Scr::MapGraphics::getImage(Sc::Unit::Type unitType)
{
    return getImage(getImageId(unitType));
}

Scr::Animation & Scr::MapGraphics::getImage(Sc::Sprite::Type spriteType)
{
    return getImage(getImageId(spriteType));
}

Scr::Animation & Scr::MapGraphics::getImage(Sc::Sprite::Type spriteType, bool isDrawnAsSprite)
{
    return isDrawnAsSprite ?
        getImage(spriteType) :
        getImage(Sc::Unit::Type(spriteType));
}

Scr::Animation & Scr::MapGraphics::getImage(const Chk::Unit & unit)
{
    return getImage(getImageId(unit));
}

Scr::Animation & Scr::MapGraphics::getImage(const Chk::Sprite & sprite)
{
    return getImage(getImageId(sprite));
}

Scr::MapGraphics::MapGraphics(Sc::Data & scData, GuiMap & guiMap) : scData(scData), map(guiMap), initialTickCount(GetTickCount64()) {}

void Scr::MapGraphics::updateGrid()
{
    if ( gridSize > 0 && gridVertices )
    {
        /* Grid lines are drawn in grid-space, which starts at (0, 0) and goes to (maxPossibleVerticalLines, maxPossibleHorizontalLines)
           Horizontal and vertical grid lines are placed from (x, y)->(1, 1) to (maxVerticalLines, maxHorizontalLines) at every 1.0 interval
           The gridToNdc matrix scales by gridSize and translates up and left to get from grid-coordinates to game-coordinates, then transforms to NDCs */
        constexpr s32 largestGridSize = 64;
        gl::Point2D<s32> gridOffset = {
            .x = mapViewBounds.left % largestGridSize,
            .y = mapViewBounds.top % largestGridSize
        };
        auto gridDimensions = gl::Size2D {
            .width = (mapViewBounds.right + largestGridSize)/gridSize,
            .height = (mapViewBounds.bottom + largestGridSize)/gridSize
        };
        auto halfFragTranslation = glm::translate(glm::mat4x4(1.f), {.5f/windowDimensions.width, -.5f/windowDimensions.height, 0.f});
        auto gridScale = glm::scale(glm::mat4x4(1.f), {gridSize, gridSize, 1.f});
        auto gridTranslation = glm::translate(glm::mat4x4(1.f), {GLfloat(-gridOffset.x), GLfloat(-gridOffset.y), 0.f});
        auto ndcScale = glm::scale(glm::mat4x4(1.f), {2.f/mapViewDimensions.width, -2.f/mapViewDimensions.height, 1.f});
        auto ndcTranslation = glm::translate(glm::mat4x4(1.f), {-1.f, 1.f, 0.f});
        gridToNdc = halfFragTranslation * ndcTranslation * ndcScale * gridTranslation * gridScale;

        auto & solidColorShader = renderDat->shaders->solidColorShader;
        solidColorShader.use();
        solidColorShader.posToNdc.setMat4(gridToNdc);
        solidColorShader.solidColor.setColor(this->gridColor);

        gridVertices.clear();
        for ( size_t x=1; x<gridDimensions.width; ++x )
        {
            gridVertices.vertices.insert(gridVertices.vertices.end(), {
                GLfloat(x), -1.f,
                GLfloat(x), GLfloat(gridDimensions.height)
            });
        }
        for ( size_t y=1; y<gridDimensions.height; ++y )
        {
            gridVertices.vertices.insert(gridVertices.vertices.end(), {
                -1.f, GLfloat(y),
                GLfloat(gridDimensions.width), GLfloat(y)
            });
        }
        gridVertices.bind();
        gridVertices.bufferData(gl::UsageHint::DynamicDraw);
    }
}

void Scr::MapGraphics::mapViewChanged()
{
    windowDimensions = {
        .width = windowBounds.right - windowBounds.left,
        .height = windowBounds.bottom - windowBounds.top
    };
    mapViewDimensions = {
        .width = s32(windowDimensions.width/zoom),
        .height = s32(windowDimensions.height/zoom)
    };
    starDimensions = {
        .width = mapViewDimensions.width * s32(loadSettings.visualQuality.scale),
        .height = mapViewDimensions.height * s32(loadSettings.visualQuality.scale)
    };
    mapViewBounds = {
        .left = windowBounds.left,
        .top = windowBounds.top,
        .right = windowBounds.left+mapViewDimensions.width,
        .bottom = windowBounds.top+mapViewDimensions.height
    };
    mapTileBounds = {
        .left = mapViewBounds.left/32,
        .top = mapViewBounds.top/32,
        .right = std::min(mapViewBounds.right/32+1, s32(map.getTileWidth()-1)),
        .bottom = std::min(mapViewBounds.bottom/32+1, s32(map.getTileHeight()-1))
    };
    imageClipBoundingBox = {
        .left = mapViewBounds.left - imageMargin.width,
        .top = mapViewBounds.top - imageMargin.height,
        .right = mapViewBounds.right + imageMargin.width,
        .bottom = mapViewBounds.bottom + imageMargin.height
    };
    gl::Point2D<s32> tileOffset = {
        .x = mapViewBounds.left % 32,
        .y = mapViewBounds.top % 32
    };
    gl::Size2D<s32> tileDimensions = {
        .width = mapViewBounds.right + (32 - tileOffset.x) - (mapViewBounds.left - tileOffset.x),
        .height = mapViewBounds.bottom + (32 - tileOffset.y) - (mapViewBounds.top - tileOffset.y)
    };
    auto halfFragTranslation = glm::translate(glm::mat4x4(1.f), {.5f/windowDimensions.width, -.5f/windowDimensions.height, 0.f});
    auto tileScale = glm::scale(glm::mat4x4(1.f), {32.f, 32.f, 1.f});
    auto gameTranslation = glm::translate(glm::mat4x4(1.f), {-mapViewBounds.left, -mapViewBounds.top, 0.f});
    auto windowToNdcScale = glm::scale(glm::mat4x4(1.f), {2.f/windowDimensions.width, -2.f/windowDimensions.height, 1.f});
    auto mapViewToNdcScale = glm::scale(glm::mat4x4(1.f), {2.f/mapViewDimensions.width, -2.f/mapViewDimensions.height, 1.f});
    auto ndcTranslation = glm::translate(glm::mat4x4(1.f), {-1.f, 1.f, 0.f});
    auto visualQualityScale = glm::scale(glm::mat4x4(1.f), {1.f/loadSettings.visualQuality.scale, 1.f/loadSettings.visualQuality.scale, 1.f});

    gameToNdc = halfFragTranslation * ndcTranslation * mapViewToNdcScale * gameTranslation;
    starToNdc = halfFragTranslation * ndcTranslation * mapViewToNdcScale * visualQualityScale;
    tileToNdc = halfFragTranslation * ndcTranslation * mapViewToNdcScale * gameTranslation * tileScale;
    glyphScaling = glm::mat2x2({2.f/windowDimensions.width, 0.f}, {0.f, -2.f/windowDimensions.height});
    unscrolledWindowToNdc = halfFragTranslation * ndcTranslation * windowToNdcScale;

    auto tileTexScale = glm::scale(glm::mat4x4(1.f), {1.f/128.f, 1.f/128.f, 1.f});
    tileToTex = tileTexScale;
    updateGrid();
}

void Scr::MapGraphics::windowBoundsChanged(gl::Rect2D<s32> windowBounds)
{
    std::swap(this->windowBounds, windowBounds);
    mapViewChanged();
}

GLfloat Scr::MapGraphics::getZoom()
{
    return this->zoom;
}

void Scr::MapGraphics::setZoom(GLfloat newZoom)
{
    this->zoom = newZoom;
    mapViewChanged();
}

void Scr::MapGraphics::skinChanged()
{
    // Recalculate imageMargin
    imageMargin = {0, 0};
    if ( loadSettings.skinId == Skin::Id::Classic )
    {
        for ( const auto & classicImage : *renderDat->classicImages )
        {
            if ( classicImage != nullptr )
            {
                if ( classicImage->grpWidth > imageMargin.width )
                    imageMargin.width = classicImage->grpWidth;
                if ( classicImage->grpHeight > imageMargin.height )
                    imageMargin.height = classicImage->grpHeight;
            }
        }
        imageMargin.width = imageMargin.width/2 + imageMargin.width%2;
        imageMargin.height = imageMargin.height/2 + imageMargin.height%2;
    }
    else
    {
        for ( const auto & image : *renderDat->images )
        {
            if ( image != nullptr )
            {
                if ( image->width > imageMargin.width )
                    imageMargin.width = image->width;
                if ( image->height > imageMargin.height )
                    imageMargin.height = image->height;
            }
        }
        imageMargin.width = imageMargin.width/2 + imageMargin.width%2;
        imageMargin.height = imageMargin.height/2 + imageMargin.height%2;
    }

    mapViewChanged();
}

bool Scr::MapGraphics::isClassicLoaded(Scr::GraphicsData & scrDat)
{
    return scrDat.visualQualityData[size_t(Scr::VisualQuality::SD.index())] != nullptr &&
        scrDat.visualQualityData[size_t(Scr::VisualQuality::SD.index())]->skin[size_t(Scr::Skin::Id::Classic)] != nullptr;
}

void Scr::MapGraphics::initVertices()
{
    gridVertices.initialize({ gl::VertexAttribute{.size = 2} }); // Position.xy
    starVertices.initialize({
        gl::VertexAttribute{.size = 2}, // Position.xy
        gl::VertexAttribute{.size = 2}  // TexCoord.xy
    });
    tileVertices.initialize({
        gl::VertexAttribute{.size = 2}, // Position.xy
        gl::VertexAttribute{.size = 2}  // TexCoord.xy
    });
    waterVertices.initialize({
        gl::VertexAttribute{.size = 2}, // Position.xy
        gl::VertexAttribute{.size = 2}, // TexCoord.xy
        gl::VertexAttribute{.size = 2}, // MapCoord.xy
        gl::VertexAttribute{.size = 2}, // MapCoord2.xy
    });
    animVertices.initialize({
        gl::VertexAttribute{.size = 2}, // Position.xy
        gl::VertexAttribute{.size = 2}  // TexCoord.xy
    });
    lineVertices.initialize({
        gl::VertexAttribute{.size = 2} // Position.xy
    });
    lineVertices2.initialize({
        gl::VertexAttribute{.size = 2} // Position.xy
    });
    triangleVertices.initialize({
        gl::VertexAttribute{.size = 2} // Position.xy
    });
    triangleVertices2.initialize({
        gl::VertexAttribute{.size = 2} // Position.xy
    });
    triangleVertices3.initialize({
        gl::VertexAttribute{.size = 2} // Position.xy
    });
    triangleVertices4.initialize({
        gl::VertexAttribute{.size = 2} // Position.xy
    });
    triangleVertices5.initialize({
        gl::VertexAttribute{.size = 2} // Position.xy
    });
    triangleVertices6.initialize({
        gl::VertexAttribute{.size = 2} // Position.xy
    });
}

void Scr::MapGraphics::setFont(gl::Font* textFont)
{
    this->textFont = textFont;
}

void Scr::MapGraphics::setGridColor(uint32_t gridColor)
{
    this->gridColor = gridColor | 0xFF000000;
    updateGrid();
}

void Scr::MapGraphics::setGridSize(s32 gridSize)
{
    this->gridSize = gridSize;
    updateGrid();
}

void Scr::MapGraphics::load(Scr::GraphicsData & scrDat, const Scr::GraphicsData::LoadSettings & loadSettings, ArchiveCluster & archiveCluster, ByteBuffer & fileData)
{
    bool skinChanged = this->renderDat == nullptr ||
        this->loadSettings.skinId != loadSettings.skinId || this->loadSettings.visualQuality != loadSettings.visualQuality;

    this->renderDat = nullptr;
    this->loadSettings = loadSettings;
    this->loadSettings.tileset = Sc::Terrain::Tileset(map.getTileset() % Sc::Terrain::NumTilesets);
    this->renderDat = scrDat.load(scData, archiveCluster, this->loadSettings, fileData);
    
    if ( skinChanged )
        this->skinChanged();

    initVertices();
}

void Scr::MapGraphics::drawTestTex(gl::Texture & tex)
{
    static auto testVerts = [](){
        auto verts = std::make_shared<gl::VertexArray<6*4>>();
        verts->initialize({
            gl::VertexAttribute{.size = 2}, // Position
            gl::VertexAttribute{.size = 2}  // TexCoord
        });
        verts->vertices = {
            -1.f, -1.f, 0.f, 0.f,
             1.f, -1.f, 1.f, 0.f,
            -1.f,  1.f, 0.f, 1.f,
            -1.f,  1.f, 0.f, 1.f,
             1.f,  1.f, 1.f, 1.f,
             1.f, -1.f, 1.f, 0.f
        };
        verts->bufferData(gl::UsageHint::StaticDraw);
        return verts;
    }();

    renderDat->shaders->simpleShader.use();
    renderDat->shaders->simpleShader.tex.setSlot(0);
    renderDat->shaders->simpleShader.posToNdc.loadIdentity();
    tex.bindToSlot(GL_TEXTURE0);
            
    testVerts->bind();
    testVerts->drawTriangles();
}

void Scr::MapGraphics::drawGrid()
{
    // Grid lines are calculated in updateGrid()
    if ( this->gridSize > 0 )
    {
        renderDat->shaders->solidColorShader.use();
        renderDat->shaders->solidColorShader.solidColor.setColor(gridColor);
        renderDat->shaders->solidColorShader.posToNdc.setMat4(gridToNdc);
        gridVertices.bind();
        gridVertices.drawLines();
    }
}

void Scr::MapGraphics::drawClassicStars()
{
    if ( renderDat->spk == nullptr )
        return;

    renderDat->shaders->simplePaletteShader.use();
    renderDat->shaders->simplePaletteShader.posToNdc.setMat4(starToNdc);
    renderDat->shaders->simplePaletteShader.texTransform.loadIdentity();
    renderDat->shaders->simplePaletteShader.tex.setSlot(0);
    renderDat->shaders->simplePaletteShader.pal.setSlot(1);
    
    renderDat->tiles->tilesetGrp.palette->tex.bindToSlot(GL_TEXTURE1);
    animVertices.bind();

    for ( s32 starOriginTop = 0; starOriginTop < mapViewDimensions.height; starOriginTop += Sc::Spk::parllaxHeight ) // The same stars repeat in 648x488 tiles
    {
        for ( s32 starOriginLeft = 0; starOriginLeft < mapViewDimensions.width; starOriginLeft += Sc::Spk::parallaxWidth )
        {
            for ( int layerIndex = int(scData.spk.layerStars.size())-1; layerIndex >= 0; --layerIndex ) // There are five separate layers of stars
            {
                s32 xOffset = (-1 * Sc::Spk::scrollFactors[layerIndex] * mapViewBounds.left) % (Sc::Spk::parallaxWidth*256);
                s32 yOffset = (-1 * Sc::Spk::scrollFactors[layerIndex] * mapViewBounds.top) % (Sc::Spk::parllaxHeight*256);
                auto & layer = scData.spk.layerStars[layerIndex];
                size_t starIndex = 0;
                for ( auto & star : layer )
                {
                    s32 width = (s32)star.bitmap->width;
                    s32 height = (s32)star.bitmap->height;
            
                    s32 xc = s32(star.xc) + (xOffset >> 8);
                    if ( xc > Sc::Spk::parallaxWidth )
                        xc -= Sc::Spk::parallaxWidth;
                    else if ( xc < 0 )
                        xc += Sc::Spk::parallaxWidth;

                    xc -= 8;

                    if ( xc < 640 && xc + width + 8 > 0 )
                    {
                        s32 yc = s32(star.yc) + (yOffset >> 8);
                        if ( yc > Sc::Spk::parllaxHeight )
                            yc -= Sc::Spk::parllaxHeight;
                        else if ( yc < 0 )
                            yc += Sc::Spk::parllaxHeight;

                        yc -= 8;

                        if ( yc < 480 && yc + height + 8 > 0 )
                        {
                            if ( starOriginLeft + xc < 0 ) // Near left-edge
                            {
                                width += xc;
                                xc = 0;
                            }
                            else if ( starOriginLeft + xc + width >= mapViewDimensions.width ) // Near right-edge
                                width = mapViewDimensions.width - starOriginLeft - xc;

                            if ( starOriginTop + yc < 0 ) // Near top-edge
                            {
                                height += yc;
                                yc = 0;
                            }
                            else if ( starOriginTop + yc + height >= mapViewDimensions.height ) // Near bottom-edge
                                height = mapViewDimensions.height - starOriginTop - yc;


                            auto & star = renderDat->spk->classicLayers[layerIndex][starIndex];
                            ++starIndex;
                            gl::Point2D<GLfloat> position { GLfloat(xc+starOriginLeft), GLfloat(yc+starOriginTop) };
                            animVertices.vertices = {
                                position.x                       , position.y                        , 0.f, 0.f,
                                position.x+GLfloat(star.texWidth), position.y                        , 1.f, 0.f,
                                position.x                       , position.y+GLfloat(star.texHeight), 0.f, 1.f,
                                position.x                       , position.y+GLfloat(star.texHeight), 0.f, 1.f,
                                position.x+GLfloat(star.texWidth), position.y+GLfloat(star.texHeight), 1.f, 1.f,
                                position.x+GLfloat(star.texWidth), position.y                        , 1.f, 0.f
                            };

                            star.tex.bindToSlot(GL_TEXTURE0);

                            animVertices.bufferSubData(gl::UsageHint::DynamicDraw);
                            animVertices.drawTriangles();
                        }
                    }
                }
            }
        }
    }
}

void Scr::MapGraphics::drawStars(u32 multiplyColor)
{
    if ( renderDat->spk == nullptr )
        return;

    Scr::SpkData::SpkHeader* spk = renderDat->spk->spk;
    Scr::SpkGrp & stars = renderDat->spk->stars;
    Scr::StarPosition* starPosition = (Scr::StarPosition*)&((u8*)spk)[spk->starOffset];
    u32 starIndex = 0;
            
    renderDat->shaders->tileShader.use();
    renderDat->shaders->tileShader.posToNdc.setMat4(starToNdc);
    renderDat->shaders->tileShader.spriteTex.setSlot(0);
    renderDat->shaders->tileShader.multiplyColor.setColor(multiplyColor);

    for ( u32 layer=0; layer<spk->layerCount; ++layer )
    {
        u32 firstLayerStarIndex = starIndex;
        starVertices.clear();
        GLfloat texTransform[4][4] { // Coverts logical texture coordinates to samplable texture coordinates
            { stars.texture[layer].xScale, 0, 0, 0 },
            { 0, stars.texture[layer].yScale, 0, 0 },
            { 0, 0, 1, 0 },
            { 0, 0, 0, 1 }
        };

        auto layerWidth = spk->layers[layer].layerWidth;
        auto layerHeight = spk->layers[layer].layerHeight;
        u32 xStart = 0;
        u32 xLimit = (starDimensions.width/layerWidth + 2) * layerWidth;
        u32 yStart = 0;
        u32 yLimit = (starDimensions.height/layerHeight + 2) * layerHeight;

        for ( ; yStart < yLimit; yStart += spk->layers[layer].layerHeight )
        {
            for ( u32 xc = xStart; xc < xLimit; xc += u32(spk->layers[layer].layerWidth) )
            {
                starIndex = firstLayerStarIndex;
                for ( u32 j = 0; j < spk->layers[layer].count; j++ )
                {
                    s32 xOffset = (-1 * Sc::Spk::scrollFactors[layer] * mapViewBounds.left) % (Sc::Spk::parallaxWidth*256);
                    s32 yOffset = (-1 * Sc::Spk::scrollFactors[layer] * mapViewBounds.top) % (Sc::Spk::parllaxHeight*256);
                    Scr::StarPosition & currStarPos = starPosition[starIndex];
                    GLfloat left = currStarPos.width/-2.0f;
                    GLfloat top = currStarPos.height/-2.0f;
                    gl::Point2D<GLfloat> position { GLfloat(xc)+currStarPos.xOffset + (xOffset >> 8), GLfloat(yStart)+currStarPos.yOffset + (yOffset >> 8) };
                    gl::Rect2D<GLfloat> vertexRect { left, top, left+currStarPos.width, top+currStarPos.height };
                    gl::Rect2D<GLfloat> texRect {
                        GLfloat(currStarPos.xTextureOffset),
                        GLfloat(currStarPos.yTextureOffset),
                        GLfloat(currStarPos.xTextureOffset)+GLfloat(currStarPos.width),
                        GLfloat(currStarPos.yTextureOffset)+GLfloat(currStarPos.height)
                    };
                    starVertices.vertices.insert(starVertices.vertices.end(), {
                        position.x+vertexRect.left , position.y+vertexRect.top   , texRect.left , texRect.top,
                        position.x+vertexRect.right, position.y+vertexRect.top   , texRect.right, texRect.top,
                        position.x+vertexRect.left , position.y+vertexRect.bottom, texRect.left , texRect.bottom,
                        position.x+vertexRect.left , position.y+vertexRect.bottom, texRect.left , texRect.bottom,
                        position.x+vertexRect.right, position.y+vertexRect.bottom, texRect.right, texRect.bottom,
                        position.x+vertexRect.right, position.y+vertexRect.top   , texRect.right, texRect.top
                    });
                    ++starIndex;
                }
            }
        }
                
        renderDat->shaders->tileShader.texTransform.setMat4(&texTransform[0][0]);
        stars.texture[layer].bindToSlot(GL_TEXTURE0);
                
        starVertices.bind();
        starVertices.bufferData(gl::UsageHint::DynamicDraw);
        starVertices.drawTriangles();
    }
}

void Scr::MapGraphics::drawTileNums()
{
    textFont->textShader.use();
    textFont->textShader.glyphScaling.setMat2(glyphScaling);
    textFont->textShader.textPosToNdc.setMat4(gameToNdc);
    textFont->setColor(255, 255, 0);

    Chk::Scope scope = useGameTileNums ? Chk::Scope::Game : Chk::Scope::Editor;
    for ( s32 y=mapTileBounds.top; y<=mapTileBounds.bottom; ++y )
    {
        for ( s32 x=mapTileBounds.left; x<=mapTileBounds.right; ++x )
            textFont->drawText(x*32+2.0f, y*32+2.0f, std::to_string(map.getTile(x, y, scope)).c_str());
    }
}

void Scr::MapGraphics::drawIsomTileNums()
{
    lineVertices.vertices.clear();
    for ( s32 y=mapTileBounds.top; y<mapTileBounds.bottom; y++ )
    {
        s32 x = mapTileBounds.left % 2 > 0 ? mapTileBounds.left-1 : mapTileBounds.left;
        for ( ; x<mapTileBounds.right; x+=2 )
        {
            if ( (x/2 + y) % 2 == 0 ) {
                lineVertices.vertices.insert(lineVertices.vertices.begin(), {
                    GLfloat(x*32+64), GLfloat(y*32),
                    GLfloat(x*32), GLfloat(y*32+32)
                });
            } else {
                lineVertices.vertices.insert(lineVertices.vertices.begin(), {
                    GLfloat(x*32), GLfloat(y*32),
                    GLfloat(x*32+64), GLfloat(y*32+32)
                });
            }
        }
    }
    if ( !lineVertices.vertices.empty() )
    {
        auto & solidColorShader = renderDat->shaders->solidColorShader;
        solidColorShader.use();
        solidColorShader.posToNdc.setMat4(gameToNdc);
        solidColorShader.solidColor.setColor(0xFFFFFFFF); // Line color: 0xAABBGGRR

        lineVertices.bind();
        lineVertices.bufferData(gl::UsageHint::DynamicDraw);
        lineVertices.drawLines();
    }

    textFont->textShader.use();
    textFont->textShader.glyphScaling.setMat2(glyphScaling);
    textFont->textShader.textPosToNdc.setMat4(gameToNdc);
    textFont->setColor(255, 255, 0);

    for ( s32 y=mapTileBounds.top; y<mapTileBounds.bottom; y++ )
    {
        s32 x = mapTileBounds.left % 2 > 0 ? mapTileBounds.left-1 : mapTileBounds.left;
        for ( ; x<mapTileBounds.right; x+=2 )
        {
            auto isomRect = map.getIsomRect(y*(s32(map.getTileWidth())/2+1)+x/2);

            textFont->drawText<gl::Align::Left>(x*32 + 2, y*32 + 10, to_hex_string((isomRect.left & 0x7FF0) >> 4, false));
            textFont->drawText<gl::Align::Right>(x*32 + 62, y*32 + 10, to_hex_string((isomRect.right & 0x7FF0) >> 4, false));
            textFont->drawText<gl::Align::Center>(x*32 + 32, y*32 + 1, to_hex_string((isomRect.top & 0x7FF0) >> 4, false));
            textFont->drawText<gl::Align::Center>(x*32 + 32, y*32 + 20, to_hex_string((isomRect.bottom & 0x7FF0) >> 4, false));
        }
    }
}

void Scr::MapGraphics::drawTileBuildabilityVertices()
{
    auto & solidColorShader = renderDat->shaders->solidColorShader;
    if ( !triangleVertices.vertices.empty() )
    {
        solidColorShader.solidColor.setColor(0x30006400); // Tile color: 0xAABBGGRR
        triangleVertices.bind();
        triangleVertices.bufferData(gl::UsageHint::DynamicDraw);
        triangleVertices.drawTriangles();
    }
    if ( !triangleVertices2.vertices.empty() )
    {
        solidColorShader.solidColor.setColor(0x30000064); // Tile color: 0xAABBGGRR
        triangleVertices2.bind();
        triangleVertices2.bufferData(gl::UsageHint::DynamicDraw);
        triangleVertices2.drawTriangles();
    }
}

void Scr::MapGraphics::drawTileElevationVertices()
{
    auto & solidColorShader = renderDat->shaders->solidColorShader;
    if ( !triangleVertices.vertices.empty() )
    {
        solidColorShader.solidColor.setColor(0x30006400); // MiniTile color: 0xAABBGGRR
        triangleVertices.bind();
        triangleVertices.bufferData(gl::UsageHint::DynamicDraw);
        triangleVertices.drawTriangles();
    }
    if ( !triangleVertices2.vertices.empty() )
    {
        solidColorShader.solidColor.setColor(0xE0001600); // MiniTile color: 0xAABBGGRR
        triangleVertices2.bind();
        triangleVertices2.bufferData(gl::UsageHint::DynamicDraw);
        triangleVertices2.drawTriangles();
    }
    if ( !triangleVertices3.vertices.empty() )
    {
        solidColorShader.solidColor.setColor(0x30640000); // MiniTile color: 0xAABBGGRR
        triangleVertices3.bind();
        triangleVertices3.bufferData(gl::UsageHint::DynamicDraw);
        triangleVertices3.drawTriangles();
    }
    if ( !triangleVertices4.vertices.empty() )
    {
        solidColorShader.solidColor.setColor(0xE0160000); // MiniTile color: 0xAABBGGRR
        triangleVertices4.bind();
        triangleVertices4.bufferData(gl::UsageHint::DynamicDraw);
        triangleVertices4.drawTriangles();
    }
    if ( !triangleVertices5.vertices.empty() )
    {
        solidColorShader.solidColor.setColor(0x30000064); // MiniTile color: 0xAABBGGRR
        triangleVertices5.bind();
        triangleVertices5.bufferData(gl::UsageHint::DynamicDraw);
        triangleVertices5.drawTriangles();
    }
    if ( !triangleVertices6.vertices.empty() )
    {
        solidColorShader.solidColor.setColor(0xE0000016); // MiniTile color: 0xAABBGGRR
        triangleVertices6.bind();
        triangleVertices6.bufferData(gl::UsageHint::DynamicDraw);
        triangleVertices6.drawTriangles();
    }
}

void Scr::MapGraphics::drawTileOverlays()
{
    const Sc::Terrain::Tiles & tiles = scData.terrain.get(map.getTileset());

    auto & solidColorShader = renderDat->shaders->solidColorShader;
    solidColorShader.use();
    solidColorShader.posToNdc.setMat4(gameToNdc);
    
    if ( displayBuildability )
    {
        triangleVertices.clear();
        triangleVertices2.clear();

        for ( s32 y=mapTileBounds.top; y<=mapTileBounds.bottom; ++y )
        {
            for ( s32 x=mapTileBounds.left; x<=mapTileBounds.right; ++x )
            {
                auto tileIndex = map.getTile(x, y);
                size_t groupIndex = Sc::Terrain::Tiles::getGroupIndex(tileIndex);
                if ( groupIndex < tiles.tileGroups.size() )
                {
                    const Sc::Terrain::TileGroup & tileGroup = tiles.tileGroups[groupIndex];
                    gl::Rect2D<GLfloat> rect {
                        GLfloat(x*32),
                        GLfloat(y*32),
                        GLfloat(x*32+32),
                        GLfloat(y*32+32),
                    };
                    auto & vertices = tileGroup.isBuildable() ? triangleVertices : triangleVertices2;
                    vertices.vertices.insert(vertices.vertices.end(), {
                        rect.left, rect.top,
                        rect.right, rect.top,
                        rect.left, rect.bottom,
                        rect.left, rect.bottom,
                        rect.right, rect.bottom,
                        rect.right, rect.top
                    });
                }
            }
        }

        drawTileBuildabilityVertices();
    }
    else if ( displayElevations )
    {
        triangleVertices.clear();
        triangleVertices2.clear();
        triangleVertices3.clear();
        triangleVertices4.clear();
        triangleVertices5.clear();
        triangleVertices6.clear();

        for ( s32 y=mapTileBounds.top; y<=mapTileBounds.bottom; ++y )
        {
            for ( s32 x=mapTileBounds.left; x<=mapTileBounds.right; ++x )
            {
                auto tileIndex = map.getTile(x, y);
                size_t groupIndex = Sc::Terrain::Tiles::getGroupIndex(tileIndex);
                if ( groupIndex < tiles.tileGroups.size() )
                {
                    const Sc::Terrain::TileGroup & tileGroup = tiles.tileGroups[groupIndex];
                    u16 megaTileIndex = tileGroup.megaTileIndex[tiles.getGroupMemberIndex(tileIndex)];
                
                    for ( s32 yMiniTile = 0; yMiniTile < 4; yMiniTile++ )
                    {
                        for ( s32 xMiniTile = 0; xMiniTile < 4; xMiniTile++ )
                        {
                            Sc::Terrain::TileFlags::MiniTileFlags miniTileFlags = tiles.tileFlags[megaTileIndex].miniTileFlags[yMiniTile][xMiniTile];
                            gl::Rect2D<GLfloat> rect {
                                GLfloat(x*32+xMiniTile*8),
                                GLfloat(y*32+yMiniTile*8),
                                GLfloat(x*32+xMiniTile*8+8),
                                GLfloat(y*32+yMiniTile*8+8),
                            };
                            auto* targetVertices = &triangleVertices;
                            bool walkable = miniTileFlags.isWalkable();
                            switch ( miniTileFlags.getElevation() )
                            {
                                case Sc::Terrain::TileElevation::Low: if ( !walkable) targetVertices = &triangleVertices2; break;
                                case Sc::Terrain::TileElevation::Mid: targetVertices = walkable ? &triangleVertices3 : &triangleVertices4; break;
                                case Sc::Terrain::TileElevation::High: targetVertices = walkable ? &triangleVertices5 : &triangleVertices6; break;
                            }
                            targetVertices->vertices.insert(targetVertices->vertices.end(), {
                                rect.left, rect.top,
                                rect.right, rect.top,
                                rect.left, rect.bottom,
                                rect.left, rect.bottom,
                                rect.right, rect.bottom,
                                rect.right, rect.top
                            });
                        }
                    }
                }
            }
        }

        drawTileElevationVertices();
    }
}

void Scr::MapGraphics::drawTileVertices(Scr::Grp & tilesetGrp, s32 width, s32 height, const glm::mat4x4 & positionTransformation, bool isBaseTerrain)
{
    if ( tileVertices.vertices.empty() )
        return;

    auto tilesetIndex = Sc::Terrain::Tileset(map.getTileset() % Sc::Terrain::NumTilesets);
    bool drawHdWater = loadSettings.visualQuality > VisualQuality::SD && isBaseTerrain &&
        (tilesetIndex == Sc::Terrain::Tileset::Badlands || tilesetIndex == Sc::Terrain::Tileset::Jungle ||
            tilesetIndex == Sc::Terrain::Tileset::Arctic || tilesetIndex == Sc::Terrain::Tileset::Twilight);

    if ( drawHdWater )
    {
        // Draw tiles without effects to a screenTex
        GLint savedViewport[4] {};
        glGetIntegerv(GL_VIEWPORT, savedViewport);
        glDisable(GL_BLEND);
            
        static GLuint framebuffer = 0;
        if ( framebuffer == 0 )
            glGenFramebuffers(1, &framebuffer);

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer); // Note: also attaching a render-buffer here *might* improve performance if needed
                
        gl::Texture screenTex {};
        screenTex.genTexture();
        screenTex.bind();
        screenTex.setMinMagFilters(GL_NEAREST);
        screenTex.loadImage2D(gl::Texture::Image2D {
            .data = NULL,
            .width = GLsizei(width),
            .height = GLsizei(height),
            .level = 0,
            .internalformat = GL_RGBA,
            .format = GL_RGBA,
            .type = GL_UNSIGNED_BYTE
        });

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTex.getId(), 0);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);

        glClearColor(0.f, 0.f, 0.f, 0.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderDat->shaders->simpleShader.use();
        renderDat->shaders->simpleShader.tex.setSlot(0);
        renderDat->shaders->simpleShader.posToNdc.setMat4(positionTransformation);
        renderDat->shaders->simpleShader.texTransform.setMat4(tileToTex);
        renderDat->tiles->tileMask.mergedTexture.bindToSlot(GL_TEXTURE0);

        tileVertices.bind();
        tileVertices.bufferData(gl::UsageHint::DynamicDraw);
        tileVertices.drawTriangles();

        gl::Texture::releaseSlots(GL_TEXTURE1);
        glBindVertexArray(0);
                
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(savedViewport[0], savedViewport[1], savedViewport[2], savedViewport[3]);
                
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Finished drawing to screenTex, now render screenTex to screen through the waterShader for effects
        renderDat->shaders->waterShader.use();
        renderDat->shaders->waterShader.posToNdc.loadIdentity();
        renderDat->shaders->waterShader.texTransform.loadIdentity();
        renderDat->shaders->waterShader.spriteTex.setSlot(0);
        renderDat->shaders->waterShader.sampleTex.setSlot(1);
        renderDat->shaders->waterShader.sampleTex2.setSlot(2);
        renderDat->shaders->waterShader.sampleTex3.setSlot(3);
        renderDat->shaders->waterShader.sampleTex4.setSlot(4);
        renderDat->shaders->waterShader.data.set(0.0f, 0.0f, (GetTickCount64() - initialTickCount)/4000.0f);

        screenTex.bindToSlot(GL_TEXTURE0);
        renderDat->waterNormal[0]->texture[n1Frame].bindToSlot(GL_TEXTURE1);
        renderDat->waterNormal[0]->texture[n1Frame+1 >= renderDat->waterNormal[0]->frames ? 0 : n1Frame+1].bindToSlot(GL_TEXTURE2);
        renderDat->waterNormal[1]->texture[n2Frame].bindToSlot(GL_TEXTURE3);
        renderDat->waterNormal[1]->texture[n2Frame+1 >= renderDat->waterNormal[1]->frames ? 0 : n2Frame+1].bindToSlot(GL_TEXTURE4);

        waterVertices.vertices = { // {pos.xy, tex.xy, map.xy, map2.xy} TODO: MapCoords should be dynamic
            -1.f, -1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f,
             1.f, -1.f, 1.f, 0.f, 1.f, 0.f, 1.f, 0.f,
            -1.f,  1.f, 0.f, 1.f, 0.f, 1.f, 0.f, 1.f,
            -1.f,  1.f, 0.f, 1.f, 0.f, 1.f, 0.f, 1.f,
             1.f,  1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f,
             1.f, -1.f, 1.f, 0.f, 1.f, 0.f, 1.f, 0.f
        };
        waterVertices.bind();
        waterVertices.bufferData(gl::UsageHint::DynamicDraw);
        waterVertices.drawTriangles();

        gl::Texture::releaseSlots(GL_TEXTURE4);
    }
    else // Draw regular tiles
    {
        if ( loadSettings.visualQuality > VisualQuality::SD )
        {
            renderDat->shaders->tileShader.use();
            renderDat->shaders->tileShader.posToNdc.setMat4(positionTransformation);
            renderDat->shaders->tileShader.texTransform.setMat4(tileToTex);
            renderDat->shaders->tileShader.spriteTex.setSlot(0);
            renderDat->shaders->tileShader.multiplyColor.setColor(0xFFFFFFFF);
            tilesetGrp.mergedTexture.bindToSlot(GL_TEXTURE0);
        }
        else if ( loadSettings.skinId == Scr::Skin::Id::Classic ) // Classic
        {
            renderDat->shaders->simplePaletteShader.use();
            renderDat->shaders->simplePaletteShader.posToNdc.setMat4(positionTransformation);
            renderDat->shaders->simplePaletteShader.texTransform.setMat4(tileToTex);
            renderDat->shaders->simplePaletteShader.tex.setSlot(0);
            renderDat->shaders->simplePaletteShader.pal.setSlot(1);
            tilesetGrp.mergedTexture.bindToSlot(GL_TEXTURE0);
            tilesetGrp.palette->tex.bindToSlot(GL_TEXTURE1);
        }
        else // Remastered SD
        {
            renderDat->shaders->paletteShader.use();
            renderDat->shaders->paletteShader.posToNdc.setMat4(positionTransformation);
            renderDat->shaders->paletteShader.texTransform.setMat4(tileToTex);
            renderDat->shaders->paletteShader.spriteTex.setSlot(0);
            renderDat->shaders->paletteShader.sampleTex.setSlot(1);
            renderDat->shaders->paletteShader.multiplyColor.setColor(0xFFFFFFFF);
            tilesetGrp.mergedTexture.bindToSlot(GL_TEXTURE0);
            tilesetGrp.palette->tex.bindToSlot(GL_TEXTURE1);
        }

        tileVertices.bind();
        tileVertices.bufferData(gl::UsageHint::DynamicDraw);
        tileVertices.drawTriangles();

        if ( loadSettings.visualQuality == VisualQuality::SD )
            gl::Texture::releaseSlots(GL_TEXTURE1);
        else
            gl::Texture::releaseSlots(GL_TEXTURE0);
    }
}

void Scr::MapGraphics::drawTerrain()
{
    auto startOff = .6f/(128*renderDat->tiles->tilesetGrp.width);
    auto endOff = -.1f/(128*renderDat->tiles->tilesetGrp.width);

    tileVertices.clear();
    auto tiles = scData.terrain.get(Sc::Terrain::Tileset(map.tileset));
    for ( s32 y=mapTileBounds.top; y<=mapTileBounds.bottom; ++y )
    {
        for ( s32 x=mapTileBounds.left; x<=mapTileBounds.right; ++x )
        {
            u16 tileIndex = map.tiles[size_t(y) * size_t(map.dimensions.tileWidth) + size_t(x)];
            size_t groupIndex = Sc::Terrain::Tiles::getGroupIndex(tileIndex);
            if ( groupIndex < tiles.tileGroups.size() )
            {
                const Sc::Terrain::TileGroup & tileGroup = tiles.tileGroups[groupIndex];
                u32 megaTileIndex = tileGroup.megaTileIndex[tiles.getGroupMemberIndex(tileIndex)];
                auto texX = GLfloat(megaTileIndex%128);
                auto texY = GLfloat(megaTileIndex/128);
                tileVertices.vertices.insert(tileVertices.vertices.end(), {
                    GLfloat(x  ), GLfloat(y  ), texX+startOff  , texY+startOff,
                    GLfloat(x+1), GLfloat(y  ), texX+1.f+endOff, texY+startOff,
                    GLfloat(x  ), GLfloat(y+1), texX+startOff  , texY+1.f+endOff,
                    GLfloat(x  ), GLfloat(y+1), texX+startOff  , texY+1.f+endOff,
                    GLfloat(x+1), GLfloat(y+1), texX+1.f+endOff, texY+1.f+endOff,
                    GLfloat(x+1), GLfloat(y  ), texX+1.f+endOff, texY+startOff
                });
            }
        }
    }
    
    if ( !tileVertices.vertices.empty() )
        drawTileVertices(renderDat->tiles->tilesetGrp, windowDimensions.width, windowDimensions.height, tileToNdc, true);
}

void Scr::MapGraphics::drawTilesetIndexed(s32 left, s32 top, s32 width, s32 height, s32 scrollY, std::optional<u16> selectedTile)
{
    auto & tilesetGrp = renderDat->tiles->tilesetGrp;

    tileVertices.clear();
    auto & tiles = scData.terrain.get(Sc::Terrain::Tileset(map.tileset));

    s32 topRow = scrollY/33;
    s32 bottomRow = (scrollY + height)/33;
    s32 totalRows = height/32;
    s32 totalColumns = width/32;

    for ( s32 row=topRow; row<=bottomRow; ++row )
    {
        GLfloat tileTop = GLfloat(row*33-scrollY);
        for ( s32 column=0; column<totalColumns; ++column )
        {
            u32 megaTileIndex = 0;
            u16 tileIndex = u16(16*row+column);
            size_t groupIndex = Sc::Terrain::Tiles::getGroupIndex(tileIndex);
            if ( groupIndex < tiles.tileGroups.size() )
            {
                const Sc::Terrain::TileGroup & tileGroup = tiles.tileGroups[groupIndex];
                megaTileIndex = tileGroup.megaTileIndex[tiles.getGroupMemberIndex(tileIndex)];
            }

            gl::Point2D<GLfloat> position {
                GLfloat(column*33),
                tileTop
            };
            auto left = float(megaTileIndex%128);
            auto top = float(megaTileIndex/128);
            auto right = left+1.f;
            auto bottom = top+1.f;
            gl::Rect2D<GLfloat> texRect {left, top, right, bottom};
            tileVertices.vertices.insert(tileVertices.vertices.end(), {
                position.x   , position.y   , texRect.left , texRect.top,
                position.x+32, position.y   , texRect.right, texRect.top,
                position.x   , position.y+32, texRect.left , texRect.bottom,
                position.x   , position.y+32, texRect.left , texRect.bottom,
                position.x+32, position.y+32, texRect.right, texRect.bottom,
                position.x+32, position.y   , texRect.right, texRect.top,
            });
        }
    }
    
    auto tileTranslation = glm::translate(glm::mat4x4(1.f), {left, top, 0.f});
    auto ndcScale = glm::scale(glm::mat4x4(1.f), {2.f/width, -2.f/height, 1.f});
    auto ndcTranslation = glm::translate(glm::mat4x4(1.f), {-1.f, 1.f, 0.f});
    auto posToNdc = ndcTranslation * ndcScale * tileTranslation;
    drawTileVertices(tilesetGrp, width, height, posToNdc);

    if ( displayBuildability || displayElevations )
    {
        auto & solidColorShader = renderDat->shaders->solidColorShader;
        solidColorShader.use();
        solidColorShader.posToNdc.setMat4(posToNdc);
    
        if ( displayBuildability )
        {
            triangleVertices.clear();
            triangleVertices2.clear();
            
            for ( s32 row=topRow; row<=bottomRow; ++row )
            {
                GLfloat tileTop = GLfloat(row*33-scrollY);
                for ( s32 column=0; column<totalColumns; ++column )
                {
                    u32 megaTileIndex = 0;
                    u16 tileIndex = u16(16*row+column);
                    size_t groupIndex = Sc::Terrain::Tiles::getGroupIndex(tileIndex);
                    if ( groupIndex < tiles.tileGroups.size() )
                    {
                        const Sc::Terrain::TileGroup & tileGroup = tiles.tileGroups[groupIndex];
                        megaTileIndex = tileGroup.megaTileIndex[tiles.getGroupMemberIndex(tileIndex)];

                        gl::Point2D<GLfloat> position {
                            GLfloat(column*33),
                            tileTop
                        };
                        gl::Rect2D<GLfloat> rect {
                            GLfloat(position.x),
                            GLfloat(position.y),
                            GLfloat(position.x+32),
                            GLfloat(position.y+32),
                        };
                        auto & vertices = tileGroup.isBuildable() ? triangleVertices : triangleVertices2;
                        vertices.vertices.insert(vertices.vertices.end(), {
                            rect.left, rect.top,
                            rect.right, rect.top,
                            rect.left, rect.bottom,
                            rect.left, rect.bottom,
                            rect.right, rect.bottom,
                            rect.right, rect.top
                        });
                    }
                }
            }

            drawTileBuildabilityVertices();
        }
        else if ( displayElevations )
        {
            triangleVertices.clear();
            triangleVertices2.clear();
            triangleVertices3.clear();
            triangleVertices4.clear();
            triangleVertices5.clear();
            triangleVertices6.clear();

            for ( s32 row=topRow; row<=bottomRow; ++row )
            {
                GLfloat tileTop = GLfloat(row*33-scrollY);
                for ( s32 column=0; column<totalColumns; ++column )
                {
                    u32 megaTileIndex = 0;
                    u16 tileIndex = u16(16*row+column);
                    size_t groupIndex = Sc::Terrain::Tiles::getGroupIndex(tileIndex);
                    if ( groupIndex < tiles.tileGroups.size() )
                    {
                        const Sc::Terrain::TileGroup & tileGroup = tiles.tileGroups[groupIndex];
                        megaTileIndex = tileGroup.megaTileIndex[tiles.getGroupMemberIndex(tileIndex)];

                        for ( s32 yMiniTile = 0; yMiniTile < 4; yMiniTile++ )
                        {
                            for ( s32 xMiniTile = 0; xMiniTile < 4; xMiniTile++ )
                            {
                                Sc::Terrain::TileFlags::MiniTileFlags miniTileFlags = tiles.tileFlags[megaTileIndex].miniTileFlags[yMiniTile][xMiniTile];
                                gl::Point2D<GLfloat> position {
                                    GLfloat(column*33) + 8*xMiniTile,
                                    tileTop + 8*yMiniTile
                                };
                                gl::Rect2D<GLfloat> rect {
                                    GLfloat(position.x),
                                    GLfloat(position.y),
                                    GLfloat(position.x+8),
                                    GLfloat(position.y+8),
                                };
                                auto* targetVertices = &triangleVertices;
                                bool walkable = miniTileFlags.isWalkable();
                                switch ( miniTileFlags.getElevation() )
                                {
                                    case Sc::Terrain::TileElevation::Low: if ( !walkable) targetVertices = &triangleVertices2; break;
                                    case Sc::Terrain::TileElevation::Mid: targetVertices = walkable ? &triangleVertices3 : &triangleVertices4; break;
                                    case Sc::Terrain::TileElevation::High: targetVertices = walkable ? &triangleVertices5 : &triangleVertices6; break;
                                }
                                targetVertices->vertices.insert(targetVertices->vertices.end(), {
                                    rect.left, rect.top,
                                    rect.right, rect.top,
                                    rect.left, rect.bottom,
                                    rect.left, rect.bottom,
                                    rect.right, rect.bottom,
                                    rect.right, rect.top
                                });
                            }
                        }
                    }
                }
            }

            drawTileElevationVertices();
        }
    }

    if ( displayTileNums )
    {
        auto glyphScaling = glm::mat2x2({2.f/width, 0.f}, {0.f, -2.f/height});
        textFont->textShader.use();
        textFont->textShader.glyphScaling.setMat2(glyphScaling);
        textFont->textShader.textPosToNdc.setMat4(posToNdc);
        textFont->setColor(255, 255, 0);

        for ( s32 row=topRow; row<=bottomRow; ++row )
        {
            GLfloat tileTop = GLfloat(row*33-scrollY);
            for ( s32 column=0; column<totalColumns; ++column )
                textFont->drawText(column*33+2.0f, tileTop+2.0f, std::to_string(u16(16*row+column)).c_str());
        }
    }

    if ( selectedTile )
    {
        auto selectedTileRow = (*selectedTile)/16;
        if ( selectedTileRow >= topRow && selectedTileRow <= bottomRow )
        {
            auto selectedTileColumn = (*selectedTile)%16;
            GLfloat tileTop = GLfloat(selectedTileRow*33-scrollY);
            GLfloat tileLeft = GLfloat(selectedTileColumn*33);
            gl::Rect2D<GLfloat> rect {
                .left = tileLeft,
                .top = tileTop,
                .right = tileLeft+33,
                .bottom = tileTop+33
            };

            auto & solidColorShader = renderDat->shaders->solidColorShader;
            solidColorShader.use();
            solidColorShader.posToNdc.setMat4(posToNdc);
            solidColorShader.solidColor.setColor(0xFF0000FF); // Rectangle color: 0xAABBGGRR
            lineVertices.vertices = {
                rect.left, rect.top,
                rect.right, rect.top,
                rect.right, rect.top,
                rect.right, rect.bottom,
                rect.right, rect.bottom,
                rect.left, rect.bottom,
                rect.left, rect.bottom,
                rect.left, rect.top,
            };
            lineVertices.bind();
            lineVertices.bufferData(gl::UsageHint::DynamicDraw);
            lineVertices.drawLines();
        }
    }
}

void Scr::MapGraphics::drawTileSelection()
{
    const Sc::Terrain::Tiles & tiles = scData.terrain.get(map.tileset);
    triangleVertices.clear();
    lineVertices.clear();
    for ( auto & tile : map.selections.tiles )
    {
        gl::Rect2D<GLfloat> rect {
            GLfloat(32*tile.xc),
            GLfloat(32*tile.yc),
            GLfloat(32*tile.xc+32),
            GLfloat(32*tile.yc+32),
        };
        triangleVertices.vertices.insert(triangleVertices.vertices.end(), {
            rect.left, rect.top,
            rect.right, rect.top,
            rect.left, rect.bottom,
            rect.left, rect.bottom,
            rect.right, rect.bottom,
            rect.right, rect.top
        });

        if ( tile.neighbors != TileNeighbor::None ) // Need to draw tile edge line
        {
            if ( (tile.neighbors & TileNeighbor::Top) == TileNeighbor::Top )
            {
                lineVertices.vertices.insert(lineVertices.vertices.end(), {
                    rect.left, rect.top,
                    rect.right, rect.top,
                });
            }
            if ( (tile.neighbors & TileNeighbor::Right) == TileNeighbor::Right )
            {
                lineVertices.vertices.insert(lineVertices.vertices.end(), {
                    rect.right, rect.top,
                    rect.right, rect.bottom,
                });
            }
            if ( (tile.neighbors & TileNeighbor::Bottom) == TileNeighbor::Bottom )
            {
                lineVertices.vertices.insert(lineVertices.vertices.end(), {
                    rect.right, rect.bottom,
                    rect.left, rect.bottom,
                });
            }
            if ( (tile.neighbors & TileNeighbor::Left) == TileNeighbor::Left )
            {
                lineVertices.vertices.insert(lineVertices.vertices.end(), {
                    rect.left, rect.bottom,
                    rect.left, rect.top,
                });
            }
        }
    }

    if ( !triangleVertices.vertices.empty() )
    {
        auto & solidColorShader = renderDat->shaders->solidColorShader;
        solidColorShader.use();
        solidColorShader.posToNdc.setMat4(gameToNdc);
        solidColorShader.solidColor.setColor(0x30640000); // MiniTile color: 0xAABBGGRR
        triangleVertices.bind();
        triangleVertices.bufferData(gl::UsageHint::DynamicDraw);
        triangleVertices.drawTriangles();
        if ( !lineVertices.vertices.empty() )
        {
            solidColorShader.solidColor.setColor(0xFFFFFF00); // Line color: 0xAABBGGRR
            lineVertices.bind();
            lineVertices.bufferData(gl::UsageHint::DynamicDraw);
            lineVertices.drawLines();
        }
    }
}

void Scr::MapGraphics::prepareImageRendering(bool isSelections)
{
    auto imageTranslation = glm::translate(glm::mat4x4(1.f), {-mapViewBounds.left, -mapViewBounds.top, 0.f});
    auto ndcScale = glm::scale(glm::mat4x4(1.f), {2.f/mapViewDimensions.width, -2.f/mapViewDimensions.height, 1.f});
    auto ndcTranslation = glm::translate(glm::mat4x4(1.f), {-1.f, 1.f, 0.f});
    auto imageToNdc = ndcTranslation * ndcScale * imageTranslation;
    if ( loadSettings.skinId == Scr::Skin::Id::Classic )
    {
        prevMappedColor = Chk::PlayerColor::None;
        renderDat->shaders->simplePaletteShader.use();
        renderDat->shaders->simplePaletteShader.posToNdc.setMat4(imageToNdc);
        renderDat->shaders->simplePaletteShader.texTransform.loadIdentity();
        renderDat->shaders->simplePaletteShader.tex.setSlot(0);
        renderDat->shaders->simplePaletteShader.pal.setSlot(1);
    }
    else
    {
        if ( isSelections )
        {
            renderDat->shaders->selectionShader.use();
            renderDat->shaders->selectionShader.posToNdc.setMat4(imageToNdc);
            renderDat->shaders->selectionShader.imageScale.setValue(loadSettings.visualQuality.imageScale());
            renderDat->shaders->selectionShader.spriteTex.setSlot(0);
        }
        else
        {
            renderDat->shaders->spriteShader.use();
            renderDat->shaders->spriteShader.posToNdc.setMat4(imageToNdc);
            renderDat->shaders->spriteShader.imageScale.setValue(loadSettings.visualQuality.imageScale());
            renderDat->shaders->spriteShader.spriteTex.setSlot(0);
            renderDat->shaders->spriteShader.teamColorTex.setSlot(1);
        }

        animVertices.vertices = {
            0.f, 0.f, 0.f, 0.f,
            1.f, 0.f, 1.f, 0.f,
            0.f, 1.f, 0.f, 1.f,
            0.f, 1.f, 0.f, 1.f,
            1.f, 1.f, 1.f, 1.f,
            1.f, 0.f, 1.f, 0.f
        };

        animVertices.bind();
        animVertices.bufferData(gl::UsageHint::DynamicDraw);
    }
}

void Scr::MapGraphics::drawImage(Scr::Animation & animation, s32 x, s32 y, u32 frameIndex, u32 multiplyColor, u32 playerColor, bool hallucinate, bool flipped)
{
    auto & frame = animation.frames[frameIndex >= animation.totalFrames ? 0 : frameIndex];
    
    // To use StarCraft's sprite shader means binding a particular texture per sprite then performing a draw, capping optimization potential
    // Below is passing data related to the image frame in one uniform and performing ~all math on the GPU
    // This approach should be highly performant given the cap, but there's also the potential to frontload/cache more math & move data to UBOs

    renderDat->shaders->spriteShader.image.setVec2Array({
        { GLfloat(x)                   , GLfloat(y)                }, // centerPos
        { GLfloat(animation.width)     , GLfloat(animation.height) }, // animSize
        { GLfloat(animation.xScale)    , GLfloat(animation.yScale) }, // animTexScale
        { GLfloat(frame.width)         , GLfloat(frame.height) }, // frameSize
        { GLfloat(frame.xOffset)       , GLfloat(frame.yOffset) }, // framePosOffset
        { GLfloat(frame.xTextureOffset), GLfloat(frame.yTextureOffset) }, // frameTexOffset
        { GLfloat(flipped ? -1.f : 1.f), GLfloat(flipped ? 1.f : 0.f) } // frameTexFlipper{mul,add}
    });

    renderDat->shaders->spriteShader.multiplyColor.setColor(multiplyColor);
    renderDat->shaders->spriteShader.teamColor.setColor(playerColor);
    renderDat->shaders->spriteShader.hallucinate.setValue(GLfloat(hallucinate));

    animation.diffuse.bindToSlot(GL_TEXTURE0);
    animation.teamColor.bindToSlot(GL_TEXTURE1);

    animVertices.drawTriangles();
}

void Scr::MapGraphics::drawSelectionImage(Scr::Animation & animation, s32 x, s32 y, u32 frameIndex, u32 multiplyColor)
{
    auto & frame = animation.frames[frameIndex >= animation.totalFrames ? 0 : frameIndex];
    
    renderDat->shaders->selectionShader.image.setVec2Array({
        { GLfloat(x)                   , GLfloat(y)                }, // centerPos
        { GLfloat(animation.width)     , GLfloat(animation.height) }, // animSize
        { GLfloat(animation.xScale)    , GLfloat(animation.yScale) }, // animTexScale
        { GLfloat(frame.width)         , GLfloat(frame.height) }, // frameSize
        { GLfloat(frame.xOffset)       , GLfloat(frame.yOffset) }, // framePosOffset
        { GLfloat(frame.xTextureOffset), GLfloat(frame.yTextureOffset) }, // frameTexOffset
        { GLfloat(1.f)                 , GLfloat(0.f) } // frameTexFlipper{mul,add}
    });

    renderDat->shaders->selectionShader.multiplyColor.setColor(multiplyColor);
    renderDat->shaders->selectionShader.solidColor.setColor(0xFF249824);

    animation.diffuse.bindToSlot(GL_TEXTURE0);

    animVertices.drawTriangles();
}

void Scr::MapGraphics::drawClassicImage(gl::Palette & palette, s32 x, s32 y, u32 frameIndex, u32 imageId, Chk::PlayerColor color, bool flipped)
{
    remapPalette<8, 8>(palette, &scData.tunit.rgbaPalette[color < 16 ? 8*color : 8*(color%16)], color, prevMappedColor);

    auto & imageInfo = (*renderDat->classicImages)[imageId];
    auto & frameInfo = imageInfo->frames[frameIndex];

    GLfloat vertexLeft = GLfloat(flipped ?
        -imageInfo->grpWidth/2 + frameInfo.xOffset - s16(frameInfo.texWidth-frameInfo.frameWidth) :
        -imageInfo->grpWidth/2 + frameInfo.xOffset);
    GLfloat vertexTop = GLfloat(-imageInfo->grpHeight/2 + frameInfo.yOffset);
    gl::Rect2D<GLfloat> vertexRect { vertexLeft, vertexTop, vertexLeft + frameInfo.texWidth, vertexTop + frameInfo.texHeight };
    if ( flipped )
    {
        animVertices.vertices = {
            x+vertexRect.left , y+vertexRect.top   , 1.f, 0.f,
            x+vertexRect.right, y+vertexRect.top   , 0.f, 0.f,
            x+vertexRect.left , y+vertexRect.bottom, 1.f, 1.f,
            x+vertexRect.left , y+vertexRect.bottom, 1.f, 1.f,
            x+vertexRect.right, y+vertexRect.bottom, 0.f, 1.f,
            x+vertexRect.right, y+vertexRect.top   , 0.f, 0.f
        };
    }
    else
    {
        animVertices.vertices = {
            x+vertexRect.left , y+vertexRect.top   , 0.f, 0.f,
            x+vertexRect.right, y+vertexRect.top   , 1.f, 0.f,
            x+vertexRect.left , y+vertexRect.bottom, 0.f, 1.f,
            x+vertexRect.left , y+vertexRect.bottom, 0.f, 1.f,
            x+vertexRect.right, y+vertexRect.bottom, 1.f, 1.f,
            x+vertexRect.right, y+vertexRect.top   , 1.f, 0.f
        };
    }


    frameInfo.tex.bindToSlot(GL_TEXTURE0);
    renderDat->tiles->tilesetGrp.palette->tex.bindToSlot(GL_TEXTURE1);

    animVertices.bind();
    animVertices.bufferData(gl::UsageHint::DynamicDraw);
    animVertices.drawTriangles();
}

void Scr::MapGraphics::drawUnitSelection(Sc::Unit::Type unitType, s32 x, s32 y)
{
    auto [selImageId, selOffset] = getSelInfo(unitType);
    if ( loadSettings.skinId == Scr::Skin::Id::Classic )
        drawClassicImage(*renderDat->tiles->tilesetGrp.palette, x, y+selOffset, 0, selImageId, Chk::PlayerColor::Red);
    else
        drawSelectionImage(getImage(selImageId), x, y+selOffset, 0, 0xFF00F518);
}

void Scr::MapGraphics::drawSpriteSelection(Sc::Sprite::Type spriteType, s32 x, s32 y, bool isDrawnAsSprite)
{
    auto [selImageId, selOffset] = getSelInfo(spriteType, isDrawnAsSprite);
    if ( loadSettings.skinId == Scr::Skin::Id::Classic )
        drawClassicImage(*renderDat->tiles->tilesetGrp.palette, x, y+selOffset, 0, selImageId, Chk::PlayerColor::Red);
    else
        drawSelectionImage(getImage(selImageId), x, y+selOffset, 0, 0xFFFFFFFF);
}

void Scr::MapGraphics::drawImageSelections()
{
    prepareImageRendering(true);
    auto & palette = renderDat->tiles->tilesetGrp.palette; // For SC:R there is no palette/this is std::nullopt
    ScopedPaletteRemap<0, 8> remapped {palette, (void*)&scData.tselect.rgbaPalette[0], prevMappedColor}; // For SC:R this does nothing
    prevMappedColor = Chk::PlayerColor::Red; // Use one unchanging value for player color here to prevent remappings

    for ( auto unitIndex : map.selections.units )
    {
        if ( unitIndex < map.units.size() )
        {
            auto & unit = map.units[unitIndex];
            drawUnitSelection(unit.type, unit.xc, unit.yc);
        }
    }

    for ( auto spriteIndex : map.selections.sprites )
    {
        if ( spriteIndex < map.sprites.size() )
        {
            auto & sprite = map.sprites[spriteIndex];
            drawSpriteSelection(sprite.type, sprite.xc, sprite.yc, sprite.isDrawnAsSprite());
        }
    }
}

void Scr::MapGraphics::drawUnit(const Chk::Unit & unit)
{
    constexpr auto none = std::numeric_limits<size_t>::max();
    auto [imageId, imageFrame, subUnitImageId, subUnitOffset, subUnitFrame, overlayImageId, attachOverlayImageId, attachFrame]
        = getUnitInfo(unit.type, unit.isAttached(), unit.isLifted());

    auto drawUnit = [&](s32 xc, s32 yc, u32 frame, size_t imageId, u8 owner, bool flipped)
    {
        if ( loadSettings.skinId == Scr::Skin::Id::Classic )
            drawClassicImage(*renderDat->tiles->tilesetGrp.palette, xc, yc, frame, imageId, (Chk::PlayerColor)owner, flipped);
        else
            drawImage(getImage(imageId), xc, yc, frame, 0xFFFFFFFF, getPlayerColor(owner), false, flipped);
    };

    drawUnit(unit.xc, unit.yc, imageFrame, imageId, unit.owner, scData.sprites.imageFlipped(imageId));
    if ( subUnitImageId != none )
        drawUnit(s32(unit.xc)-s32(subUnitOffset.x), s32(unit.yc)-s32(subUnitOffset.y), subUnitFrame, subUnitImageId, unit.owner, false);

    if ( overlayImageId != none )
        drawUnit(unit.xc, unit.yc, 0, overlayImageId, unit.owner, false);

    if ( attachOverlayImageId != none )
        drawUnit(unit.xc, unit.yc, attachFrame, attachOverlayImageId, unit.owner, false);
}

void Scr::MapGraphics::drawSprite(const Chk::Sprite & sprite)
{
    if ( loadSettings.skinId == Scr::Skin::Id::Classic )
        drawClassicImage(*renderDat->tiles->tilesetGrp.palette, sprite.xc, sprite.yc, 0, getImageId(sprite.type, sprite.isDrawnAsSprite()), (Chk::PlayerColor)sprite.owner);
    else
        drawImage(getImage(sprite.type, sprite.isDrawnAsSprite()), sprite.xc, sprite.yc, 0, 0xFFFFFFFF, getPlayerColor(sprite.owner), false);
}

void Scr::MapGraphics::drawImages()
{
    prepareImageRendering();
    auto & palette = renderDat->tiles->tilesetGrp.palette; // For SC:R there is no palette/this is std::nullopt
    ScopedPaletteRestore<8, 8> remapped {palette, prevMappedColor}; // For SC:R this does nothing

    for ( const auto & unit : map.units )
        drawUnit(unit);

    for ( const auto & sprite : map.sprites )
        drawSprite(sprite);
}

void Scr::MapGraphics::drawLocations()
{
    auto selectedLocation = map.selections.getSelectedLocation();
    lineVertices.clear();
    triangleVertices.clear();
    triangleVertices2.clear();
    for ( size_t i=0; i<map.locations.size(); ++i )
    {
        if ( i == Chk::LocationId::Anywhere && map.LockAnywhere() )
            continue;

        const auto & location = map.locations[i];
        auto & triVertices = location.right < location.left || location.bottom < location.top ?
            triangleVertices2.vertices : triangleVertices.vertices;

        triVertices.insert(triVertices.end(), {
            GLfloat(location.left), GLfloat(location.top),
            GLfloat(location.right), GLfloat(location.top),
            GLfloat(location.left), GLfloat(location.bottom),
            GLfloat(location.left), GLfloat(location.bottom),
            GLfloat(location.right), GLfloat(location.bottom),
            GLfloat(location.right), GLfloat(location.top)
        });
        lineVertices.vertices.insert(lineVertices.vertices.end(), {
            GLfloat(location.left), GLfloat(location.top),
            GLfloat(location.right), GLfloat(location.top),
            GLfloat(location.right), GLfloat(location.top),
            GLfloat(location.right), GLfloat(location.bottom),
            GLfloat(location.right), GLfloat(location.bottom),
            GLfloat(location.left), GLfloat(location.bottom),
            GLfloat(location.left), GLfloat(location.bottom),
            GLfloat(location.left), GLfloat(location.top),
        });
    }
    
    auto & solidColorShader = renderDat->shaders->solidColorShader;
    solidColorShader.use();
    solidColorShader.posToNdc.setMat4(gameToNdc);

    if ( !triangleVertices.vertices.empty() ) // Regular shading
    {
        solidColorShader.solidColor.setColor(0x20D0A800); // Location color: 0xAABBGGRR
        triangleVertices.bind();
        triangleVertices.bufferData(gl::UsageHint::DynamicDraw);
        triangleVertices.drawTriangles();
    }

    if ( !triangleVertices2.vertices.empty() ) // Inverted shading
    {
        solidColorShader.solidColor.setColor(0x200000D0); // Location color: 0xAABBGGRR
        triangleVertices2.bind();
        triangleVertices2.bufferData(gl::UsageHint::DynamicDraw);
        triangleVertices2.drawTriangles();
    }

    // Location bounding box
    if ( !lineVertices.vertices.empty() )
    {
        solidColorShader.solidColor.setColor(0xFF000000); // Line color: 0xAABBGGRR
        lineVertices.bind();
        lineVertices.bufferData(gl::UsageHint::DynamicDraw);
        lineVertices.drawLines();

        if ( selectedLocation != 0 && selectedLocation < map.numLocations() )
        {
            lineVertices.vertices.clear();
            auto & location = map.getLocation(selectedLocation);
            lineVertices.vertices.insert(lineVertices.vertices.end(), {
                GLfloat(location.left), GLfloat(location.top),
                GLfloat(location.right), GLfloat(location.top),
                GLfloat(location.right), GLfloat(location.top),
                GLfloat(location.right), GLfloat(location.bottom),
                GLfloat(location.right), GLfloat(location.bottom),
                GLfloat(location.left), GLfloat(location.bottom),
                GLfloat(location.left), GLfloat(location.bottom),
                GLfloat(location.left), GLfloat(location.top),
            });
            solidColorShader.solidColor.setColor(0xFFFFFFFF); // Line color: 0xAABBGGRR
            lineVertices.bufferData(gl::UsageHint::DynamicDraw);
            lineVertices.drawLines();
        }
    }

    if ( clipLocationNames )
    {
        textFont->clippedTextShader.use();
        textFont->clippedTextShader.glyphScaling.setMat2(glyphScaling);
        textFont->clippedTextShader.textPosToNdc.setMat4(gameToNdc);
        textFont->setColor(255, 255, 0);

        for ( size_t i=0; i<map.locations.size(); ++i )
        {
            if ( i == Chk::LocationId::Anywhere && map.LockAnywhere() )
                continue;

            const auto & location = map.locations[i];

            auto locationName = map.getLocationName<RawString>(i);
            if ( locationName )
            {
                u32 visualLeft = std::min(location.left, location.right);
                u32 visualTop = std::min(location.top, location.bottom);
                u32 visualRight = std::max(location.left, location.right);
                u32 visualBottom = std::max(location.top, location.bottom);
                u32 locationWidth = visualRight - visualLeft;
                u32 locationHeight = visualBottom - visualTop;
                s32 clipWidth = s32(locationWidth)*windowDimensions.width/mapViewDimensions.width; // Convert from game to window cordinates
                s32 clipHeight = s32(locationHeight)*windowDimensions.height/mapViewDimensions.height;
                if ( clipWidth > 2 && clipHeight > 2 ) // There is some space in which to potentially draw text
                {
                    textFont->clippedTextShader.lowerRightBound.setVec2(visualRight-1.f, visualBottom-1.f);
                    textFont->drawClippedText(visualLeft+2.f, visualTop+2.f, 12.f, locationName->c_str(), clipWidth-3, clipHeight);
                }
            }
        }
    }
    else
    {
        textFont->textShader.use();
        textFont->textShader.glyphScaling.setMat2(glyphScaling);
        textFont->textShader.textPosToNdc.setMat4(gameToNdc);
        textFont->setColor(255, 255, 0);
        for ( size_t i=0; i<map.locations.size(); ++i )
        {
            const auto & location = map.locations[i];
            auto locationName = map.getLocationName<RawString>(i);
            if ( locationName )
            {
                u32 visualLeft = std::min(location.left, location.right);
                u32 visualTop = std::min(location.top, location.bottom);
                textFont->drawText(visualLeft+2.f, visualTop+2.f, locationName->c_str());
            }
        }
    }
}

void Scr::MapGraphics::drawSelectionRectangle(const gl::Rect2D<GLfloat> & rectangle)
{
    auto & solidColorShader = renderDat->shaders->solidColorShader;
    solidColorShader.use();
    solidColorShader.posToNdc.setMat4(gameToNdc);
    solidColorShader.solidColor.setColor(0xFF0000FF); // Rectangle color: 0xAABBGGRR
    lineVertices.vertices = {
        rectangle.left, rectangle.top,
        rectangle.right, rectangle.top,
        rectangle.right, rectangle.top,
        rectangle.right, rectangle.bottom,
        rectangle.right, rectangle.bottom,
        rectangle.left, rectangle.bottom,
        rectangle.left, rectangle.bottom,
        rectangle.left, rectangle.top,
    };
    lineVertices.bind();
    lineVertices.bufferData(gl::UsageHint::DynamicDraw);
    lineVertices.drawLines();
}

void Scr::MapGraphics::drawTemporaryLocations()
{
    auto start = map.selections.startDrag;
    auto end = map.selections.endDrag;
    auto selectedLocation = map.selections.getSelectedLocation();
    if ( map.selections.getLocationFlags() == LocSelFlags::None ) // Draw location creation preview
    {
        drawSelectionRectangle({ GLfloat(start.x), GLfloat(start.y), GLfloat(end.x), GLfloat(end.y) });
    }
    else if ( selectedLocation != NO_LOCATION && selectedLocation < map.numLocations() ) // Draw location resize/movement graphics
    {
        auto locFlags = map.selections.getLocationFlags();
        const Chk::Location & loc = map.getLocation((size_t)selectedLocation);
        s32 locLeft = loc.left;
        s32 locRight = loc.right;
        s32 locTop = loc.top;
        s32 locBottom = loc.bottom;
        s32 dragX = end.x-start.x;
        s32 dragY = end.y-start.y;
        if ( locFlags != LocSelFlags::Middle )
        {
            if ( locTop > locBottom )
            {
                if ( (locFlags & LocSelFlags::North) == LocSelFlags::North )
                    locFlags = LocSelFlags(locFlags&(~LocSelFlags::North)|LocSelFlags::South);
                else if ( (locFlags & LocSelFlags::South) == LocSelFlags::South )
                    locFlags = LocSelFlags(locFlags&(~LocSelFlags::South)|LocSelFlags::North);
            }

            if ( locLeft > locRight )
            {
                if ( (locFlags & LocSelFlags::West) == LocSelFlags::West )
                    locFlags = LocSelFlags(locFlags&(~LocSelFlags::West)|LocSelFlags::East);
                else if ( (locFlags & LocSelFlags::East) == LocSelFlags::East )
                    locFlags = LocSelFlags(locFlags&(~LocSelFlags::East)|LocSelFlags::West);
            }
        }

        switch ( locFlags )
        {
            case LocSelFlags::North: drawSelectionRectangle({ GLfloat(loc.left), GLfloat(loc.top+dragY), GLfloat(loc.right), GLfloat(loc.bottom) }); break;
            case LocSelFlags::South: drawSelectionRectangle({ GLfloat(loc.left), GLfloat(loc.top), GLfloat(loc.right), GLfloat(loc.bottom+dragY) }); break;
            case LocSelFlags::East: drawSelectionRectangle({ GLfloat(loc.left), GLfloat(loc.top), GLfloat(loc.right+dragX), GLfloat(loc.bottom) }); break;
            case LocSelFlags::West: drawSelectionRectangle({ GLfloat(loc.left+dragX), GLfloat(loc.top), GLfloat(loc.right), GLfloat(loc.bottom) }); break;
            case LocSelFlags::NorthWest: drawSelectionRectangle({ GLfloat(loc.left+dragX), GLfloat(loc.top+dragY), GLfloat(loc.right), GLfloat(loc.bottom) }); break;
            case LocSelFlags::NorthEast: drawSelectionRectangle({ GLfloat(loc.left), GLfloat(loc.top+dragY), GLfloat(loc.right+dragX), GLfloat(loc.bottom) }); break;
            case LocSelFlags::SouthWest: drawSelectionRectangle({ GLfloat(loc.left+dragX), GLfloat(loc.top), GLfloat(loc.right), GLfloat(loc.bottom+dragY) }); break;
            case LocSelFlags::SouthEast: drawSelectionRectangle({ GLfloat(loc.left), GLfloat(loc.top), GLfloat(loc.right+dragX), GLfloat(loc.bottom+dragY) }); break;

            case LocSelFlags::Middle:
                drawSelectionRectangle({ GLfloat(loc.left+dragX), GLfloat(loc.top+dragY), GLfloat(loc.right+dragX), GLfloat(loc.bottom+dragY) });
                break;
        }
    }
}

void Scr::MapGraphics::drawFog()
{
    triangleVertices.clear();
    u8 currPlayer = map.getCurrPlayer();
    u8 currPlayerMask = u8Bits[currPlayer];
    for ( s32 y=mapTileBounds.top; y<=mapTileBounds.bottom; ++y )
    {
        for ( s32 x=mapTileBounds.left; x<=mapTileBounds.right; ++x )
        {
            if ( (map.getFog(x, y) & currPlayerMask) != 0 )
            {
                gl::Rect2D<GLfloat> rect {
                    GLfloat(x*32),
                    GLfloat(y*32),
                    GLfloat(x*32+32),
                    GLfloat(y*32+32),
                };
                triangleVertices.vertices.insert(triangleVertices.vertices.begin(), {
                    rect.left, rect.top,
                    rect.right, rect.top,
                    rect.left, rect.bottom,
                    rect.left, rect.bottom,
                    rect.right, rect.bottom,
                    rect.right, rect.top
                });
            }
        }
    }

    if ( !triangleVertices.vertices.empty() )
    {
        auto & solidColorShader = renderDat->shaders->solidColorShader;
        solidColorShader.use();
        solidColorShader.posToNdc.setMat4(gameToNdc);
        solidColorShader.solidColor.setColor(0x60000000); // Tile color: 0xAABBGGRR
        triangleVertices.bind();
        triangleVertices.bufferData(gl::UsageHint::DynamicDraw);
        triangleVertices.drawTriangles();
    }
}

void Scr::MapGraphics::drawFogTileSelection()
{
    triangleVertices.clear();
    lineVertices.vertices.clear();
    u8 currPlayer = map.getCurrPlayer();
    u8 currPlayerMask = u8Bits[currPlayer];
    for ( auto & tile : map.selections.fogTiles )
    {
        gl::Rect2D<GLfloat> rect {
            GLfloat(tile.xc*32),
            GLfloat(tile.yc*32),
            GLfloat(tile.xc*32+32),
            GLfloat(tile.yc*32+32),
        };
        triangleVertices.vertices.insert(triangleVertices.vertices.begin(), {
            rect.left, rect.top,
            rect.right, rect.top,
            rect.left, rect.bottom,
            rect.left, rect.bottom,
            rect.right, rect.bottom,
            rect.right, rect.top
        });

        if ( tile.neighbors != TileNeighbor::None ) // Some edges need to be drawn
        {
            if ( (tile.neighbors & TileNeighbor::Top) == TileNeighbor::Top )
            {
                lineVertices.vertices.insert(lineVertices.vertices.end(), {
                    rect.left, rect.top,
                    rect.right, rect.top,
                });
            }
            if ( (tile.neighbors & TileNeighbor::Right) == TileNeighbor::Right )
            {
                lineVertices.vertices.insert(lineVertices.vertices.end(), {
                    rect.right, rect.top,
                    rect.right, rect.bottom,
                });
            }
            if ( (tile.neighbors & TileNeighbor::Bottom) == TileNeighbor::Bottom )
            {
                lineVertices.vertices.insert(lineVertices.vertices.end(), {
                    rect.right, rect.bottom,
                    rect.left, rect.bottom,
                });
            }
            if ( (tile.neighbors & TileNeighbor::Left) == TileNeighbor::Left )
            {
                lineVertices.vertices.insert(lineVertices.vertices.end(), {
                    rect.left, rect.bottom,
                    rect.left, rect.top,
                });
            }
        }
    }

    if ( !triangleVertices.vertices.empty() )
    {
        auto & solidColorShader = renderDat->shaders->solidColorShader;
        solidColorShader.use();
        solidColorShader.posToNdc.setMat4(gameToNdc);
        solidColorShader.solidColor.setColor(0x300080FF); // Tile color: 0xAABBGGRR
        triangleVertices.bind();
        triangleVertices.bufferData(gl::UsageHint::DynamicDraw);
        triangleVertices.drawTriangles();

        if ( !lineVertices.vertices.empty() )
        {
            solidColorShader.solidColor.setColor(0xFFFF32FF); // Rectangle color: 0xAABBGGRR
            lineVertices.bind();
            lineVertices.bufferData(gl::UsageHint::DynamicDraw);
            lineVertices.drawLines();
        }
    }
}

void Scr::MapGraphics::drawDoodadSelection()
{
    lineVertices.vertices.clear();
    const auto & tileset = scData.terrain.get(map.tileset);
    for ( auto index : map.selections.doodads )
    {
        const auto & selDoodad = map.getDoodad(index);
        if ( auto doodadGroupIndex = tileset.getDoodadGroupIndex(selDoodad.type) )
        {
            const auto & doodad = (Sc::Terrain::DoodadCv5 &)tileset.tileGroups[size_t(*doodadGroupIndex)];
            s32 doodadWidth = 32*s32(doodad.tileWidth);
            s32 doodadHeight = 32*s32(doodad.tileHeight);
            s32 left = (s32(selDoodad.xc) - doodadWidth/2 + 16)/32*32;
            s32 top = (s32(selDoodad.yc) - doodadHeight/2 + 16)/32*32;
            s32 right = left + doodadWidth;
            s32 bottom = top + doodadHeight;

            lineVertices.vertices.insert(lineVertices.vertices.begin(), {
                GLfloat(left), GLfloat(top),
                GLfloat(right), GLfloat(top),
                GLfloat(right), GLfloat(top),
                GLfloat(right), GLfloat(bottom),
                GLfloat(left), GLfloat(bottom),
                GLfloat(right), GLfloat(bottom),
                GLfloat(left), GLfloat(top),
                GLfloat(left), GLfloat(bottom)
            });
        }
    }
    if ( !lineVertices.vertices.empty() )
    {
        auto & solidColorShader = renderDat->shaders->solidColorShader;
        solidColorShader.use();
        solidColorShader.posToNdc.setMat4(gameToNdc);
        solidColorShader.solidColor.setColor(0xFF0000FF); // Rectangle color: 0xAABBGGRR
        lineVertices.bind();
        lineVertices.bufferData(gl::UsageHint::DynamicDraw);
        lineVertices.drawLines();
    }
}

void Scr::MapGraphics::drawFps()
{
    fps.update(std::chrono::system_clock::now());
    textFont->textShader.use();
    textFont->textShader.glyphScaling.setMat2(glyphScaling);
    textFont->textShader.textPosToNdc.setMat4(unscrolledWindowToNdc);
    textFont->drawAffixedText<gl::Align::Center>(windowDimensions.width/2, 10.f, fps.displayNumber, " fps", "");
}

void Scr::MapGraphics::drawPastes()
{
    auto layer = map.getLayer();
    auto subLayer = map.getSubLayer();
    auto drawPasteTerrain = [&](point paste) {
        if ( subLayer == TerrainSubLayer::Isom && layer != Layer::CutCopyPaste )
        {
            auto diamond = Chk::IsomDiamond::fromMapCoordinates(paste.x, paste.y);

            s32 diamondCenterX = s32(diamond.x)*64;
            s32 diamondCenterY = s32(diamond.y)*32;
            
            auto & solidColorShader = renderDat->shaders->solidColorShader;
            solidColorShader.use();
            solidColorShader.posToNdc.setMat4(gameToNdc);
            solidColorShader.solidColor.setColor(0xFF0000FF); // Rectangle color: 0xAABBGGRR
            lineVertices.vertices = {
                GLfloat(diamondCenterX-64), GLfloat(diamondCenterY),
                GLfloat(diamondCenterX), GLfloat(diamondCenterY-32),
                GLfloat(diamondCenterX), GLfloat(diamondCenterY-32),
                GLfloat(diamondCenterX+64), GLfloat(diamondCenterY),
                GLfloat(diamondCenterX-64), GLfloat(diamondCenterY),
                GLfloat(diamondCenterX), GLfloat(diamondCenterY+32),
                GLfloat(diamondCenterX), GLfloat(diamondCenterY+32),
                GLfloat(diamondCenterX+64), GLfloat(diamondCenterY),
            };
            lineVertices.bind();
            lineVertices.bufferData(gl::UsageHint::DynamicDraw);
            lineVertices.drawLines();
        }
        else if ( subLayer == TerrainSubLayer::Rectangular || layer == Layer::CutCopyPaste )
        {
            point center { paste.x+16, paste.y+16 };
            const Sc::Terrain::Tiles & tiles = scData.terrain.get(map.tileset);
            auto & pasteTiles = map.clipboard.getTiles();
            tileVertices.vertices.clear();
            lineVertices.vertices.clear();
            for ( auto & tile : pasteTiles )
            {
                u16 tileIndex = tile.value;
                size_t groupIndex = Sc::Terrain::Tiles::getGroupIndex(tileIndex);
                if ( groupIndex < tiles.tileGroups.size() )
                {
                    const Sc::Terrain::TileGroup & tileGroup = tiles.tileGroups[groupIndex];
                    u32 megaTileIndex = tileGroup.megaTileIndex[tiles.getGroupMemberIndex(tileIndex)];
                    auto texX = GLfloat(megaTileIndex%128);
                    auto texY = GLfloat(megaTileIndex/128);
                    gl::Rect2D<GLfloat> rect {
                        GLfloat((tile.xc + center.x)/32),
                        GLfloat((tile.yc + center.y)/32),
                        GLfloat((tile.xc + 32 + center.x)/32),
                        GLfloat((tile.yc + 32 + center.y)/32)
                    };
                    tileVertices.vertices.insert(tileVertices.vertices.end(), {
                        rect.left , rect.top   , texX    , texY,
                        rect.right, rect.top   , texX+1.f, texY,
                        rect.left , rect.bottom, texX    , texY+1.f,
                        rect.left , rect.bottom, texX    , texY+1.f,
                        rect.right, rect.bottom, texX+1.f, texY+1.f,
                        rect.right, rect.top   , texX+1.f, texY
                    });

                    if ( tile.neighbors != TileNeighbor::None ) // Some edges need to be drawn
                    {
                        if ( (tile.neighbors & TileNeighbor::Top) == TileNeighbor::Top )
                        {
                            lineVertices.vertices.insert(lineVertices.vertices.end(), {
                                rect.left*32, rect.top*32,
                                rect.right*32, rect.top*32,
                            });
                        }
                        if ( (tile.neighbors & TileNeighbor::Right) == TileNeighbor::Right )
                        {
                            lineVertices.vertices.insert(lineVertices.vertices.end(), {
                                rect.right*32, rect.top*32,
                                rect.right*32, rect.bottom*32,
                            });
                        }
                        if ( (tile.neighbors & TileNeighbor::Bottom) == TileNeighbor::Bottom )
                        {
                            lineVertices.vertices.insert(lineVertices.vertices.end(), {
                                rect.right*32, rect.bottom*32,
                                rect.left*32, rect.bottom*32,
                            });
                        }
                        if ( (tile.neighbors & TileNeighbor::Left) == TileNeighbor::Left )
                        {
                            lineVertices.vertices.insert(lineVertices.vertices.end(), {
                                rect.left*32, rect.bottom*32,
                                rect.left*32, rect.top*32,
                            });
                        }
                    }
                }
            }
            drawTileVertices(renderDat->tiles->tilesetGrp, windowDimensions.width, windowDimensions.height, tileToNdc);
            if ( !lineVertices.vertices.empty() )
            {
                auto & solidColorShader = renderDat->shaders->solidColorShader;
                solidColorShader.use();
                solidColorShader.posToNdc.setMat4(gameToNdc);
                solidColorShader.solidColor.setColor(0xFFFFFF00); // Line color: 0xAABBGGRR
                lineVertices.bind();
                lineVertices.bufferData(gl::UsageHint::DynamicDraw);
                lineVertices.drawLines();
            }
        }
    };
    auto drawPasteFog = [&](point paste) {
        triangleVertices.clear();
        lineVertices.vertices.clear();
        point center { paste.x+16, paste.y+16 };
        u8 currPlayer = map.getCurrPlayer();
        u8 currPlayerMask = u8Bits[currPlayer];
        auto fogTiles = map.clipboard.getFogTiles();
        for ( auto & fogTile : fogTiles )
        {
            gl::Rect2D<GLfloat> rect {
                GLfloat((fogTile.xc + center.x)/32*32),
                GLfloat((fogTile.yc + center.y)/32*32),
                GLfloat((fogTile.xc + 32 + center.x)/32*32),
                GLfloat((fogTile.yc + 32 + center.y)/32*32)
            };
            if ( (fogTile.value & currPlayerMask) != 0 )
            {
                triangleVertices.vertices.insert(triangleVertices.vertices.begin(), {
                    rect.left, rect.top,
                    rect.right, rect.top,
                    rect.left, rect.bottom,
                    rect.left, rect.bottom,
                    rect.right, rect.bottom,
                    rect.right, rect.top
                });
            }

            if ( fogTile.neighbors != TileNeighbor::None ) // Some edges need to be drawn
            {
                if ( (fogTile.neighbors & TileNeighbor::Top) == TileNeighbor::Top )
                {
                    lineVertices.vertices.insert(lineVertices.vertices.end(), {
                        rect.left, rect.top,
                        rect.right, rect.top,
                    });
                }
                if ( (fogTile.neighbors & TileNeighbor::Right) == TileNeighbor::Right )
                {
                    lineVertices.vertices.insert(lineVertices.vertices.end(), {
                        rect.right, rect.top,
                        rect.right, rect.bottom,
                    });
                }
                if ( (fogTile.neighbors & TileNeighbor::Bottom) == TileNeighbor::Bottom )
                {
                    lineVertices.vertices.insert(lineVertices.vertices.end(), {
                        rect.right, rect.bottom,
                        rect.left, rect.bottom,
                    });
                }
                if ( (fogTile.neighbors & TileNeighbor::Left) == TileNeighbor::Left )
                {
                    lineVertices.vertices.insert(lineVertices.vertices.end(), {
                        rect.left, rect.bottom,
                        rect.left, rect.top,
                    });
                }
            }
        }

        if ( !triangleVertices.vertices.empty() )
        {
            auto & solidColorShader = renderDat->shaders->solidColorShader;
            solidColorShader.use();
            solidColorShader.posToNdc.setMat4(gameToNdc);
            solidColorShader.solidColor.setColor(0x60000000); // Tile color: 0xAABBGGRR
            triangleVertices.bind();
            triangleVertices.bufferData(gl::UsageHint::DynamicDraw);
            triangleVertices.drawTriangles();

            if ( !lineVertices.vertices.empty() )
            {
                solidColorShader.solidColor.setColor(0xFFFF32FF); // Rectangle color: 0xAABBGGRR
                lineVertices.bind();
                lineVertices.bufferData(gl::UsageHint::DynamicDraw);
                lineVertices.drawLines();
            }
        }
    };
    auto drawPasteDoodads = [&](point paste) {
        bool allowIllegalDoodads = map.AllowIllegalDoodadPlacement();
        const auto & doodads = map.clipboard.getDoodads();
        if ( !doodads.empty() )
        {
            const Sc::Terrain::Tiles & tiles = scData.terrain.get(map.tileset);
            point center { paste.x, paste.y };
            tileVertices.vertices.clear();
            triangleVertices.clear();
            triangleVertices2.clear();
            for ( auto & doodad : doodads )
            {
                auto tileWidth = doodad.tileWidth;
                auto tileHeight = doodad.tileHeight;
                bool evenWidth = tileWidth%2 == 0;
                bool evenHeight = tileHeight %2 == 0;
                auto xStart = evenWidth ? -16*doodad.tileWidth + (center.x+doodad.tileX*32+16)/32*32 : -16*(doodad.tileWidth-1) + (center.x + doodad.tileX*32)/32*32;
                auto yStart = evenHeight ? -16*doodad.tileHeight + (center.y+doodad.tileY*32+16)/32*32 : -16*(doodad.tileHeight-1) + (center.y+doodad.tileY*32)/32*32;
                auto xTileStart = xStart/32;
                auto yTileStart = yStart/32;
                const auto & placability = tiles.doodadPlacibility[doodad.doodadId];
                for ( u16 y=0; y<tileHeight; ++y )
                {
                    for ( u16 x=0; x<tileWidth; ++x )
                    {
                        if ( doodad.tileIndex[x][y] != 0 )
                        {
                            u16 tileIndex = doodad.tileIndex[x][y];
                            size_t groupIndex = Sc::Terrain::Tiles::getGroupIndex(tileIndex);
                            if ( groupIndex < tiles.tileGroups.size() )
                            {
                                const Sc::Terrain::TileGroup & tileGroup = tiles.tileGroups[groupIndex];
                                u32 megaTileIndex = tileGroup.megaTileIndex[tiles.getGroupMemberIndex(tileIndex)];
                                auto texX = GLfloat(megaTileIndex%128);
                                auto texY = GLfloat(megaTileIndex/128);
                                gl::Rect2D<GLfloat> rect {
                                    GLfloat(xTileStart + x),
                                    GLfloat(yTileStart + y),
                                    GLfloat(xTileStart + x + 1),
                                    GLfloat(yTileStart + y + 1)
                                };
                                tileVertices.vertices.insert(tileVertices.vertices.begin(), {
                                    rect.left , rect.top   , texX    , texY,
                                    rect.right, rect.top   , texX+1.f, texY,
                                    rect.left , rect.bottom, texX    , texY+1.f,
                                    rect.left , rect.bottom, texX    , texY+1.f,
                                    rect.right, rect.bottom, texX+1.f, texY+1.f,
                                    rect.right, rect.top   , texX+1.f, texY
                                });
                                if ( placability.tileGroup[y*tileWidth+x] != 0 )
                                {
                                    size_t tileXc = xTileStart + x;
                                    size_t tileYc = yTileStart + y;
                                    if ( tileXc < map.dimensions.tileWidth && tileYc < map.dimensions.tileHeight )
                                    {
                                        u16 existingTileGroup = map.getTile(tileXc, tileYc) / 16;
                                        bool placeable = existingTileGroup == placability.tileGroup[y*tileWidth+x] || allowIllegalDoodads;
                                        auto & verts = placeable ? triangleVertices : triangleVertices2;
                                        verts.vertices.insert(verts.vertices.begin(), {
                                            rect.left*32, rect.top*32,
                                            rect.right*32, rect.top*32,
                                            rect.left*32, rect.bottom*32,
                                            rect.left*32, rect.bottom*32,
                                            rect.right*32, rect.bottom*32,
                                            rect.right*32, rect.top*32
                                        });
                                    }
                                }
                            }
                        }
                    }
                }
            }
            drawTileVertices(renderDat->tiles->tilesetGrp, windowDimensions.width, windowDimensions.height, tileToNdc);
            auto & solidColorShader = renderDat->shaders->solidColorShader;
            solidColorShader.use();
            solidColorShader.posToNdc.setMat4(gameToNdc);
            if ( !triangleVertices.vertices.empty() )
            {
                solidColorShader.solidColor.setColor(0x30008000); // Doodad tile shading: 0xAABBGGRR
                triangleVertices.bind();
                triangleVertices.bufferData(gl::UsageHint::DynamicDraw);
                triangleVertices.drawTriangles();
            }
            if ( !triangleVertices2.vertices.empty() )
            {
                solidColorShader.solidColor.setColor(0x200000FF); // Doodad tile shading: 0xAABBGGRR
                triangleVertices2.bind();
                triangleVertices2.bufferData(gl::UsageHint::DynamicDraw);
                triangleVertices2.drawTriangles();
            }
            
            auto getDoodadImageId = [&](const auto & doodad) {
                return doodad.isSprite() ? getImageId(Sc::Sprite::Type(doodad.overlayIndex)) : getImageId(Sc::Unit::Type(doodad.overlayIndex));
            };
            auto getDoodadImage = [&](const auto & doodad) -> Scr::Animation & {
                return doodad.isSprite() ? getImage(Sc::Sprite::Type(doodad.overlayIndex)) : getImage(Sc::Unit::Type(doodad.overlayIndex));
            };

            prepareImageRendering();
            auto & palette = renderDat->tiles->tilesetGrp.palette;
            ScopedPaletteRestore<8, 8> remapped {palette, prevMappedColor};

            if ( loadSettings.skinId == Scr::Skin::Id::Classic )
            {
                for ( auto & doodad : doodads )
                {
                    auto tileWidth = doodad.tileWidth;
                    auto tileHeight = doodad.tileHeight;
                    bool evenWidth = tileWidth%2 == 0;
                    bool evenHeight = tileHeight %2 == 0;
                    auto xStart = evenWidth ? -16*doodad.tileWidth + (center.x+doodad.tileX*32+16)/32*32 : -16*(doodad.tileWidth-1) + (center.x + doodad.tileX*32)/32*32;
                    auto yStart = evenHeight ? -16*doodad.tileHeight + (center.y+doodad.tileY*32+16)/32*32 : -16*(doodad.tileHeight-1) + (center.y+doodad.tileY*32)/32*32;
                    if ( doodad.overlayIndex != 0 )
                        drawClassicImage(*palette, s32(xStart+tileWidth*16), s32(yStart+tileHeight*16), 0, getDoodadImageId(doodad), (Chk::PlayerColor)doodad.owner);
                }
            }
            else
            {
                for ( auto & doodad : doodads )
                {
                    auto tileWidth = doodad.tileWidth;
                    auto tileHeight = doodad.tileHeight;
                    bool evenWidth = tileWidth%2 == 0;
                    bool evenHeight = tileHeight %2 == 0;
                    auto xStart = evenWidth ? -16*doodad.tileWidth + (center.x+doodad.tileX*32+16)/32*32 : -16*(doodad.tileWidth-1) + (center.x + doodad.tileX*32)/32*32;
                    auto yStart = evenHeight ? -16*doodad.tileHeight + (center.y+doodad.tileY*32+16)/32*32 : -16*(doodad.tileHeight-1) + (center.y+doodad.tileY*32)/32*32;
                    if ( doodad.overlayIndex != 0 )
                        drawImage(getDoodadImage(doodad), s32(xStart+tileWidth*16), s32(yStart+tileHeight*16), 0, 0, 0xFFFFFFFF, getPlayerColor(doodad.owner), false);
                }
            }
        }
    };
    auto drawPasteUnits = [&](point paste) {
        if ( paste.x != -1 && paste.y != -1 && (map.clipboard.hasUnits() || map.clipboard.hasQuickUnits()) )
        {
            auto & units = map.clipboard.getUnits();
            
            lineVertices.vertices.clear();
            lineVertices2.vertices.clear();
            for ( auto & pasteUnit : units )
            {
                if ( pasteUnit.unit.type < Sc::Unit::TotalTypes )
                {
                    const auto & unitDat = scData.units.getUnit(pasteUnit.unit.type);
                    bool isValidPlacement = map.isValidUnitPlacement(pasteUnit.unit.type, paste.x + pasteUnit.xc, paste.y + pasteUnit.yc);
                    bool isBuilding = (unitDat.flags & Sc::Unit::Flags::Building) == Sc::Unit::Flags::Building;
                    auto & vertices = isValidPlacement ? lineVertices : lineVertices2;
                    gl::Rect2D<GLfloat> rect {};
                    if ( isBuilding )
                    {
                        rect = {
                            GLfloat(paste.x + pasteUnit.xc - unitDat.starEditPlacementBoxWidth/2),
                            GLfloat(paste.y + pasteUnit.yc - unitDat.starEditPlacementBoxHeight/2),
                            GLfloat(paste.x + pasteUnit.xc + unitDat.starEditPlacementBoxWidth/2),
                            GLfloat(paste.y + pasteUnit.yc + unitDat.starEditPlacementBoxHeight/2)
                        };
                    }
                    else
                    {
                        rect = {
                            GLfloat(paste.x + pasteUnit.xc - unitDat.unitSizeLeft),
                            GLfloat(paste.y + pasteUnit.yc - unitDat.unitSizeUp),
                            GLfloat(paste.x + pasteUnit.xc + unitDat.unitSizeRight),
                            GLfloat(paste.y + pasteUnit.yc + unitDat.unitSizeDown)
                        };
                    }
                    vertices.vertices.insert(vertices.vertices.end(), {
                        rect.left, rect.top,
                        rect.right, rect.top,
                        rect.right, rect.top,
                        rect.right, rect.bottom,
                        rect.right, rect.bottom,
                        rect.left, rect.bottom,
                        rect.left, rect.bottom,
                        rect.left, rect.top,
                    });
                }
            }

            prepareImageRendering();
            auto & palette = renderDat->tiles->tilesetGrp.palette;
            ScopedPaletteRestore<8, 8> remapped {palette, prevMappedColor};

            if ( loadSettings.skinId == Scr::Skin::Id::Classic )
            {
                for ( auto & pasteUnit : units )
                    drawClassicImage(*palette, paste.x+pasteUnit.xc, paste.y+pasteUnit.yc, 0, getImageId(pasteUnit.unit), (Chk::PlayerColor)pasteUnit.unit.owner);
            }
            else
            {
                for ( auto & pasteUnit : units )
                    drawImage(getImage(pasteUnit.unit), paste.x+pasteUnit.xc, paste.y+pasteUnit.yc, 0, 0xFFFFFFFF, getPlayerColor(pasteUnit.unit.owner), false);
            }

            auto & solidColorShader = renderDat->shaders->solidColorShader;
            solidColorShader.use();
            solidColorShader.posToNdc.setMat4(gameToNdc);
            if ( !lineVertices.vertices.empty() )
            {
                solidColorShader.solidColor.setColor(0xFF00FF00); // Rectangle color: 0xAABBGGRR
                lineVertices.bind();
                lineVertices.bufferData(gl::UsageHint::DynamicDraw);
                lineVertices.drawLines();
            }
            if ( !lineVertices2.vertices.empty() )
            {
                solidColorShader.solidColor.setColor(0xFF0000FF); // Rectangle color: 0xAABBGGRR
                lineVertices2.bind();
                lineVertices2.bufferData(gl::UsageHint::DynamicDraw);
                lineVertices2.drawLines();
            }
        }
    };
    auto drawPasteSprites = [&](point paste) {
        if ( paste.x != -1 && paste.y != -1 && (map.clipboard.hasSprites() || map.clipboard.hasQuickSprites()) )
        {
            auto & sprites = map.clipboard.getSprites();

            prepareImageRendering();

            auto & palette = renderDat->tiles->tilesetGrp.palette;
            ScopedPaletteRestore<8, 8> remapped {palette, prevMappedColor};

            if ( loadSettings.skinId == Scr::Skin::Id::Classic )
            {
                for ( auto & pasteSprite : sprites )
                    drawClassicImage(*palette, paste.x+pasteSprite.xc, paste.y+pasteSprite.yc, 0, getImageId(pasteSprite.sprite), (Chk::PlayerColor)pasteSprite.sprite.owner);
            }
            else
            {
                for ( auto & pasteSprite : sprites )
                    drawImage(getImage(pasteSprite.sprite), paste.x+pasteSprite.xc, paste.y+pasteSprite.yc, 0, 0xFFFFFFFF, getPlayerColor(pasteSprite.sprite.owner), false);
            }
        }
    };

    switch ( layer )
    {
        case Layer::Terrain: drawPasteTerrain(map.selections.endDrag); break;
        case Layer::Doodads: drawPasteDoodads(map.selections.endDrag); break;
        case Layer::Units: drawPasteUnits(map.selections.endDrag); break;
        case Layer::Sprites: drawPasteSprites(map.selections.endDrag); break;
        case Layer::FogEdit:
        {
            const auto brushWidth = map.clipboard.getFogBrush().width;
            const auto brushHeight = map.clipboard.getFogBrush().height;
            s32 hoverTileX = (map.selections.endDrag.x + (brushWidth % 2 == 0 ? 16 : 0))/32;
            s32 hoverTileY = (map.selections.endDrag.y + (brushHeight % 2 == 0 ? 16 : 0))/32;

            const auto startX = 32*(hoverTileX - brushWidth/2);
            const auto startY = 32*(hoverTileY - brushHeight/2);
            const auto endX = startX+32+brushWidth;
            const auto endY = startY+32+brushHeight;
            gl::Rect2D<GLfloat> rect {
                GLfloat(startX), GLfloat(startY),
                GLfloat(endX), GLfloat(endY)
            };

            auto & solidColorShader = renderDat->shaders->solidColorShader;
            solidColorShader.use();
            solidColorShader.posToNdc.setMat4(gameToNdc);
            solidColorShader.solidColor.setColor(0xFF0000FF); // Rectangle color: 0xAABBGGRR
            lineVertices.vertices = {
                rect.left, rect.top,
                rect.right, rect.top,
                rect.right, rect.top,
                rect.right, rect.bottom,
                rect.right, rect.bottom,
                rect.left, rect.bottom,
                rect.left, rect.bottom,
                rect.left, rect.top,
            };
            lineVertices.bind();
            lineVertices.bufferData(gl::UsageHint::DynamicDraw);
            lineVertices.drawLines();
        }
        break;
        case Layer::CutCopyPaste:
        {
            point paste = map.selections.endDrag;
            bool pastingTerrain = map.getCutCopyPasteTerrain() && map.clipboard.hasTiles();
            bool pastingDoodads = map.getCutCopyPasteDoodads() && map.clipboard.hasDoodads();
            bool pastingFog = map.getCutCopyPasteFog() && map.clipboard.hasFogTiles();
            if ( pastingTerrain || pastingDoodads || pastingFog )
            {
                paste.x = (paste.x+16)/32*32;
                paste.y = (paste.y+16)/32*32;
            }
            if ( pastingTerrain )
                drawPasteTerrain(paste);
            if ( pastingDoodads )
                drawPasteDoodads(paste);
            if ( pastingFog )
                drawPasteFog(paste);
            if ( map.getCutCopyPasteUnits() )
                drawPasteUnits(paste);
            if ( map.getCutCopyPasteSprites() )
                drawPasteSprites(paste);
        }
        break;
    }
}

void Scr::MapGraphics::render()
{
    auto layer = map.getLayer();

    if ( loadSettings.skinId == Skin::Id::Classic )
        drawClassicStars();
    else
        drawStars(0xFFFFFFFF);

    drawTerrain();
    if ( displayElevations || displayBuildability )
        drawTileOverlays();

    drawGrid();
    drawImageSelections();
    drawImages();

    switch ( layer ) {
        case Layer::Locations: drawLocations(); break;
        case Layer::FogEdit: case Layer::CutCopyPaste: drawFog(); break;
    }

    if ( displayTileNums )
        drawTileNums();
    
    if ( displayIsomNums )
        drawIsomTileNums();

    if ( fpsEnabled )
        drawFps();

    if ( map.selections.hasTiles() && (layer == Layer::Terrain || layer == Layer::CutCopyPaste) )
        drawTileSelection();
    else if ( layer == Layer::Locations )
        drawTemporaryLocations();

    if ( layer == Layer::Doodads || layer == Layer::CutCopyPaste )
        drawDoodadSelection();

    if ( layer == Layer::CutCopyPaste )
        drawFogTileSelection();

    if ( map.clipboard.isPasting() )
        drawPastes();

    if ( layer != Layer::Locations && map.isDragging() && !map.clipboard.isPasting() )
        drawSelectionRectangle({GLfloat(map.selections.startDrag.x), GLfloat(map.selections.startDrag.y), GLfloat(map.selections.endDrag.x), GLfloat(map.selections.endDrag.y)});
}

bool Scr::MapGraphics::updateGraphics(u64 ticks)
{
    bool updated = false;

    // update classic/SD color cycling
    auto & tilesetGrp = renderDat->tiles->tilesetGrp;
    if ( tilesetGrp.palette )
    {
        if ( colorCycler.cycleColors(GetTickCount64(), map.getTileset(), tilesetGrp.palette.value()) )
        {
            tilesetGrp.palette->update();
            updated = true;
        }
    }

    // update HD water animation
    if ( loadSettings.visualQuality > VisualQuality::SD && renderDat->waterNormal[0] && renderDat->waterNormal[0]->frames > 0 )
    {
        nIncrement += ticks;
        if ( nIncrement >= 4000 )
        {
            nIncrement = 0;

            n1Frame++;
            if ( n1Frame >= renderDat->waterNormal[0]->frames )
                n1Frame = 0;

            n2Frame++;
            if ( n2Frame >= renderDat->waterNormal[1]->frames )
                n2Frame = 0;

            updated = true;
        }
    }

    // If HD water is used, updates happen on every change to ticks/ever call to updateGraphics
    bool drawHdWater = loadSettings.visualQuality > VisualQuality::SD &&
        (loadSettings.tileset == Sc::Terrain::Tileset::Badlands || loadSettings.tileset == Sc::Terrain::Tileset::Jungle ||
            loadSettings.tileset == Sc::Terrain::Tileset::Arctic || loadSettings.tileset == Sc::Terrain::Tileset::Twilight);

    return updated || drawHdWater;
}
