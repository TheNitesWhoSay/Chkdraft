#pragma once
#include <ft2build.h>
#include <gl/base.h>
#include <gl/program.h>
#include <gl/uniform.h>
#include <gl/vertices.h>
#include <hb-ft.h>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string_view>
#include <vector>
#include FT_FREETYPE_H
#include FT_GLYPH_H

namespace gl
{
    enum Align
    {
        Left,
        Right,
        Center
    };

    struct Font
    {
        struct Memory
        {
            std::vector<std::uint8_t> data;
            Memory(size_t size) : data(size, std::uint8_t(0)) {}
        };

        std::shared_ptr<Memory> fontMemory = nullptr; // Fonts from memory must stay in scope for the life of the font

    private:
        struct FtLibrary
        {
            FT_Library library;
            
            FtLibrary() : library(nullptr) {}
            
            ~FtLibrary()
            {
                if ( library != nullptr )
                    FT_Done_FreeType(library);
            }

            void init() // returns true if the freetype library wasn't previously initialized
            {
                if ( library == nullptr )
                {
                    if ( FT_Init_FreeType(&library) )
                        throw std::runtime_error("Failed to initialize FreeType!");
                }
            }
        };

        struct TextShader : gl::Program
        {
            static constexpr std::string_view vertexCode = "#version 330 core\n"
                "layout (location = 0) in vec2 glyphPos;\n"
                "layout (location = 1) in vec2 texPos;\n"
                "out vec2 TexCoords;\n"
                
                "uniform mat2 glyphScaling;\n"
                "uniform mat4 textPosToNdc;\n"
                "uniform vec2 textOrigin;\n"

                "void main()\n"
                "{\n"
                    "gl_Position = vec4(vec4(textPosToNdc * vec4(textOrigin, 0.0, 1.0)).xy + (glyphScaling * glyphPos), 0.0, 1.0);\n"
                    "TexCoords = texPos.xy;\n"
                "}";

            static constexpr std::string_view fragmentCode = "#version 330 core\n"
                "in vec2 TexCoords;\n"
                "out vec4 color;\n"

                "uniform sampler2D tex;\n"
                "uniform vec4 textColor;\n"

                "void main()\n"
                "{\n"
                    "if ( texture(tex, TexCoords).r == 0.0 )\n"
                        "discard;\n"
                    "vec4 sampled = vec4(1.0, 1.0, 1.0, texture(tex, TexCoords).r);\n"
                    "color = textColor * sampled;\n"
                "}";
            
            gl::uniform::Mat2 glyphScaling;
            gl::uniform::Mat4 textPosToNdc;
            gl::uniform::Vec2 textPos;
            gl::uniform::Int tex;
            gl::uniform::Vec4 textColor;
            
            TextShader()
                : glyphScaling{"glyphScaling"}, textPosToNdc{"textPosToNdc"}, textPos{"textOrigin"}, tex{"tex"}, textColor{"textColor"}
            {}
            
            void load()
            {
                gl::Program::create();
                gl::Program::attachShader(gl::Shader(gl::Shader::Type::vertex, vertexCode));
                gl::Program::attachShader(gl::Shader(gl::Shader::Type::fragment, fragmentCode));
                gl::Program::link();
                gl::Program::use();
                gl::Program::findUniforms(glyphScaling, textPosToNdc, textPos, tex, textColor);
                this->glyphScaling.loadIdentity();
                this->textPosToNdc.loadIdentity();
                this->textPos.setVec2(0.f, 0.f);
                this->textColor.setVec4(0.f, 0.f, 0.f, 1.f);
            }

            void setColor(GLfloat red, GLfloat green, GLfloat blue)
            {
                this->textColor.setVec4(red, green, blue, 1.f);
            }
        };

        struct ClippedTextShader : gl::Program
        {
            static constexpr std::string_view vertexCode = "#version 330 core\n"
                "layout (location = 0) in vec2 glyphPos;\n"
                "layout (location = 1) in vec2 texPos;\n"
                "out vec2 Pos;\n"
                "out vec2 TexCoords;\n"
                
                "uniform mat2 glyphScaling;\n"
                "uniform mat4 textPosToNdc;\n"
                "uniform vec2 textOrigin;\n"

