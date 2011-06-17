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

#if defined(WIN32) || defined(_WIN32)
#define HAMMERJS_OS_WINDOWS
#endif

#include <iostream>

#include <stdlib.h>

#ifdef HAMMERJS_OS_WINDOWS
#include <windows.h>
#else
#include <unistd.h>
#endif

using namespace v8;

extern int hammerjs_argc;
extern char** hammerjs_argv;

static Handle<Value> system_execute(const Arguments& args)
{
    HandleScope handle_scope;

    if (args.Length() != 1)
        return ThrowException(String::New("Exception: function system.execute() accepts 1 argument"));

    String::Utf8Value cmd(args[0]);
    ::system(*cmd);


    return Undefined();
}

static Handle<Value> system_exit(const Arguments& args)
{
    HandleScope handle_scope;

    if (args.Length() != 0 && args.Length() != 1)
        return ThrowException(String::New("Exception: function system.exit() accepts 1 argument"));

    int status = (args.Length() == 1) ? args[0]->Int32Value() : 0;
    exit(status);

    return Undefined();
}

static Handle<Value> system_print(const Arguments& args)
{
    HandleScope handle_scope;

    for (int i = 0; i < args.Length(); i++) {
        String::Utf8Value value(args[i]);
        std::cout << *value;
        if (i < args.Length() - 1)
            std::cout << ' ';
    }
    std::cout << std::endl;

    return Undefined();
}

static Handle<Value> system_sleep(const Arguments& args)
{
    HandleScope handle_scope;

    if (args.Length() != 1)
        return ThrowException(String::New("Exception: function system.sleep() accepts 1 argument"));

    long ms = args[0]->NumberValue() * 1000.0;
    if (ms > 0) {
#ifdef HAMMERJS_OS_WINDOWS
        ::Sleep(ms);
#else
        usleep(1000 * ms);
#endif
    }

    return Undefined();
}

void setup_system(Handle<Object> object, Handle<Array> args)
{
    Handle<FunctionTemplate> systemObject = FunctionTemplate::New();

    systemObject->Set(String::New("args"), args);
    systemObject->Set(String::New("execute"), FunctionTemplate::New(system_execute)->GetFunction());
    systemObject->Set(String::New("exit"), FunctionTemplate::New(system_exit)->GetFunction());
    systemObject->Set(String::New("print"), FunctionTemplate::New(system_print)->GetFunction());
    systemObject->Set(String::New("sleep"), FunctionTemplate::New(system_sleep)->GetFunction());

    object->Set(String::New("system"), systemObject->GetFunction());
}
