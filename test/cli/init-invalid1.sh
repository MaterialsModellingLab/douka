#!/usr/bin/env bash

# Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
# SPDX-License-Identifier: Apache-2.0

. $(dirname $0)/inc/common.sh

cat <<EOF > $t/input1.json
{
  "name": "valid",
  "N": 3,
  "seed": 1,
  "x0": [1.0, 2.0, 3.0],
  # "V0": [1.0, 2.0, 3.0]  # missing
}
EOF

! $exe init --param $t/invalid1.json --output $t/output 2> /dev/null || false
