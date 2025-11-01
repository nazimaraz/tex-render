//
// Created by Nazım Can on 30.10.2025.
//

#include <iostream>
#include <quickjs.h>
#include <fstream>
#include <sstream>

static std::string load_file(const std::string& path)
{
    const auto ifs = std::ifstream{path, std::ios::in | std::ios::binary};
    if (!ifs)
        throw std::runtime_error{std::format("Could not open file: {}", path)};

    auto ss = std::ostringstream{};
    ss << ifs.rdbuf();
    return ss.str();
}

int main()
{
    constexpr auto latex = "x^2 + 3 = 123";
    constexpr auto display = false;
    const auto js_runtime = JS_NewRuntime();
    const auto js_context = JS_NewContext(js_runtime);
    const auto js_library = load_file("../../javascript/dist/mathjax-lite-bundle.js");
    JS_Eval(js_context, js_library.c_str(), js_library.size(), "mathjax-lite-js_library.js", JS_EVAL_TYPE_GLOBAL);
    const auto js_global_object = JS_GetGlobalObject(js_context);
    const auto js_latex_to_svg_function = JS_GetPropertyStr(js_context, js_global_object, "latexToSVG");
    const auto arg = JS_NewString(js_context, latex);
    const auto arg2 = JS_NewBool(js_context, display);
    JSValue argv_js[2] = { arg, arg2 };
    const auto result = JS_Call(js_context, js_latex_to_svg_function, js_global_object, 2, argv_js);
    if (JS_IsException(result))
    {
        const auto error = JS_GetException(js_context);
        if (const auto error_string = JS_ToCString(js_context, error))
        {
            std::println(stderr, "Exception: {}", error_string);
            JS_FreeCString(js_context, error_string);
        }
        else
        {
            std::println(stderr, "Exception: (unknown)");
        }

        JS_FreeValue(js_context, error);
    }
    else
    {
        if (const auto response = JS_ToCString(js_context, result))
        {
            std::println("{}", response);
            JS_FreeCString(js_context, response);
        }
    }

    JS_FreeValue(js_context, result);
    JS_FreeValue(js_context, js_latex_to_svg_function);
    JS_FreeValue(js_context, js_global_object);
    JS_FreeContext(js_context);
    JS_FreeRuntime(js_runtime);

    return 0;
}
