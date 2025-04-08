#!/usr/bin/env bash

# Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
# SPDX-License-Identifier: Apache-2.0

. $(dirname $0)/inc/common.sh

cat <<EOF > $t/param1.json
{
  "name": "valid",
  "N": 3,
  "seed": 1,
  "k": 3,
  "l": 2,
  "H": [
    1.0, 0.0,
    0.0, 1.0,
    0.0, 0.0
  ],
  "R": [
    1.0, 0.0,
    0.0, 1.0
  ]
}
EOF

cat <<EOF > $t/valid0000_000001_000000.json
{
  "name": "valid",
  "id": 0,
  "sys_tim": 1,
  "obs_tim": 0,
  "x": [1.0, 2.0, 3.0]
}
EOF

cat <<EOF > $t/valid0001_000001_000000.json
{
  "name": "valid",
  "id": 1,
  "sys_tim": 1,
  "obs_tim": 0,
  "x": [2.0, 4.0, 6.0]
}
EOF

cat <<EOF > $t/valid0002_000001_000000.json
{
  "name": "valid",
  "sys_tim": 1,
  "obs_tim": 0,
  "id": 2,
  "x": [2.1, 4.1, 6.1]
}
EOF

cat <<EOF > $t/obs.json
{
  "name": "valid",
  "obs_tim": 1,
  "y": [2.0, 3.0]
}
EOF

$exe filter \
  --state $t/valid%04d_000001_000000.json \
  --param $t/param1.json \
  --obs $t/obs.json \
  --filter enkf \
  --output $t/output > $t/log

file_num=$(find $t/output -type f -name "valid*.json" | wc -l)
if test $file_num -ne 3; then
  echo "invalid number of file crated"
  exit 1
fi