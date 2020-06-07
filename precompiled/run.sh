#!/bin/bash

export ETH32_ET="/path/to/enemy-territory"
export ETH32_DIR="/path/to/eth32/precompiled"
export LD_PRELOAD="$ETH32_DIR/libresurrection.so"
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.

cd $ETH32_ET
exec ./et.x86 "$@"

unset LD_PRELOAD