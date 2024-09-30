#include "Fonts.hpp"
#include "GuiFunctions.hpp"

#include "fw-imgui/FontFancy.cpp"
#include "fw-imgui/FontInfoblock.cpp"
#include "fw-imgui/FontMain.cpp"


void load_fonts(OurImGuiContext* ctx, ImGuiIO& io) {
    // reset imgui font atlas
    io.Fonts->Clear();
    io.FontGlobalScale = 1.0f;

    // main text font
    ctx->main_font = io.Fonts->AddFontFromMemoryCompressedTTF(font_main_compressed_data, font_main_compressed_size, 24.0f);
    IM_ASSERT(ctx->main_font != nullptr);

    // big font
    ctx->fancy_font = io.Fonts->AddFontFromMemoryCompressedTTF(font_fancy_compressed_data, font_fancy_compressed_size, 48.0f);
    IM_ASSERT(ctx->fancy_font != nullptr);

    // infoblock font
    ctx->infobox_font = io.Fonts->AddFontFromMemoryCompressedTTF(font_infoblock_compressed_data, font_infoblock_compressed_size, 28.0f);
    IM_ASSERT(ctx->infobox_font != nullptr);

    // rebuild font atlas
    io.Fonts->Build();
}
