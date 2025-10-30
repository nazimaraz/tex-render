//
// Created by Nazım Can on 30.10.2025.
//

#include <iostream>
#include <quickjs.h>
#include <fstream>
#include <sstream>

static std::string loadFile(const std::string& path)
{
    const auto ifs = std::ifstream{path, std::ios::in | std::ios::binary};
    if (!ifs)
        throw std::runtime_error{std::format("Could not open file: {}", path)};

    std::ostringstream ss;
    ss << ifs.rdbuf();
    return ss.str();
}

int main()
{
    // auto latex = "x^2 + 3 = 123";
    auto latex = R"(\\[x^2 + 3 = 123\\])";
    bool display = true;

    auto rt = JS_NewRuntime();
    auto ctx = JS_NewContext(rt);
    const auto bundle = loadFile("../../javascript/dist/mathjax-lite-bundle.js");
    JS_Eval(ctx, bundle.c_str(), bundle.size(), "mathjax-lite-bundle.js", JS_EVAL_TYPE_GLOBAL);
    const auto globalObj = JS_GetGlobalObject(ctx);
    const auto func = JS_GetPropertyStr(ctx, globalObj, "latexToSVG");
    if (!JS_IsFunction(ctx, func))
    {
        std::cerr << "latexToSVG is not a function\n";
        JS_FreeValue(ctx, func);
        JS_FreeValue(ctx, globalObj);
        JS_FreeContext(ctx);
        JS_FreeRuntime(rt);
        return 2;
    }

    JSValue arg = JS_NewString(ctx, latex);
    JSValue arg2 = JS_NewBool(ctx, display);
    JSValue argv_js[2] = { arg, arg2 };
    JSValue result = JS_Call(ctx, func, globalObj, 2, argv_js);
    if (JS_IsException(result))
    {
        JSValue err = JS_GetException(ctx);
        const char *errstr = JS_ToCString(ctx, err);
        std::cerr << "Exception: " << (errstr ? errstr : "(unknown)") << "\n";
        if (errstr) JS_FreeCString(ctx, errstr);
        JS_FreeValue(ctx, err);
    }
    else
    {
        const char *res = JS_ToCString(ctx, result);
        if (res)
        {
            std::cout << res << std::endl;
            JS_FreeCString(ctx, res);
        }
    }

    JS_FreeValue(ctx, result);
    JS_FreeValue(ctx, func);
    JS_FreeValue(ctx, globalObj);
    JS_FreeContext(ctx);
    JS_FreeRuntime(rt);

    return 0;
}
