#ifndef SCRGRAPHICS_H
#define SCRGRAPHICS_H
#include "mapping/color_cycler.h"
#include <mapping_core/mapping_core.h>
#include <rarecpp/reflect.h>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include "gl/context_semaphore.h"
#include "gl/font.h"
#include "gl/fps.h"
#include "gl/palette.h"
#include "gl/program.h"
#include "gl/shader.h"
#include "gl/texture.h"
#include "gl/uniform.h"
#include "gl/vertices.h"
#include <glm/glm.hpp>

namespace Scr {

    inline namespace DataStructs
    {
        struct Grp
        {
            struct Header
            {
                u32 fileSize = 0;
                u16 frames = 0;
                u16 type = 0; // 0x1001=SD, 0x1002=HD2, 0x1004=HD, 0x1011=SD bitmaps

                constexpr bool containsDdsFiles() { return (type & 0x0010) == 0; }
            };
            
            struct File
            {
                u32 unknown = 0;
                u16 width = 0;
                u16 height = 0;
                u32 size = 0;
            };
            
            struct Bmp // type == 0x1011
            {
                u16 width = 0;
                u16 height = 0;
                u32 palette[256] {};
            };

            u32 frames = 0;
            u32 width = 0;
            u32 height = 0;
            std::optional<gl::Palette> palette {};
            std::vector<gl::Texture> texture {};
            gl::Texture mergedTexture {};
        };

        struct ClassicGrp
        {
            struct Frame
            {
                u32 frameWidth = 0;
                u32 frameHeight = 0;
                u32 texWidth = 0; // frameWidth padded to 4-bytes
                u32 texHeight = 0; // frameHeight padded to 4-bytes
                s16 xOffset = 0;
                s16 yOffset = 0;
                gl::Texture tex {};
            };

            std::vector<Frame> frames;
            u16 grpWidth = 0;
            u16 grpHeight = 0;
        };

        struct TileMask
        {
            struct Entry
            {
                u16 vr4Index = 0;
                u16 maskIndex = 0;
            };

            struct File
            {
                u32 fileType = 0; // 'KSMT'
                u16 unknown = 0;
                u16 count = 0;
                Entry tileMasks[1] {}; // [count]
            };
        };

        struct ClassicStar
        {
            u16 xc = 0;
            u16 yc = 0;
            u16 width = 0;
            u16 height = 0;
            u16 texWidth = 0;
            u16 texHeight = 0;
            gl::Texture tex {};
        };

        struct StarPosition
        {
            u16 xOffset = 0;
            u16 yOffset = 0;
            u16 xTextureOffset = 0;
            u16 yTextureOffset = 0;
            u16 width = 0;
            u16 height = 0;
        };

        struct SpkGrp
        {
            struct Texture : gl::Texture
            {
                float xScale = 0.f;
                float yScale = 0.f;

                Texture(gl::Texture && texture) : gl::Texture(std::move(texture)) {}
            };

            u16 layers = 0;
            std::vector<Texture> texture {};
        };

        class SpkData
        {
            std::vector<u8> spkHeaderDat {};

        public:
            struct SpkHeader
            {
                struct SpkLayer
                {
                    u32 count = 0;
                    u16 layerWidth = 0;
                    u16 layerHeight = 0;
                };

                u32 layerCount = 0;
                u32 starOffset = 0;
                SpkLayer layers[5] {}; // layers[layerCount]
            };
            SpkHeader* spk = nullptr;
            SpkGrp stars {};
            std::vector<std::vector<ClassicStar>> classicLayers {};

            void load(std::vector<u8> && spkHeaderDat)
            {
                this->spkHeaderDat.swap(spkHeaderDat);
                this->spk = (SpkHeader*)this->spkHeaderDat.data();
            }
        };

        struct Animation
        {
            struct MainSd
            {
                static constexpr u32 BMP = 0x20504D42; // "BMP "

                struct Entry
                {
                    struct Image
                    {
                        u32 offset = 0;
                        u32 size = 0;
                        u16 textureWidth = 0;
                        u16 textureHeight = 0;
                    };

                    u16 frames = 0; // zero indicates a reference
                    u16 unknown = 0; // 0xFFFF
                    u16 grpWidth = 0; // for SD use classic GRP files width/height, not the values from MainSD
                    u16 grpHeight = 0;
                    u32 offset = 0;
                    Image images[10] {};
                };

                struct Reference
                {
                    u16 frames = 0; // non-zero indicates an entry
                    u16 referencedImage = 0; // image index to refer to
                    u32 unknown = 0;
                    u32 unknown1 = 0;
                    u16 unknown2 = 0;
                };
            };

            struct Header
            {
                static constexpr u32 ANIM = 0x4D494E41; // "ANIM"

                u32 fileType = ANIM;
                u16 version = 0; // SD = 0x0101, HD2 = 0x0202, HD = 0x0204
                u16 unknown = 0;
                u16 layers = 0;
                u16 entries = 0; // 999
                u8 layernames[10][32] {};
            };

            struct Frame
            {
                u16 xTextureOffset = 0;
                u16 yTextureOffset = 0;
                s16 xOffset = 0;
                s16 yOffset = 0;
                u16 width = 0;
                u16 height = 0;
                u32 unknown = 0;
            };

            u32 totalFrames = 0;
            u32 width = 0;
            u32 height = 0;
            float xScale = 0.f;
            float yScale = 0.f;
            gl::Texture diffuse {};
            gl::Texture teamColor {};
            std::vector<Frame> frames {};
        };

        struct VisualQuality
        {
            u32 scale;
            bool halfAnims;
            std::string_view prefix;

            friend constexpr auto operator==(const VisualQuality & l, const VisualQuality & r) { return l.scale == r.scale; }
            friend constexpr auto operator<=>(const VisualQuality & l, const VisualQuality & r) { return l.scale <=> r.scale; }

            static const VisualQuality SD, HD2, HD;
            static constexpr size_t total = 3;

            constexpr size_t index() const
            {
                switch ( scale )
                {
                    case 1: return 0;
                    case 2: return 1;
                    case 4: return 2;
                    default: throw std::logic_error("Unrecognized visual quality");
                }
            }

            constexpr float imageScale() const
            {
                if ( halfAnims )
                    return 1.f/float(scale)/2.f;
                else
                    return 1.f/float(scale);
            }
        };
        inline constexpr VisualQuality
            VisualQuality::SD { 1, false, "SD" },
            VisualQuality::HD2 { 2, true, "HD2" },
            VisualQuality::HD { 4, false, "" };

        struct Skin
        {
            enum class Id { Classic = 0, Remastered = 1, Carbot = 2, Total };
            static constexpr size_t total = size_t(Id::Total);

            static constexpr std::string_view skinNames[] { "", "", "carbot" };

            std::string skinName {};
            u8 imageUsesSkinTexture[999] {}; // 0 = use default, 1 = use skin
            
            friend auto operator/(const Skin & skin, const std::filesystem::path & path) { return skin.skinName.empty() ? path : skin.skinName / path; }
            friend auto operator/(const std::filesystem::path & path, const Skin & skin) { return skin.skinName.empty() ? path : path / skin.skinName; }
        };
    }

