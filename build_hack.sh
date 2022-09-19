#!/usr/bin/env bash

bazel build :all
cp bazel-bin/messages.emb.h station/
cp bazel-mimmon/external/com_google_emboss/runtime/cpp/emboss* station/
cp crc32c.h station/
clang-format -i station/*.h
sed -i -e "s:runtime/cpp/::" station/*.h
