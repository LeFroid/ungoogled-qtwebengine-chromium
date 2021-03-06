// Copyright 2008 The Closure Library Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS-IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

goog.module('goog.testing.FunctionMockTest');
goog.setTestOnly();

const FunctionMock = goog.require('goog.testing.FunctionMock');
const Mock = goog.require('goog.testing.Mock');
const StrictMock = goog.require('goog.testing.StrictMock');
const asserts = goog.require('goog.testing.asserts');
const googArray = goog.require('goog.array');
const googString = goog.require('goog.string');
const mockmatchers = goog.require('goog.testing.mockmatchers');
const testSuite = goog.require('goog.testing.testSuite');
const testing = goog.require('goog.testing');

// Global scope so we can tear it down safely
let mockGlobal;

//----- Global functions for goog.testing.GlobalFunctionMock to mock

window.globalFoo = function() {
  return 'I am Spartacus!';
};

window.globalBar = function(who, what) {
  return [who, 'is', what].join(' ');
};

//----- Functions for goog.testing.MethodMock to mock

const mynamespace = {};

mynamespace.myMethod = () => 'I should be mocked.';

//----- Functions for goog.testing.createConstructorMock to mock

const constructornamespace = {};

constructornamespace.MyConstructor = class {
  myMethod() {
    return 'I should be mocked.';
  }
};

constructornamespace.MyConstructorWithArgument = class {
  constructor(argument) {
    this.argument_ = argument;
  }

  myMethod() {
    return this.argument_;
  }
};

constructornamespace.MyConstructorWithClassMembers = class {};

/**
 * Defined this way to preseve enumerability of the property for testing.
 *
 * @return {string}
 */
constructornamespace.MyConstructorWithClassMembers.classMethod = function() {
  return 'class method return value';
};

constructornamespace.MyConstructorWithClassMembers.CONSTANT = 42;

//----- Helper assertions