    struct GraphicsData
    {
        struct Shaders
        {
            static inline auto ensureVersioned = [](std::vector<u8> & shader) {
                constexpr std::string_view versionDirective = "#version";
                constexpr std::string_view defaultVersion = "#version 330 core\n";
                if ( std::string_view((const char*)shader.data(), shader.size()).find(versionDirective) == std::string_view::npos )
                    shader.insert(shader.begin(), (u8*)defaultVersion.data(), (u8*)(defaultVersion.data() + defaultVersion.size()));
            };

            template <typename Preprocessor>
            static gl::Shader fragmentShaderFromDatFile(ArchiveCluster & archiveCluster, const std::string & filePath,
                Preprocessor && preprocessor, bool requireNonEmpty = true)
            {
                if ( auto fileData = archiveCluster.getFile(filePath) )
                {
                    preprocessor(fileData.value());
                    return gl::Shader(gl::Shader::Type::fragment, (const char*)fileData->data(), fileData->size(), requireNonEmpty);
                }
                else
                    throw std::runtime_error("Failed to get file " + filePath + " from archives");
            }
    
            static std::vector<u8> fragmentShaderStringFromDatFile(ArchiveCluster & archiveCluster, const std::string & filePath)
            {
                if ( auto fileData = archiveCluster.getFile(filePath) )
                    return *fileData;
                else
                    throw std::runtime_error("Failed to get file " + filePath + " from archives");
            }

