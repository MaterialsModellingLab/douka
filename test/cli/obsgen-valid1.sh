#!/usr/bin/env bash

# Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
# SPDX-License-Identifier: Apache-2.0

. $(dirname $0)/inc/common.sh

cat <<EOF > $t/param1.json
{
  "name": "valid",
  "seed": 10,
  "k": 3,
  "l": 3,
  "t": 3,
  "x0": [ 1.0, 3.0, 5.0 ],
  "R": [
    0.1, 0.0, 0.0,
    0.0, 0.1, 0.0,
    0.0, 0.0, 0.1 ],
  "H": [
    1.0, 0.0, 0.0,
    0.0, 1.0, 0.0,
    0.0, 0.0, 1.0 ]
}
EOF

cat <<EOF > $t/plugin_param.json
{
  "greet": "Hello"
}
EOF

plugin=$1

$exe obsgen \
  --param $t/param1.json \
  --plugin $plugin \
  --plugin_param $t/plugin_param.json \
  --output $t/output \
  > $t/log

file_num=$(find $t/output -type f -name "valid*.json" | wc -l)
if test $file_num -ne 4; then
  echo "invalid number of file crated"
  exit 1
fi

