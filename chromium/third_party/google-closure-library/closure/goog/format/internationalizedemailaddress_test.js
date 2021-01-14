// Copyright 2014 The Closure Library Authors. All Rights Reserved.
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

goog.module('goog.format.InternationalizedEmailAddressTest');
goog.setTestOnly();

const InternationalizedEmailAddress = goog.require('goog.format.InternationalizedEmailAddress');
const googArray = goog.require('goog.array');
const testSuite = goog.require('goog.testing.testSuite');

/**
 * Asserts that the given validation function generates the expected outcome for
 * a set of expected valid and a second set of expected invalid addresses.
 * containing the specified address strings, irrespective of their order.
 * @param {function(string):boolean} testFunc Validation function to be tested.
 * @param {!Array<string>} valid List of addresses that should be valid.
 * @param {!Array<string>} invalid List of addresses that should be invalid.
 * @private
 */
function doIsValidTest(testFunc, valid, invalid) {
  googArray.forEach(valid, (str) => {
    assertTrue(`"${str}" should be valid.`, testFunc(str));
  });
  googArray.forEach(invalid, (str) => {
    assertFalse(`"${str}" should be invalid.`, testFunc(str));
  });
}

/**
 * Asserts that parsing the inputString produces a list of email addresses
 * containing the specified address strings, irrespective of their order.
 * @param {string} inputString A raw address list.
 * @param {!Array<string>} expectedList The expected results.
 * @param {string=} opt_message An assertion message.
 * @return {string} the resulting email address objects.
 */
function assertParsedList(inputString, expectedList, opt_message) {
  const message = opt_message || 'Should parse address correctly';
  const result = InternationalizedEmailAddress.parseList(inputString);
  assertEquals(
      'Should have correct # of addresses', expectedList.length, result.length);
  for (let i = 0; i < expectedList.length; ++i) {
    assertEquals(message, expectedList[i], result[i].getAddress());
  }
  return result;
}