            class TextureVertexShader : public gl::Program
            {
                static constexpr std::string_view vertexShaderCode =
                    "#version 330 core\n"

                    "layout (location = 0) in vec2 pos;"
                    "layout (location = 1) in vec2 texCoordIn;"

                    "out vec2 texCoord;"
                    
                    "uniform mat4 posToNdc;"
                    "uniform mat4 texTransform;"

                    "void main() {"
                    "    gl_Position = posToNdc * vec4(pos, 0.0, 1.0);"
                    "    texCoord = vec4(texTransform * vec4(texCoordIn, 0.0, 1.0)).xy;"
                    "};";

                public:
                    gl::uniform::Mat4 posToNdc { "posToNdc" };
                    gl::uniform::Mat4 texTransform { "texTransform" };

                    void attach()
                    {
                        gl::Program::attachShader(gl::Shader(gl::Shader::Type::vertex, vertexShaderCode));
                    }
            };

            class SimpleShader : public TextureVertexShader
            {
                static constexpr std::string_view fragmentCode =
                    "#version 330 core\n"

                    "in vec2 texCoord;"
                    "out vec4 fragColor;"
                    "uniform sampler2D tex;"

                    "void main() {"
                    "    fragColor = texture(tex, texCoord);"
                    "};";

            public:
                gl::uniform::Sampler2D tex { "tex" };

                void load() {
                    gl::Program::create();
                    TextureVertexShader::attach();
                    gl::Program::attachShader(gl::Shader(gl::Shader::Type::fragment, fragmentCode));
                    gl::Program::link();
                    gl::Program::use();
                    gl::Program::findUniforms(posToNdc, texTransform, tex);
                    posToNdc.loadIdentity();
                    texTransform.loadIdentity();
                }
            };

            class SolidColorShader : public gl::Program
            {
                static constexpr std::string_view vertexCode =
                    "#version 330 core\n"

                    "layout (location = 0) in vec2 pos;"
                    
                    "uniform mat4 posToNdc;"

                    "void main() {"
                    "    gl_Position = posToNdc * vec4(pos, 0.0, 1.0);"
                    "};";

                static constexpr std::string_view fragmentCode =
                    "#version 330 core\n"

                    "out vec4 fragColorOut;"
                    "uniform vec4 solidColor;"

                    "void main() {"
                    "    fragColorOut = solidColor;"
                    "};";
            public:

                gl::uniform::Mat4 posToNdc { "posToNdc" };
                gl::uniform::Color solidColor { "solidColor" };

                void load() {
                    gl::Program::create();
                    gl::Program::attachShader(gl::Shader(gl::Shader::Type::vertex, vertexCode));
                    gl::Program::attachShader(gl::Shader(gl::Shader::Type::fragment, fragmentCode));
                    gl::Program::link();
                    gl::Program::use();
                    gl::Program::findUniforms(posToNdc, solidColor);
                    posToNdc.loadIdentity();
                    solidColor.setColor(0xFFFFFFFF);
                }
            };

            class SimplePaletteShader : public TextureVertexShader
            {
                static constexpr std::string_view fragmentCode =
                    "#version 330 core\n"

                    "in vec2 texCoord;"
                    "out vec4 fragColor;"
                    "uniform usampler2D tex;"
                    "uniform sampler2D pal;"

                    "void main() {"
                    "    uint palIndex = texture(tex, texCoord).r;"
                    "    if ( palIndex == uint(0) ) discard;"
                    "    fragColor = vec4(texture(pal, vec2(palIndex/256., 0.)).rgb, 1.0);"
                    "};";

            public:
                gl::uniform::Sampler2D tex { "tex" };
                gl::uniform::Sampler2D pal { "pal" };

                void load() {
                    gl::Program::create();
                    TextureVertexShader::attach();
                    gl::Program::attachShader(gl::Shader(gl::Shader::Type::fragment, fragmentCode));
                    gl::Program::link();
                    gl::Program::use();
                    gl::Program::findUniforms(posToNdc, texTransform, tex, pal);
                    posToNdc.loadIdentity();
                    texTransform.loadIdentity();
                }
            };

