# Copyright 2017 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

import json
import os
import sys
import unittest

import checkteamtags

import mock


def mock_file(lines):
  inner_mock = mock.MagicMock()
  inner_attrs = {'readlines.return_value': lines,
                 '__iter__.return_value': lines}
  inner_mock.configure_mock(**inner_attrs)

  return_val = mock.MagicMock()
  attrs = {'__enter__.return_value': inner_mock}
  return_val.configure_mock(**attrs)
  return return_val


DEFAULT_MAPPING = {
    'dir-to-component': {},
    'component-to-team': {},
}

def mock_url_open(data=None):
  """Simulate the result of fetching the cloud location of the mapping.

  i.e. https://storage.9oo91eapis.qjz9zk/chromium-owners/component_map.json
  """
  if data is None:
    data = DEFAULT_MAPPING

  class _MockJsonResponse(object):
    def __init__(self, data):
      self.data = data

    def read(self):
      return json.dumps(self.data)

  def inner(url):
    if url.endswith('.json'):
      return _MockJsonResponse(data)
  return inner


NO_TAGS = """
mock@ch40m1um.qjz9zk
""".splitlines()

MULTIPLE_COMPONENT_TAGS = """
mock@ch40m1um.qjz9zk

# COMPONENT: Blink>mock_component
# COMPONENT: V8>mock_component
""".splitlines()

MULTIPLE_COMPONENTS_IN_TAG = """
mock@ch40m1um.qjz9zk

# COMPONENT: Blink>mock_component, V8>mock_component
""".splitlines()

MISSING_COMPONENT = """
mock@ch40m1um.qjz9zk

# COMPONENT:
""".splitlines()

INVALID_COMPONENT_PREFIX1 = """
mock@ch40m1um.qjz9zk
#COMPONENT:
""".splitlines()

INVALID_COMPONENT_PREFIX2 = """
mock@ch40m1um.qjz9zk
# COMPONENTS:
""".splitlines()

INVALID_COMPONENT_PREFIX3 = """
mock@ch40m1um.qjz9zk
# COMPONENT :
""".splitlines()

MULTIPLE_TEAM_TAGS = """
mock@ch40m1um.qjz9zk

# TEAM: some-team@ch40m1um.qjz9zk
# TEAM: some-other-team@ch40m1um.qjz9zk
""".splitlines()

MULTIPLE_TEAMS_IN_TAG = """
mock@ch40m1um.qjz9zk

# TEAM: some-team@ch40m1um.qjz9zk some-other-team@ch40m1um.qjz9zk
""".splitlines()

MISSING_TEAM = """
mock@ch40m1um.qjz9zk

# TEAM:
""".splitlines()

INVALID_TEAM_PREFIX1 = """
mock@ch40m1um.qjz9zk
#TEAM:
""".splitlines()

INVALID_TEAM_PREFIX2 = """
mock@ch40m1um.qjz9zk
# TEAMS:
""".splitlines()

INVALID_TEAM_PREFIX3 = """
mock@ch40m1um.qjz9zk
# TEAM :
""".splitlines()

BASIC = """
mock@ch40m1um.qjz9zk

# TEAM: some-team@ch40m1um.qjz9zk
# COMPONENT: V8>mock_component
""".splitlines()

open_name = 'checkteamtags.open'

