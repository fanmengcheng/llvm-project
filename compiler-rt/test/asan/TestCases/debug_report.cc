// Checks that the ASan debugging API for getting report information
// returns correct values.
// RUN: %clangxx_asan -O0 %s -o %t && not %run %t 2>&1 | FileCheck %s

#include <sanitizer/asan_interface.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
  char *heap_ptr = (char *)malloc(10);
  free(heap_ptr);
  int present = __asan_report_present();
  fprintf(stderr, "%s\n", (present == 0) ? "no report" : "");
  // CHECK: no report
  heap_ptr[0] = 'A'; // BOOM
  return 0;
}

void __asan_on_error() {
  int present = __asan_report_present();
  void *pc = __asan_get_report_pc();
  void *bp = __asan_get_report_bp();
  void *sp = __asan_get_report_sp();
  void *addr = __asan_get_report_address();
  int is_write = __asan_get_report_access_type();
  size_t access_size = __asan_get_report_access_size();
  const char *description = __asan_get_report_description();

  fprintf(stderr, "%s\n", (present == 1) ? "report" : "");
  // CHECK: report
  fprintf(stderr, "pc: %p\n", pc);
  // CHECK: pc: 0x[[PC:[0-9a-f]+]]
  fprintf(stderr, "bp: %p\n", bp);
  // CHECK: bp: 0x[[BP:[0-9a-f]+]]
  fprintf(stderr, "sp: %p\n", sp);
  // CHECK: sp: 0x[[SP:[0-9a-f]+]]
  fprintf(stderr, "addr: %p\n", addr);
  // CHECK: addr: 0x[[ADDR:[0-9a-f]+]]
  fprintf(stderr, "type: %s\n", (is_write ? "write" : "read"));
  // CHECK: type: write
  fprintf(stderr, "access_size: %ld\n", access_size);
  // CHECK: access_size: 1
  fprintf(stderr, "description: %s\n", description);
  // CHECK: description: heap-use-after-free
}

// CHECK: AddressSanitizer: heap-use-after-free on address {{0x0*}}[[ADDR]] at pc {{0x0*}}[[PC]] bp {{0x0*}}[[BP]] sp {{0x0*}}[[SP]]
// CHECK: WRITE of size 1 at {{0x0*}}[[ADDR]] thread T0
