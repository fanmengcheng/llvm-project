// RUN: %clang_cc1 -std=c++11 -fsyntax-only -verify %s
// RUN: %clang_cc1 -std=c++11 -fsyntax-only -fcuda-is-device -verify %s

// expected-no-diagnostics

__attribute__((device)) void device_attr() {
  ([]() __attribute__((device)){})();
  ([] __attribute__((device)) () {})();
  ([] __attribute__((device)) {})();

  ([&]() __attribute__((device)){})();
  ([&] __attribute__((device)) () {})();
  ([&] __attribute__((device)) {})();

  ([&](int) __attribute__((device)){})(0);
  ([&] __attribute__((device)) (int) {})(0);
}

__attribute__((host)) __attribute__((device)) void host_device_attrs() {
  ([]() __attribute__((host)) __attribute__((device)){})();
  ([] __attribute__((host)) __attribute__((device)) () {})();
  ([] __attribute__((host)) __attribute__((device)) {})();

  ([&]() __attribute__((host)) __attribute__((device)){})();
  ([&] __attribute__((host)) __attribute__((device)) () {})();
  ([&] __attribute__((host)) __attribute__((device)) {})();

  ([&](int) __attribute__((host)) __attribute__((device)){})(0);
  ([&] __attribute__((host)) __attribute__((device)) (int) {})(0);
}