            class EffectMask : public TextureVertexShader
            {
                static inline const std::string filePath = "ShadersGLSL\\effect_mask.glsl";

            public:
                gl::uniform::Sampler2D spriteTex { "spriteTex" };
                gl::uniform::Sampler2D sampleTex { "sampleTex" };

                void load(ArchiveCluster & archiveCluster) {
                    gl::Program::create();
                    TextureVertexShader::attach();
                    gl::Program::attachShader(std::move(Shaders::fragmentShaderFromDatFile(archiveCluster, filePath, ensureVersioned)));
                    gl::Program::link();
                    gl::Program::use();
                    gl::Program::findUniforms(posToNdc, texTransform, spriteTex, sampleTex);
                    posToNdc.loadIdentity();
                    texTransform.loadIdentity();
                }
            };

            class SpriteShader : public gl::Program
            {
                static constexpr std::string_view vertexShaderCode =
                    "#version 330 core\n"

                    "layout (location = 0) in vec2 normalPos;" // 0 or 1 if this is left/top or right/bottom respectively
                    "layout (location = 1) in vec2 normalTex;" // 0 or 1 if this is left/top or right/bottom respectively

                    "out vec2 texCoord;"
                    
                    "uniform mat4 posToNdc;"
                    "uniform float imageScale;"
                    "uniform vec2 image[6];" // centerPos, animSize, animTexScale, frameSize, framePosOffset, frameTexOffset

                    "void main() {"
                    "    vec2 centerPos = image[0];"
                    "    vec2 animSize = image[1];"
                    "    vec2 animTexScale = image[2];"
                    "    vec2 frameSize = image[3];"
                    "    vec2 framePosOffset = image[4];"
                    "    vec2 frameTexOffset = image[5];"

                    "    vec2 imageOriginOffset = -animSize/2.0 + framePosOffset;"
                    "    gl_Position = posToNdc * vec4(centerPos + imageScale*(imageOriginOffset + normalPos*frameSize), 0.0, 1.0);"
                    "    texCoord = animTexScale * (frameTexOffset + normalTex*frameSize);"
                    "};";

                static inline const std::string filePath = "ShadersGLSL\\sprite_frag.glsl";

            public:
                gl::uniform::Mat4 posToNdc { "posToNdc" };
                gl::uniform::Float imageScale { "imageScale" };
                gl::uniform::Vec2Array image { "image" };
                gl::uniform::Sampler2D spriteTex { "spriteTex" };
                gl::uniform::Sampler2D teamColorTex { "teamcolorTex" };
                gl::uniform::Color multiplyColor { "multiplyColor" };
                gl::uniform::Color teamColor { "teamColor" };
                gl::uniform::FloatV4 hallucinate { "data" };

                template <typename Preprocessor>
                void load(ArchiveCluster & archiveCluster, Preprocessor && preprocessor) {
                    gl::Program::create();
                    gl::Program::attachShader(gl::Shader(gl::Shader::Type::vertex, vertexShaderCode));
                    gl::Program::attachShader(std::move(fragmentShaderFromDatFile(archiveCluster, filePath, std::forward<Preprocessor>(preprocessor))));
                    gl::Program::link();
                    gl::Program::use();
                    gl::Program::findUniforms(posToNdc, imageScale, image, spriteTex, teamColorTex, hallucinate, multiplyColor, teamColor);
                    posToNdc.loadIdentity();
                }
            };

            class TileShader : public TextureVertexShader
            {
                static inline const std::string filePath = "ShadersGLSL\\textured_frag.glsl";

            public:
                gl::uniform::Sampler2D spriteTex { "spriteTex" };
                gl::uniform::Color multiplyColor { "multiplyColor" };

                template <typename Preprocessor>
                void load(ArchiveCluster & archiveCluster, Preprocessor && preprocessor) {
                    gl::Program::create();
                    TextureVertexShader::attach();
                    gl::Program::attachShader(std::move(fragmentShaderFromDatFile(archiveCluster, filePath, std::forward<Preprocessor>(preprocessor))));
                    gl::Program::link();
                    gl::Program::use();
                    gl::Program::findUniforms(posToNdc, texTransform, spriteTex, multiplyColor);
                    posToNdc.loadIdentity();
                    texTransform.loadIdentity();
                }
            };

