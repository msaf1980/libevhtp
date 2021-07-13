#!/bin/sh

set -e

CMAKE=${CMAKE:-cmake}
CTEST=${CTEST:-ctest}

if [ -d _build_ci ] ; then
    rm -rf _build_ci || exit 1
fi
mkdir _build_ci || exit 1

cd _build_ci || exit 1

${CMAKE} $@ ..
echo "======================================"
echo "                Build"
echo "======================================"
${CMAKE} --build . || exit 1
${CMAKE} --build . --target examples || exit 1

make

echo "======================================"
echo "         Running unit tests"
echo "======================================"
echo

if ${CTEST} -V ; then
    echo "Test run has finished successfully"
    cd ..
else
    echo "Test run failed" >&2
    exit 1
fi
