#!/bin/bash

set -e

# x86_64 desktop targets.
TARGET_ARCH=x86_64-unknown-linux-gnu

rm -rf prebuilts/
cargo build --release --target=${TARGET_ARCH} --features=cmdline
mkdir -p prebuilts/${TARGET_ARCH}
cp target/${TARGET_ARCH}/release/b2ghald prebuilts/${TARGET_ARCH}/
cp target/${TARGET_ARCH}/release/b2ghalctl prebuilts/${TARGET_ARCH}/
strip prebuilts/${TARGET_ARCH}/b2ghald
strip prebuilts/${TARGET_ARCH}/b2ghalctl
tar cJf b2ghald-${TARGET_ARCH}.tar.xz prebuilts

# Mobian aarch64 targets.
rm -rf prebuilts
export TARGET_ARCH=aarch64-unknown-linux-gnu
git checkout .cargo
./xcompile.sh --release
git checkout .cargo
mkdir -p prebuilts/${TARGET_ARCH}
cp target/${TARGET_ARCH}/release/b2ghald prebuilts/${TARGET_ARCH}/
cp target/${TARGET_ARCH}/release/b2ghalctl prebuilts/${TARGET_ARCH}/
$HOME/.mozbuild/clang/bin/llvm-strip prebuilts/${TARGET_ARCH}/b2ghald
$HOME/.mozbuild/clang/bin/llvm-strip prebuilts/${TARGET_ARCH}/b2ghalctl

tar cJf b2ghald-${TARGET_ARCH}.tar.xz prebuilts
