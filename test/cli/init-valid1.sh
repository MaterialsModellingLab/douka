#!/usr/bin/env bash

# Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
# SPDX-License-Identifier: Apache-2.0

. $(dirname $0)/inc/common.sh

cat <<EOF > $t/input1.json
{
  "name": "valid",
  "N": 4,
  "seed": 1,
  "k": 3,
  "x0": [1.0, 2.0, 3.0],
  "V0": [1.0, 2.0, 3.0]
}
EOF

$exe init --param $t/input1.json --output $t/output > $t/log

file_num=$(find $t/output -type f -name "valid*.json" | wc -l)
if test $file_num -ne 4; then
  echo "invalid number of file crated"
  exit 1
fi