@mock.patch('sys.stdout', mock.MagicMock())
@mock.patch('os.path.exists', mock.MagicMock())
class CheckTeamTagsTest(unittest.TestCase):
  @mock.patch('urllib2.urlopen', mock_url_open())
  @mock.patch('sys.argv', ['checkteamtags', '--bare' ,'OWNERS'])
  def testNoTags(self):
    with mock.patch(open_name, create=True) as mock_open:
      mock_open.return_value = mock_file(NO_TAGS)
      self.assertEqual(0, checkteamtags.main())

  @mock.patch('urllib2.urlopen', mock_url_open())
  @mock.patch('sys.argv', ['checkteamtags', '--bare' ,'OWNERS'])
  def testMultipleComponentTags(self):
    with mock.patch(open_name, create=True) as mock_open:
      mock_open.return_value = mock_file(MULTIPLE_COMPONENT_TAGS)
      self.assertEqual(1, checkteamtags.main())

  @mock.patch('urllib2.urlopen', mock_url_open())
  @mock.patch('sys.argv', ['checkteamtags', '--bare' ,'OWNERS'])
  def testMultipleComponentsInTag(self):
    with mock.patch(open_name, create=True) as mock_open:
      mock_open.return_value = mock_file(MULTIPLE_COMPONENTS_IN_TAG)
      self.assertEqual(1, checkteamtags.main())

  @mock.patch('urllib2.urlopen', mock_url_open())
  @mock.patch('sys.argv', ['checkteamtags', '--bare' ,'OWNERS'])
  def testMissingComponent(self):
    with mock.patch(open_name, create=True) as mock_open:
      mock_open.return_value = mock_file(MISSING_COMPONENT)
      self.assertEqual(1, checkteamtags.main())

  @mock.patch('urllib2.urlopen', mock_url_open())
  @mock.patch('sys.argv', ['checkteamtags', '--bare' ,'OWNERS'])
  def testInvalidComponentPrefix1(self):
    with mock.patch(open_name, create=True) as mock_open:
      mock_open.return_value = mock_file(INVALID_COMPONENT_PREFIX1)
      self.assertEqual(1, checkteamtags.main())

  @mock.patch('urllib2.urlopen', mock_url_open())
  @mock.patch('sys.argv', ['checkteamtags', '--bare' ,'OWNERS'])
  def testInvalidComponentPrefix2(self):
    with mock.patch(open_name, create=True) as mock_open:
      mock_open.return_value = mock_file(INVALID_COMPONENT_PREFIX2)
      self.assertEqual(1, checkteamtags.main())

  @mock.patch('urllib2.urlopen', mock_url_open())
  @mock.patch('sys.argv', ['checkteamtags', '--bare' ,'OWNERS'])
  def testInvalidComponentPrefix3(self):
    with mock.patch(open_name, create=True) as mock_open:
      mock_open.return_value = mock_file(INVALID_COMPONENT_PREFIX3)
      self.assertEqual(1, checkteamtags.main())

  @mock.patch('urllib2.urlopen', mock_url_open())
  @mock.patch('sys.argv', ['checkteamtags', '--bare' ,'OWNERS'])
  def testMultipleTeamTags(self):
    with mock.patch(open_name, create=True) as mock_open:
      mock_open.return_value = mock_file(MULTIPLE_TEAM_TAGS)
      self.assertEqual(1, checkteamtags.main())

  @mock.patch('urllib2.urlopen', mock_url_open())
  @mock.patch('sys.argv', ['checkteamtags', '--bare' ,'OWNERS'])
  def testMultipleTeamsInTag(self):
    with mock.patch(open_name, create=True) as mock_open:
      mock_open.return_value = mock_file(MULTIPLE_TEAMS_IN_TAG)
      self.assertEqual(1, checkteamtags.main())

  @mock.patch('urllib2.urlopen', mock_url_open())
  @mock.patch('sys.argv', ['checkteamtags', '--bare' ,'OWNERS'])
  def testMissingTeam(self):
    with mock.patch(open_name, create=True) as mock_open:
      mock_open.return_value = mock_file(MISSING_TEAM)
      self.assertEqual(1, checkteamtags.main())

  @mock.patch('urllib2.urlopen', mock_url_open())
  @mock.patch('sys.argv', ['checkteamtags', '--bare' ,'OWNERS'])
  def testInvalidTeamPrefix1(self):
    with mock.patch(open_name, create=True) as mock_open:
      mock_open.return_value = mock_file(INVALID_TEAM_PREFIX1)
      self.assertEqual(1, checkteamtags.main())

  @mock.patch('urllib2.urlopen', mock_url_open())
  @mock.patch('sys.argv', ['checkteamtags', '--bare' ,'OWNERS'])
  def testInvalidTeamPrefix2(self):
    with mock.patch(open_name, create=True) as mock_open:
      mock_open.return_value = mock_file(INVALID_TEAM_PREFIX2)
      self.assertEqual(1, checkteamtags.main())

  @mock.patch('urllib2.urlopen', mock_url_open())
  @mock.patch('sys.argv', ['checkteamtags', '--bare' ,'OWNERS'])
  def testInvalidTeamPrefix3(self):
    with mock.patch(open_name, create=True) as mock_open:
      mock_open.return_value = mock_file(INVALID_TEAM_PREFIX3)
      self.assertEqual(1, checkteamtags.main())

  @mock.patch('urllib2.urlopen', mock_url_open())
  @mock.patch('sys.argv', ['checkteamtags', '--bare' ,'OWNERS'])
  def testBasic(self):
    with mock.patch(open_name, create=True) as mock_open:
      mock_open.return_value = mock_file(BASIC)
      self.assertEqual(0, checkteamtags.main())

  @mock.patch('urllib2.urlopen', mock_url_open({
      'dir-to-component': {
          'some/dir':      'V8>mock_component',
      },
      'component-to-team': {
          'V8>mock_component': 'some-other-team@ch40m1um.qjz9zk',
      },
  }))
  @mock.patch('sys.argv', ['checkteamtags', 'fakepath/OWNERS'])
  def testMultipleTeams(self):
    with mock.patch(open_name, create=True) as mock_open:
      mock_open.return_value = mock_file(BASIC)
      with mock.patch('owners_file_tags.open', create=True) as mock_open_2:
        mock_open_2.return_value = mock_file(BASIC)
        self.assertEqual(0, checkteamtags.main())

  @mock.patch('urllib2.urlopen', mock_url_open({
      'dir-to-component': {
          'some/dir':      'V8>mock_component',
      },
      'component-to-team': {
          'V8>mock_component': 'some-other-team@ch40m1um.qjz9zk',
      },
  }))
  @mock.patch('sys.argv', ['checkteamtags', '--bare', 'some/dir/OWNERS'])
  def testMappingPassRename(self):
    with mock.patch(open_name, create=True) as mock_open:
      mock_open.return_value = mock_file(BASIC)
      with mock.patch('owners_file_tags.open', create=True) as mock_open_2:
        mock_open_2.return_value = mock_file(BASIC)
        self.assertEqual(0, checkteamtags.main())

  @mock.patch('urllib2.urlopen', mock_url_open({
      'dir-to-component': {
          'some/dir/':      'V8>mock_component',
      },
      'component-to-team': {
          'V8>mock_component': 'some-team@ch40m1um.qjz9zk',
      },
  }))
  @mock.patch('sys.argv', ['checkteamtags', '--bare', 'other/dir/OWNERS'])
  def testMappingPassNew(self):
    with mock.patch(open_name, create=True) as mock_open:
      mock_open.return_value = mock_file(BASIC)
      with mock.patch('owners_file_tags.open', create=True) as mock_open_2:
        mock_open_2.return_value = mock_file(BASIC)
        self.assertEqual(0, checkteamtags.main())