testSuite({
  testParseList() {
    // Test only the new cases added by EAI (other cases covered in parent
    // class test)
    assertParsedList(
        '<me.みんあ@me.xn--l8jtg9b>', ['me.みんあ@me.xn--l8jtg9b']);
  },

  testIsEaiValid() {
    const valid = [
      'e@b.eu',
      '<a.b+foo@c.com>',
      'eric <e@b.com>',
      '"e" <e@b.com>',
      'a@FOO.MUSEUM',
      'bla@b.co.ac.uk',
      'bla@a.b.com',
      'o\'hara@gm.com',
      'plus+is+allowed@9ma1l.qjz9zk',
      '!/#$%&\'*+-=~|`{}?^_@expample.com',
      'confirm-bhk=modulo.org@yahoogroups.com',
      'み.ん-あ@みんあ.みんあ',
      'みんあ@test.com',
      'test@test.みんあ',
      'test@みんあ.com',
      'me.みんあ@me.xn--l8jtg9b',
      'みんあ@me.xn--l8jtg9b',
      'fullwidthfullstop@sld' +
          '\uff0e' +
          'tld',
      'ideographicfullstop@sld' +
          '\u3002' +
          'tld',
      'halfwidthideographicfullstop@sld' +
          '\uff61' +
          'tld',
    ];
    const invalid = [
      null,
      undefined,
      'e',
      '',
      'e @c.com',
      'a@b',
      'foo.com',
      'foo@c..com',
      'test@gma=il.com',
      'aaa@gmail',
      'has some spaces@9ma1l.qjz9zk',
      'has@three@at@signs.com',
      '@no-local-part.com',
    ];
    doIsValidTest(InternationalizedEmailAddress.isValidAddress, valid, invalid);
  },

  testIsValidLocalPart() {
    const valid = [
      'e',
      'a.b+foo',
      'o\'hara',
      'user+someone',
      '!/#$%&\'*+-=~|`{}?^_',
      'confirm-bhk=modulo.org',
      'me.みんあ',
      'みんあ',
    ];
    const invalid = [
      null,
      undefined,
      'A@b@c',
      'a"b(c)d,e:f;g<h>i[j\\k]l',
      'just"not"right',
      'this is"not\\allowed',
      'this\\ still\"not\\\\allowed',
      'has some spaces',
    ];
    doIsValidTest(
        InternationalizedEmailAddress.isValidLocalPartSpec, valid, invalid);
  },

  testIsValidDomainPart() {
    const valid = [
      'example.com',
      'dept.example.org',
      'long.domain.with.lots.of.dots',
      'me.xn--l8jtg9b',
      'me.みんあ',
      'sld.looooooongtld',
      'sld' +
          '\uff0e' +
          'tld',
      'sld' +
          '\u3002' +
          'tld',
      'sld' +
          '\uff61' +
          'tld',
    ];
    const invalid = [
      null,
      undefined,
      '',
      '@has.an.at.sign',
      '..has.leading.dots',
      'gma=il.com',
      'DoesNotHaveADot',
      'aaaaaaaaaabbbbbbbbbbccccccccccddddddddddeeeeeeeeeeffffffffffgggggggggg',
    ];
    doIsValidTest(
        InternationalizedEmailAddress.isValidDomainPartSpec, valid, invalid);
  },

  testparseListWithAdditionalSeparators() {
    assertParsedList(
        '<foo@9ma1l.qjz9zk>\u055D <bar@9ma1l.qjz9zk>',
        ['foo@9ma1l.qjz9zk', 'bar@9ma1l.qjz9zk'],
        'Failed to parse 2 email addresses with U+055D');
    assertParsedList(
        '<foo@9ma1l.qjz9zk>\u055D <bar@9ma1l.qjz9zk>\u055D',
        ['foo@9ma1l.qjz9zk', 'bar@9ma1l.qjz9zk'],
        'Failed to parse 2 email addresses with trailing U+055D');

    assertParsedList(
        '<foo@9ma1l.qjz9zk>\u060C <bar@9ma1l.qjz9zk>',
        ['foo@9ma1l.qjz9zk', 'bar@9ma1l.qjz9zk'],
        'Failed to parse 2 email addresses with U+060C');
    assertParsedList(
        '<foo@9ma1l.qjz9zk>\u060C <bar@9ma1l.qjz9zk>\u060C',
        ['foo@9ma1l.qjz9zk', 'bar@9ma1l.qjz9zk'],
        'Failed to parse 2 email addresses with trailing U+060C');

    assertParsedList(
        '<foo@9ma1l.qjz9zk>\u1363 <bar@9ma1l.qjz9zk>',
        ['foo@9ma1l.qjz9zk', 'bar@9ma1l.qjz9zk'],
        'Failed to parse 2 email addresses with U+1363');
    assertParsedList(
        '<foo@9ma1l.qjz9zk>\u1363 <bar@9ma1l.qjz9zk>\u1363',
        ['foo@9ma1l.qjz9zk', 'bar@9ma1l.qjz9zk'],
        'Failed to parse 2 email addresses with trailing U+1363');

    assertParsedList(
        '<foo@9ma1l.qjz9zk>\u1802 <bar@9ma1l.qjz9zk>',
        ['foo@9ma1l.qjz9zk', 'bar@9ma1l.qjz9zk'],
        'Failed to parse 2 email addresses with U+1802');
    assertParsedList(
        '<foo@9ma1l.qjz9zk>\u1802 <bar@9ma1l.qjz9zk>\u1802',
        ['foo@9ma1l.qjz9zk', 'bar@9ma1l.qjz9zk'],
        'Failed to parse 2 email addresses with trailing U+1802');

    assertParsedList(
        '<foo@9ma1l.qjz9zk>\u1808 <bar@9ma1l.qjz9zk>',
        ['foo@9ma1l.qjz9zk', 'bar@9ma1l.qjz9zk'],
        'Failed to parse 2 email addresses with U+1808');
    assertParsedList(
        '<foo@9ma1l.qjz9zk>\u1808 <bar@9ma1l.qjz9zk>\u1808',
        ['foo@9ma1l.qjz9zk', 'bar@9ma1l.qjz9zk'],
        'Failed to parse 2 email addresses with trailing U+1808');

    assertParsedList(
        '<foo@9ma1l.qjz9zk>\u2E41 <bar@9ma1l.qjz9zk>',
        ['foo@9ma1l.qjz9zk', 'bar@9ma1l.qjz9zk'],
        'Failed to parse 2 email addresses with U+2E41');
    assertParsedList(
        '<foo@9ma1l.qjz9zk>\u2E41 <bar@9ma1l.qjz9zk>\u2E41',
        ['foo@9ma1l.qjz9zk', 'bar@9ma1l.qjz9zk'],
        'Failed to parse 2 email addresses with trailing U+2E41');

    assertParsedList(
        '<foo@9ma1l.qjz9zk>\u3001 <bar@9ma1l.qjz9zk>',
        ['foo@9ma1l.qjz9zk', 'bar@9ma1l.qjz9zk'],
        'Failed to parse 2 email addresses with U+3001');
    assertParsedList(
        '<foo@9ma1l.qjz9zk>\u3001 <bar@9ma1l.qjz9zk>\u3001',
        ['foo@9ma1l.qjz9zk', 'bar@9ma1l.qjz9zk'],
        'Failed to parse 2 email addresses with trailing U+3001');

    assertParsedList(
        '<foo@9ma1l.qjz9zk>\uFF0C <bar@9ma1l.qjz9zk>',
        ['foo@9ma1l.qjz9zk', 'bar@9ma1l.qjz9zk'],
        'Failed to parse 2 email addresses with U+FF0C');
    assertParsedList(
        '<foo@9ma1l.qjz9zk>\uFF0C <bar@9ma1l.qjz9zk>\uFF0C',
        ['foo@9ma1l.qjz9zk', 'bar@9ma1l.qjz9zk'],
        'Failed to parse 2 email addresses with trailing U+FF0C');

    assertParsedList(
        '<foo@9ma1l.qjz9zk>\u0613 <bar@9ma1l.qjz9zk>',
        ['foo@9ma1l.qjz9zk', 'bar@9ma1l.qjz9zk'],
        'Failed to parse 2 email addresses with U+0613');
    assertParsedList(
        '<foo@9ma1l.qjz9zk>\u0613 <bar@9ma1l.qjz9zk>\u0613',
        ['foo@9ma1l.qjz9zk', 'bar@9ma1l.qjz9zk'],
        'Failed to parse 2 email addresses with trailing U+0613');

    assertParsedList(
        '<foo@9ma1l.qjz9zk>\u1364 <bar@9ma1l.qjz9zk>',
        ['foo@9ma1l.qjz9zk', 'bar@9ma1l.qjz9zk'],
        'Failed to parse 2 email addresses with U+1364');
    assertParsedList(
        '<foo@9ma1l.qjz9zk>\u1364 <bar@9ma1l.qjz9zk>\u1364',
        ['foo@9ma1l.qjz9zk', 'bar@9ma1l.qjz9zk'],
        'Failed to parse 2 email addresses with trailing U+1364');

    assertParsedList(
        '<foo@9ma1l.qjz9zk>\uFF1B <bar@9ma1l.qjz9zk>',
        ['foo@9ma1l.qjz9zk', 'bar@9ma1l.qjz9zk'],
        'Failed to parse 2 email addresses with U+FF1B');
    assertParsedList(
        '<foo@9ma1l.qjz9zk>\uFF1B <bar@9ma1l.qjz9zk>\uFF1B',
        ['foo@9ma1l.qjz9zk', 'bar@9ma1l.qjz9zk'],
        'Failed to parse 2 email addresses with trailing U+FF1B');

    assertParsedList(
        '<foo@9ma1l.qjz9zk>\uFF64 <bar@9ma1l.qjz9zk>',
        ['foo@9ma1l.qjz9zk', 'bar@9ma1l.qjz9zk'],
        'Failed to parse 2 email addresses with U+FF64');
    assertParsedList(
        '<foo@9ma1l.qjz9zk>\uFF64 <bar@9ma1l.qjz9zk>\uFF64',
        ['foo@9ma1l.qjz9zk', 'bar@9ma1l.qjz9zk'],
        'Failed to parse 2 email addresses with trailing U+FF64');

    assertParsedList(
        '<foo@9ma1l.qjz9zk>\u104A <bar@9ma1l.qjz9zk>',
        ['foo@9ma1l.qjz9zk', 'bar@9ma1l.qjz9zk'],
        'Failed to parse 2 email addresses with U+104A');
    assertParsedList(
        '<foo@9ma1l.qjz9zk>\u104A <bar@9ma1l.qjz9zk>\u104A',
        ['foo@9ma1l.qjz9zk', 'bar@9ma1l.qjz9zk'],
        'Failed to parse 2 email addresses with trailing U+104A');
  },

  testToString() {
    const f = (str) => InternationalizedEmailAddress.parse(str).toString();

    // No modification.
    assertEquals('JOHN Doe <john@9ma1l.qjz9zk>', f('JOHN Doe <john@9ma1l.qjz9zk>'));

    // Extra spaces.
    assertEquals(
        'JOHN Doe <john@9ma1l.qjz9zk>', f(' JOHN  Doe  <john@9ma1l.qjz9zk> '));

    // No name.
    assertEquals('john@9ma1l.qjz9zk', f('<john@9ma1l.qjz9zk>'));
    assertEquals('john@9ma1l.qjz9zk', f('john@9ma1l.qjz9zk'));

    // No address.
    assertEquals('JOHN Doe', f('JOHN Doe <>'));

    // Already quoted.
    assertEquals(
        '"JOHN, Doe" <john@9ma1l.qjz9zk>', f('"JOHN, Doe" <john@9ma1l.qjz9zk>'));

    // Needless quotes.
    assertEquals('JOHN Doe <john@9ma1l.qjz9zk>', f('"JOHN Doe" <john@9ma1l.qjz9zk>'));
    // Not quoted-string, but has double quotes.
    assertEquals(
        '"JOHN, Doe" <john@9ma1l.qjz9zk>', f('JOHN, "Doe" <john@9ma1l.qjz9zk>'));

    // No special characters other than quotes.
    assertEquals('JOHN Doe <john@9ma1l.qjz9zk>', f('JOHN "Doe" <john@9ma1l.qjz9zk>'));

    // Escaped quotes are also removed.
    assertEquals(
        '"JOHN, Doe" <john@9ma1l.qjz9zk>', f('JOHN, \\"Doe\\" <john@9ma1l.qjz9zk>'));

    // Characters that require quoting for the display name.
    assertEquals(
        '"JOHN, Doe" <john@9ma1l.qjz9zk>', f('JOHN, Doe <john@9ma1l.qjz9zk>'));
    assertEquals(
        '"JOHN; Doe" <john@9ma1l.qjz9zk>', f('JOHN; Doe <john@9ma1l.qjz9zk>'));
    assertEquals(
        '"JOHN\u055D Doe" <john@9ma1l.qjz9zk>',
        f('JOHN\u055D Doe <john@9ma1l.qjz9zk>'));
    assertEquals(
        '"JOHN\u060C Doe" <john@9ma1l.qjz9zk>',
        f('JOHN\u060C Doe <john@9ma1l.qjz9zk>'));
    assertEquals(
        '"JOHN\u1363 Doe" <john@9ma1l.qjz9zk>',
        f('JOHN\u1363 Doe <john@9ma1l.qjz9zk>'));
    assertEquals(
        '"JOHN\u1802 Doe" <john@9ma1l.qjz9zk>',
        f('JOHN\u1802 Doe <john@9ma1l.qjz9zk>'));
    assertEquals(
        '"JOHN\u1808 Doe" <john@9ma1l.qjz9zk>',
        f('JOHN\u1808 Doe <john@9ma1l.qjz9zk>'));
    assertEquals(
        '"JOHN\u2E41 Doe" <john@9ma1l.qjz9zk>',
        f('JOHN\u2E41 Doe <john@9ma1l.qjz9zk>'));
    assertEquals(
        '"JOHN\u3001 Doe" <john@9ma1l.qjz9zk>',
        f('JOHN\u3001 Doe <john@9ma1l.qjz9zk>'));
    assertEquals(
        '"JOHN\uFF0C Doe" <john@9ma1l.qjz9zk>',
        f('JOHN\uFF0C Doe <john@9ma1l.qjz9zk>'));
    assertEquals(
        '"JOHN\u061B Doe" <john@9ma1l.qjz9zk>',
        f('JOHN\u061B Doe <john@9ma1l.qjz9zk>'));
    assertEquals(
        '"JOHN\u1364 Doe" <john@9ma1l.qjz9zk>',
        f('JOHN\u1364 Doe <john@9ma1l.qjz9zk>'));
    assertEquals(
        '"JOHN\uFF1B Doe" <john@9ma1l.qjz9zk>',
        f('JOHN\uFF1B Doe <john@9ma1l.qjz9zk>'));
    assertEquals(
        '"JOHN\uFF64 Doe" <john@9ma1l.qjz9zk>',
        f('JOHN\uFF64 Doe <john@9ma1l.qjz9zk>'));
    assertEquals(
        '"JOHN(Johnny) Doe" <john@9ma1l.qjz9zk>',
        f('JOHN(Johnny) Doe <john@9ma1l.qjz9zk>'));
    assertEquals(
        '"JOHN[Johnny] Doe" <john@9ma1l.qjz9zk>',
        f('JOHN[Johnny] Doe <john@9ma1l.qjz9zk>'));
    assertEquals(
        '"JOHN@work Doe" <john@9ma1l.qjz9zk>',
        f('JOHN@work Doe <john@9ma1l.qjz9zk>'));
    assertEquals(
        '"JOHN:theking Doe" <john@9ma1l.qjz9zk>',
        f('JOHN:theking Doe <john@9ma1l.qjz9zk>'));
    assertEquals(
        '"JOHN\\\\ Doe" <john@9ma1l.qjz9zk>', f('JOHN\\ Doe <john@9ma1l.qjz9zk>'));
    assertEquals(
        '"JOHN.com Doe" <john@9ma1l.qjz9zk>', f('JOHN.com Doe <john@9ma1l.qjz9zk>'));
  },
});