            class PaletteShader : public TextureVertexShader
            {
                static inline const std::string filePath = "ShadersGLSL\\palette_color_frag.glsl";

            public:
                gl::uniform::Sampler2D spriteTex { "spriteTex" };
                gl::uniform::Sampler2D sampleTex { "sampleTex" };
                gl::uniform::Color multiplyColor { "multiplyColor" };

                template <typename Preprocessor>
                void load(ArchiveCluster & archiveCluster, Preprocessor && preprocessor) {
                    gl::Program::create();
                    TextureVertexShader::attach();
                    gl::Program::attachShader(std::move(fragmentShaderFromDatFile(archiveCluster, filePath, std::forward<Preprocessor>(preprocessor))));
                    gl::Program::link();
                    gl::Program::use();
                    gl::Program::findUniforms(posToNdc, texTransform, spriteTex, sampleTex, multiplyColor);
                    posToNdc.loadIdentity();
                    texTransform.loadIdentity();
                }

            };
            
            class WaterShader : public gl::Program
            {
                static inline const std::string filePath = "ShadersGLSL\\water.glsl";

            public:
                gl::uniform::Mat4 posToNdc { "posToNdc" };
                gl::uniform::Mat4 texTransform { "texTransform" };
                gl::uniform::Sampler2D spriteTex { "spriteTex" };
                gl::uniform::Sampler2D sampleTex { "sampleTex" };
                gl::uniform::Sampler2D sampleTex2 { "sampleTex2" };
                gl::uniform::Sampler2D sampleTex3 { "sampleTex3" };
                gl::uniform::Sampler2D sampleTex4 { "sampleTex4" };
                struct : gl::uniform::Vec4 {
                    using gl::uniform::Vec4::Vec4;

                    void set(GLfloat gameX, GLfloat textureFade, GLfloat time) {
                        gl::uniform::Vec4::setVec4(gameX, textureFade, time, 0.0f);
                    }
                } data { "data" };

                template <typename Preprocessor>
                void load(ArchiveCluster & archiveCluster, Preprocessor && preprocessor)
                {
                    gl::Program::create();
                    std::string_view vertexShaderCode =
                        "#version 330 core\n"

                        "layout (location = 0) in vec2 pos;"
                        "layout (location = 1) in vec2 texCoordIn;"
                        "layout (location = 2) in vec2 mapCoordIn;"
                        "layout (location = 3) in vec2 mapCoord2In;"
                        
                        "out vec2 texCoord;"
                        "out vec2 mapCoord;"
                        "out vec2 mapCoord2;"
                        
                        "uniform mat4 posToNdc;"
                        "uniform mat4 texTransform;"

                        "void main() {"
                        "    gl_Position = posToNdc * vec4(pos, 0.0, 1.0);"
                        "    mapCoord = mapCoordIn;"
                        "    mapCoord2 = mapCoord2In;"
                        "    texCoord = vec4(texTransform * vec4(texCoordIn, 0.0, 1.0)).xy;"
                        "};";
                    gl::Program::attachShader(gl::Shader(gl::Shader::Type::vertex, vertexShaderCode));
                    gl::Program::attachShader(std::move(fragmentShaderFromDatFile(archiveCluster, filePath, std::forward<Preprocessor>(preprocessor))));
                    gl::Program::link();
                    gl::Program::use();
                    gl::Program::findUniforms(posToNdc, texTransform, spriteTex, sampleTex, sampleTex2, sampleTex3, sampleTex4, data);
                    posToNdc.loadIdentity();
                    texTransform.loadIdentity();
                }
            };

            class HeatShader : public gl::Program
            {
                static inline const std::string filePath = "ShadersGLSL\\heat_distortion.glsl";
                gl::uniform::Vec2 invResolution { "invResolution" };
                gl::uniform::FloatV4_3 timeShift { "data" };
                gl::uniform::Sampler2D spriteTex { "spriteTex" };
                gl::uniform::Sampler2D sampleTex { "sampleTex" };

