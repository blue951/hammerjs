/*
    Copyright (c) 2011 Sencha Inc.
    Copyright (c) 2010 Sencha Inc.

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
*/

#include <v8.h>
#include <v8-debug.h>

#include <iostream>

#include <stdlib.h>
#include <string.h>

using namespace v8;

void setup_system(Handle<Object> object, Handle<Array> args);   // modules/system/system.cpp
void setup_fs(Handle<Object> object, Handle<Array> args);       // modules/fs/fs.cpp
void setup_Reflect(Handle<Object> object, Handle<Array> args);  // modules/reflect/reflect.cpp

void showUsage()
{
    std::cout << "Usage: hammerjs [options] script.js [arguments]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --debug    Enables remote debugging" << std::endl;
    std::cout << std::endl;
    ::exit(0);
}

int main(int argc, char* argv[])
{
    V8::Initialize();

    HandleScope handle_scope;
    Handle<ObjectTemplate> global = ObjectTemplate::New();
    Handle<Context> context = Context::New(NULL, global);

    Context::Scope context_scope(context);

    Handle<Array> args = Array::New();
    const char* inputFile = 0;
    bool debug = false;
    for (int i = 1, index = 0; i < argc; ++i) {
        const char* arg = argv[i];
        if (arg[0] == '-') {
            if (!strcmp(arg, "--debug")) {
                debug = true;
                continue;
            }
            std::cerr << "Unknown option: " << arg << std::endl;
            return 0;
        } else {
            if (!inputFile)
                inputFile = arg;
            args->Set(index, String::New(arg));
            ++index;
        }
    }

    if (!inputFile)
        showUsage();

    FILE* f = fopen(inputFile, "rb");
    if (!f) {
        std::cerr << "Error: unable to open file " << argv[1] << std::endl;
        return 0;
    }
    fseek(f, 0, SEEK_END);
    int len = ftell(f);
    rewind(f);
    char* buf = new char[len + 1];
    memset(buf, '\0', len + 1);
    fread(buf, 1, len, f);
    fclose(f);
    Handle<String> code = String::New(buf);
    delete [] buf;

    setup_system(context->Global(), args);
    setup_fs(context->Global(), args);
    setup_Reflect(context->Global(), args);

    Handle<Script> script = Script::Compile(code);
    if (script.IsEmpty()) {
        std::cerr << "Error: unable to run " << inputFile << std::endl;
    } else {
        if (debug)
            v8::Debug::EnableAgent(inputFile, 5858, true);
        script->Run();
    }

    return 0;
}
