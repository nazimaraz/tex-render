//
// Created by Nazım Can on 1.11.2025.
//

#pragma once

#include <memory>
#include <optional>
#include <string>

struct JSRuntime;
struct JSContext;
struct JSValue;

class tex_render
{
public:
    static void initialize();
    static void destroy();

    static std::optional<std::string> latex_to_svg(const std::string& latex);
    // static std::optional<std::string> latex_to_svg_file(const std::string& latex);
    // static std::optional<std::string> latex_file_to_svg(const std::string& latex);
    // static std::optional<std::string> latex_file_to_svg_file(const std::string& latex);

    // static std::optional<std::string> latex_to_png(const std::string& latex);
    // static std::optional<std::string> latex_file_to_png(const std::string& latex);

private:
    static inline JSRuntime* js_runtime_{};
    static inline JSContext* js_context_{};
    static std::unique_ptr<JSValue> js_global_object_;
    static std::unique_ptr<JSValue> js_latex_to_svg_function_;
};