                "void main()\n"
                "{\n"
                    "gl_Position = vec4(vec4(textPosToNdc * vec4(textOrigin, 0.0, 1.0)).xy + (glyphScaling * glyphPos), 0.0, 1.0);\n"
                    "Pos = gl_Position.xy;\n"
                    "TexCoords = texPos.xy;\n"
                "}";

            static constexpr std::string_view fragmentCode = "#version 330 core\n"
                "in vec2 Pos;\n"
                "in vec2 TexCoords;\n"
                "out vec4 color;\n"

                "uniform sampler2D tex;\n"
                "uniform vec4 textColor;\n"
                "uniform mat4 textPosToNdc;\n"
                "uniform vec2 lowerRightBound;\n"

                "void main()\n"
                "{\n"
                    "vec4 ndcUpperRightBound = textPosToNdc * vec4(lowerRightBound, 0.0, 1.0);\n"
                    "if ( texture(tex, TexCoords).r == 0.0 || Pos.x > ndcUpperRightBound.x || Pos.y < ndcUpperRightBound.y )\n"
                    "    discard;\n"
                    "vec4 sampled = vec4(1.0, 1.0, 1.0, texture(tex, TexCoords).r);\n"
                    "color = textColor * sampled;\n"
                "}";
            
            gl::uniform::Mat2 glyphScaling;
            gl::uniform::Mat4 textPosToNdc;
            gl::uniform::Vec2 textPos;
            gl::uniform::Int tex;
            gl::uniform::Vec4 textColor;
            gl::uniform::Vec2 lowerRightBound;
            
            ClippedTextShader()
                : glyphScaling{"glyphScaling"}, textPosToNdc{"textPosToNdc"}, textPos{"textOrigin"}, tex{"tex"}, textColor{"textColor"},
                lowerRightBound {"lowerRightBound"}
            {}
            
            void load()
            {
                gl::Program::create();
                gl::Program::attachShader(gl::Shader(gl::Shader::Type::vertex, vertexCode));
                gl::Program::attachShader(gl::Shader(gl::Shader::Type::fragment, fragmentCode));
                gl::Program::link();
                gl::Program::use();
                gl::Program::findUniforms(glyphScaling, textPosToNdc, textPos, tex, textColor, lowerRightBound);
                this->glyphScaling.loadIdentity();
                this->textPosToNdc.loadIdentity();
                this->textPos.setVec2(0.f, 0.f);
                this->textColor.setVec4(0.f, 0.f, 0.f, 1.f);
            }

            void setColor(GLfloat red, GLfloat green, GLfloat blue)
            {
                this->textColor.setVec4(red, green, blue, 1.f);
            }
        };


        static inline FtLibrary ftLibrary {};
        static inline gl::VertexVector<> textVertices {};

        FT_Face ftFontFace = nullptr;
        hb_font_t* hbFont = nullptr;

        static void initTextRendering()
        {
            if ( ftLibrary.library == nullptr ) // Run first time initializing only
            {
                ftLibrary.init();

                textShader.load();
                clippedTextShader.load();

                textVertices.initialize({
                    gl::VertexAttribute{.size = 2}, // Position.xy
                    gl::VertexAttribute{.size = 2}  // TexCoord.xy
                });

                textVertices.vertices.assign(6*4, 0.f);
                textVertices.bind();
                textVertices.bufferData(gl::UsageHint::DynamicDraw);
            }
        }

        std::unordered_map<hb_codepoint_t, std::unique_ptr<gl::Texture>> glyphCache {};

        struct RenderGlyph
        {
            gl::Texture* tex = nullptr;
            gl::Rect2D<GLfloat> rc {};
        };

        struct PrepareText
        {
            std::vector<RenderGlyph> renderGlyphs {};
            gl::Size2D<GLfloat> dimensions {};
            gl::Point2D<GLfloat> offset {};

