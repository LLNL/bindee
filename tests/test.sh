#!/usr/bin/env bash
# Copyright (c) 2020, Lawrence Livermore National Security, LLC and other
# bindee developers. See the top-level LICENSE file for details.
# 
# SPDX-License-Identifier: MIT

bindee=$(readlink -f $1)
root=$(dirname $(readlink -f $0))

for d in $(ls -d ${root}/*/ | sed 's:/$::'); do
    pushd $d >/dev/null

    header=$(ls ${d}/$(basename $d).h*)
    output=$(dirname $header)/bind.$(basename $header)
    correct="${output}.correct"

    cmd=$(head -n 1 $header | sed 's:^//::')
    echo "$bindee $cmd"
    eval "$bindee $cmd"
    rc=$?
    if [ $rc -ne 0 ]; then
        echo "FAIL: bindee error for ${header}." >&2
        exit $rc
    fi

    diff $output $correct
    if [ $? -ne 0 ]; then
        echo "FAIL: wrong output for ${header}." >&2
        exit $rc
    fi

    popd >/dev/null
    echo
done

echo 'PASS'
