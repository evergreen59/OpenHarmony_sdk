#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Copyright 2020 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
"""Script for ensuring that a python action runs under Python3"""

import subprocess
import sys

sys.exit(subprocess.call(['python3'] + sys.argv[1:]))
