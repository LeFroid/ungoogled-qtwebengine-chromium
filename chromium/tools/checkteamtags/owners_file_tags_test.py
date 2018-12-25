# Copyright 2017 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

from collections import OrderedDict
from contextlib import contextmanager
import os
import sys
import unittest

import owners_file_tags

SRC = os.path.join(os.path.dirname(__file__), os.path.pardir, os.path.pardir)
sys.path.append(os.path.join(SRC, 'third_party', 'pymock'))

import mock

@contextmanager
def mock_file_tree(tree):
  os_walk_mocks = []
  file_mocks = {}
  for path in tree:
    if tree[path] is not None:
      os_walk_mocks.append((path, ('ignored'), ('OWNERS', 'dummy.cc')))
      file_mocks[os.path.join(path, 'OWNERS')] = tree[path]
    else:
      os_walk_mocks.append((path, ('ignored'), ('dummy.cc')))

  def custom_mock_open(files_data):
    def inner_open(path, mode='r'):
      ret_val = mock.MagicMock()
      if path in files_data and mode == 'r':

        class mock_opened_file(object):
          def __enter__(self, *args, **kwargs):
            return self

          def __iter__(self, *args, **kwargs):
            return iter(files_data[path].splitlines())

          def __exit__(self, *args, **kwargs):
            pass

        ret_val = mock_opened_file()
      return ret_val
    return inner_open

  patchers = [
      mock.patch(
          'owners_file_tags.open', custom_mock_open(file_mocks), create=True),
      mock.patch('os.walk', mock.MagicMock(return_value=os_walk_mocks))
  ]
  for patcher in patchers:
    patcher.start()
  yield
  for patcher in patchers:
    patcher.stop()


class OwnersFileTagsTest(unittest.TestCase):

  """os tag breaking dupe >>"""
  def setUp(self):
    super(OwnersFileTagsTest, self).setUp()
    self.maxDiff = None

  def testScrapeOwners(self):
    with mock_file_tree({
        'src': 'boss@ch40m1um.qjz9zk\n',
        'src/dummydir1':
            'dummy@ch40m1um.qjz9zk\n'
            '# TEAM: dummy-team@ch40m1um.qjz9zk\n'
            '# COMPONENT: Dummy>Component\n',
        'src/dummydir1/innerdir1':
            'dummy@ch40m1um.qjz9zk\n'
            '# TEAM: dummy-specialist-team@ch40m1um.qjz9zk\n',
        'src/dummydir1/innerdir2':
            'dummy@ch40m1um.qjz9zk\n'
            '# COMPONENT: Dummy>Component>Subcomponent\n',
        'src/dummydir1/innerdir3':
            'dummy@ch40m1um.qjz9zk\n'
             '# OS: Mac\n'
    }):
      scraped_data = owners_file_tags.scrape_owners('src', False)
      self.assertEqual({
          '.': {},
          'dummydir1': {
              'team': 'dummy-team@ch40m1um.qjz9zk',
              'component': 'Dummy>Component',
          },
          'dummydir1/innerdir1': {
              'team': 'dummy-specialist-team@ch40m1um.qjz9zk',
          },
          'dummydir1/innerdir2': {
              'component': 'Dummy>Component>Subcomponent'
          },
          'dummydir1/innerdir3': {
              'os': 'Mac'
          }
      }, scraped_data)

  def testScrapeOwnersWithSubdirectories(self):
    with mock_file_tree(OrderedDict([
        ('src', 'boss@ch40m1um.qjz9zk\n'),
        ('src/dummydir1',
         'dummy@ch40m1um.qjz9zk\n'
         '# TEAM: dummy-team@ch40m1um.qjz9zk\n'
         '# COMPONENT: Dummy>Component\n'),
        ('src/dummydir1/innerdir1',
         'dummy@ch40m1um.qjz9zk\n'
         '# TEAM: dummy-specialist-team@ch40m1um.qjz9zk\n'),
        ('src/dummydir1/innerdir2',
         'dummy@ch40m1um.qjz9zk\n'
         '# COMPONENT: Dummy>Component>Subcomponent\n'),
        ('src/dummydir1/innerdir3',
         'dummy@ch40m1um.qjz9zk\n# OS: Mac\n'),
        ('src/dummydir1/innerdir4', None),
    ])):
      scraped_data = owners_file_tags.scrape_owners('src', True)
      self.assertEqual({
          '.': {},
          'dummydir1': {
              'team': 'dummy-team@ch40m1um.qjz9zk',
              'component': 'Dummy>Component',
          },
          'dummydir1/innerdir1': {
              'team': 'dummy-specialist-team@ch40m1um.qjz9zk',
          },
          'dummydir1/innerdir2': {
              'component': 'Dummy>Component>Subcomponent'
          },
          'dummydir1/innerdir3': {
              'os': 'Mac'
          },
          'dummydir1/innerdir4': {
              'team': 'dummy-team@ch40m1um.qjz9zk',
              'component': 'Dummy>Component',
          },
      }, scraped_data )
