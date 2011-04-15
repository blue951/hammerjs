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

#include <JSGlobalData.h>
#include <SourceCode.h>
#include <UString.h>

using namespace v8;

static Handle<Value> reflect_parse(const Arguments& args);

int hammerjs_argc;
char** hammerjs_argv;

void setup_system(Handle<Object> object); // modules/system/system.cpp
void setup_fs(Handle<Object> object); // modules/fs/fs.cpp

void setup_Reflect(Handle<Object> object)
{
    Handle<FunctionTemplate> reflectObject = FunctionTemplate::New();

    reflectObject->Set(String::New("parse"), FunctionTemplate::New(reflect_parse)->GetFunction());

    object->Set(String::New("Reflect"), reflectObject->GetFunction());
}

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
    fread(buf, 1, len, f);
    buf[len - 1] = '\0';
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

static Handle<Value> reflect_parse(const Arguments& args)
{
    if (args.Length() != 1)
        return ThrowException(String::New("Exception: Reflect.parse() accepts 1 argument"));

    String::Utf8Value code(args[0]);
    UChar *content = new UChar[code.length()];
    for (int i = 0; i < code.length(); ++i)
        content[i] = (*code)[i];
    JSC::UString scriptCode = JSC::UString(content, code.length());
    delete [] content;

    JSC::JSGlobalData* globalData = new JSC::JSGlobalData;
    JSC::UString tree = globalData->parser->createSyntaxTree(globalData, JSC::makeSource(scriptCode));
    delete globalData;

    if (tree.length() == 0)
        return Undefined();

    char *buffer = new char[tree.length() + 1];
    buffer[tree.length()] = '\0';
    const UChar *uchars = tree.characters();
    for (size_t i = 0; i < tree.length(); ++i)
        buffer[i] = uchars[i];

    Handle<ObjectTemplate> global = ObjectTemplate::New();
    global->Set("tree", String::New(buffer));
    Handle<Context> context = Context::New(NULL, global);
    Context::Scope context_scope(context);

    Handle<Script> script = Script::Compile(String::New("JSON.parse(tree)"));
    delete [] buffer;
    return script->Run();
}