            PrepareText(const std::string & utf8Text, bool renderTextures, FT_Face ftFontFace, hb_font_t* hbFont,
                std::unordered_map<hb_codepoint_t, std::unique_ptr<gl::Texture>> & glyphCache)
            {
                GLfloat x = 0.f;
                GLfloat y = 0.f;

                hb_buffer_t* buf = hb_buffer_create();
                hb_buffer_add_utf8(buf, utf8Text.c_str(), utf8Text.size(), 0, -1);
                hb_buffer_set_direction(buf, HB_DIRECTION_LTR);
                hb_buffer_set_script(buf, HB_SCRIPT_LATIN);
                hb_buffer_set_language(buf, hb_language_from_string("en", -1));
            
                hb_shape(hbFont, buf, NULL, 0);
                unsigned int glyphCount = 0;
                hb_glyph_info_t* glyphInfo = hb_buffer_get_glyph_infos(buf, &glyphCount);
                hb_glyph_position_t* glyphPos = hb_buffer_get_glyph_positions(buf, &glyphCount);

                renderGlyphs.reserve(glyphCount);
                gl::Rect2D<GLfloat> boundingBox {
                    .left = std::numeric_limits<GLfloat>::max(),
                    .top = std::numeric_limits<GLfloat>::max(),
                    .right = std::numeric_limits<GLfloat>::min(),
                    .bottom = std::numeric_limits<GLfloat>::min()
                };

                GLint defaultAlignment = 0;
                glGetIntegerv(GL_UNPACK_ALIGNMENT, &defaultAlignment);
                glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
                for ( unsigned int i=0; i<glyphCount; ++i )
                {
                    auto found = glyphCache.find(glyphInfo[i].codepoint);
                    FT_Load_Glyph(ftFontFace, glyphInfo[i].codepoint, found == glyphCache.end() ? FT_LOAD_RENDER : FT_LOAD_DEFAULT);
                    auto glyph = ftFontFace->glyph;
                
                    GLfloat width = glyph->bitmap.width;
                    GLfloat height = glyph->bitmap.rows;
                    GLfloat left = x + ((glyphPos[i].x_offset >> 6) + glyph->bitmap_left);
                    GLfloat top = y + ((glyphPos[i].y_offset >> 6) - glyph->bitmap_top);
                    GLfloat right = left + width;
                    GLfloat bottom = top + height;
                    if ( width == 0 ) // Presumed to be spacing, for which advance must be used to determine bounds
                    {
                        if ( left < boundingBox.left )
                            boundingBox.left = left;
                        if ( top < boundingBox.top )
                            boundingBox.top = top;
                        if ( right+(glyphPos[i].x_advance >> 6) > boundingBox.right )
                            boundingBox.right = right+(glyphPos[i].x_advance >> 6);
                        if ( bottom+(glyphPos[i].y_advance >> 6) > boundingBox.bottom )
                            boundingBox.bottom = bottom+(glyphPos[i].y_advance >> 6);
                    }
                    else
                    {
                        if ( left < boundingBox.left )
                            boundingBox.left = left;
                        if ( top < boundingBox.top )
                            boundingBox.top = top;
                        if ( right > boundingBox.right )
                            boundingBox.right = right;
                        if ( bottom > boundingBox.bottom )
                            boundingBox.bottom = bottom;
                    }

                    if ( renderTextures )
                    {
                        if ( found != glyphCache.end() )
                        {
                            renderGlyphs.emplace_back(found->second.get(), gl::Rect2D<GLfloat>{left, top, right, bottom});
                        }
                        else
                        {
                            auto inserted = glyphCache.emplace(glyphInfo[i].codepoint, std::make_unique<gl::Texture>());
                            gl::Texture* glyphTex = inserted.first->second.get();
                            renderGlyphs.emplace_back(glyphTex, gl::Rect2D<GLfloat>{left, top, right, bottom});
                            glyphTex->genTexture();
                            glyphTex->bind();
                            glyphTex->loadImage2D(gl::Texture::Image2D{
                                .data = glyph->bitmap.buffer,
                                .width = GLsizei(width),
                                .height = GLsizei(height),
                                .internalformat = GL_RED,
                                .format = GL_RED
                            });
                            glyphTex->setTextureWrap(GL_CLAMP_TO_EDGE);
                            glyphTex->setMinMagFilters(GL_LINEAR);
                        }
                    }

                    x += (glyphPos[i].x_advance >> 6);
                    y += (glyphPos[i].y_advance >> 6);
                }
                glPixelStorei(GL_UNPACK_ALIGNMENT, defaultAlignment);

                dimensions.width = boundingBox.right - boundingBox.left;
                dimensions.height = boundingBox.bottom - boundingBox.top;
                offset.x = boundingBox.left;
                offset.y = boundingBox.top;
                hb_buffer_destroy(buf);
            }
        };

