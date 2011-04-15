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

using namespace v8;

int hammerjs_argc;
char** hammerjs_argv;

void setup_system(Handle<Object> object); // modules/system/system.cpp
void setup_fs(Handle<Object> object); // modules/fs/fs.cpp
void setup_Reflect(Handle<Object> object); // modules/reflect/reflect.cpp

int main(int argc, char* argv[])
{
    hammerjs_argc = argc;
    hammerjs_argv = argv;

    if (argc < 2) {
        std::cout << "Usage: hammerjs inputfile.js" << std::endl;
        return 0;
    }

    FILE* f = fopen(argv[1], "rb");
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

    v8::Debug::EnableAgent(argv[1], 5858, true);

    HandleScope handle_scope;
    Handle<ObjectTemplate> global = ObjectTemplate::New();
    Handle<Context> context = Context::New(NULL, global);

    Context::Scope context_scope(context);

    setup_system(context->Global());
    setup_fs(context->Global());
    setup_Reflect(context->Global());

    Handle<Script> script = Script::Compile(String::New(buf));
    if (script.IsEmpty())
        std::cerr << "Error: unable to run " << argv[1] << std::endl;
    else
        script->Run();

    delete [] buf;
    return 0;
}
