#pragma once
#include <ft2build.h>
#include <gl/base.h>
#include <gl/program.h>
#include <gl/uniform.h>
#include <gl/vertices.h>
#include <hb-ft.h>
#include <stdexcept>
#include <string_view>
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
            FT_Library library = nullptr;

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
                "layout (location = 0) in vec2 pos;\n"
                "layout (location = 1) in vec2 texPos;\n"
                "out vec2 TexCoords;\n"

                "uniform mat4 projection;\n"

                "void main()\n"
                "{\n"
                    "gl_Position = projection * vec4(pos.xy, 0.0, 1.0);\n"
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

            gl::uniform::Mat4 projection {"projection"};
            gl::uniform::Int tex {"tex"};
            gl::uniform::Vec4 textColor {"textColor"};

            void load()
            {
                gl::Program::create();
                gl::Program::attachShader(gl::Shader(gl::Shader::Type::vertex, vertexCode));
                gl::Program::attachShader(gl::Shader(gl::Shader::Type::fragment, fragmentCode));
                gl::Program::link();
                gl::Program::use();
                gl::Program::findUniforms(projection, tex, textColor);
                this->projection.loadIdentity();
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
                    .left = 0,
                    .top = 0,
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
                    GLfloat left = x + (glyphPos[i].x_offset >> 6) + glyph->bitmap_left;
                    GLfloat top = y + (glyphPos[i].y_offset >> 6) - glyph->bitmap_top + glyph->bitmap.rows;
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
                            renderGlyphs.emplace_back(found->second.get(), gl::Rect2D<GLfloat>{left, top, right, bottom}).tex;
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
                hb_buffer_destroy(buf);
            }
        };

        GLfloat red = 0.f;
        GLfloat green = 0.f;
        GLfloat blue = 0.f;

    public:
        static inline TextShader textShader {};

        void setColor(GLfloat red, GLfloat green, GLfloat blue)
        {
            this->red = red;
            this->green = green;
            this->blue = blue;
        }

        Font(FT_Face ftFontFace, hb_font_t* hbFont) : ftFontFace(ftFontFace), hbFont(hbFont) {}

        Font(FT_Face ftFontFace, hb_font_t* hbFont, std::shared_ptr<Memory> fontMemory) : ftFontFace(ftFontFace), hbFont(hbFont), fontMemory(fontMemory) {}

        ~Font() { FT_Done_Face(ftFontFace); }

        static std::unique_ptr<Font> load(const std::string & fontFilePath, FT_F26Dot6 width, FT_F26Dot6 height, FT_UInt hozResolution, FT_UInt vertResolution)
        {
            initTextRendering();

            FT_Face ftFontFace = nullptr;
            if ( FT_New_Face(ftLibrary.library, fontFilePath.c_str(), 0, &ftFontFace) )
                throw std::runtime_error("Failed to load font face from font file " + fontFilePath);

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
            auto & [renderGlyphs, dim] = preparedText;

            GLfloat xAdjust = 0;
            if constexpr ( Alignment == Align::Right)
                xAdjust = -dim.width;
            else if constexpr ( Alignment == Align::Center )
                xAdjust = -dim.width/2.f;

            textShader.use();
            textShader.setColor(red, green, blue);
            textShader.tex.setValue(0);
            textVertices.bind();
            for ( auto & glyph : renderGlyphs )
            {
                auto glyphHeight = glyph.rc.bottom-glyph.rc.top;
                auto vertOffset = y + dim.height - glyphHeight;
                glyph.tex->bindToSlot(GL_TEXTURE0);
                textVertices.vertices.assign({
                    x+glyph.rc.left  + xAdjust, glyph.rc.bottom + vertOffset, 0.0f, 1.0f,
                    x+glyph.rc.left  + xAdjust, glyph.rc.top    + vertOffset, 0.0f, 0.0f,
                    x+glyph.rc.right + xAdjust, glyph.rc.top    + vertOffset, 1.0f, 0.0f,
                    x+glyph.rc.left  + xAdjust, glyph.rc.bottom + vertOffset, 0.0f, 1.0f,
                    x+glyph.rc.right + xAdjust, glyph.rc.top    + vertOffset, 1.0f, 0.0f,
                    x+glyph.rc.right + xAdjust, glyph.rc.bottom + vertOffset, 1.0f, 1.0f
                });
                textVertices.bufferSubData(gl::UsageHint::DynamicDraw);
                textVertices.drawTriangles();
            }
        }

        template <gl::Align Alignment = gl::Align::Left>
        void drawText(GLfloat x, GLfloat y, const std::string & utf8Text)
        {
            auto preparedText = PrepareText {utf8Text, true, ftFontFace, hbFont, glyphCache};
            drawPreparedText<Alignment>(x, y, preparedText);
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
