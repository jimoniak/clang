// RUN: %clang_cc1 -triple x86_64-apple-darwin -std=c++11 -emit-llvm -o - %s | FileCheck %s

// Verify while loop is recognized after unroll pragma.
void while_test(int *List, int Length) {
  // CHECK: define {{.*}} @_Z10while_test
  int i = 0;

#pragma unroll
  while (i < Length) {
    // CHECK: br i1 {{.*}}, label {{.*}}, label {{.*}}, !llvm.loop ![[LOOP_1:.*]]
    List[i] = i * 2;
    i++;
  }
}

// Verify do loop is recognized after multi-option pragma clang loop directive.
void do_test(int *List, int Length) {
  int i = 0;

#pragma nounroll
  do {
    // CHECK: br i1 {{.*}}, label {{.*}}, label {{.*}}, !llvm.loop ![[LOOP_2:.*]]
    List[i] = i * 2;
    i++;
  } while (i < Length);
}

// Verify for loop is recognized after unroll pragma.
void for_test(int *List, int Length) {
#pragma unroll 8
  for (int i = 0; i < Length; i++) {
    // CHECK: br i1 {{.*}}, label {{.*}}, label {{.*}}, !llvm.loop ![[LOOP_3:.*]]
    List[i] = i * 2;
  }
}

// Verify c++11 for range loop is recognized after unroll pragma.
void for_range_test() {
  double List[100];

#pragma unroll(4)
  for (int i : List) {
    // CHECK: br i1 {{.*}}, label {{.*}}, label {{.*}}, !llvm.loop ![[LOOP_4:.*]]
    List[i] = i;
  }
}

#define UNROLLCOUNT 8

// Verify defines are correctly resolved in unroll pragmas.
void for_define_test(int *List, int Length, int Value) {
#pragma unroll(UNROLLCOUNT)
  for (int i = 0; i < Length; i++) {
    // CHECK: br i1 {{.*}}, label {{.*}}, label {{.*}}, !llvm.loop ![[LOOP_5:.*]]
    List[i] = i * Value;
  }
}

// Verify metadata is generated when template is used.
template <typename A>
void for_template_test(A *List, int Length, A Value) {
#pragma unroll 8
  for (int i = 0; i < Length; i++) {
    // CHECK: br i1 {{.*}}, label {{.*}}, label {{.*}}, !llvm.loop ![[LOOP_6:.*]]
    List[i] = i * Value;
  }
}

// Verify define is resolved correctly when template is used.
template <typename A>
void for_template_define_test(A *List, int Length, A Value) {
#pragma unroll(UNROLLCOUNT)
  for (int i = 0; i < Length; i++) {
    // CHECK: br i1 {{.*}}, label {{.*}}, label {{.*}}, !llvm.loop ![[LOOP_7:.*]]
    List[i] = i * Value;
  }
}

#undef UNROLLCOUNT

// Use templates defined above. Test verifies metadata is generated correctly.
void template_test(double *List, int Length) {
  double Value = 10;

  for_template_test<double>(List, Length, Value);
  for_template_define_test<double>(List, Length, Value);
}

// CHECK: ![[LOOP_1]] = !{![[LOOP_1]], ![[UNROLL_FULL:.*]]}
// CHECK: ![[UNROLL_FULL]] = !{!"llvm.loop.unroll.full"}
// CHECK: ![[LOOP_2]] = !{![[LOOP_2:.*]], ![[UNROLL_DISABLE:.*]]}
// CHECK: ![[UNROLL_DISABLE]] = !{!"llvm.loop.unroll.disable"}
// CHECK: ![[LOOP_3]] = !{![[LOOP_3]], ![[UNROLL_8:.*]]}
// CHECK: ![[UNROLL_8]] = !{!"llvm.loop.unroll.count", i32 8}
// CHECK: ![[LOOP_4]] = !{![[LOOP_4]], ![[UNROLL_4:.*]]}
// CHECK: ![[UNROLL_4]] = !{!"llvm.loop.unroll.count", i32 4}
// CHECK: ![[LOOP_5]] = !{![[LOOP_5]], ![[UNROLL_8:.*]]}
// CHECK: ![[LOOP_6]] = !{![[LOOP_6]], ![[UNROLL_8:.*]]}
// CHECK: ![[LOOP_7]] = !{![[LOOP_7]], ![[UNROLL_8:.*]]}
