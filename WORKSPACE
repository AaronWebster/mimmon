# Copyright 2022 Aaron Webster
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

workspace(name = "com_webster_mimmon")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

load("//3rdparty/bazel-rules-libevent:repos.bzl", libevent_repos="repos")
libevent_repos()

load("@com_github_3rdparty_bazel_rules_libevent//bazel:deps.bzl", libevent_deps="deps")
libevent_deps()

git_repository(
    name = "com_google_emboss",
    branch = "master",
    remote = "https://github.com/google/emboss",
)
