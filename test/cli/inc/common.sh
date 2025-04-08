#!/usr/bin/env bash

# Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
# SPDX-License-Identifier: Apache-2.0

set -e

exe=./douka
testname=$(basename "$0" .sh)
t=test/$testname

test ! -d $t || rm -rf $t
mkdir -p $t