        struct RenderLine
        {
            std::vector<RenderGlyph> renderGlyphs {};
            gl::Size2D<GLfloat> dimensions {};
            gl::Point2D<GLfloat> offset {};
        };

        struct PrepareClippedText
        {
            std::vector<RenderLine> renderLines {};

            PrepareClippedText(const std::string & utf8Text, bool renderTextures, FT_Face ftFontFace, hb_font_t* hbFont,
                std::unordered_map<hb_codepoint_t, std::unique_ptr<gl::Texture>> & glyphCache, int32_t clipWidth, int32_t clipHeight)
            {
                std::vector<std::vector<std::string_view>> textLines {};
                std::string_view remainingText = utf8Text;
                auto newLinePos = remainingText.find_first_of('\n');
                while ( newLinePos != std::string_view::npos )
                {
                    if ( newLinePos > 0 )
                    {
                        auto & textLineClusters = textLines.emplace_back();
                        std::string_view textLine = remainingText.substr(0, newLinePos);
                        auto clusterStart = 0;
                        auto textLineSize = textLine.size();
                        size_t i=0;
                        for ( ; i<textLineSize; ++i )
                        {
                            if ( std::isspace(textLine[i]) )
                            {
                                for ( ; i<textLineSize && std::isspace(textLine[i]); ++i );
                                textLineClusters.emplace_back(textLine.substr(clusterStart, i-clusterStart));
                                if ( textLineClusters.back().back() == '\r' )
                                    textLineClusters.back().remove_suffix(1);
                                clusterStart = i;
                            }
                        }

                        if ( clusterStart < textLineSize )
                            textLineClusters.emplace_back(textLine.substr(clusterStart, i-clusterStart));
                    }
                    remainingText.remove_prefix(newLinePos+1);
                    newLinePos = remainingText.find_first_of('\n');
                }
                auto & textLineClusters = textLines.emplace_back();
                auto textLineSize = remainingText.size();
                auto clusterStart = 0;
                size_t i=0;
                for ( i=0; i<textLineSize; ++i )
                {
                    if ( std::isspace(remainingText[i]) )
                    {
                        for ( ; i<textLineSize && std::isspace(remainingText[i]); ++i );
                        textLineClusters.emplace_back(remainingText.substr(clusterStart, i-clusterStart));
                        clusterStart = i;
                    }
                }

                if ( clusterStart < textLineSize )
                    textLineClusters.emplace_back(remainingText.substr(clusterStart, i-clusterStart));

                GLint defaultAlignment = 0;
                glGetIntegerv(GL_UNPACK_ALIGNMENT, &defaultAlignment);
                glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
                for ( size_t lineIndex=0; lineIndex<textLines.size(); ++lineIndex )
                {
                    auto & renderLine = renderLines.emplace_back();
                    GLfloat x = 0.f;
                    GLfloat y = 0.f;

                    gl::Rect2D<GLfloat> boundingBox {
                        .left = std::numeric_limits<GLfloat>::max(),
                        .top = std::numeric_limits<GLfloat>::max(),
                        .right = std::numeric_limits<GLfloat>::min(),
                        .bottom = std::numeric_limits<GLfloat>::min()
                    };
                    size_t glyphCountPerPrevCluster = 0;
                    gl::Rect2D<GLfloat> boundingBoxPerPrevCluster {};

                    for ( size_t clusterIndex=0; clusterIndex<textLines[lineIndex].size(); ++clusterIndex )
                    {
                        std::vector<std::string_view> & textLine = textLines[lineIndex];
                        glyphCountPerPrevCluster = renderLine.renderGlyphs.size();
                        boundingBoxPerPrevCluster = boundingBox;
                        auto & cluster = textLine[clusterIndex];

                        hb_buffer_t* buf = hb_buffer_create();
                        hb_buffer_add_utf8(buf, cluster.data(), cluster.size(), 0, -1);
                        hb_buffer_set_direction(buf, HB_DIRECTION_LTR);
                        hb_buffer_set_script(buf, HB_SCRIPT_LATIN);
                        hb_buffer_set_language(buf, hb_language_from_string("en", -1));

                        hb_shape(hbFont, buf, NULL, 0);
                        unsigned int glyphCount = 0;
                        hb_glyph_info_t* glyphInfo = hb_buffer_get_glyph_infos(buf, &glyphCount);
                        hb_glyph_position_t* glyphPos = hb_buffer_get_glyph_positions(buf, &glyphCount);

                        renderLine.renderGlyphs.reserve(glyphCount);

                        for ( unsigned int i=0; i<glyphCount; ++i )
                        {
                            auto found = glyphCache.find(glyphInfo[i].codepoint);
                            FT_Load_Glyph(ftFontFace, glyphInfo[i].codepoint, found == glyphCache.end() ? FT_LOAD_RENDER : FT_LOAD_DEFAULT);
                            auto glyph = ftFontFace->glyph;
                
                            GLfloat width = glyph->bitmap.width;
                            GLfloat height = glyph->bitmap.rows;
                            GLfloat left = x + ((glyphPos[i].x_offset >> 6) + glyph->bitmap_left);
                            GLfloat top = y + ((glyphPos[i].y_offset >> 6) - glyph->bitmap_top);
                            GLfloat right = left + width;
                            GLfloat bottom = top + height;
                            if ( (width == 0 && std::max(boundingBox.right - left, right+(glyphPos[i].x_advance >> 6) - boundingBox.left) > clipWidth) ||
                                 (width > 0 && std::max(boundingBox.right - left, right - boundingBox.left) > clipWidth) ) // This glyph exceeds clipWidth
                            {
                                if ( clusterIndex == 0 )
                                {
                                    bool hasMoreClusters = textLine.size() > 1;
                                    bool hasMoreGlyphs = i+1 < cluster.size();
                                    bool isFirstGlyph = i == 0;
                                    bool isFirstAndOnlyGlyph = (isFirstGlyph && !hasMoreGlyphs);
                                    if ( hasMoreClusters || hasMoreGlyphs || !isFirstAndOnlyGlyph )
                                    {
                                        // Move this glyph and others in the cluster to the next line, exclude this glyph if it's the first on the line
                                        auto & nextLine = *textLines.insert(lineIndex == textLines.size()-1 ? textLines.end() : textLines.begin()+lineIndex+1,
                                            std::vector<std::string_view>{});
                                        auto & textLine = textLines[lineIndex];
                                        auto & firstCluster = nextLine.emplace_back(cluster);
                                        firstCluster.remove_prefix(isFirstGlyph ? i+1 : i);
                                        cluster.remove_suffix(isFirstGlyph ? cluster.size()-i-1 : cluster.size()-i);

                                        if ( hasMoreClusters )
                                        {
                                            nextLine.insert(nextLine.end(), textLine.begin()+1, textLine.end());
                                            textLine.erase(textLine.begin()+1, textLine.end());
                                        }
                                    }
                                }
                                else // This is not the first cluster on the line, move the entire cluster & remaining clusters (if any) to the next line
                                {
                                    textLines.insert(textLines.begin()+lineIndex+1, std::vector(textLine.begin()+clusterIndex, textLine.end()));
                                    auto & textLine = textLines[lineIndex];
                                    textLine.erase(textLine.begin()+clusterIndex, textLine.end());
                                    boundingBox = boundingBoxPerPrevCluster; // Since the current cluster is taken out, reset line size as it was last cluster
                                    if ( glyphCountPerPrevCluster < renderLine.renderGlyphs.size() )
                                        renderLine.renderGlyphs.erase(renderLine.renderGlyphs.begin()+glyphCountPerPrevCluster, renderLine.renderGlyphs.end());
                                }
                                break; // Finish processing this cluster, which is now the last on the line, so finish the line as well
                            }

                            if ( width == 0 ) // Presumed to be spacing, for which advance must be used to determine bounds
                            {
                                if ( left < boundingBox.left )
                                    boundingBox.left = left;
                                if ( top < boundingBox.top )
                                    boundingBox.top = top;
                                if ( right+(glyphPos[i].x_advance >> 6) > boundingBox.right )
                                    boundingBox.right = right+(glyphPos[i].x_advance >> 6);
                                if ( bottom+(glyphPos[i].y_advance >> 6) > boundingBox.bottom )
                                    boundingBox.bottom = bottom+(glyphPos[i].y_advance >> 6);
                            }
                            else
                            {
                                if ( left < boundingBox.left )
                                    boundingBox.left = left;
                                if ( top < boundingBox.top )
                                    boundingBox.top = top;
                                if ( right > boundingBox.right )
                                    boundingBox.right = right;
                                if ( bottom > boundingBox.bottom )
                                    boundingBox.bottom = bottom;
                            }

                            if ( renderTextures )
                            {
                                if ( found != glyphCache.end() )
                                {
                                    renderLine.renderGlyphs.emplace_back(found->second.get(), gl::Rect2D<GLfloat>{left, top, right, bottom});
                                }
                                else
                                {
                                    auto inserted = glyphCache.emplace(glyphInfo[i].codepoint, std::make_unique<gl::Texture>());
                                    gl::Texture* glyphTex = inserted.first->second.get();
                                    renderLine.renderGlyphs.emplace_back(glyphTex, gl::Rect2D<GLfloat>{left, top, right, bottom});
                                    glyphTex->genTexture();
                                    glyphTex->bind();
                                    glyphTex->loadImage2D(gl::Texture::Image2D{
                                        .data = glyph->bitmap.buffer,
                                        .width = GLsizei(width),
                                        .height = GLsizei(height),
                                        .internalformat = GL_RED,
                                        .format = GL_RED
                                    });
                                    glyphTex->setTextureWrap(GL_CLAMP_TO_EDGE);
                                    glyphTex->setMinMagFilters(GL_LINEAR);
                                }
                            }

                            x += (glyphPos[i].x_advance >> 6);
                            y += (glyphPos[i].y_advance >> 6);
                        }
                        hb_buffer_destroy(buf);
                    }
                    renderLine.dimensions.width = boundingBox.right - boundingBox.left;
                    renderLine.dimensions.height = boundingBox.bottom - boundingBox.top;
                    renderLine.offset.x = boundingBox.left;
                    renderLine.offset.y = boundingBox.top;
                }
                glPixelStorei(GL_UNPACK_ALIGNMENT, defaultAlignment);
            }
        };