function assertQuacksLike(obj, target) {
  for (const meth in target.prototype) {
    if (!googString.endsWith(meth, '_')) {
      assertNotUndefined(
          `Should have implemented ${meth}` +
              '()',
          obj[meth]);
    }
  }
}
testSuite({
  tearDown() {
    if (mockGlobal) {
      mockGlobal.$tearDown();
    }
  },

  //----- Tests for goog.testing.FunctionMock
  testMockFunctionCallOrdering() {
    const doOneTest = (mockFunction, success, expected_args, actual_args) => {
      googArray.forEach(expected_args, (arg) => {
        mockFunction(arg);
      });
      mockFunction.$replay();
      const callFunction = () => {
        googArray.forEach(actual_args, (arg) => {
          mockFunction(arg);
        });
        mockFunction.$verify();
      };
      if (success) {
        callFunction();
      } else {
        assertThrowsJsUnitException(callFunction);
      }
    };

    const doTest = (strict_ok, loose_ok, expected_args, actual_args) => {
      doOneTest(
          testing.createFunctionMock(), strict_ok, expected_args, actual_args);
      doOneTest(
          testing.createFunctionMock('name'), strict_ok, expected_args,
          actual_args);
      doOneTest(
          testing.createFunctionMock('name', Mock.STRICT), strict_ok,
          expected_args, actual_args);
      doOneTest(
          testing.createFunctionMock('name', Mock.LOOSE), loose_ok,
          expected_args, actual_args);
    };

    doTest(true, true, [1, 2], [1, 2]);
    doTest(false, true, [1, 2], [2, 1]);
    doTest(false, false, [1, 2], [2, 2]);
    doTest(false, false, [1, 2], [1]);
    doTest(false, false, [1, 2], [1, 1]);
    doTest(false, false, [1, 2], [1]);
  },

  testMocksFunctionWithNoArgs() {
    const mockFoo = testing.createFunctionMock();
    mockFoo();
    mockFoo.$replay();
    mockFoo();
    mockFoo.$verify();
  },

  testMocksFunctionWithOneArg() {
    const mockFoo = testing.createFunctionMock();
    mockFoo('x');
    mockFoo.$replay();
    mockFoo('x');
    mockFoo.$verify();
  },

  testMocksFunctionWithMultipleArgs() {
    const mockFoo = testing.createFunctionMock();
    mockFoo('x', 'y');
    mockFoo.$replay();
    mockFoo('x', 'y');
    mockFoo.$verify();
  },

  testFailsIfCalledWithIncorrectArgs() {
    const mockFoo = testing.createFunctionMock();

    mockFoo();
    mockFoo.$replay();
    assertThrowsJsUnitException(() => {
      mockFoo('x');
    });
    mockFoo.$reset();

    mockFoo('x');
    mockFoo.$replay();
    assertThrowsJsUnitException(() => {
      mockFoo();
    });
    mockFoo.$reset();

    mockFoo('x');
    mockFoo.$replay();
    assertThrowsJsUnitException(() => {
      mockFoo('x', 'y');
    });
    mockFoo.$reset();

    mockFoo('x', 'y');
    mockFoo.$replay();
    assertThrowsJsUnitException(() => {
      mockFoo('x');
    });
    mockFoo.$reset();

    mockFoo('correct');
    mockFoo.$replay();
    assertThrowsJsUnitException(() => {
      mockFoo('wrong');
    });
    mockFoo.$reset();

    mockFoo('correct', 'args');
    mockFoo.$replay();
    assertThrowsJsUnitException(() => {
      mockFoo('wrong', 'args');
    });
    mockFoo.$reset();
  },

  testMocksFunctionWithReturnValue() {
    const mockFoo = testing.createFunctionMock();
    mockFoo().$returns('bar');
    mockFoo.$replay();
    assertEquals('bar', mockFoo());
    mockFoo.$verify();
  },

  testFunctionMockWorksWhenPassedAsACallback() {
    const invoker = {
      register: function(callback) {
        this.callback = callback;
      },

      invoke: function(args) {
        return this.callback(args);
      },
    };

    const mockFunction = testing.createFunctionMock();
    mockFunction('bar').$returns('baz');

    mockFunction.$replay();
    invoker.register(mockFunction);
    assertEquals('baz', invoker.invoke('bar'));
    mockFunction.$verify();
  },

  testFunctionMockQuacksLikeAStrictMock() {
    const mockFunction = testing.createFunctionMock();
    assertQuacksLike(mockFunction, StrictMock);
  },

  //----- Tests for goog.testing.createGlobalFunctionMock
  testMocksGlobalFunctionWithNoArgs() {
    mockGlobal = testing.createGlobalFunctionMock('globalFoo');
    mockGlobal().$returns('No, I am Spartacus!');

    mockGlobal.$replay();
    assertEquals('No, I am Spartacus!', globalFoo());
    mockGlobal.$verify();
  },

  testMocksGlobalFunctionUsingGlobalName() {
    testing.createGlobalFunctionMock('globalFoo');
    globalFoo().$returns('No, I am Spartacus!');

    globalFoo.$replay();
    assertEquals('No, I am Spartacus!', globalFoo());
    globalFoo.$verify();
    globalFoo.$tearDown();
  },

  testMocksGlobalFunctionWithArgs() {
    const mockReturnValue = 'Noam is Chomsky!';
    mockGlobal = testing.createGlobalFunctionMock('globalBar');
    mockGlobal('Noam', 'Spartacus').$returns(mockReturnValue);

    mockGlobal.$replay();
    assertEquals(mockReturnValue, globalBar('Noam', 'Spartacus'));
    mockGlobal.$verify();
  },

  testGlobalFunctionMockFailsWithIncorrectArgs() {
    mockGlobal = testing.createGlobalFunctionMock('globalBar');
    mockGlobal('a', 'b');

    mockGlobal.$replay();

    assertThrowsJsUnitException(() => {
      globalBar('b', 'a');
    });
  },

  testGlobalFunctionMockQuacksLikeAFunctionMock() {
    mockGlobal = testing.createGlobalFunctionMock('globalFoo');
    assertQuacksLike(mockGlobal, FunctionMock);
  },

  testMockedFunctionsAvailableInGlobalAndGoogGlobalAndWindowScope() {
    mockGlobal = testing.createGlobalFunctionMock('globalFoo');

    // we expect this call 3 times through global, goog.global and window scope
    mockGlobal().$times(3);

    mockGlobal.$replay();
    goog.global.globalFoo();
    window.globalFoo();
    globalFoo();
    mockGlobal.$verify();
  },

  testTearDownRestoresOriginalGlobalFunction() {
    mockGlobal = testing.createGlobalFunctionMock('globalFoo');
    mockGlobal().$returns('No, I am Spartacus!');

    mockGlobal.$replay();
    assertEquals('No, I am Spartacus!', globalFoo());
    mockGlobal.$tearDown();
    assertEquals('I am Spartacus!', globalFoo());
    mockGlobal.$verify();
  },

  testTearDownHandlesMultipleMocking() {
    const mock1 = testing.createGlobalFunctionMock('globalFoo');
    const mock2 = testing.createGlobalFunctionMock('globalFoo');
    const mock3 = testing.createGlobalFunctionMock('globalFoo');
    mock1().$returns('No, I am Spartacus 1!');
    mock2().$returns('No, I am Spartacus 2!');
    mock3().$returns('No, I am Spartacus 3!');

    mock1.$replay();
    mock2.$replay();
    mock3.$replay();
    assertEquals('No, I am Spartacus 3!', globalFoo());
    mock3.$tearDown();
    assertEquals('No, I am Spartacus 2!', globalFoo());
    mock2.$tearDown();
    assertEquals('No, I am Spartacus 1!', globalFoo());
    mock1.$tearDown();
    assertEquals('I am Spartacus!', globalFoo());
  },

  testGlobalFunctionMockCallOrdering() {
    let mock = testing.createGlobalFunctionMock('globalFoo');
    mock(1);
    mock(2);
    mock.$replay();
    assertThrowsJsUnitException(() => {
      globalFoo(2);
    });
    mock.$tearDown();

    mock = testing.createGlobalFunctionMock('globalFoo', Mock.STRICT);
    mock(1);
    mock(2);
    mock.$replay();
    globalFoo(1);
    globalFoo(2);
    mock.$verify();
    mock.$tearDown();

    mock = testing.createGlobalFunctionMock('globalFoo', Mock.STRICT);
    mock(1);
    mock(2);
    mock.$replay();
    assertThrowsJsUnitException(() => {
      globalFoo(2);
    });
    mock.$tearDown();

    mock = testing.createGlobalFunctionMock('globalFoo', Mock.LOOSE);
    mock(1);
    mock(2);
    mock.$replay();
    globalFoo(2);
    globalFoo(1);
    mock.$verify();
    mock.$tearDown();
  },

  testMocksMethod() {
    const mockMethod = testing.createMethodMock(mynamespace, 'myMethod');
    mockMethod().$returns('I have been mocked!');

    mockMethod.$replay();
    assertEquals('I have been mocked!', mockMethod());
    mockMethod.$verify();
  },

  testMocksMethodInNamespace() {
    testing.createMethodMock(mynamespace, 'myMethod');
    mynamespace.myMethod().$returns('I have been mocked!');

    mynamespace.myMethod.$replay();
    assertEquals('I have been mocked!', mynamespace.myMethod());
    mynamespace.myMethod.$verify();
    mynamespace.myMethod.$tearDown();
  },

  testMethodMockCanOnlyMockExistingMethods() {
    assertThrows(() => {
      testing.createMethodMock(mynamespace, 'doesNotExist');
    });
  },

  testMethodMockCallOrdering() {
    testing.createMethodMock(mynamespace, 'myMethod');
    mynamespace.myMethod(1);
    mynamespace.myMethod(2);
    mynamespace.myMethod.$replay();
    assertThrowsJsUnitException(() => {
      mynamespace.myMethod(2);
    });
    mynamespace.myMethod.$tearDown();

    testing.createMethodMock(mynamespace, 'myMethod', Mock.STRICT);
    mynamespace.myMethod(1);
    mynamespace.myMethod(2);
    mynamespace.myMethod.$replay();
    mynamespace.myMethod(1);
    mynamespace.myMethod(2);
    mynamespace.myMethod.$verify();
    mynamespace.myMethod.$tearDown();

    testing.createMethodMock(mynamespace, 'myMethod', Mock.STRICT);
    mynamespace.myMethod(1);
    mynamespace.myMethod(2);
    mynamespace.myMethod.$replay();
    assertThrowsJsUnitException(() => {
      mynamespace.myMethod(2);
    });
    mynamespace.myMethod.$tearDown();

    testing.createMethodMock(mynamespace, 'myMethod', Mock.LOOSE);
    mynamespace.myMethod(1);
    mynamespace.myMethod(2);
    mynamespace.myMethod.$replay();
    mynamespace.myMethod(2);
    mynamespace.myMethod(1);
    mynamespace.myMethod.$verify();
    mynamespace.myMethod.$tearDown();
  },

  testConstructorMock() {
    const mockObject = new StrictMock(constructornamespace.MyConstructor);
    const mockConstructor =
        testing.createConstructorMock(constructornamespace, 'MyConstructor');
    mockConstructor().$returns(mockObject);
    mockObject.myMethod().$returns('I have been mocked!');

    mockConstructor.$replay();
    mockObject.$replay();
    assertEquals(
        'I have been mocked!',
        new constructornamespace.MyConstructor().myMethod());
    mockConstructor.$verify();
    mockObject.$verify();
    mockConstructor.$tearDown();
  },

  testConstructorMockWithArgument() {
    const mockObject =
        new StrictMock(constructornamespace.MyConstructorWithArgument);
    const mockConstructor = testing.createConstructorMock(
        constructornamespace, 'MyConstructorWithArgument');
    mockConstructor(mockmatchers.isString).$returns(mockObject);
    mockObject.myMethod().$returns('I have been mocked!');

    mockConstructor.$replay();
    mockObject.$replay();
    assertEquals(
        'I have been mocked!',
        new constructornamespace
            .MyConstructorWithArgument('I should be mocked.')
            .myMethod());
    mockConstructor.$verify();
    mockObject.$verify();
    mockConstructor.$tearDown();
  },

  /** Test that class members are copied to the mock constructor. */
  testConstructorMockWithClassMembers() {
    const mockConstructor = testing.createConstructorMock(
        constructornamespace, 'MyConstructorWithClassMembers');
    assertEquals(
        42, constructornamespace.MyConstructorWithClassMembers.CONSTANT);
    assertEquals(
        'class method return value',
        constructornamespace.MyConstructorWithClassMembers.classMethod());
    mockConstructor.$tearDown();
  },

  testConstructorMockCallOrdering() {
    const instance = {};

    testing.createConstructorMock(
        constructornamespace, 'MyConstructorWithArgument');
    constructornamespace.MyConstructorWithArgument(1).$returns(instance);
    constructornamespace.MyConstructorWithArgument(2).$returns(instance);
    constructornamespace.MyConstructorWithArgument.$replay();
    assertThrowsJsUnitException(() => {
      new constructornamespace.MyConstructorWithArgument(2);
    });
    constructornamespace.MyConstructorWithArgument.$tearDown();

    testing.createConstructorMock(
        constructornamespace, 'MyConstructorWithArgument', Mock.STRICT);
    constructornamespace.MyConstructorWithArgument(1).$returns(instance);
    constructornamespace.MyConstructorWithArgument(2).$returns(instance);
    constructornamespace.MyConstructorWithArgument.$replay();
    new constructornamespace.MyConstructorWithArgument(1);
    new constructornamespace.MyConstructorWithArgument(2);
    constructornamespace.MyConstructorWithArgument.$verify();
    constructornamespace.MyConstructorWithArgument.$tearDown();

    testing.createConstructorMock(
        constructornamespace, 'MyConstructorWithArgument', Mock.STRICT);
    constructornamespace.MyConstructorWithArgument(1).$returns(instance);
    constructornamespace.MyConstructorWithArgument(2).$returns(instance);
    constructornamespace.MyConstructorWithArgument.$replay();
    assertThrowsJsUnitException(() => {
      new constructornamespace.MyConstructorWithArgument(2);
    });
    constructornamespace.MyConstructorWithArgument.$tearDown();

    testing.createConstructorMock(
        constructornamespace, 'MyConstructorWithArgument', Mock.LOOSE);
    constructornamespace.MyConstructorWithArgument(1).$returns(instance);
    constructornamespace.MyConstructorWithArgument(2).$returns(instance);
    constructornamespace.MyConstructorWithArgument.$replay();
    new constructornamespace.MyConstructorWithArgument(2);
    new constructornamespace.MyConstructorWithArgument(1);
    constructornamespace.MyConstructorWithArgument.$verify();
    constructornamespace.MyConstructorWithArgument.$tearDown();
  },
});
