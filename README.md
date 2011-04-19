HammerJS is a powerful JavaScript shell. It uses [V8](http://v8.googlecode.com), the fast JavaScript engine for Google Chrome, as the back-end.

HammerJS has a rich set of API to do file I/O, system access, and syntax parsing.

**Warning**: HammerJS is still in the development so the API is not final and might change at any time.

# Build instructions

Building HammerJS is as easy as:

    ./configure
    make

To install the executable (with the target directory <code>/usr/local/bin</code>):

    make install

Alternatively copy <code>hammerjs</code> executable to some directory in the PATH.

For more detailed instructions, see the INSTALL file.

# Quick Start

The typical usage is:

    hammerjs script.js foo bar baz

where <code>script.js</code> will be executed by HammerJS. The three arguments (foo, bar, baz) will be available from the script.

Pretty much standard JavaScript code will run with HammerJS. Since it is pure JavaScript interpreter, obviously it does not have support for DOM objects.

Here is the simplest HammerJS script, <code>hello.js</code>:

    system.print('Hello','world!');
    system.exit();

which can be executed by running

    hammerjs hello.js

For more examples, see the Examples section below.

# API

There are few objects at the global scope: 'system', 'fs', and 'Reflect'.

## system

'system' object has the following functions:

* print(obj, ...) displays obj as a string to the console output.
  It is possible to print several objects separated by comma,
  the output will be separated by white space.

Example:

      system.print('Hello', 'world');

* exit(status) terminates the application and returns the status.
  If status is not specified, 0 is returned instead.

Example:

      if (error)
          system.exit(1);
      else
          system.exit(0);

* execute(cmd) pauses the application and runs the specified command
  externally. This is useful to transfer the control to another
  shell or utility.

'system' object has the following property:

* args, an array of string which contain all the arguments passed when
  invoking the script. The first string, i.e. args[0], is always the
  name of the script. See the example/args.js for details.

## fs

'fs' object has the following functions:

* exists(path) returns true if the specified path exists, otherwise
  returns false.

Example:

      if (fs.exists('/etc/passwd')) {
          system.print('You have passwd file');
      }

* isDirectory(path) returns true if the specified path is a directory
  (not a file), otherwise returns false.

* isFile(path) returns true if the specified path is a file (not
  a directory), otherwise returns false.

* makeDirectory(path) creates a new directory if the directory does not
  exist yet.

* list(path) finds all the files and subdirectories in the specified
  path and returns it as an array of string.

* open(fileName, mode) opens the specified file and returns a Stream
  object which can be used to read or write to the file. The file
  will be opened for read operation if mode is 'r' or write operation
  if mode is 'w'. If the file can not be opened, an exception is thrown.

* workingDirectory() returns the current working directory.

'fs' object has the following property:

* pathSeparator (read-only), a single-character that denotes the separator
  for the path name.

## Reflect

'Reflect' object has the following functions:

* parse(code) returns JSON-formatted syntax tree corresponding to the code.
  See [SpiderMonkey Parser API](https://wiki.mozilla.org/JavaScript:SpiderMonkey:Parser_API) for the
  details of the syntax tree structure.

Example:

      Reflect.parse("var answer = 42;");

will give the following output:

    {
        "type": "Program",
        "body": [
            {
                "type": "VariableDeclaration",
                "declarations": [
                    {
                        "type": "AssignmentExpression",
                        "operator": "=",
                        "left": {
                            "type": "Identifier",
                            "name": "answer"
                        },
                        "right": {
                            "type": "Literal",
                            "value": "42"
                        }
                    }
                ]
            }
        ]
    }

## Stream

Stream is created using fs.open(path). It has the following functions:

* close() flushes pending buffer and closes the stream. Further operation
  on a closed stream will throw an exception.

* flush() ensures that pending data to be written is flushed to the file
  system.

* next() reads a line from the stream. If there is nothing more to read
  (end of file), an exception is thrown.

* readLine() reads a line from the stream, including the '\n' suffix.
  If there is nothing more to read (end of file), an empty string is
  returned instead.

* writeLine() writes a string to the stream and then appends '\n'.

# Examples

All the example scripts below are available in the <code>examples/</code> directory.

<code>hello.js</code>: Shows a simple text message.

    system.print('Hello','world!');
    system.exit();

<code>args.js</code>: Displays all the arguments passed to the script.

    if (system.args.length === 1) {
        system.print('Try to pass some args when invoking this script!');
    } else {
        system.args.forEach(function (arg, i) {
            system.print(i + ': ' + arg);
        });
    }
    system.exit();

  Example output:

    > hammerjs args.js Foo Bar
    0: args.js
    1: Foo
    2: Bar

<code>scandir.js</code>: Recursively traverses directory and prints all found *.js files.

    if (system.args.length !== 2) {
        system.exit(-1);
    }
    var scanDirectory = function (path) {
        if (fs.exists(path) && fs.isFile(path) && path.match('.js$')) {
            system.print(path);
        } else if (fs.isDirectory(path)) {
            fs.list(path).forEach(function (e) {
                scanDirectory(path + '/' + e);
            });
        }
    };
    scanDirectory(system.args[1]);

<code>syntax.js</code>: Loads a script file and prints the syntax tree.

    var f, line, content = '';
    if (system.args.length !== 2) {
        system.exit(-1);
    }
    f = fs.open(system.args[1], 'r');
    while (true) {
        line = f.readLine();
        if (line.length === 0) {
            break;
        }
        content += line;
    }
    f.close();
    system.print(JSON.stringify(Reflect.parse(content), undefined, 4));

# Debugging

Requirement: [node-inspector](https://github.com/dannycoates/node-inspector).
Note: This may change in the future and HammerJS will have its own
built-in debugger handler.

Run the script as usual but with <code>--debug</code> option, e.g.:

    hammerjs --debug examples/countdown.js

Instead of running the script directly, now HammerJS pauses until the
debugger starts.

Run the node-inspector:

    node-inspector

Open a WebKit-based web browser, e.g. Safari or Chrome, then go to the
following URL:

    http://0.0.0.0:8080/debug?port=5858

Now it is possible to breakpoint, single step, run/pause, add watch, and
so on from the debugger interface.

HammerJS also understand <code>debugger</code> statement inside the
script (if any) and always breaks there.