        GLfloat red = 0.f;
        GLfloat green = 0.f;
        GLfloat blue = 0.f;

    public:
        static inline TextShader textShader {};
        static inline ClippedTextShader clippedTextShader {};

        void setColor(GLfloat red, GLfloat green, GLfloat blue)
        {
            this->red = red;
            this->green = green;
            this->blue = blue;
        }

        Font(FT_Face ftFontFace, hb_font_t* hbFont) : ftFontFace(ftFontFace), hbFont(hbFont) {}

        Font(FT_Face ftFontFace, hb_font_t* hbFont, std::shared_ptr<Memory> fontMemory)
            : ftFontFace(ftFontFace), hbFont(hbFont), fontMemory(fontMemory) {}

        ~Font() {}

        static std::unique_ptr<Font> load(const std::string & fontFilePath, FT_F26Dot6 width, FT_F26Dot6 height, FT_UInt hozResolution, FT_UInt vertResolution)
        {
            initTextRendering();

            FT_Face ftFontFace = nullptr;
            if ( FT_New_Face(ftLibrary.library, fontFilePath.c_str(), 0, &ftFontFace) )
            {
                if ( FT_New_Face(ftLibrary.library, ("../" + fontFilePath).c_str(), 0, &ftFontFace) ) // Try one directory up
                    throw std::runtime_error("Failed to load font face from font file " + fontFilePath);
            }

            if ( FT_Set_Char_Size(ftFontFace, width, height, hozResolution, vertResolution) )
                throw std::runtime_error("Failed to set font dimensions");
                
            hb_font_t* hbFont = hb_ft_font_create_referenced(ftFontFace);
            hb_ft_font_set_funcs(hbFont);
            return std::make_unique<Font>(ftFontFace, hbFont);
        }

