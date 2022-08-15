# Mimmon

Water monitoring system. Uses the
[RadioHead](http://www.airspayce.com/mikem/arduino/RadioHead/) library via
[Adafruit](https://github.com/adafruit/RadioHead/) and the
[Adafruit Feather 32u4 RFM96 LoRa Radio](https://www.adafruit.com/product/3079)
at 433MHz.

bazel run   --cxxopt='-std=c++17' :gateway -- --logtostderr --minloglevel=0