            public:
                template <typename Preprocessor>
                void load(ArchiveCluster & archiveCluster, Preprocessor && preprocessor)
                {
                    gl::Program::create();
                    gl::Program::attachShader(std::move(fragmentShaderFromDatFile(archiveCluster, filePath, std::forward<Preprocessor>(preprocessor))));
                    gl::Program::link();
                    gl::Program::use();
                    gl::Program::findUniforms(invResolution, timeShift, spriteTex, sampleTex);
                }
            };

            SpriteShader spriteShader {}; // Remastered
            TileShader tileShader {}; // Remastered
            PaletteShader paletteShader {}; // Remastered
            WaterShader waterShader {}; // Remastered
            HeatShader heatShader {}; // Remastered
            SimpleShader simpleShader {}; // Remastered
            SolidColorShader solidColorShader {}; // Remastered & Classic
            SimplePaletteShader simplePaletteShader {}; // Classic
            
            void loadClassic();
            void load(ArchiveCluster & archiveCluster);
        };

        struct SkinData
        {
            int id = 0;
            std::vector<int> imageList;
            std::string name;
            std::vector<int> unitList;

            REFLECT(SkinData, id, imageList, name, unitList)
        };

        static void loadSkin(Skin & skin, ArchiveCluster & archiveCluster, VisualQuality visualQuality, ByteBuffer & fileData);

        struct DDS_HEADER
        { // DDS code adapted from https://gist.github.com/tilkinsc/13191c0c1e5d6b25fbe79bbd2288a673#file-load_dds-c-L44
            static constexpr u32 DDS  = 0x20534444; // "DDS "
            static constexpr u32 DXT1 = 0x31545844; // "DXT1"
            static constexpr u32 DXT3 = 0x33545844; // "DXT3"
            static constexpr u32 DXT5 = 0x35545844; // "DXT5"

            u32 fileType = 0;
            u32 size = 0;
            u32 flags = 0;
            u32 height = 0;
            u32 width = 0;
            u32 pitchOrLinearSize = 0;
            u32 depth = 0;
            u32 mipMapCount = 0;
            u32 reserved[11] {};
            struct DDS_PIXELFORMAT
            {
                u32 size = 0;
                u32 flags = 0;
                u32 fourCharacterCode = 0;
                u32 rgbBitCount = 0;
                u32 rBitMask = 0;
                u32 gBitMask = 0;
                u32 bBitMask = 0;
                u32 aBitMask = 0;
            } pixelFormat {};
            u32 caps = 0;
            u32 caps1 = 0;
            u32 caps2 = 0;
            u32 caps3 = 0;
            u32 reserved1 = 0;

            struct Format
            {
                GLenum id = 0;
                size_t blockSize = 0;
            };

            Format getFormat();
        };

        static gl::Texture loadDdsTexture(u8* data, gl::ContextSemaphore* contextSemaphore = nullptr);

        static void loadDdsTextureSubImage(u8* data, gl::Texture & texture, GLint xOffset, GLint yOffset, GLsizei width, GLsizei height);

        static gl::Texture loadBmpLuminanceTexture(u8* data, GLsizei width, GLsizei height, gl::ContextSemaphore* contextSemaphore = nullptr);

        static std::shared_ptr<Animation> loadSdAnim(Sc::Data & scData, u8* data, u32 index, gl::ContextSemaphore* contextSemaphore = nullptr);

        static std::shared_ptr<Animation> loadHdAnim(bool halfAnim, ArchiveCluster & archiveCluster, const std::filesystem::path & path, ByteBuffer & fileData, gl::ContextSemaphore* contextSemaphore = nullptr);

        static bool loadGrp(ArchiveCluster & archiveCluster, const std::filesystem::path & path, Grp & grp, ByteBuffer & fileData, bool framedTex, bool mergedTex);

        static SpkGrp loadStarGrp(ArchiveCluster & archiveCluster, const std::filesystem::path & path, ByteBuffer & fileData);

        static void loadMaskedTiles(ArchiveCluster & archiveCluster, Sc::Terrain::Tileset tileset, Grp & tilesetGrp, Grp & tileMasks, std::vector<u16> & maskIds);

