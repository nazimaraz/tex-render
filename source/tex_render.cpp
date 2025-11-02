//
// Created by Nazım Can on 1.11.2025.
//

#include "tex_render.hpp"
#include <format>
#include <fstream>
#include <print>
#include <quickjs.h>
#include <sstream>
#include <string>

std::unique_ptr<JSValue> tex_render::js_global_object_{};
std::unique_ptr<JSValue> tex_render::js_latex_to_svg_function_{};

static std::string load_file(const std::string& path)
{
    const auto ifs = std::ifstream{path, std::ios::in | std::ios::binary};
    if (!ifs)
        throw std::runtime_error{std::format("Could not open file: {}", path)};

    auto ss = std::ostringstream{};
    ss << ifs.rdbuf();
    return ss.str();
}

void tex_render::initialize()
{
    js_runtime_ = JS_NewRuntime();
    JS_SetMaxStackSize(js_runtime_, 2 * 1024 * 1024);
    js_context_ = JS_NewContext(js_runtime_);
    const auto js_library = load_file("../../javascript/dist/mathjax-lite-bundle.js");
    JS_Eval(js_context_, js_library.c_str(), js_library.size(), "mathjax-lite-js_library.js", JS_EVAL_TYPE_GLOBAL);
    js_global_object_ = std::make_unique<JSValue>(JS_GetGlobalObject(js_context_));
    js_latex_to_svg_function_ = std::make_unique<JSValue>(JS_GetPropertyStr(js_context_, *js_global_object_, "latexToSVG"));
}

void tex_render::destroy()
{
    JS_FreeValue(js_context_, *js_latex_to_svg_function_);
    JS_FreeValue(js_context_, *js_global_object_);
    JS_FreeContext(js_context_);
    JS_FreeRuntime(js_runtime_);
}

std::optional<std::string> tex_render::latex_to_svg(const std::string& latex)
{
    const auto arg = JS_NewString(js_context_, latex.c_str());
    const auto arg2 = JS_NewBool(js_context_, false);
    JSValue argv_js[2]{arg, arg2};
    const auto result = JS_Call(js_context_, *js_latex_to_svg_function_, *js_global_object_, 2, argv_js);
    if (JS_IsException(result))
    {
        const auto error = JS_GetException(js_context_);
        if (const auto error_string = JS_ToCString(js_context_, error))
        {
            std::println(stderr, "Exception: {}", error_string);
            JS_FreeCString(js_context_, error_string);
        }
        else
        {
            std::println(stderr, "Exception: (unknown)");
        }

        JS_FreeValue(js_context_, error);
        JS_FreeValue(js_context_, result);
        return {};
    }

    if (const auto raw_response = JS_ToCString(js_context_, result))
    {
        const auto response = std::string{raw_response};
        JS_FreeCString(js_context_, raw_response);
        JS_FreeValue(js_context_, result);
        return response;
    }

    JS_FreeValue(js_context_, result);
    return std::string{};
}

void tex_render::latex_to_svg_file(const std::string& latex)
{
    latex_to_svg_file(latex, "output");
}

void tex_render::latex_to_svg_file(const std::string& latex, const std::string& output_svg_file)
{
    const auto svg = latex_to_svg(latex);
    if (!svg)
        return;

    auto file = std::ofstream{std::format("{}.svg", output_svg_file)};
    if (!file.is_open())
    {
        std::println(stderr, "Unable to open file for writing");
        return;
    }

    file << *svg;
    file.close();
    std::println("File {}.svg written successfully", output_svg_file);
}

std::optional<std::string> tex_render::latex_file_to_svg(const std::string& input_latex_file)
{
    const auto file = std::ifstream{input_latex_file};
    if (!file)
    {
        std::println(stderr, "Unable to open file for writing");
        return {};
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return latex_to_svg(buffer.str());
}

void tex_render::latex_file_to_svg_file(const std::string& input_latex_file)
{
    latex_file_to_svg_file(input_latex_file, "output");
}

void tex_render::latex_file_to_svg_file(const std::string& input_latex_file, const std::string& output_svg_file)
{
    const auto file = std::ifstream{input_latex_file};
    if (!file)
    {
        std::println(stderr, "Unable to open file for writing");
        return;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    latex_to_svg_file(buffer.str(), output_svg_file);
}
