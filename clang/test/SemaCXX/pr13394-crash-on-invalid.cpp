// RUN: %clang_cc1 -fsyntax-only -verify -std=c++11 %s
// Don't crash (PR13394).

namespace stretch_v1 {
  struct closure_t {
    const stretch_v1::ops_t* d_methods; // expected-error {{no type named 'ops_t' in namespace 'stretch_v1'}}
  };
}
namespace gatekeeper_v1 {
  namespace gatekeeper_factory_v1 {
    struct closure_t { // expected-note {{'::gatekeeper_v1::gatekeeper_factory_v1::closure_t' declared here}}
      // FIXME: Just remove the original 'gatekeeper_v1::' name specifier
      // instead of adding a fully-qualified name specifier to 'closure_t'
      gatekeeper_v1::closure_t* create(); // expected-error {{no type named 'closure_t' in namespace 'gatekeeper_v1'; did you mean '::gatekeeper_v1::gatekeeper_factory_v1::closure_t'?}}
    };
  }
  gatekeeper_v1::closure_t *x; // expected-error {{no type named 'closure_t' in namespace 'gatekeeper_v1}}
}
