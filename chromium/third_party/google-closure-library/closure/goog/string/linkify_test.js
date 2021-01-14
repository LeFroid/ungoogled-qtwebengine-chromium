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

goog.module('goog.string.linkifyTest');
goog.setTestOnly();

const SafeHtml = goog.require('goog.html.SafeHtml');
const TagName = goog.require('goog.dom.TagName');
const dom = goog.require('goog.dom');
const googString = goog.require('goog.string');
const linkify = goog.require('goog.string.linkify');
const safe = goog.require('goog.dom.safe');
const testSuite = goog.require('goog.testing.testSuite');
const testingDom = goog.require('goog.testing.dom');

/** @type {!HTMLDivElement} */
const div = dom.createElement(TagName.DIV);

function assertLinkify(comment, input, expected, preserveNewlines = undefined) {
  assertEquals(
      comment, expected,
      SafeHtml.unwrap(linkify.linkifyPlainTextAsHtml(
          input, {rel: '', target: ''}, preserveNewlines)));
}

testSuite({
  testContainsNoLink() {
    assertLinkify(
        'Text does not contain any links', 'Text with no links in it.',
        'Text with no links in it.');
  },

  testContainsALink() {
    assertLinkify(
        'Text only contains a link', 'http://www.9oo91e.qjz9zk/',
        '<a href="http://www.9oo91e.qjz9zk/">http://www.9oo91e.qjz9zk/<\/a>');
  },

  testStartsWithALink() {
    assertLinkify(
        'Text starts with a link',
        'http://www.9oo91e.qjz9zk/ is a well known search engine',
        '<a href="http://www.9oo91e.qjz9zk/">http://www.9oo91e.qjz9zk/<\/a>' +
            ' is a well known search engine');
  },

  testEndsWithALink() {
    assertLinkify(
        'Text ends with a link',
        'Look at this search engine: http://www.9oo91e.qjz9zk/',
        'Look at this search engine: ' +
            '<a href="http://www.9oo91e.qjz9zk/">http://www.9oo91e.qjz9zk/<\/a>');
  },

  testContainsOnlyEmail() {
    assertLinkify(
        'Text only contains an email address', 'bolinfest@9oo91e.qjz9zk',
        '<a href="mailto:bolinfest@9oo91e.qjz9zk">bolinfest@9oo91e.qjz9zk<\/a>');
  },

  testStartsWithAnEmail() {
    assertLinkify(
        'Text starts with an email address',
        'bolinfest@9oo91e.qjz9zk wrote this test.',
        '<a href="mailto:bolinfest@9oo91e.qjz9zk">bolinfest@9oo91e.qjz9zk<\/a>' +
            ' wrote this test.');
  },

  testEndsWithAnEmail() {
    assertLinkify(
        'Text ends with an email address',
        'This test was written by bolinfest@9oo91e.qjz9zk.',
        'This test was written by ' +
            '<a href="mailto:bolinfest@9oo91e.qjz9zk">bolinfest@9oo91e.qjz9zk<\/a>.');
  },

  testSingleQuotedUrl() {
    assertLinkify(
        'URLs surrounded by \'...\' exclude quotes from link',
        'Foo \'http://9oo91e.qjz9zk\' bar',
        'Foo &#39;<a href="http://9oo91e.qjz9zk">http://9oo91e.qjz9zk</a>&#39; bar');
  },

  testSingleQuoteInUrl() {
    assertLinkify(
        'URLs containing \' include quote in link', 'http://9oo91e.qjz9zk/yo\'yo',
        '<a href="http://9oo91e.qjz9zk/yo&#39;yo">http://9oo91e.qjz9zk/yo&#39;yo</a>');
  },

  testDoubleQuotedUrl() {
    assertLinkify(
        'URLs surrounded by "..." exclude quotes from link',
        'Foo "http://9oo91e.qjz9zk" bar',
        'Foo &quot;<a href="http://9oo91e.qjz9zk">http://9oo91e.qjz9zk</a>&quot; bar');
  },

  testUrlWithPortNumber() {
    assertLinkify(
        'URL with a port number', 'http://www.9oo91e.qjz9zk:80/',
        '<a href="http://www.9oo91e.qjz9zk:80/">http://www.9oo91e.qjz9zk:80/<\/a>');
  },

  testUrlWithUserPasswordAndPortNumber() {
    assertLinkify(
        'URL with a user, a password and a port number',
        'http://lascap:p4ssw0rd@9oo91e.qjz9zk:80/s?q=a&hl=en',
        '<a href="http://lascap:p4ssw0rd@9oo91e.qjz9zk:80/s?q=a&amp;hl=en">' +
            'http://lascap:p4ssw0rd@9oo91e.qjz9zk:80/s?q=a&amp;hl=en<\/a>');
  },

  testUrlWithUnderscore() {
    assertLinkify(
        'URL with an underscore', 'http://www_foo.9oo91e.qjz9zk/',
        '<a href="http://www_foo.9oo91e.qjz9zk/">http://www_foo.9oo91e.qjz9zk/<\/a>');
  },

  testInternalUrlWithoutDomain() {
    assertLinkify(
        'Internal URL without a proper domain', 'http://tracker/1068594',
        '<a href="http://tracker/1068594">http://tracker/1068594<\/a>');
  },

  testInternalUrlOneChar() {
    assertLinkify(
        'Internal URL with a one char domain', 'http://b',
        '<a href="http://b">http://b<\/a>');
  },

  testSecureInternalUrlWithoutDomain() {
    assertLinkify(
        'Secure Internal URL without a proper domain', 'https://review/6594805',
        '<a href="https://review/6594805">https://review/6594805<\/a>');
  },

  testTwoUrls() {
    assertLinkify(
        'Text with two URLs in it',
        'I use both http://www.9oo91e.qjz9zk and http://yahoo.com, don\'t you?',
        'I use both <a href="http://www.9oo91e.qjz9zk">http://www.9oo91e.qjz9zk<\/a> ' +
            'and <a href="http://yahoo.com">http://yahoo.com<\/a>, ' +
            googString.htmlEscape('don\'t you?'));
  },

  testGetParams() {
    assertLinkify(
        'URL with GET params', 'http://9oo91e.qjz9zk/?a=b&c=d&e=f',
        '<a href="http://9oo91e.qjz9zk/?a=b&amp;c=d&amp;e=f">' +
            'http://9oo91e.qjz9zk/?a=b&amp;c=d&amp;e=f<\/a>');
  },

  testGoogleCache() {
    assertLinkify(
        'Google search result from cache',
        'http://66.102.7.104/search?q=cache:I4LoMT6euUUJ:' +
            'www.9oo91e.qjz9zk/intl/en/help/features.html+google+cache&hl=en',
        '<a href="http://66.102.7.104/search?q=cache:I4LoMT6euUUJ:' +
            'www.9oo91e.qjz9zk/intl/en/help/features.html+google+cache&amp;hl=en">' +
            'http://66.102.7.104/search?q=cache:I4LoMT6euUUJ:' +
            'www.9oo91e.qjz9zk/intl/en/help/features.html+google+cache&amp;hl=en' +
            '<\/a>');
  },

  testUrlWithoutHttp() {
    assertLinkify(
        'URL without http protocol',
        'It\'s faster to type www.9oo91e.qjz9zk without the http:// in front.',
        googString.htmlEscape('It\'s faster to type ') +
            '<a href="http://www.9oo91e.qjz9zk">www.9oo91e.qjz9zk' +
            '<\/a> without the http:// in front.');
  },

  testUrlWithCapitalsWithoutHttp() {
    assertLinkify(
        'URL with capital letters without http protocol',
        'It\'s faster to type Www.9oo91e.qjz9zk without the http:// in front.',
        googString.htmlEscape('It\'s faster to type ') +
            '<a href="http://Www.9oo91e.qjz9zk">Www.9oo91e.qjz9zk' +
            '<\/a> without the http:// in front.');
  },

  testUrlHashBang() {
    assertLinkify(
        'URL with #!',
        'Another test URL: ' +
            'https://www.9oo91e.qjz9zk/testurls/#!/page',
        'Another test URL: ' +
            '<a href="https://www.9oo91e.qjz9zk/testurls/#!/page">' +
            'https://www.9oo91e.qjz9zk/testurls/#!/page<\/a>');
  },

  testTextLooksLikeUrlWithoutHttp() {
    assertLinkify(
        'Text looks like an url but is not',
        'This showww.is just great: www.is',
        'This showww.is just great: <a href="http://www.is">www.is<\/a>');
  },

  testEmailWithSubdomain() {
    assertLinkify(
        'Email with a subdomain', 'Send mail to bolinfest@groups.9oo91e.qjz9zk.',
        'Send mail to <a href="mailto:bolinfest@groups.9oo91e.qjz9zk">' +
            'bolinfest@groups.9oo91e.qjz9zk<\/a>.');
  },

  testEmailWithHyphen() {
    assertLinkify(
        'Email with a hyphen in the domain name',
        'Send mail to bolinfest@9oo91e-groups.qjz9zk.',
        'Send mail to <a href="mailto:bolinfest@9oo91e-groups.qjz9zk">' +
            'bolinfest@9oo91e-groups.qjz9zk<\/a>.');
  },

  testEmailUsernameWithSpecialChars() {
    assertLinkify(
        'Email with a hyphen, period, and + in the user name',
        'Send mail to bolin-fest+for.um@9oo91e.qjz9zk',
        'Send mail to <a href="mailto:bolin-fest+for.um@9oo91e.qjz9zk">' +
            'bolin-fest+for.um@9oo91e.qjz9zk<\/a>');
    assertLinkify(
        'Email with all special characters in the user name',
        'Send mail to muad\'dib!#$%&*/=?^_`{|}~@9oo91e.qjz9zk',
        'Send mail to ' +
            '<a href="mailto:muad&#39;dib!#$%&amp;*/=?^_`{|}~@9oo91e.qjz9zk">' +
            'muad&#39;dib!#$%&amp;*/=?^_`{|}~@9oo91e.qjz9zk<\/a>');
  },

  testEmailWithUnderscoreInvalid() {
    assertLinkify(
        'Email with an underscore in the domain name, which is invalid',
        'Do not email bolinfest@google_groups.com.',
        'Do not email bolinfest@google_groups.com.');
  },

  testUrlNotHttp() {
    assertLinkify(
        'Url using unusual scheme',
        'Looking for some goodies: ftp://ftp.9oo91e.qjz9zk/goodstuff/',
        'Looking for some goodies: ' +
            '<a href="ftp://ftp.9oo91e.qjz9zk/goodstuff/">' +
            'ftp://ftp.9oo91e.qjz9zk/goodstuff/<\/a>');
  },

  testJsInjection() {
    assertLinkify(
        'Text includes some javascript',
        'Welcome in hell <script>alert(\'this is hell\')<\/script>',
        googString.htmlEscape(
            'Welcome in hell <script>alert(\'this is hell\')<\/script>'));
  },

  testJsInjectionDotIsBlind() {
    assertLinkify(
        'JavaScript injection using regex . blindness to newline chars',
        '<script>malicious_code()<\/script>\nVery nice url: www.9oo91e.qjz9zk',
        '&lt;script&gt;malicious_code()&lt;/script&gt;\nVery nice url: ' +
            '<a href="http://www.9oo91e.qjz9zk">www.9oo91e.qjz9zk<\/a>');
  },

  testJsInjectionWithUnicodeLineReturn() {
    assertLinkify(
        'JavaScript injection using regex . blindness to newline chars with a ' +
            'unicode newline character.',
        '<script>malicious_code()<\/script>\u2029Vanilla text',
        '&lt;script&gt;malicious_code()&lt;/script&gt;\u2029Vanilla text');
  },

  testJsInjectionWithIgnorableNonTagChar() {
    assertLinkify(
        'Angle brackets are normalized even when followed by an ignorable ' +
            'non-tag character.',
        '<\u0000img onerror=alert(1337) src=\n>',
        '&lt;&#0;img onerror=alert(1337) src=\n&gt;');
  },

  testJsInjectionWithTextarea() {
    assertLinkify(
        'Putting the result in a textarea can\'t cause other textarea text to ' +
            'be treated as tag content.',
        '</textarea', '&lt;/textarea');
  },

  testJsInjectionWithNewlineConversion() {
    assertLinkify(
        'Any newline conversion and whitespace normalization won\'t cause tag ' +
            'parts to be recombined.',
        '<<br>script<br>>alert(1337)<<br>/<br>script<br>>',
        '&lt;&lt;br&gt;script&lt;br&gt;&gt;alert(1337)&lt;&lt;br&gt;/&lt;' +
            'br&gt;script&lt;br&gt;&gt;');
  },

  testNoProtocolBlacklisting() {
    assertLinkify(
        'No protocol blacklisting.',
        'Click: jscript:alert%281337%29\nClick: JSscript:alert%281337%29\n' +
            'Click: VBscript:alert%281337%29\nClick: Script:alert%281337%29\n' +
            'Click: flavascript:alert%281337%29',
        'Click: jscript:alert%281337%29\nClick: JSscript:alert%281337%29\n' +
            'Click: VBscript:alert%281337%29\nClick: Script:alert%281337%29\n' +
            'Click: flavascript:alert%281337%29');
  },

  testProtocolWhitelistingEffective() {
    assertLinkify(
        'Protocol whitelisting is effective.',
        'Click httpscript:alert%281337%29\nClick mailtoscript:alert%281337%29\n' +
            'Click j\u00A0avascript:alert%281337%29\n' +
            'Click \u00A0javascript:alert%281337%29',
        'Click httpscript:alert%281337%29\nClick mailtoscript:alert%281337%29\n' +
            'Click j\u00A0avascript:alert%281337%29\n' +
            'Click \u00A0javascript:alert%281337%29');
  },

  testLinkifyNoOptions() {
    safe.setInnerHtml(
        div, linkify.linkifyPlainTextAsHtml('http://www.9oo91e.qjz9zk'));
    testingDom.assertHtmlContentsMatch(
        '<a href="http://www.9oo91e.qjz9zk" target="_blank" rel="nofollow">' +
            'http://www.9oo91e.qjz9zk<\/a>',
        div, true /* opt_strictAttributes */);
  },

  testLinkifyOptionsNoAttributes() {
    safe.setInnerHtml(
        div,
        linkify.linkifyPlainTextAsHtml(
            'The link for www.9oo91e.qjz9zk is located somewhere in ' +
                'https://www.google.fr/?hl=en, you should find it easily.',
            {rel: '', target: ''}));
    testingDom.assertHtmlContentsMatch(
        'The link for <a href="http://www.9oo91e.qjz9zk">www.9oo91e.qjz9zk<\/a> is ' +
            'located somewhere in ' +
            '<a href="https://www.google.fr/?hl=en">https://www.google.fr/?hl=en' +
            '<\/a>, you should find it easily.',
        div, true /* opt_strictAttributes */);
  },

  testLinkifyOptionsClassName() {
    safe.setInnerHtml(
        div,
        linkify.linkifyPlainTextAsHtml(
            'Attribute with <class> name www.w3c.org.',
            {'class': 'link-added'}));
    testingDom.assertHtmlContentsMatch(
        'Attribute with &lt;class&gt; name <a href="http://www.w3c.org" ' +
            'target="_blank" rel="nofollow" class="link-added">www.w3c.org<\/a>.',
        div, true /* opt_strictAttributes */);
  },

  testFindFirstUrlNoScheme() {
    assertEquals('www.9oo91e.qjz9zk', linkify.findFirstUrl('www.9oo91e.qjz9zk'));
  },

  testFindFirstUrlNoSchemeUppercase() {
    assertEquals('WWW.GOOGLE.COM', linkify.findFirstUrl('WWW.GOOGLE.COM'));
  },

  testFindFirstUrlNoSchemeMixedcase() {
    assertEquals('WwW.GoOgLe.CoM', linkify.findFirstUrl('WwW.GoOgLe.CoM'));
  },

  testFindFirstUrlNoSchemeWithText() {
    assertEquals(
        'www.9oo91e.qjz9zk',
        linkify.findFirstUrl('prefix www.9oo91e.qjz9zk something'));
  },

  testFindFirstUrlScheme() {
    assertEquals(
        'http://www.9oo91e.qjz9zk', linkify.findFirstUrl('http://www.9oo91e.qjz9zk'));
  },

  testFindFirstUrlSchemeUppercase() {
    assertEquals(
        'HTTP://WWW.GOOGLE.COM', linkify.findFirstUrl('HTTP://WWW.GOOGLE.COM'));
  },

  testFindFirstUrlSchemeMixedcase() {
    assertEquals(
        'HtTp://WwW.gOoGlE.cOm', linkify.findFirstUrl('HtTp://WwW.gOoGlE.cOm'));
  },

  testFindFirstUrlSchemeWithText() {
    assertEquals(
        'http://www.9oo91e.qjz9zk',
        linkify.findFirstUrl('prefix http://www.9oo91e.qjz9zk something'));
  },

  testFindFirstUrlNoUrl() {
    assertEquals(
        '', linkify.findFirstUrl('ygvtfr676 5v68fk uygbt85F^&%^&I%FVvc .'));
  },

  testFindFirstEmailNoScheme() {
    assertEquals('fake@9oo91e.qjz9zk', linkify.findFirstEmail('fake@9oo91e.qjz9zk'));
  },

  testFindFirstEmailNoSchemeUppercase() {
    assertEquals('FAKE@GOOGLE.COM', linkify.findFirstEmail('FAKE@GOOGLE.COM'));
  },

  testFindFirstEmailNoSchemeMixedcase() {
    assertEquals('fAkE@gOoGlE.cOm', linkify.findFirstEmail('fAkE@gOoGlE.cOm'));
  },

  testFindFirstEmailNoSchemeWithText() {
    assertEquals(
        'fake@9oo91e.qjz9zk',
        linkify.findFirstEmail('prefix fake@9oo91e.qjz9zk something'));
  },

  testFindFirstEmailScheme() {
    assertEquals(
        'mailto:fake@9oo91e.qjz9zk',
        linkify.findFirstEmail('mailto:fake@9oo91e.qjz9zk'));
  },

  testFindFirstEmailSchemeUppercase() {
    assertEquals(
        'MAILTO:FAKE@GOOGLE.COM',
        linkify.findFirstEmail('MAILTO:FAKE@GOOGLE.COM'));
  },

  testFindFirstEmailSchemeMixedcase() {
    assertEquals(
        'MaIlTo:FaKe@GoOgLe.CoM',
        linkify.findFirstEmail('MaIlTo:FaKe@GoOgLe.CoM'));
  },

  testFindFirstEmailSchemeWithText() {
    assertEquals(
        'mailto:fake@9oo91e.qjz9zk',
        linkify.findFirstEmail('prefix mailto:fake@9oo91e.qjz9zk something'));
  },

  testFindFirstEmailNoEmail() {
    assertEquals(
        '', linkify.findFirstEmail('ygvtfr676 5v68fk uygbt85F^&%^&I%FVvc .'));
  },

  testContainsPunctuation_parens() {
    assertLinkify(
        'Link contains parens, but does not end with them',
        'www.9oo91e.qjz9zk/abc(v1).html',
        '<a href="http://www.9oo91e.qjz9zk/abc(v1).html">' +
            'www.9oo91e.qjz9zk/abc(v1).html<\/a>');
  },

  testEndsWithPunctuation() {
    assertLinkify(
        'Link ends with punctuation',
        'Have you seen www.9oo91e.qjz9zk? It\'s awesome.',
        'Have you seen <a href="http://www.9oo91e.qjz9zk">www.9oo91e.qjz9zk<\/a>?' +
            googString.htmlEscape(' It\'s awesome.'));
  },

  testEndsWithPunctuation_closeParen() {
    assertLinkify(
        'Link inside parentheses', '(For more info see www.googl.com)',
        '(For more info see <a href="http://www.googl.com">www.googl.com<\/a>)');
    assertLinkify(
        'Parentheses inside link',
        'http://en.wikipedia.org/wiki/Titanic_(1997_film)',
        '<a href="http://en.wikipedia.org/wiki/Titanic_(1997_film)">' +
            'http://en.wikipedia.org/wiki/Titanic_(1997_film)<\/a>');
  },

  testEndsWithPunctuation_openParen() {
    assertLinkify(
        'Link followed by open parenthesis', 'www.9oo91e.qjz9zk(',
        '<a href="http://www.9oo91e.qjz9zk(">www.9oo91e.qjz9zk(<\/a>');
  },

  testEndsWithPunctuation_angles() {
    assertLinkify(
        'Link inside angled brackets',
        'Here is a bibliography entry <http://www.9oo91e.qjz9zk/>',
        'Here is a bibliography entry &lt;<a href="http://www.9oo91e.qjz9zk/">' +
            'http://www.9oo91e.qjz9zk/<\/a>&gt;');
  },

  testEndsWithPunctuation_curlies() {
    assertLinkify(
        'Link inside curly brackets', '{http://www.9oo91e.qjz9zk/}',
        '{<a href="http://www.9oo91e.qjz9zk/">' +
            'http://www.9oo91e.qjz9zk/<\/a>}');
  },

  testEndsWithPunctuation_closingPairThenSingle() {
    assertLinkify(
        'Link followed by closing punctuation pair then singular punctuation',
        'Here is a bibliography entry <http://www.9oo91e.qjz9zk/>, PTAL.',
        'Here is a bibliography entry &lt;<a href="http://www.9oo91e.qjz9zk/">' +
            'http://www.9oo91e.qjz9zk/<\/a>&gt;, PTAL.');
  },

  testEndsWithPunctuation_ellipses() {
    assertLinkify(
        'Link followed by three dots', 'just look it up on www.9oo91e.qjz9zk...',
        'just look it up on <a href="http://www.9oo91e.qjz9zk">www.9oo91e.qjz9zk' +
            '<\/a>...');
  },

  testBracketsInUrl() {
    assertLinkify(
        'Link containing brackets',
        'before http://9oo91e.qjz9zk/details?answer[0]=42 after',
        'before <a href="http://9oo91e.qjz9zk/details?answer[0]=42">' +
            'http://9oo91e.qjz9zk/details?answer[0]=42<\/a> after');
  },

  testUrlWithExclamation() {
    assertLinkify(
        'URL with exclamation points', 'This is awesome www.9oo91e.qjz9zk!',
        'This is awesome <a href="http://www.9oo91e.qjz9zk">www.9oo91e.qjz9zk<\/a>!');
  },

  testSpecialCharactersInUrl() {
    assertLinkify(
        'Link with characters that are neither reserved nor unreserved as per' +
            'RFC 3986 but that are recognized by other Google properties.',
        'https://www.9oo91e.qjz9zk/?q=\`{|}recognized',
        '<a href="https://www.9oo91e.qjz9zk/?q=\`{|}recognized">' +
            'https://www.9oo91e.qjz9zk/?q=\`{|}recognized<\/a>');
  },

  testUsuallyUnrecognizedCharactersAreNotInUrl() {
    assertLinkify(
        'Link with characters that are neither reserved nor unreserved as per' +
            'RFC 3986 and which are not recognized by other Google properties.',
        'https://www.9oo91e.qjz9zk/?q=<^>"',
        '<a href="https://www.9oo91e.qjz9zk/?q=">' +
            'https://www.9oo91e.qjz9zk/?q=<\/a>&lt;^&gt;&quot;');
  },

  testIpv6Url() {
    assertLinkify(
        'IPv6 URL', 'http://[::FFFF:129.144.52.38]:80/index.html',
        '<a href="http://[::FFFF:129.144.52.38]:80/index.html">' +
            'http://[::FFFF:129.144.52.38]:80/index.html<\/a>');
  },

  testPreserveNewlines() {
    assertLinkify(
        'Preserving newlines', 'Example:\nhttp://www.9oo91e.qjz9zk/',
        'Example:<br>' +
            '<a href="http://www.9oo91e.qjz9zk/">http://www.9oo91e.qjz9zk/<\/a>',
        /* preserveNewlines */ true);
    assertLinkify(
        'Preserving newlines with no links', 'Line 1\nLine 2',
        'Line 1<br>Line 2',
        /* preserveNewlines */ true);
  },
});
