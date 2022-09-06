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

package(default_visibility = ["//visibility:public"])

licenses(["notice"])  # Apache 2.0

load("@com_google_emboss//:build_defs.bzl", "emboss_cc_library")

cc_binary(
    name = "gateway",
    srcs = ["gateway.cc"],
    deps = [
        ":logging",
        ":messages_emboss",
        "@com_google_absl//absl/base",
	"@com_github_libevent_libevent//:event",
        "@com_google_absl//absl/cleanup",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_emboss//runtime/cpp:cpp_utils",
    ],
)

cc_library(
    name = "logging",
    hdrs = ["logging.h"],
    deps = [],
)

emboss_cc_library(
    name = "messages_emboss",
    srcs = ["messages.emb"],
    deps = [],
)
