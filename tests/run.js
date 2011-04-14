/*global system:true, fs:true, Reflect:true */
system.print('Running unit tests for HammerJS...');

var total = 0;

function readFile(fname) {
    var f = fs.open(fname, 'r'),
        line, content = '';
    if (fs.exists(fname)) {
        while (true) {
            line = f.readLine();
            if (line.length === 0) {
                break;
            }
            content += line;
        }
        f.close();
    }
    return content;
}

function writeFile(fname, content) {
    var f = fs.open(fname, 'w');
    f.writeLine(content);
    f.close();
}

function scanDirectory(path) {
    var entries = [],
        subdirs;
    if (fs.exists(path) && fs.isFile(path) && path.match('.js$')) {
        entries.push(path);
    } else if (fs.isDirectory(path)) {
        fs.list(path).forEach(function (e) {
            subdirs = scanDirectory(path + fs.pathSeparator + e);
            subdirs.forEach(function (s) {
                entries.push(s);
            });
        });
    }
    return entries;
}

function assert(passed) {
    total += 1;
    if (!passed) {
        throw new Error('Test FAILS');
    }
}

function test_fs() {
    assert(typeof fs === 'function');
    assert(typeof fs.exists === 'function');
    assert(typeof fs.isDirectory === 'function');
    assert(typeof fs.isFile === 'function');
    assert(typeof fs.makeDirectory === 'function');
    assert(typeof fs.list === 'function');
    assert(typeof fs.open === 'function');
    assert(typeof fs.workingDirectory === 'function');
}

function test_system() {
    assert(typeof system === 'function');
    assert(typeof system.execute === 'function');
    assert(typeof system.exit === 'function');
    assert(typeof system.print === 'function');
}

function test_Reflect() {
    assert(typeof Reflect === 'function');
    assert(typeof Reflect.parse === 'function');
}

function test_parser() {
    var sources = scanDirectory('tests/syntax');
    sources.forEach(function (fileName) {
        var i, content, syntax, actual, ref,
            syntaxFileName = fileName.replace(/\.js$/, '.syntax');

        system.print(' ', fileName);
        content = readFile(fileName);
        if (content.length === 0) {
            system.print('    File is empty or unreadable');
            return;
        }

        syntax = Reflect.parse(content);
        ref = readFile(syntaxFileName).replace(/[\r\n]/g, '');
        if (ref.length === 0) {
            system.print('    No expected syntax tree. Creating a new one to', syntaxFileName);
            writeFile(syntaxFileName, JSON.stringify(syntax, undefined, 4));
        } else {
            actual = JSON.stringify(syntax, undefined, 4).replace(/[\r\n]/g, '');
            if (actual !== ref) {
                syntaxFileName = fileName.replace(/\.js$/, '.syntax-actual');
                system.print('    Different syntax. Compare to', syntaxFileName);
                writeFile(syntaxFileName, JSON.stringify(syntax, undefined, 4));
            }
        }
    });
    system.print('');
    system.print('Total syntax tests:', sources.length);
}

try {
    test_fs();
    test_system();
    test_Reflect();
} catch (e) {
    system.print(e.message);
    system.print(e.stack);
}

system.print('No failure. Total tests: ' + total + ' tests.');
system.print('');

system.print('Running syntax parsing tests...');
test_parser();
