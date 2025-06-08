#include "Fonts.hpp"
#include "GuiFunctions.hpp"

#include "fw-imgui/FontFancy.cpp"
#include "fw-imgui/FontInfoblock.cpp"
#include "fw-imgui/FontMain.cpp"

const float font_scaled(float size, unsigned int dpi) {
    static constexpr float default_dpi = 96.0;
    const float result = size * ((float)dpi / default_dpi);
    return glm::floor(result);
}

void load_fonts(OurImGuiContext* ctx, ImGuiIO& io, unsigned int dpi) {
    spdlog::error("DPI: {}", dpi);

    // reset imgui font atlas
    io.Fonts->Clear();
    io.FontGlobalScale = 1.0f;

    // main text font
    ctx->main_font = io.Fonts->AddFontFromMemoryCompressedTTF(font_main_compressed_data, font_main_compressed_size, font_scaled(24.0f, dpi));
    IM_ASSERT(ctx->main_font != nullptr);

    // big font
    ctx->fancy_font = io.Fonts->AddFontFromMemoryCompressedTTF(font_fancy_compressed_data, font_fancy_compressed_size, font_scaled(48.0f, dpi));
    IM_ASSERT(ctx->fancy_font != nullptr);

    // infoblock font
    ctx->infobox_font = io.Fonts->AddFontFromMemoryCompressedTTF(font_infoblock_compressed_data, font_infoblock_compressed_size, font_scaled(28.0f, dpi));
    IM_ASSERT(ctx->infobox_font != nullptr);

    ctx->modname_font = io.Fonts->AddFontFromMemoryCompressedTTF(font_main_compressed_data, font_main_compressed_size, font_scaled(38.0f, dpi));
    IM_ASSERT(ctx->modname_font != nullptr);

    ctx->tony_font = io.Fonts->AddFontFromFileTTF("tony_font.ttf", font_scaled(18.0f, dpi));
    IM_ASSERT(ctx->tony_font != nullptr);

    // rebuild font atlas
    io.Fonts->Build();
}