        static void loadTileMasks(ArchiveCluster & archiveCluster, std::filesystem::path path, std::vector<u16> & maskIds);

        struct RenderSettings
        {
            VisualQuality visualQuality = VisualQuality::SD;
            Scr::Skin::Id skinId = Scr::Skin::Id::Classic;
            Sc::Terrain::Tileset tileset = Sc::Terrain::Tileset::Badlands;
            bool forceShowStars = false;

            constexpr bool showStars() const { return forceShowStars || tileset == Sc::Terrain::Tileset::SpacePlatform; }
        };

        struct Data // Data for a particular visual quality
        {
            struct Skin // Data for a particular skin and visual quality
            {
                struct Tileset // Data for a particular tileset, skin, and visual quality
                {
                    Grp tileMask {};
                    Grp tilesetGrp {};
                    std::vector<u16> maskIds {};

                    void load(ArchiveCluster & archiveCluster, const RenderSettings & renderSettings, ByteBuffer & fileData);
                };
                std::shared_ptr<SpkData> spk {};
                std::shared_ptr<Tileset> tiles[Sc::Terrain::NumTilesets] {};
                std::shared_ptr<std::vector<std::shared_ptr<Animation>>> images {}; // 999 images
                std::shared_ptr<std::vector<std::shared_ptr<Scr::ClassicGrp>>> classicImages {}; // 999 images

                void loadClassicStars(Sc::Data & scData);
                void loadStars(ArchiveCluster & archiveCluster, std::filesystem::path texPrefix, ByteBuffer & fileData);
                
                void loadClassicTiles(Sc::Data & scData, const RenderSettings & renderSettings);
                void loadTiles(ArchiveCluster & archiveCluster, const RenderSettings & renderSettings, ByteBuffer & fileData);
                
                void loadClassicImages(Sc::Data & scData);
                void loadImages(Sc::Data & scData, ArchiveCluster & archiveCluster, std::filesystem::path texPrefix, const RenderSettings & renderSettings, ByteBuffer & fileData, gl::ContextSemaphore* contextSemaphore = nullptr);
            };
            std::shared_ptr<Skin> skin[Scr::Skin::total] {};
            std::shared_ptr<Grp> waterNormal[2] {};

            void loadWaterNormals(ArchiveCluster & archiveCluster, std::filesystem::path texPrefix, ByteBuffer & fileData);
        };
        gl::ContextSemaphore* openGlContextSemaphore = nullptr;
        std::unique_ptr<gl::Font> defaultFont = nullptr;
        std::shared_ptr<Shaders> shaders {};
        std::shared_ptr<Data> visualQualityData[Scr::VisualQuality::total] {};

        struct RenderData // Data required for rendering a given map with a given visual quality, skin, and tileset
        {
            std::shared_ptr<Shaders> shaders = nullptr;
            std::shared_ptr<SpkData> spk = nullptr;
            std::shared_ptr<Data::Skin::Tileset> tiles = nullptr;
            std::shared_ptr<std::vector<std::shared_ptr<Animation>>> images = nullptr; // 999 images
            std::shared_ptr<std::vector<std::shared_ptr<Scr::ClassicGrp>>> classicImages = nullptr; // 999 images
            std::shared_ptr<Grp> waterNormal[2] { nullptr, nullptr };
        };

        void unload(const RenderSettings & renderSettings);

        // Checks whether load requires potential disk accesses, does not perform exaustive validation
        bool isLoaded(const RenderSettings & renderSettings);

        std::shared_ptr<Scr::GraphicsData::RenderData> load(Sc::Data & scData, ArchiveCluster & archiveCluster, const RenderSettings & renderSettings, ByteBuffer & fileData);
    };

