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

#if defined(WIN32) || defined(_WIN32)
#define HAMMERJS_OS_WINDOWS
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <fstream>
#include <iostream>
#include <sstream>

#include <JSGlobalData.h>
#include <SourceCode.h>
#include <UString.h>

#if defined(HAMMERJS_OS_WINDOWS)
#include <windows.h>
#if !defined(PATH_MAX)
#define PATH_MAX MAX_PATH
#endif
#else // HAMMERJS_OS_WINDOWS
#include <dirent.h>
#include <unistd.h>
#endif

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

static Handle<Value> fs_exists(const Arguments& args)
{
    HandleScope handle_scope;

    if (args.Length() != 1)
        return ThrowException(String::New("Exception: function fs.exists() accepts 1 argument"));

    String::Utf8Value fileName(args[0]);

#if defined(HAMMERJS_OS_WINDOWS)
    WIN32_FILE_ATTRIBUTE_DATA attr;
    bool canStat = ::GetFileAttributesEx(*fileName, GetFileExInfoStandard, &attr) != 0;
#else
    struct stat statbuf;
    bool canStat = ::stat(*fileName, &statbuf) == 0;
#endif
    return Boolean::New(canStat);
}

static Handle<Value> fs_isDirectory(const Arguments& args)
{
    HandleScope handle_scope;

    if (args.Length() != 1)
        return ThrowException(String::New("Exception: fs.isDirectory() accepts 1 argument"));

    String::Utf8Value name(args[0]);

#if defined(HAMMERJS_OS_WINDOWS)
    WIN32_FILE_ATTRIBUTE_DATA attr;
    if (!::GetFileAttributesEx(*name, GetFileExInfoStandard, &attr))
        return ThrowException(String::New("Exception: fs.isDirectory() can't access the directory"));

    return Boolean::New((attr.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0);
#else
    struct stat statbuf;
    if (::stat(*name, &statbuf))
        return ThrowException(String::New("Exception: fs.isDirectory() can't access the directory"));

    return Boolean::New(S_ISDIR(statbuf.st_mode));
#endif
}

static Handle<Value> fs_isFile(const Arguments& args)
{
    HandleScope handle_scope;

    if (args.Length() != 1)
        return ThrowException(String::New("Exception: fs.isFile() accepts 1 argument"));

    String::Utf8Value name(args[0]);

#if defined(HAMMERJS_OS_WINDOWS)
    WIN32_FILE_ATTRIBUTE_DATA attr;
    if (!::GetFileAttributesEx(*name, GetFileExInfoStandard, &attr))
        return ThrowException(String::New("Exception: fs.isFile() can't access the file"));

    return Boolean::New((attr.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY)) == 0);
#else
    struct stat statbuf;
    if (::stat(*name, &statbuf))
        return ThrowException(String::New("Exception: fs.isFile() can't access the file"));

    return Boolean::New(S_ISREG(statbuf.st_mode));
#endif
}

static Handle<Value> fs_makeDirectory(const Arguments& args)
{
    HandleScope handle_scope;

    if (args.Length() != 1)
        return ThrowException(String::New("Exception: function fs.makeDirectory() accepts 1 argument"));

    String::Utf8Value directoryName(args[0]);

#if defined(HAMMERJS_OS_WINDOWS)
    if (::CreateDirectory(*directoryName, NULL) == 0)
        return ThrowException(String::New("Exception: fs.makeDirectory() can't create the directory"));
#else
    if (::mkdir(*directoryName, 0777) != 0)
        return ThrowException(String::New("Exception: fs.makeDirectory() can't create the directory"));
#endif

    return Undefined();
}

static Handle<Value> fs_list(const Arguments& args)
{
    HandleScope handle_scope;

    if (args.Length() != 1)
        return ThrowException(String::New("Exception: fs.list() accepts 1 argument"));

    String::Utf8Value dirname(args[0]);

#if defined(HAMMERJS_OS_WINDOWS)
    char *search = new char[dirname.length() + 3];
    strcpy(search, *dirname);
    strcat(search, "\\*");
    WIN32_FIND_DATA entry;
    HANDLE dir = INVALID_HANDLE_VALUE;
    dir = FindFirstFile(search, &entry);
    if (dir == INVALID_HANDLE_VALUE)
        return ThrowException(String::New("Exception: fs.list() can't access the directory"));

    Handle<Array> entries = Array::New();
    int count = 0;
    do {
        if (strcmp(entry.cFileName, ".") && strcmp(entry.cFileName, "..")) {
            entries->Set(count++, String::New(entry.cFileName));
        }
    } while (FindNextFile(dir, &entry) != 0);
    FindClose(dir);

    return entries;
#else
    DIR *dir = opendir(*dirname);
    if (!dir)
        return ThrowException(String::New("Exception: fs.list() can't access the directory"));

    Handle<Array> entries = Array::New();
    int count = 0;
    struct dirent entry;
    struct dirent *ptr = NULL;
    ::readdir_r(dir, &entry, &ptr);
    while (ptr) {
        if (strcmp(entry.d_name, ".") && strcmp(entry.d_name, ".."))
            entries->Set(count++, String::New(entry.d_name));
        ::readdir_r(dir, &entry, &ptr);
    }
    ::closedir(dir);

    return entries;
#endif
}

static Handle<Value> fs_open(const Arguments& args)
{
    HandleScope handle_scope;

    if (args.Length() != 1 && args.Length() != 2)
        return ThrowException(String::New("Exception: function fs.open() accepts 1 or 2 arguments"));

    Handle<Context> context = Context::GetCurrent();
    Handle<Value> streamClass = context->Global()->Get(String::New("Stream"));
    Function *streamFunction = Function::Cast(*streamClass);

    Handle<Value> argv[2];
    argv[0] = args[0];
    if (args.Length() == 2)
        argv[1] = args[1];

    Handle<Object> result = streamFunction->NewInstance(args.Length(), argv);
    return result;
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