        static std::unique_ptr<Font> load(std::shared_ptr<Memory> fontMemory, FT_UInt width, FT_UInt height)
        {
            initTextRendering();

            FT_Face ftFontFace = nullptr;
            if ( FT_New_Memory_Face(ftLibrary.library, (const FT_Byte*)fontMemory->data.data(), fontMemory->data.size(), 0, &ftFontFace) )
                throw std::runtime_error("Failed to load font face from font memory");

            if ( FT_Set_Pixel_Sizes(ftFontFace, width, height) )
                throw std::runtime_error("Failed to set font dimensions");
                
            hb_font_t* hbFont = hb_ft_font_create_referenced(ftFontFace);
            hb_ft_font_set_funcs(hbFont);
            return std::make_unique<Font>(ftFontFace, hbFont, fontMemory);
        }

        gl::Size2D<GLfloat> measureText(const std::string & utf8Text)
        {
            auto preparedText = PrepareText {utf8Text, true, ftFontFace, hbFont, glyphCache};
            return preparedText.dimensions;
        }

        template <gl::Align Alignment = gl::Align::Left>
        void drawPreparedText(GLfloat x, GLfloat y, PrepareText & preparedText)
        {
            auto & [renderGlyphs, dim, offset] = preparedText;

            GLfloat xAdjust = -offset.x;
            if constexpr ( Alignment == Align::Right)
                xAdjust -= dim.width;
            else if constexpr ( Alignment == Align::Center )
                xAdjust -= dim.width/2.f;

            textShader.use();
            textShader.setColor(red, green, blue);
            textShader.textPos.setVec2(x, y);
            textShader.tex.setValue(0);
            textVertices.bind();
            for ( auto & glyph : renderGlyphs )
            {
                auto glyphHeight = glyph.rc.bottom-glyph.rc.top;
                glyph.tex->bindToSlot(GL_TEXTURE0);
                textVertices.vertices.assign({
                    glyph.rc.left  + xAdjust, glyph.rc.bottom - offset.y, 0.0f, 1.0f,
                    glyph.rc.left  + xAdjust, glyph.rc.top    - offset.y, 0.0f, 0.0f,
                    glyph.rc.right + xAdjust, glyph.rc.top    - offset.y, 1.0f, 0.0f,
                    glyph.rc.left  + xAdjust, glyph.rc.bottom - offset.y, 0.0f, 1.0f,
                    glyph.rc.right + xAdjust, glyph.rc.top    - offset.y, 1.0f, 0.0f,
                    glyph.rc.right + xAdjust, glyph.rc.bottom - offset.y, 1.0f, 1.0f
                });
                textVertices.bufferSubData(gl::UsageHint::DynamicDraw);
                textVertices.drawTriangles();
            }
        }

