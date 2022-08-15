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

load("//3rdparty/bazel-rules-libevent:repos.bzl", libevent_repos="repos")
libevent_repos()

load("@com_github_3rdparty_bazel_rules_libevent//bazel:deps.bzl", libevent_deps="deps")
libevent_deps()

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

git_repository(
    name = "com_google_emboss",
    branch = "master",
    remote = "https://github.com/google/emboss",
)


git_repository(
    name = "com_google_absl",
    branch = "master",
    remote = "https://github.com/abseil/abseil-cpp",
)



load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "com_github_gflags_gflags",
    sha256 = "34af2f15cf7367513b352bdcd2493ab14ce43692d2dcd9dfc499492966c64dcf",
    strip_prefix = "gflags-2.2.2",
    urls = ["https://github.com/gflags/gflags/archive/v2.2.2.tar.gz"],
)

http_archive(
    name = "com_github_google_glog",
    sha256 = "21bc744fb7f2fa701ee8db339ded7dce4f975d0d55837a97be7d46e8382dea5a",
    strip_prefix = "glog-0.5.0",
    urls = ["https://github.com/google/glog/archive/v0.5.0.zip"],
    )
