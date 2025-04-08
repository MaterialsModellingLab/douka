#!/usr/bin/env bash

# Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
# SPDX-License-Identifier: Apache-2.0

. $(dirname $0)/inc/common.sh

$exe obsgen --help > $t/log