        template <gl::Align Alignment = gl::Align::Left>
        void drawPreparedClippedText(GLfloat x, GLfloat y, GLfloat lineHeight, PrepareClippedText & preparedClippedText)
        {
            auto & [renderLines] = preparedClippedText;
            int32_t lineIndex = 0;
            for ( auto & renderLine : renderLines )
            {
                auto & [renderGlyphs, dim, offset] = renderLine;
                GLfloat xAdjust = -offset.x;
                if constexpr ( Alignment == Align::Right)
                    xAdjust -= dim.width;
                else if constexpr ( Alignment == Align::Center )
                    xAdjust -= dim.width/2.f;

                clippedTextShader.use();
                clippedTextShader.setColor(red, green, blue);
                clippedTextShader.textPos.setVec2(x, y);
                clippedTextShader.tex.setValue(0);
                textVertices.bind();
                for ( auto & glyph : renderGlyphs )
                {
                    auto glyphHeight = glyph.rc.bottom-glyph.rc.top;
                    glyph.tex->bindToSlot(GL_TEXTURE0);
                    textVertices.vertices.assign({
                        glyph.rc.left  + xAdjust, glyph.rc.bottom - offset.y + lineIndex*lineHeight, 0.0f, 1.0f,
                        glyph.rc.left  + xAdjust, glyph.rc.top    - offset.y + lineIndex*lineHeight, 0.0f, 0.0f,
                        glyph.rc.right + xAdjust, glyph.rc.top    - offset.y + lineIndex*lineHeight, 1.0f, 0.0f,
                        glyph.rc.left  + xAdjust, glyph.rc.bottom - offset.y + lineIndex*lineHeight, 0.0f, 1.0f,
                        glyph.rc.right + xAdjust, glyph.rc.top    - offset.y + lineIndex*lineHeight, 1.0f, 0.0f,
                        glyph.rc.right + xAdjust, glyph.rc.bottom - offset.y + lineIndex*lineHeight, 1.0f, 1.0f
                    });
                    textVertices.bufferSubData(gl::UsageHint::DynamicDraw);
                    textVertices.drawTriangles();
                }
                ++lineIndex;
            }
        }

