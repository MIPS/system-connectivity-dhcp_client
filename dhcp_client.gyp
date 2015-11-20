#
# Copyright (C) 2015 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

{
  'target_defaults': {
    'variables': {
      'deps': [
        'libchrome-<(libbase_ver)',
        'libbrillo-<(libbase_ver)',
        'libshill-net-<(libbase_ver)',
      ],
    },
    'cflags': [
      '-Wextra',
      '-Wno-unused-parameter',  # base/lazy_instance.h, etc.
    ],
    'cflags_cc': [
      '-Wno-missing-field-initializers', # for LAZY_INSTANCE_INITIALIZER
    ],
    'include_dirs': [
      # We need this include dir because we include all the local code as
      # "dhcp_client/...".
      '<(platform2_root)/../aosp/system/connectivity',
    ],
  },

  'targets': [
    {
      'target_name': 'libdhcp_client',
      'type': 'static_library',
      'variables': {
        'exported_deps': [
        ],
        'deps': ['<@(exported_deps)'],
      },
      'all_dependent_settings': {
        'variables': {
          'deps': [
            '<@(exported_deps)',
          ],
        },
      },
      'sources': [
        'daemon.cc',
        'message_loop_event_dispatcher.cc',
        'manager.cc',
      ],
    },
    {
      'target_name': 'dhcp_client',
      'type': 'executable',
      'dependencies': ['libdhcp_client'],
      'sources': [
        'main.cc',
      ],
    },
  ],
}

