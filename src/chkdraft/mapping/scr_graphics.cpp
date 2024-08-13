#include "scr_graphics.h"
#include <cross_cut/logger.h>
#include <rarecpp/json.h>
#include <chrono>
#include <cstring>
#include <Windows.h>
#include "memory_tiered_tasks.h"
#include <glad/gl.h>
#include "gl/base.h"

extern Logger logger;

void Scr::GraphicsData::Shaders::load(ArchiveCluster & archiveCluster)
{
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

    simpleShader.load();

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
        texture.setMinMagFilters(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    }
    else // No mipMaps
        texture.setMinMagFilters(GL_LINEAR);

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
    texture.setMinMagFilters(GL_LINEAR);
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
        
std::shared_ptr<Scr::Animation> Scr::GraphicsData::loadHdAnim(ArchiveCluster & archiveCluster, const std::filesystem::path & path, ByteBuffer & fileData, gl::ContextSemaphore* contextSemaphore)
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
        animation->xScale = 1.0f/textureWidth;
        animation->yScale = 1.0f/textureHeight;

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

void Scr::GraphicsData::loadImages(Sc::Data & scData, ArchiveCluster & archiveCluster, Skin & skin, VisualQuality visualQuality, std::filesystem::path & texPrefix, std::vector<std::shared_ptr<Animation>> & images, ByteBuffer & fileData, gl::ContextSemaphore* contextSemaphore)
{
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
    else if ( visualQuality == VisualQuality::SD || contextSemaphore == nullptr ) // SD (not reading additional casc files) or no context semaphore
    {
        for ( u32 i=0; i<header->entries; i++ )
        {
            Animation::MainSd::Entry* entry = (Animation::MainSd::Entry*)&sdData[entryOffsets[i]];
            if ( entry->frames == 0 )
            {
                Animation::MainSd::Reference* ref = (Animation::MainSd::Reference*)(entry);
                if ( ref->referencedImage > i )
                    throw std::logic_error("Referenced anim was not yet loaded!");
                else
                    images[i] = images[ref->referencedImage];
            }
            else if ( visualQuality == VisualQuality::SD )
                images[i] = loadSdAnim(scData, sdData, i);
            else if ( skin.imageUsesSkinTexture[i] != 0 )
                images[i] = loadHdAnim(archiveCluster, skinAnimPrefix / (std::string("main_") + (to_zero_padded_str(3, i) + ".anim")), fileData);
            else
                images[i] = loadHdAnim(archiveCluster, animPrefix / (std::string("main_") + (to_zero_padded_str(3, i) + ".anim")), fileData);
        }
    }
    else // Have a context semaphore and loading VisualQuality::HD2/HD which reads ~1000 casc files, use multithreading for up to ~70% speedup
    {
        struct Task
        {
            ArchiveCluster* archiveCluster;
            std::filesystem::path path;
            std::shared_ptr<Animation>* anim;
            gl::ContextSemaphore* contextSemaphore;

            size_t requiredMemory = 0;

            void perform(ByteBuffer & memory) { *anim = GraphicsData::loadHdAnim(*archiveCluster, path.string(), memory, contextSemaphore); }
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
                    if ( visualQuality == VisualQuality::SD )
                        images[i] = loadSdAnim(scData, sdData, i);
                    else
                    {
                        std::filesystem::path path {};
                        if ( skin.imageUsesSkinTexture[i] != 0 )
                            path = skinAnimPrefix / (std::string("main_") + (to_zero_padded_str(3, i) + ".anim"));
                        else
                            path = animPrefix / (std::string("main_") + (to_zero_padded_str(3, i) + ".anim"));

                        memoryTieredTasks.add_task({
                            .archiveCluster = &archiveCluster,
                            .path = path,
                            .anim = &images[i],
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
                if ( ref->referencedImage > i )
                    throw std::logic_error("Referenced anim was not yet loaded!");
                else
                    images[i] = images[ref->referencedImage];
            }
        }
    }
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
                    grp.texture[i].setMinMagFilters(GL_LINEAR);
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
    tileMasks.mergedTexture.setMinMagFilters(GL_LINEAR);
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
    blackMask.setMinMagFilters(GL_LINEAR);
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

void Scr::GraphicsData::Data::Skin::Tileset::load(ArchiveCluster & archiveCluster, const RenderSettings & renderSettings, ByteBuffer & fileData)
{
    constexpr std::string_view tilesetNames[8] { "badlands", "platform", "ashworld", "install", "jungle", "desert", "ice", "twilight" };
    std::string tilesetName = std::string(tilesetNames[size_t(renderSettings.tileset) % size_t(Sc::Terrain::NumTilesets)]);
    auto visualQuality = renderSettings.visualQuality;
    auto skinName = Scr::Skin::skinNames[size_t(renderSettings.skinId)];
    std::filesystem::path tilesetPath {};
    if ( renderSettings.skinId != Scr::Skin::Id::Remastered && visualQuality > VisualQuality::SD )
        tilesetPath = std::filesystem::path(visualQuality.prefix) / skinName / "tileset" / (tilesetName + ".dds.vr4");
    else
        tilesetPath = std::filesystem::path(visualQuality.prefix) / "tileset" / (tilesetName + ".dds.vr4");

    if ( visualQuality > VisualQuality::SD && // HD tileset effects for badlands, ashworld, jungle, desert, ice, twilight
        (Sc::Terrain::Tileset)renderSettings.tileset != Sc::Terrain::Tileset::SpacePlatform &&
        (Sc::Terrain::Tileset)renderSettings.tileset != Sc::Terrain::Tileset::Installation )
    {
        auto path = std::filesystem::path(visualQuality.prefix) / skinName / "tileset" / (tilesetName + ".tmsk");
        loadTileMasks(archiveCluster, path, maskIds);

        path = std::filesystem::path(visualQuality.prefix) / skinName / "tileset" / (tilesetName + "_mask.dds.grp");
        loadGrp(archiveCluster, path, tileMask, fileData, true, false);
        loadGrp(archiveCluster, tilesetPath, tilesetGrp, fileData, true, true); // Loads the merged tex and (temporarily) the individual tiles
        loadMaskedTiles(archiveCluster, renderSettings.tileset, tilesetGrp, tileMask, maskIds);
        tileMask.texture.clear(); // Individual tiles/tile-masks only used for loading masked tiles and can be cleared after
        tilesetGrp.texture.clear();
    }
    else
        loadGrp(archiveCluster, tilesetPath, tilesetGrp, fileData, false, true);
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

void Scr::GraphicsData::Data::Skin::loadTiles(ArchiveCluster & archiveCluster, const RenderSettings & renderSettings, ByteBuffer & fileData)
{
    auto tilesetIndex = size_t(renderSettings.tileset) % size_t(Sc::Terrain::NumTilesets);
    if ( tiles[tilesetIndex] == nullptr )
    {
        tiles[tilesetIndex] = std::make_shared<Tileset>();
        tiles[tilesetIndex]->load(archiveCluster, renderSettings, fileData);
    }
}

void Scr::GraphicsData::Data::Skin::loadImages(Sc::Data & scData, ArchiveCluster & archiveCluster, std::filesystem::path texPrefix, const RenderSettings & renderSettings, ByteBuffer & fileData, gl::ContextSemaphore* contextSemaphore)
{
    if ( this->images == nullptr )
        this->images = std::make_shared<std::vector<std::shared_ptr<Animation>>>(size_t(999), nullptr);

    Scr::Skin skin {};
    skin.skinName = Scr::Skin::skinNames[size_t(renderSettings.skinId)];
    loadSkin(skin, archiveCluster, renderSettings.visualQuality, fileData);

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
    else if ( renderSettings.visualQuality == VisualQuality::SD || contextSemaphore == nullptr ) // SD (not reading additional casc files) or no context semaphore
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
            else if ( renderSettings.visualQuality == VisualQuality::SD )
                (*images)[i] = loadSdAnim(scData, sdData, i);
            else if ( skin.imageUsesSkinTexture[i] != 0 )
                (*images)[i] = loadHdAnim(archiveCluster, skinAnimPrefix / (std::string("main_") + (to_zero_padded_str(3, i) + ".anim")), fileData);
            else
                (*images)[i] = loadHdAnim(archiveCluster, animPrefix / (std::string("main_") + (to_zero_padded_str(3, i) + ".anim")), fileData);
        }
    }
    else // Have a context semaphore and loading VisualQuality::HD2/HD which reads ~1000 casc files, use multithreading for up to ~70% speedup
    {
        struct Task
        {
            ArchiveCluster* archiveCluster;
            std::filesystem::path path;
            std::shared_ptr<Animation>* anim;
            gl::ContextSemaphore* contextSemaphore;

            size_t requiredMemory = 0;

            void perform(ByteBuffer & memory) { *anim = GraphicsData::loadHdAnim(*archiveCluster, path.string(), memory, contextSemaphore); }
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
                    if ( renderSettings.visualQuality == VisualQuality::SD )
                        (*images)[i] = loadSdAnim(scData, sdData, i);
                    else
                    {
                        std::filesystem::path path {};
                        if ( skin.imageUsesSkinTexture[i] != 0 )
                            path = skinAnimPrefix / (std::string("main_") + (to_zero_padded_str(3, i) + ".anim"));
                        else
                            path = animPrefix / (std::string("main_") + (to_zero_padded_str(3, i) + ".anim"));

                        memoryTieredTasks.add_task({
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

void Scr::GraphicsData::unload(const RenderSettings & renderSettings)
{
    auto visualQualityIndex = renderSettings.visualQuality.index();
    if ( visualQualityData[visualQualityIndex] != nullptr )
    {
        if ( visualQualityData[visualQualityIndex].use_count() == 1 ) // Visual quality unused
            visualQualityData[visualQualityIndex] = nullptr;
        else // Visual quality still used, check individual component usage
        {
            auto skinIndex = size_t(renderSettings.skinId);
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

                    auto tilesetIndex = size_t(renderSettings.tileset) % Sc::Terrain::NumTilesets;
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

bool Scr::GraphicsData::isLoaded(const RenderSettings & renderSettings)
{
    if ( shaders == nullptr )
        return false;

    auto visualQualityIndex = renderSettings.visualQuality.index();
    if ( visualQualityData[visualQualityIndex] == nullptr )
        return false;

    auto & data = *(visualQualityData[visualQualityIndex]);
    if ( Sc::Terrain::hasWater(renderSettings.tileset) && renderSettings.visualQuality > VisualQuality::SD &&
        (data.waterNormal[0] == nullptr || data.waterNormal[1] == nullptr) )
    {
        return false;
    }

    auto skinIndex = size_t(renderSettings.skinId);
    if ( skinIndex >= Scr::Skin::total )
        throw std::logic_error("Invalid skinId");
    else if ( data.skin[skinIndex] == nullptr )
        return false;

    auto & skin = *(data.skin[skinIndex]);
    if ( renderSettings.showStars() && skin.spk == nullptr )
        return false;

    auto tilesetIndex = size_t(renderSettings.tileset) % Sc::Terrain::NumTilesets;
    if ( skin.tiles[tilesetIndex] == nullptr )
        return false;

    if ( skin.images == nullptr )
        return false;

    return true;
}

std::shared_ptr<Scr::GraphicsData::ClassicData> Scr::GraphicsData::loadClassic(Sc::Data & scData, const RenderSettings & renderSettings)
{
    if ( this->classicData == nullptr )
        this->classicData = std::make_shared<Scr::GraphicsData::ClassicData>();
    else if ( this->classicData->tilesetGrp[renderSettings.tileset % Sc::Terrain::NumTilesets].frames > 0 )
        return this->classicData; // Already loaded

    this->classicData->solidColorShader.load();
    this->classicData->paletteShader.load();

    // Populate tileTextureData
    constexpr size_t width = 32*128;
    std::vector<u8> tileTextureData(size_t(width*4096), u8(0)); // 4096x4096 palette indexes (128x128 tiles)
    auto tiles = scData.terrain.get(renderSettings.tileset);
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
    auto & tilesetGrp = classicData->tilesetGrp[renderSettings.tileset % Sc::Terrain::NumTilesets];
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

    // Load images
    classicData->images.assign(999, nullptr);
    for ( size_t i=0; i<999; ++i )
    {
        auto & imageDat = scData.sprites.getImage(i);
        auto & grp = scData.sprites.getGrp(imageDat.grpFile);
        auto & grpFile = grp.get();
        size_t numFrames = size_t(grpFile.numFrames);
        if ( numFrames > 0 )
        {
            classicData->images[i] = std::make_shared<Scr::ClassicGrp>();
            classicData->images[i]->frames.reserve(numFrames);
            classicData->images[i]->grpWidth = grpFile.grpWidth;
            classicData->images[i]->grpHeight = grpFile.grpHeight;
            classicData->images[i]->frames.reserve(numFrames);
            for ( size_t frame=0; frame<numFrames; ++frame )
            {
                classicData->images[i]->frames.emplace_back();
                const Sc::Sprite::GrpFrameHeader & grpFrameHeader = grpFile.frameHeaders[frame];
                s64 frameWidth = s64(grpFrameHeader.frameWidth);
                s64 frameHeight = s64(grpFrameHeader.frameHeight);
                s64 bitmapWidth = (frameWidth+3)/4*4;
                s64 bitmapHeight = (frameHeight+3)/4*4;
                classicData->images[i]->frames[frame].frameWidth = grpFrameHeader.frameWidth;
                classicData->images[i]->frames[frame].frameHeight = grpFrameHeader.frameHeight;
                classicData->images[i]->frames[frame].texWidth = u32(bitmapWidth);
                classicData->images[i]->frames[frame].texHeight = u32(bitmapHeight);
                classicData->images[i]->frames[frame].xOffset = grpFrameHeader.xOffset;
                classicData->images[i]->frames[frame].yOffset = grpFrameHeader.yOffset;
                
                std::vector<u8> bitmap(bitmapWidth*bitmapHeight, u8(0));

                if ( frameWidth == 0 || frameHeight == 0 ) // A dimension is zero, nothing to draw
                    continue;
        
                size_t frameOffset = size_t(grpFrameHeader.frameOffset);
                const Sc::Sprite::GrpFrame & grpFrame = (const Sc::Sprite::GrpFrame &)((u8*)&grpFile)[frameOffset];
                bool foundFirst = false;
                auto firstLoc = bitmap.begin();
                for ( s64 row=0; row < frameHeight; row++ )
                {
                    size_t rowOffset = size_t(grpFrame.rowOffsets[row]);
                    const Sc::Sprite::PixelRow & grpPixelRow = (const Sc::Sprite::PixelRow &)((u8*)&grpFile)[frameOffset+rowOffset];
                    const s64 rowStart = row*bitmapWidth;
                    s64 currPixelIndex = rowStart;
                    size_t pixelLineOffset = 0;
                    auto addition = size_t(row*bitmapWidth);
                    auto currPixel = bitmap.begin()+addition; // Start from the left-most pixel of this row of the frame
                    if ( !foundFirst )
                    {
                        firstLoc = currPixel;
                        foundFirst = true;
                    }
                    auto rowEnd = row == bitmapHeight-1 ? bitmap.end() : bitmap.begin()+size_t(currPixelIndex+frameWidth);

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
                
                classicData->images[i]->frames[frame].tex.genTexture();
                classicData->images[i]->frames[frame].tex.bind();
                classicData->images[i]->frames[frame].tex.setMinMagFilters(GL_NEAREST);
                classicData->images[i]->frames[frame].tex.loadImage2D({
                    .data = &bitmap[0],
                    .width = int(bitmapWidth),
                    .height = int(bitmapHeight),
                    .level = 0,
                    .internalformat = GL_R8UI,
                    .format = GL_RED_INTEGER,
                    .type = GL_UNSIGNED_BYTE
                });
                gl::Texture::bindDefault();
            }
        }
    }

    return classicData;
}

std::shared_ptr<Scr::GraphicsData::RenderData> Scr::GraphicsData::load(Sc::Data & scData, ArchiveCluster & archiveCluster, const RenderSettings & renderSettings, ByteBuffer & fileData)
{
    if ( shaders == nullptr )
    {
        shaders = std::make_shared<Shaders>();
        shaders->load(archiveCluster);
    }

    auto visualQualityIndex = renderSettings.visualQuality.index();
    if ( visualQualityData[visualQualityIndex] == nullptr )
        visualQualityData[visualQualityIndex] = std::make_shared<Data>();

    auto & data = *(visualQualityData[visualQualityIndex]);
    std::filesystem::path visualQualityPrefix = std::filesystem::path(renderSettings.visualQuality.prefix);
    if ( Sc::Terrain::hasWater(renderSettings.tileset) && renderSettings.visualQuality > VisualQuality::SD )
        data.loadWaterNormals(archiveCluster, visualQualityPrefix, fileData);

    auto skinIndex = size_t(renderSettings.skinId);
    Scr::Skin skinDescriptor { .skinName = std::string(Scr::Skin::skinNames[skinIndex]) };
    if ( skinIndex >= Scr::Skin::total )
        throw std::logic_error("Invalid skinId");
    else if ( data.skin[skinIndex] == nullptr )
        data.skin[skinIndex] = std::make_shared<Data::Skin>();
            
    auto & skin = *(data.skin[skinIndex]);
    if ( renderSettings.showStars() && skin.spk == nullptr )
        skin.loadStars(archiveCluster, visualQualityPrefix / skinDescriptor, fileData);

    auto tilesetIndex = size_t(renderSettings.tileset) % Sc::Terrain::NumTilesets;
    if ( skin.tiles[tilesetIndex] == nullptr )
        skin.loadTiles(archiveCluster, renderSettings, fileData);

    if ( skin.images == nullptr )
        skin.loadImages(scData, archiveCluster, visualQualityPrefix, renderSettings, fileData, openGlContextSemaphore);

    std::shared_ptr<RenderData> renderData = std::make_shared<RenderData>();
    renderData->shaders = this->shaders;
    renderData->spk = skin.spk;
    renderData->tiles = skin.tiles[renderSettings.tileset];
    renderData->images = skin.images;
    renderData->waterNormal[0] = data.waterNormal[0];
    renderData->waterNormal[1] = data.waterNormal[1];
    return renderData;
}

Scr::MapGraphics::MapGraphics(MapFile & mapFile) : mapFile(mapFile), initialTickCount(GetTickCount64()) {}

bool Scr::MapGraphics::isClassicLoaded(Scr::GraphicsData & scrDat)
{
    return scrDat.classicData != nullptr;
}

void Scr::MapGraphics::initVertices()
{
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
    triangleVertices.initialize({
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
}

void Scr::MapGraphics::setGridSize(s32 gridSize)
{
    this->gridSize = gridSize;
}

bool Scr::MapGraphics::displayingFps()
{
    return this->fpsEnabled;
}

void Scr::MapGraphics::toggleDisplayFps()
{
    this->fpsEnabled = !this->fpsEnabled;
}

void Scr::MapGraphics::drawTileNums()
{

}

void Scr::MapGraphics::drawTileOverlays()
{

}

GLfloat Scr::MapGraphics::getScaleFactor()
{
    return this->scaleFactor;
}

void Scr::MapGraphics::setScaleFactor(GLfloat scaleFactor)
{
    this->scaleFactor = scaleFactor;
}

void Scr::MapGraphics::loadClassic(Sc::Data & scData, Scr::GraphicsData & scrDat, const Scr::GraphicsData::RenderSettings & renderSettings)
{
    this->renderSettings = renderSettings;
    this->classicDat = scrDat.loadClassic(scData, renderSettings);

    initVertices();
}

void Scr::MapGraphics::load(Sc::Data & scData, Scr::GraphicsData & scrDat, ArchiveCluster & archiveCluster, const Scr::GraphicsData::RenderSettings & renderSettings, ByteBuffer & fileData)
{
    this->classicDat = nullptr;
    this->renderSettings = renderSettings;
    this->renderSettings.tileset = Sc::Terrain::Tileset(mapFile.getTileset() % Sc::Terrain::NumTilesets);
    this->scrDat = scrDat.load(scData, archiveCluster, this->renderSettings, fileData);
    
    initVertices();
}

void Scr::MapGraphics::setupNdcTransformation(s32 width, s32 height)
{
    posToNdc[0][0] = 2.0f/(width*renderSettings.visualQuality.scale/scaleFactor);
    posToNdc[1][1] = -2.0f/(height*renderSettings.visualQuality.scale/scaleFactor);
    unscaledPosToNdc[0][0] = 2.0f/width;
    unscaledPosToNdc[1][1] = -2.0f/height;
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

    scrDat->shaders->simpleShader.use();
    scrDat->shaders->simpleShader.tex.setSlot(0);
    scrDat->shaders->simpleShader.posToNdc.loadIdentity();
    tex.bindToSlot(GL_TEXTURE0);
            
    testVerts->bind();
    testVerts->drawTriangles();
}

void Scr::MapGraphics::drawGrid(s32 left, s32 top, s32 width, s32 height)
{
    s32 scaledWidth = s32(width/scaleFactor);
    s32 scaledHeight = s32(height/scaleFactor);
    if ( this->gridSize != 0 )
    {
        GLfloat pixel = 1.f * renderSettings.visualQuality.scale;
        GLfloat topAdjust = GLfloat(-top%this->gridSize);
        GLfloat leftAdjust = GLfloat(-left%this->gridSize)+pixel;
        GLfloat gridSize = GLfloat(this->gridSize);
        GLfloat gridSpacing = gridSize * renderSettings.visualQuality.scale;
        size_t hozLineCount = scaledHeight/this->gridSize+1;
        size_t vertLineCount = scaledWidth/this->gridSize+1;
        lineVertices.clear();
        lineVertices.vertices.reserve(size_t(hozLineCount*4+vertLineCount*4));
        for ( size_t y=(top % this->gridSize == 0 ? 1 : 0); y<=hozLineCount; ++y )
        {
            lineVertices.vertices.insert(lineVertices.vertices.end(), {
                0.f, y*gridSpacing+topAdjust,
                GLfloat(scaledWidth)*renderSettings.visualQuality.scale, y*gridSpacing+topAdjust
            });
        }
        for ( size_t x=(left % this->gridSize == 0 ? 1 : 0); x<=vertLineCount; ++x )
        {
            lineVertices.vertices.insert(lineVertices.vertices.end(), {
                x*gridSpacing+leftAdjust, 0.f,
                x*gridSpacing+leftAdjust, GLfloat(scaledHeight)*renderSettings.visualQuality.scale
            });
        }
    
        auto & solidColorShader = scrDat != nullptr ? scrDat->shaders->solidColorShader : classicDat->solidColorShader;
        solidColorShader.use();
        solidColorShader.posToNdc.setMat4(posToNdc);
        solidColorShader.solidColor.setColor(this->gridColor);
        lineVertices.bind();
        lineVertices.bufferData(gl::UsageHint::DynamicDraw);
        lineVertices.drawLines();
    }
}

void Scr::MapGraphics::drawLocations(s32 left, s32 top, s32 width, s32 height)
{
    auto leftAdjust = left*renderSettings.visualQuality.scale;
    auto topAdjust = top*renderSettings.visualQuality.scale;

    lineVertices.clear();
    triangleVertices.clear();
    for ( size_t i=0; i<mapFile.locations.size(); ++i )
    {
        const auto & location = mapFile.locations[i];
        gl::Rect2D<GLfloat> rect {
            GLfloat(location.left*renderSettings.visualQuality.scale),
            GLfloat(location.top*renderSettings.visualQuality.scale),
            GLfloat(location.right*renderSettings.visualQuality.scale),
            GLfloat(location.bottom*renderSettings.visualQuality.scale)
        };
        triangleVertices.vertices.insert(triangleVertices.vertices.end(), {
            rect.left-leftAdjust, rect.top-topAdjust,
            rect.right-leftAdjust, rect.top-topAdjust,
            rect.left-leftAdjust, rect.bottom-topAdjust,
            rect.left-leftAdjust, rect.bottom-topAdjust,
            rect.right-leftAdjust, rect.bottom-topAdjust,
            rect.right-leftAdjust, rect.top-topAdjust
        });
        lineVertices.vertices.insert(lineVertices.vertices.end(), {
            .5f+rect.left-leftAdjust, .5f+rect.top-topAdjust,
            .5f+rect.left-leftAdjust, .5f+rect.bottom-topAdjust,
            .5f+rect.left-leftAdjust, .5f+rect.top-topAdjust,
            .5f+rect.right-leftAdjust, .5f+rect.top-topAdjust,
            .5f+rect.right-leftAdjust, .5f+rect.top-topAdjust,
            .5f+rect.right-leftAdjust, .5f+rect.bottom-topAdjust,
            .5f+rect.left-leftAdjust, .5f+rect.bottom-topAdjust,
            .5f+rect.right-leftAdjust, .5f+rect.bottom-topAdjust
        });
    }
    
    auto & solidColorShader = scrDat != nullptr ? scrDat->shaders->solidColorShader : classicDat->solidColorShader;
    solidColorShader.use();
    solidColorShader.posToNdc.setMat4(posToNdc);
    solidColorShader.solidColor.setColor(0x3092B809); // Location color: 0xAABBGGRR
    triangleVertices.bind();
    triangleVertices.bufferData(gl::UsageHint::DynamicDraw);
    triangleVertices.drawTriangles();
    solidColorShader.solidColor.setColor(0xFF000000); // Line color: 0xAABBGGRR
    lineVertices.bind();
    lineVertices.bufferData(gl::UsageHint::DynamicDraw);
    lineVertices.drawLines();

    textFont->textShader.use();
    textFont->textShader.projection.setMat4(unscaledPosToNdc);
    textFont->setColor(255, 255, 0);
    for ( size_t i=0; i<mapFile.locations.size(); ++i )
    {
        const auto & location = mapFile.locations[i];
        auto locationName = mapFile.getLocationName<RawString>(i);
        if ( locationName )
            textFont->drawText((location.left-left)*scaleFactor+2.0f, (location.top-top)*scaleFactor+2.0f, locationName->c_str());
    }
}

void Scr::MapGraphics::drawStars(s32 x, s32 y, s32 scaledWidth, s32 scaledHeight, u32 multiplyColor)
{
    if ( renderSettings.skinId == Scr::Skin::Id::Classic )
        return; // TODO: OpenGL classic star rendering

    if ( scrDat->spk == nullptr )
        return;

    Scr::SpkData::SpkHeader* spk = scrDat->spk->spk;
    Scr::SpkGrp & stars = scrDat->spk->stars;
    Scr::StarPosition* starPosition = (Scr::StarPosition*)&((u8*)spk)[spk->starOffset];
    u32 starIndex = 0;
            
    scrDat->shaders->tileShader.use();
    scrDat->shaders->tileShader.posToNdc.setMat4(posToNdc);
    scrDat->shaders->tileShader.spriteTex.setSlot(0);
    scrDat->shaders->tileShader.multiplyColor.setColor(multiplyColor);

    for ( u32 layer=0; layer<spk->layerCount; ++layer )
    {
        u32 firstLayerStarIndex = starIndex;
        starVertices.clear();
        GLfloat texScale[2][2] { // Coverts logical texture coordinates to samplable texture coordinates
            { stars.texture[layer].xScale, 0 },
            { 0, stars.texture[layer].yScale }
        };

        u32 xStart = (x / spk->layers[layer].layerWidth) * spk->layers[layer].layerWidth;
        u32 xLimit = ((x + scaledWidth) / spk->layers[layer].layerWidth + 1) * spk->layers[layer].layerWidth;
        u32 yStart = (y / spk->layers[layer].layerHeight) * spk->layers[layer].layerHeight;
        u32 yLimit = ((y + scaledHeight) / spk->layers[layer].layerHeight + 1) * spk->layers[layer].layerHeight;

        for ( ; yStart < yLimit; yStart += spk->layers[layer].layerHeight )
        {
            for ( u32 xc = xStart; xc < xLimit; xc += u32(spk->layers[layer].layerWidth) )
            {
                starIndex = firstLayerStarIndex;
                for ( u32 j = 0; j < spk->layers[layer].count; j++ )
                {
                    Scr::StarPosition & currStarPos = starPosition[starIndex];
                    GLfloat left = -(currStarPos.width/2.0f);
                    GLfloat top = -(currStarPos.height/2.0f);
                    gl::Point2D<GLfloat> position { GLfloat(x + xc)+currStarPos.xOffset, GLfloat(y + yStart)+currStarPos.yOffset };
                    gl::Rect2D<GLfloat> vertexRect { left, top, left+currStarPos.width, top+currStarPos.height };
                    gl::Rect2D<GLfloat> texRect {
                        GLfloat(currStarPos.xTextureOffset),
                        GLfloat(currStarPos.yTextureOffset),
                        GLfloat(currStarPos.xTextureOffset)+GLfloat(currStarPos.width),
                        GLfloat(currStarPos.yTextureOffset)+GLfloat(currStarPos.height)
                    };
                    starVertices.vertices.insert(starVertices.vertices.end(), {
                        position.x+vertexRect.left, position.y+vertexRect.top, texRect.left, texRect.top,
                        position.x+vertexRect.right, position.y+vertexRect.top, texRect.right, texRect.top,
                        position.x+vertexRect.left, position.y+vertexRect.bottom, texRect.left, texRect.bottom,
                        position.x+vertexRect.left, position.y+vertexRect.bottom, texRect.left, texRect.bottom,
                        position.x+vertexRect.right, position.y+vertexRect.bottom, texRect.right, texRect.bottom,
                        position.x+vertexRect.right, position.y+vertexRect.top, texRect.right, texRect.top,
                    });
                    ++starIndex;
                }
            }
        }
                
        scrDat->shaders->tileShader.texScale.setMat2(&texScale[0][0]);
        stars.texture[layer].bindToSlot(GL_TEXTURE0);
                
        starVertices.bind();
        starVertices.bufferData(gl::UsageHint::DynamicDraw);
        starVertices.drawTriangles();
    }
}

void Scr::MapGraphics::drawTileVertices(Scr::Grp & tilesetGrp, s32 left, s32 top, s32 width, s32 height)
{
    auto tilesetIndex = Sc::Terrain::Tileset(mapFile.getTileset() % Sc::Terrain::NumTilesets);
    bool drawHdWater = renderSettings.visualQuality > VisualQuality::SD &&
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
        screenTex.setMinMagFilters(GL_LINEAR);
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

        scrDat->shaders->simpleShader.use();
        scrDat->shaders->simpleShader.tex.setSlot(0);
        scrDat->shaders->simpleShader.posToNdc.setMat4(posToNdc);
        scrDat->tiles->tileMask.mergedTexture.bindToSlot(GL_TEXTURE0);

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
        scrDat->shaders->waterShader.use();
        scrDat->shaders->waterShader.posToNdc.loadIdentity();
        scrDat->shaders->waterShader.texScale.loadIdentity();
        scrDat->shaders->waterShader.spriteTex.setSlot(0);
        scrDat->shaders->waterShader.sampleTex.setSlot(1);
        scrDat->shaders->waterShader.sampleTex2.setSlot(2);
        scrDat->shaders->waterShader.sampleTex3.setSlot(3);
        scrDat->shaders->waterShader.sampleTex4.setSlot(4);
        scrDat->shaders->waterShader.data.set(0.0f, 0.0f, (GetTickCount64() - initialTickCount)/4000.0f);

        screenTex.bindToSlot(GL_TEXTURE0);
        scrDat->waterNormal[0]->texture[n1Frame].bindToSlot(GL_TEXTURE1);
        scrDat->waterNormal[0]->texture[n1Frame+1 >= scrDat->waterNormal[0]->frames ? 0 : n1Frame+1].bindToSlot(GL_TEXTURE2);
        scrDat->waterNormal[1]->texture[n2Frame].bindToSlot(GL_TEXTURE3);
        scrDat->waterNormal[1]->texture[n2Frame+1 >= scrDat->waterNormal[1]->frames ? 0 : n2Frame+1].bindToSlot(GL_TEXTURE4);

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
        if ( renderSettings.visualQuality > VisualQuality::SD )
        {
            scrDat->shaders->tileShader.use();
            scrDat->shaders->tileShader.posToNdc.setMat4(posToNdc);
            scrDat->shaders->tileShader.texScale.loadIdentity();
            scrDat->shaders->tileShader.spriteTex.setSlot(0);
            scrDat->shaders->tileShader.multiplyColor.setColor(0xFFFFFFFF);
            tilesetGrp.mergedTexture.bindToSlot(GL_TEXTURE0);
        }
        else if ( renderSettings.skinId == Scr::Skin::Id::Classic ) // Classic
        {
            classicDat->paletteShader.use();
            classicDat->paletteShader.posToNdc.setMat4(posToNdc);
            classicDat->paletteShader.texScale.loadIdentity();
            classicDat->paletteShader.tex.setSlot(0);
            classicDat->paletteShader.pal.setSlot(1);
            tilesetGrp.mergedTexture.bindToSlot(GL_TEXTURE0);
            tilesetGrp.palette->tex.bindToSlot(GL_TEXTURE1);
        }
        else // Remastered SD
        {
            scrDat->shaders->paletteShader.use();
            scrDat->shaders->paletteShader.posToNdc.setMat4(posToNdc);
            scrDat->shaders->paletteShader.texScale.loadIdentity();
            scrDat->shaders->paletteShader.spriteTex.setSlot(0);
            scrDat->shaders->paletteShader.sampleTex.setSlot(1);
            scrDat->shaders->paletteShader.multiplyColor.setColor(0xFFFFFFFF);
            tilesetGrp.mergedTexture.bindToSlot(GL_TEXTURE0);
            tilesetGrp.palette->tex.bindToSlot(GL_TEXTURE1);
        }

        tileVertices.bind();
        tileVertices.bufferData(gl::UsageHint::DynamicDraw);
        tileVertices.drawTriangles();

        if ( renderSettings.visualQuality == VisualQuality::SD )
            gl::Texture::releaseSlots(GL_TEXTURE1);
        else
            gl::Texture::releaseSlots(GL_TEXTURE0);
    }
}

void Scr::MapGraphics::drawTerrain(Sc::Data & scData, s32 left, s32 top, s32 width, s32 height)
{
    s32 scaledWidth = width/scaleFactor;
    s32 scaledHeight = height/scaleFactor;
    auto & tilesetGrp = renderSettings.skinId == Scr::Skin::Id::Classic ? classicDat->tilesetGrp[renderSettings.tileset] : scrDat->tiles->tilesetGrp;
    tileVertices.clear();
    auto tiles = scData.terrain.get(Sc::Terrain::Tileset(mapFile.tileset));

    GLfloat texWidth = GLfloat(tilesetGrp.width) / 2.0f;
    GLfloat texHeight = GLfloat(tilesetGrp.height) / 2.0f;
    gl::Rect2D<GLfloat> vertexRect {-texWidth, -texHeight, texWidth, texHeight};
    s32 xStart = std::max(0, left/32-2);
    s32 yStart = std::max(0, top/32-2);
    s32 xLimit = std::min((left+scaledWidth)/32, s32(mapFile.dimensions.tileWidth-1));
    s32 yLimit = std::min((top+scaledHeight)/32, s32(mapFile.dimensions.tileHeight-1));

    GLfloat texelOffset = scaleFactor != 1.f ? 0.5f/32.f/128.f : 0;

    for ( s32 y = yStart; y <= yLimit; y++ )
    {
        for ( s32 x = xStart; x <= xLimit; x++ )
        {
            u32 megaTileIndex = 0;
            u16 tileIndex = mapFile.tiles[size_t(y) * size_t(mapFile.dimensions.tileWidth) + size_t(x)];
            size_t groupIndex = Sc::Terrain::Tiles::getGroupIndex(tileIndex);
            if ( groupIndex < tiles.tileGroups.size() )
            {
                const Sc::Terrain::TileGroup & tileGroup = tiles.tileGroups[groupIndex];
                megaTileIndex = tileGroup.megaTileIndex[tiles.getGroupMemberIndex(tileIndex)];
            }

            gl::Point2D<GLfloat> position {
                GLfloat((x*32-left)*s32(renderSettings.visualQuality.scale)+32*s32(renderSettings.visualQuality.scale)/2),
                GLfloat((y*32-top)*s32(renderSettings.visualQuality.scale)+32*s32(renderSettings.visualQuality.scale)/2)
            };
            auto left = float(megaTileIndex%128)/float(128);
            auto top = float(megaTileIndex/128)/float(128);
            auto right = left+1.f/128.f-texelOffset;
            auto bottom = top+1.f/128.f-texelOffset;
            gl::Rect2D<GLfloat> texRect {left, top, right, bottom};
            tileVertices.vertices.insert(tileVertices.vertices.end(), {
                position.x+vertexRect.left, position.y+vertexRect.top, texRect.left, texRect.top,
                position.x+vertexRect.right, position.y+vertexRect.top, texRect.right, texRect.top,
                position.x+vertexRect.left, position.y+vertexRect.bottom, texRect.left, texRect.bottom,
                position.x+vertexRect.left, position.y+vertexRect.bottom, texRect.left, texRect.bottom,
                position.x+vertexRect.right, position.y+vertexRect.bottom, texRect.right, texRect.bottom,
                position.x+vertexRect.right, position.y+vertexRect.top, texRect.right, texRect.top,
            });
        }
    }

    drawTileVertices(tilesetGrp, left, top, width, height);
}

void Scr::MapGraphics::drawTilesetIndexed(Sc::Data & scData, s32 left, s32 top, s32 width, s32 height, s32 scrollY)
{
    auto & tilesetGrp = renderSettings.skinId == Scr::Skin::Id::Classic ? classicDat->tilesetGrp[renderSettings.tileset] : scrDat->tiles->tilesetGrp;

    tileVertices.clear();
    setupNdcTransformation(width, height);
    auto & tiles = scData.terrain.get(Sc::Terrain::Tileset(mapFile.tileset));

    GLfloat texWidth = GLfloat(tilesetGrp.width) / 2.0f;
    GLfloat texHeight = GLfloat(tilesetGrp.height) / 2.0f;
    gl::Rect2D<GLfloat> vertexRect {-texWidth, -texHeight, texWidth, texHeight};
    s32 topRow = scrollY/33;
    s32 bottomRow = (scrollY + height)/33;
    s32 totalRows = height/32;
    s32 totalColumns = width/32;

    for ( s32 row=topRow; row<=bottomRow; ++row )
    {
        GLfloat tileTop = GLfloat(row*33-scrollY)*s32(renderSettings.visualQuality.scale)+32*s32(renderSettings.visualQuality.scale)/2;
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
                GLfloat(column*33)*s32(renderSettings.visualQuality.scale)+32*s32(renderSettings.visualQuality.scale)/2,
                tileTop
            };
            auto left = float(megaTileIndex%128)/float(128);
            auto top = float(megaTileIndex/128)/float(128);
            auto right = left+1.f/128.f;
            auto bottom = top+1.f/128.f;
            gl::Rect2D<GLfloat> texRect {left, top, right, bottom};
            tileVertices.vertices.insert(tileVertices.vertices.end(), {
                position.x+vertexRect.left, position.y+vertexRect.top, texRect.left, texRect.top,
                position.x+vertexRect.right, position.y+vertexRect.top, texRect.right, texRect.top,
                position.x+vertexRect.left, position.y+vertexRect.bottom, texRect.left, texRect.bottom,
                position.x+vertexRect.left, position.y+vertexRect.bottom, texRect.left, texRect.bottom,
                position.x+vertexRect.right, position.y+vertexRect.bottom, texRect.right, texRect.bottom,
                position.x+vertexRect.right, position.y+vertexRect.top, texRect.right, texRect.top,
            });
        }
    }

    drawTileVertices(tilesetGrp, left, top, width, height);
}

void Scr::MapGraphics::drawAnim(Scr::Animation & animation, s32 x, s32 y, u32 frame, u32 playerColor, u32 multiplyColor, bool hallucinate, bool halfAnims)
{
    if ( frame >= animation.totalFrames )
        frame = 0;

    GLfloat texScale[2][2] { // Coverts logical texture coordinates to samplable texture coordinates
        { animation.xScale, 0 },
        { 0, animation.yScale }
    };

    scrDat->shaders->spriteShader.use();
    scrDat->shaders->spriteShader.posToNdc.setMat4(posToNdc);
    scrDat->shaders->spriteShader.texScale.setMat2(&texScale[0][0]);
    scrDat->shaders->spriteShader.spriteTex.setSlot(0);
    scrDat->shaders->spriteShader.teamColorTex.setSlot(1);
    scrDat->shaders->spriteShader.hallucinate.setValue(GLfloat(hallucinate));
    scrDat->shaders->spriteShader.multiplyColor.setColor(multiplyColor);
    scrDat->shaders->spriteShader.teamColor.setColor(playerColor);

    animation.diffuse.bindToSlot(GL_TEXTURE0);
    animation.teamColor.bindToSlot(GL_TEXTURE1);

    if ( halfAnims )
    {
        GLfloat left = GLfloat(animation.width/-4.0 + animation.frames[frame].xOffset/2);
        GLfloat top = GLfloat(animation.height/-4.0 + animation.frames[frame].yOffset/2);
        GLint width = animation.frames[frame].width/2;
        GLint height = animation.frames[frame].height/2;
        GLint texLeft = animation.frames[frame].xTextureOffset/2;
        GLint texTop = animation.frames[frame].yTextureOffset/2;
        gl::Point2D<GLfloat> position { GLfloat(x), GLfloat(y) };
        gl::Rect2D<GLfloat> vertexRect { left, top, left + width, top + height };
        gl::Rect2D<GLfloat> texRect { GLfloat(texLeft), GLfloat(texTop), GLfloat(texLeft)+GLfloat(width), GLfloat(texTop)+GLfloat(height) };
        animVertices.vertices = {
            position.x+vertexRect.left, position.y+vertexRect.top, texRect.left, texRect.top,
            position.x+vertexRect.right, position.y+vertexRect.top, texRect.right, texRect.top,
            position.x+vertexRect.left, position.y+vertexRect.bottom, texRect.left, texRect.bottom,
            position.x+vertexRect.left, position.y+vertexRect.bottom, texRect.left, texRect.bottom,
            position.x+vertexRect.right, position.y+vertexRect.bottom, texRect.right, texRect.bottom,
            position.x+vertexRect.right, position.y+vertexRect.top, texRect.right, texRect.top,
        };
    }
    else
    {
        GLfloat left = GLfloat(animation.width/-2.0 + animation.frames[frame].xOffset);
        GLfloat top = GLfloat(animation.height/-2.0 + animation.frames[frame].yOffset);
        GLint width = animation.frames[frame].width;
        GLint height = animation.frames[frame].height;
        GLint texLeft = animation.frames[frame].xTextureOffset;
        GLint texTop = animation.frames[frame].yTextureOffset;
        gl::Point2D<GLfloat> position { GLfloat(x), GLfloat(y) };
        gl::Rect2D<GLfloat> vertexRect { left, top, left + width, top + height };
        gl::Rect2D<GLfloat> texRect { GLfloat(texLeft), GLfloat(texTop), GLfloat(texLeft)+GLfloat(width), GLfloat(texTop)+GLfloat(height) };
        animVertices.vertices = {
            position.x+vertexRect.left, position.y+vertexRect.top, texRect.left, texRect.top,
            position.x+vertexRect.right, position.y+vertexRect.top, texRect.right, texRect.top,
            position.x+vertexRect.left, position.y+vertexRect.bottom, texRect.left, texRect.bottom,
            position.x+vertexRect.left, position.y+vertexRect.bottom, texRect.left, texRect.bottom,
            position.x+vertexRect.right, position.y+vertexRect.bottom, texRect.right, texRect.bottom,
            position.x+vertexRect.right, position.y+vertexRect.top, texRect.right, texRect.top,
        };
    }
    animVertices.bind();
    animVertices.bufferData(gl::UsageHint::DynamicDraw);
    animVertices.drawTriangles();
}

void Scr::MapGraphics::drawClassicImage(Sc::Data & scData, gl::Palette & palette, s32 x, s32 y, u32 imageId, Chk::PlayerColor color)
{
    // Preserve previous palette colors
    std::uint32_t remapped[8];
    constexpr size_t sizeChanged = sizeof(remapped);
    std::memcpy(remapped, &palette[8], sizeof(remapped));

    // Replace palette with player colors
    std::memcpy(&palette[8], &scData.tunit.rgbaPalette[color < 16 ? 8*color : 8*(color%16)], sizeof(remapped));

    palette.update();
            
    auto & imageInfo = classicDat->images[imageId];
    u32 frame = 0;
    auto & frameInfo = imageInfo->frames[frame];

    GLfloat texScale[2][2] { // Coverts logical texture coordinates to samplable texture coordinates
        { 1.f/frameInfo.texWidth, 0 },
        { 0, 1.f/frameInfo.texHeight }
    };

    GLfloat vertexLeft = GLfloat(-imageInfo->grpWidth/2 + frameInfo.xOffset);
    GLfloat vertexTop = GLfloat(-imageInfo->grpHeight/2 + frameInfo.yOffset);
    gl::Point2D<GLfloat> position { GLfloat(x), GLfloat(y) };
    gl::Rect2D<GLfloat> vertexRect { vertexLeft, vertexTop, vertexLeft + frameInfo.texWidth, vertexTop + frameInfo.texHeight };
    gl::Rect2D<GLfloat> texRect { GLfloat(0.f), GLfloat(0.f), GLfloat(1.f), GLfloat(1.f) };
    animVertices.vertices = {
        position.x+vertexRect.left, position.y+vertexRect.top, texRect.left, texRect.top,
        position.x+vertexRect.right, position.y+vertexRect.top, texRect.right, texRect.top,
        position.x+vertexRect.left, position.y+vertexRect.bottom, texRect.left, texRect.bottom,
        position.x+vertexRect.left, position.y+vertexRect.bottom, texRect.left, texRect.bottom,
        position.x+vertexRect.right, position.y+vertexRect.bottom, texRect.right, texRect.bottom,
        position.x+vertexRect.right, position.y+vertexRect.top, texRect.right, texRect.top,
    };

    classicDat->paletteShader.use();
    classicDat->paletteShader.posToNdc.setMat4(posToNdc);
    classicDat->paletteShader.texScale.loadIdentity();
    classicDat->paletteShader.tex.setSlot(0);
    classicDat->paletteShader.pal.setSlot(1);

    frameInfo.tex.bindToSlot(GL_TEXTURE0);
    classicDat->tilesetGrp[renderSettings.tileset].palette->tex.bindToSlot(GL_TEXTURE1);

    animVertices.bind();
    animVertices.bufferData(gl::UsageHint::DynamicDraw);
    animVertices.drawTriangles();
    gl::Texture::releaseSlots(GL_TEXTURE1);

    // Restore palette colors
    std::memcpy(&palette[8], remapped, sizeof(remapped));
    palette.update();
}

void Scr::MapGraphics::drawSprites(Sc::Data & scData, s32 left, s32 top)
{
    if ( renderSettings.skinId == Scr::Skin::Id::Classic )
    {
        auto & palette = *classicDat->tilesetGrp[renderSettings.tileset].palette;
        for ( auto & unit : mapFile.units )
        {
            u32 flingyId = u32(scData.units.getUnit(Sc::Unit::Type(unit.type >= 228 ? 0 : unit.type)).graphics);
            u32 spriteId = u32(scData.units.getFlingy(flingyId >= 209 ? 0 : flingyId).sprite);
            u32 imageId = scData.sprites.getSprite(spriteId >= 517 ? 0 : spriteId).imageFile;
            drawClassicImage(scData, palette, unit.xc-left, unit.yc-top, imageId, (Chk::PlayerColor)unit.owner);
        }

        for ( auto & sprite : mapFile.sprites )
        {
            if ( sprite.isDrawnAsSprite() )
            {
                u32 imageId = scData.sprites.getSprite(sprite.type >= 517 ? 0 : sprite.type).imageFile;
                drawClassicImage(scData, palette, sprite.xc-left, sprite.yc-top, imageId, (Chk::PlayerColor)sprite.owner);
            }
            else
            {
                u32 flingyId = u32(scData.units.getUnit(Sc::Unit::Type(sprite.type >= 228 ? 0 : sprite.type)).graphics);
                u32 spriteId = u32(scData.units.getFlingy(flingyId >= 209 ? 0 : flingyId).sprite);
                u32 imageId = scData.sprites.getSprite(spriteId >= 517 ? 0 : spriteId).imageFile;
                drawClassicImage(scData, palette, sprite.xc-left, sprite.yc-top, imageId, (Chk::PlayerColor)sprite.owner);
            }
        }
    }
    else
    {
        for ( auto & unit : mapFile.units )
        {
            u32 flingyId = u32(scData.units.getUnit(Sc::Unit::Type(unit.type >= 228 ? 0 : unit.type)).graphics);
            u32 spriteId = u32(scData.units.getFlingy(flingyId >= 209 ? 0 : flingyId).sprite);
            u32 imageId = scData.sprites.getSprite(spriteId >= 517 ? 0 : spriteId).imageFile;
            drawAnim(*((*scrDat->images)[imageId]), (unit.xc-left)*renderSettings.visualQuality.scale, (unit.yc-top)*renderSettings.visualQuality.scale, 0,
                (u32 &)(scData.tunit.rgbaPalette[8*size_t(unit.owner)]), 0xFFFFFFFF, false, renderSettings.visualQuality.halfAnims);
        }

        for ( auto & sprite : mapFile.sprites )
        {
            if ( sprite.isDrawnAsSprite() )
            {
                u32 imageId = scData.sprites.getSprite(sprite.type >= 517 ? 0 : sprite.type).imageFile;
                drawAnim(*((*scrDat->images)[imageId]), (sprite.xc-left)*renderSettings.visualQuality.scale, (sprite.yc-top)*renderSettings.visualQuality.scale, 0,
                    (u32 &)(scData.tunit.rgbaPalette[8*size_t(sprite.owner)]), 0xFFFFFFFF, false, renderSettings.visualQuality.halfAnims);
            }
            else
            {
                u32 flingyId = u32(scData.units.getUnit(Sc::Unit::Type(sprite.type >= 228 ? 0 : sprite.type)).graphics);
                u32 spriteId = u32(scData.units.getFlingy(flingyId >= 209 ? 0 : flingyId).sprite);
                u32 imageId = scData.sprites.getSprite(spriteId >= 517 ? 0 : spriteId).imageFile;
                drawAnim(*((*scrDat->images)[imageId]), (sprite.xc-left)*renderSettings.visualQuality.scale, (sprite.yc-top)*renderSettings.visualQuality.scale, 0,
                    (u32 &)(scData.tunit.rgbaPalette[8*size_t(sprite.owner)]), 0xFFFFFFFF, false, renderSettings.visualQuality.halfAnims);
            }
        }
    }
}

void Scr::MapGraphics::render(Sc::Data & scData, s32 left, s32 top, s32 width, s32 height, bool renderLocations)
{
    setupNdcTransformation(width, height);
    drawStars(left, top, width*renderSettings.visualQuality.scale, height*renderSettings.visualQuality.scale, 0xFFFFFFFF);
    drawTerrain(scData, left, top, width, height);
    drawGrid(left, top, width, height);
    drawSprites(scData, left, top);
    if ( renderLocations )
        drawLocations(left, top, width, height);

    if ( fpsEnabled )
    {
        fps.update(std::chrono::system_clock::now());
        textFont->textShader.use();
        textFont->textShader.projection.setMat4(unscaledPosToNdc);
        textFont->drawAffixedText<gl::Align::Center>(width/2, 10.f, fps.displayNumber, " fps", "");
    }

    //textFont->textShader.use();
    //textFont->textShader.projection.setMat4(unscaledPosToNdc);

    //textFont->drawText(10.f, 10.f, "THE QUICK BROWN FOX JUMPED OVER THE LAZY DOG");
    //textFont->drawText(10.f, 50.f, "the quick brown fox jumped over the lazy dog");
}

void Scr::MapGraphics::updateGraphics(u64 ticks)
{
    // update classic/SD color cycling
    auto & tilesetGrp = renderSettings.skinId == Scr::Skin::Id::Classic ? classicDat->tilesetGrp[renderSettings.tileset] : scrDat->tiles->tilesetGrp;
    if ( tilesetGrp.palette )
    {
        if ( colorCycler.cycleColors(GetTickCount64(), mapFile.getTileset(), tilesetGrp.palette.value()) )
            tilesetGrp.palette->update();
    }

    // update HD water animation
    if ( renderSettings.visualQuality > VisualQuality::SD && scrDat->waterNormal[0] && scrDat->waterNormal[0]->frames > 0 )
    {
        nIncrement += ticks;
        if ( nIncrement >= 4000 )
        {
            nIncrement = 0;

            n1Frame++;
            if ( n1Frame >= scrDat->waterNormal[0]->frames )
                n1Frame = 0;

            n2Frame++;
            if ( n2Frame >= scrDat->waterNormal[1]->frames )
                n2Frame = 0;
        }
    }
}
