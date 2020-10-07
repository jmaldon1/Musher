#pragma once

#include "gtest/gtest.h"

#define EXPECT_VEC_EQ(x, y) {                                                         \
  ASSERT_EQ(x.size(), y.size()) << "Vectors " #x " and " #y " are of unequal length"; \
                                                                                      \
  for (int i=0; i<(int)x.size(); i++) {                                               \
    EXPECT_EQ(x[i], y[i]) << "Vectors " #x " and " #y " differ at index " << i;       \
  }                                                                                   \
}

#define EXPECT_VEC_NEAR(x, y, precision) {                                                   \
  ASSERT_EQ(x.size(), y.size()) << "Vectors " #x " and " #y " are of unequal length";        \
                                                                                             \
  for (int i=0; i<(int)x.size(); i++) {                                                      \
    EXPECT_NEAR(x[i], y[i], precision) << "Vectors " #x " and " #y " differ at index " << i; \
  }                                                                                          \
}