    class MapGraphics
    {
        gl::Size2D<s32> imageMargin {};
        gl::Rect2D<s32> windowBounds {};
        gl::Size2D<s32> windowDimensions {};
        gl::Size2D<s32> mapViewDimensions {};
        gl::Size2D<s32> starDimensions {};
        gl::Rect2D<s32> mapViewBounds {};
        gl::Rect2D<s32> mapTileBounds {};
        gl::Rect2D<s32> imageClipBoundingBox {};
        gl::VertexVector<> gridVertices {};
        glm::mat4 unscrolledWindowToNdc {};
        glm::mat4 gameToNdc {};
        glm::mat4 starToNdc {};
        glm::mat4 tileToNdc {};
        glm::mat4 tileToTex {};
        glm::mat4 gridToNdc {};
        glm::mat2 glyphScaling {};
        GLfloat zoom = 1.f;
        Chk::PlayerColor prevMappedColor = std::numeric_limits<Chk::PlayerColor>::max();

        u32 n1Frame = 0;
        u32 n2Frame = 0;
        u32 nIncrement = 0;
        u64 initialTickCount = 0;

        MapFile & mapFile;
        gl::Font* textFont = nullptr;
        Scr::GraphicsData::RenderSettings renderSettings {};
        std::shared_ptr<Scr::GraphicsData::RenderData> renderDat = nullptr;
        ColorCycler colorCycler {};
        uint32_t gridColor = 0xFF000000; // 0xAABBGGRR
        s32 gridSize = 0;
        bool fpsEnabled = true;
        gl::Fps fps {};
        gl::VertexVector<> starVertices {};
        gl::VertexVector<> tileVertices {};
        gl::VertexArray<6*8> waterVertices {}; // 6 verticies forming the two triangles for a quad, 8 elements per vertex (pos.xy, tex.xy, map.xy, map2.xy)
        gl::VertexArray<6*4> animVertices {}; // 6 verticies forming the two triangles for a quad, 4 elements per vertex (posX, posY, texX, texY)
        gl::VertexVector<> lineVertices {};
        gl::VertexVector<> triangleVertices {};
        gl::VertexVector<> triangleVertices2 {};
        gl::VertexVector<> triangleVertices3 {};
        gl::VertexVector<> triangleVertices4 {};
        gl::VertexVector<> triangleVertices5 {};
        gl::VertexVector<> triangleVertices6 {};

    public:
        MapGraphics(MapFile & mapFile);

        void updateGrid(); // Occurs when the map view, grid size or grid color changes
        void mapViewChanged(); // Occurs when the window bounds, zoom-level or skin changes
        void windowBoundsChanged(gl::Rect2D<s32> windowBounds);
        void skinChanged();
        GLfloat getZoom();
        void setZoom(GLfloat newZoom);
        
        bool isClassicLoaded(Scr::GraphicsData & scrDat);

        void initVertices();

        void setFont(gl::Font* textFont);

        void setGridColor(uint32_t gridColor);
        void setGridSize(s32 gridSize);
        
        bool displayingFps();
        void toggleDisplayFps();

        void load(Sc::Data & scData, Scr::GraphicsData & scrDat, const Scr::GraphicsData::RenderSettings & renderSettings, ArchiveCluster & archiveCluster, ByteBuffer & fileData);

        void drawTestTex(gl::Texture & tex);

        void drawGrid();
        void drawClassicStars(Sc::Data & scData);
        void drawStars(u32 multiplyColor);
        void drawTileNums(Sc::Data & scData);
        void drawTileOverlays(Sc::Data & scData);
        void drawTileVertices(Scr::Grp & tilesetGrp, s32 width, s32 height, const glm::mat4x4 & positionTransformation);
        void drawTerrain(Sc::Data & scData);
        void drawTilesetIndexed(Sc::Data & scData, s32 left, s32 top, s32 width, s32 height, s32 scrollY);
        void prepareImageRendering();
        void drawImage(Scr::Animation & animation, s32 x, s32 y, u32 frame, u32 multiplyColor, u32 playerColor, bool hallucinate);
        void drawClassicImage(Sc::Data & scData, gl::Palette & palette, s32 x, s32 y, u32 imageId, Chk::PlayerColor color);
        void drawSprites(Sc::Data & scData);
        void drawLocations(bool clipLocationNames, bool showAnywhere, size_t selectedLocation);
        void drawFps();

        void render(Sc::Data & scData, bool renderLocations, bool renderTileElevations, bool renderTileNums, bool clipLocationNames, bool showAnywhere, size_t selectedLocation);

        void updateGraphics(u64 ticks); // Runs every few ms, with ticks being the ms since the last frame
    };
}

#endif