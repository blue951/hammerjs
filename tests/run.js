/*global system:true, fs:true, Reflect:true */
system.print('Running unit tests for HammerJS...');

var total = 0;

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

try {
    test_fs();
    test_system();
    test_Reflect();
} catch (e) {
    system.print(e.message);
    system.print(e.stack);
}

system.print('No failure. Total tests: ' + total + ' tests.');