        template <gl::Align Alignment = gl::Align::Left>
        void drawText(GLfloat x, GLfloat y, const std::string & utf8Text)
        {
            auto preparedText = PrepareText {utf8Text, true, ftFontFace, hbFont, glyphCache};
            drawPreparedText<Alignment>(x, y, preparedText);
        }

        template <gl::Align Alignment = gl::Align::Left>
        void drawClippedText(GLfloat x, GLfloat y, GLfloat lineHeight, const std::string & utf8Text, int32_t clipWidth, int32_t clipHeight)
        {
            auto preparedText = PrepareClippedText {utf8Text, true, ftFontFace, hbFont, glyphCache, clipWidth, clipHeight};
            drawPreparedClippedText<Alignment>(x, y, lineHeight, preparedText);
        }

        // Draws the fixed text at the given x, y position with the given alignment, dynamically positions the prefix before & postfix after
        template <gl::Align Alignment = gl::Align::Left>
        void drawAffixedText(GLfloat x, GLfloat y, const std::string & prefix, const std::string & fixedText, const std::string & postfix)
        {
            auto prefixSize = prefix.empty() ? gl::Size2D<GLfloat>{0.f, 0.f} : measureText(prefix);
            auto postfixSize = postfix.empty() ? gl::Size2D<GLfloat>{0.f, 0.f} : measureText(postfix);
            auto preparedText = PrepareText {fixedText, true, ftFontFace, hbFont, glyphCache};
            auto maxHeight = std::max(std::max(prefixSize.height, postfixSize.height), preparedText.dimensions.height);
            auto fixedTextSize = preparedText.dimensions;
            preparedText.dimensions.height = maxHeight;
            
            drawPreparedText<Alignment>(x, y, preparedText);
            if ( !prefix.empty() )
            {
                preparedText = PrepareText {prefix, true, ftFontFace, hbFont, glyphCache};
                preparedText.dimensions.height = maxHeight;
                if constexpr ( Alignment == gl::Align::Left )
                    drawPreparedText<Alignment>(x-preparedText.dimensions.width, y, preparedText);
                else if constexpr ( Alignment == gl::Align::Right )
                    drawPreparedText<Alignment>(x-fixedTextSize.width, y, preparedText);
                else if constexpr ( Alignment == gl::Align::Center )
                    drawPreparedText<Alignment>(x-fixedTextSize.width/2-preparedText.dimensions.width/2, y, preparedText);
            }
            if ( !postfix.empty() )
            {
                preparedText = PrepareText {postfix, true, ftFontFace, hbFont, glyphCache};
                preparedText.dimensions.height = maxHeight;
                if constexpr ( Alignment == gl::Align::Left )
                    drawPreparedText<Alignment>(x+fixedTextSize.width, y, preparedText);
                else if constexpr ( Alignment == gl::Align::Right )
                    drawPreparedText<Alignment>(x+preparedText.dimensions.width, y, preparedText);
                else if constexpr ( Alignment == gl::Align::Center )
                    drawPreparedText<Alignment>(x+fixedTextSize.width/2+preparedText.dimensions.width/2, y, preparedText);
            }
        }
    };
}
