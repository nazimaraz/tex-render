//
// Created by Nazım Can on 30.10.2025.
//

#include <print>
#include "tex_render.hpp"

int main()
{
    tex_render::initialize();
    if (const auto svg = tex_render::latex_to_svg("x^2 + 3 = 123"))
        std::println("{}", *svg);

    if (const auto svg = tex_render::latex_to_svg("x^2 + 3 = 456"))
        std::println("{}", *svg);

    tex_render::latex_to_svg_file("x^2 + 3 = 789", "test");

    if (const auto svg = tex_render::latex_file_to_svg("basic.tex"))
        std::println("{}", *svg);

    tex_render::latex_file_to_svg_file("basic.tex", "foo");

    tex_render::destroy();

    return 0;
}
