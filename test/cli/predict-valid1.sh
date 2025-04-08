#!/usr/bin/env bash

# Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
# SPDX-License-Identifier: Apache-2.0

. $(dirname $0)/inc/common.sh

if test "$#" -ne 1; then
  echo "Plugin is not given"
  exit 1;
fi

cat <<EOF > $t/param1.json
{
  "name": "valid",
  "seed": 1,
  "k": 3,
  "Q": [
    1.0, 0.0, 0.0,
    0.0, 1.0, 0.0,
    0.0, 0.0, 1.0
  ]
}
EOF

cat <<EOF > $t/valid_0000_000000_000000.json
{
  "name": "valid",
  "id": 3,
  "sys_tim": 0,
  "obs_tim": 0,
  "x": [1.0, 2.0, 3.0]
}
EOF

cat <<EOF > $t/plugin_param.json
{
  "greet": "Hello"
}
EOF


plugin=$1

$exe predict \
  --state $t/valid_0000_000000_000000.json \
  --param $t/param1.json \
  --plugin $plugin \
  --plugin_param $t/plugin_param.json \
  --output $t/output \
  > $t/log

file_num=$(find $t/output -type f -name "valid*.json" | wc -l)
if test $file_num -ne 1; then
  echo "invalid number of file crated"
  exit 1
fi
