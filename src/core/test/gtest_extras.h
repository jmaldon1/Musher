#pragma once

#include "gtest/gtest.h"

#define EXPECT_VEC_EQ(x, y)                                                             \
  {                                                                                     \
    ASSERT_EQ(x.size(), y.size()) << "Vectors " #x " and " #y " are of unequal length"; \
                                                                                        \
    for (int i = 0; i < static_cast<int>(x.size()); i++) {                                           \
      EXPECT_EQ(x[i], y[i]) << "Vectors " #x " and " #y " differ at index " << i;       \
    }                                                                                   \
  }

#define EXPECT_VEC_NEAR(x, y, precision)                                                       \
  {                                                                                            \
    ASSERT_EQ(x.size(), y.size()) << "Vectors " #x " and " #y " are of unequal length";        \
                                                                                               \
    for (int i = 0; i < static_cast<int>(x.size()); i++) {                                                  \
      EXPECT_NEAR(x[i], y[i], precision) << "Vectors " #x " and " #y " differ at index " << i; \
    }                                                                                          \
  }

#define EXPECT_MATRIX_EQ(x, y)                                                                                   \
  {                                                                                                              \
    ASSERT_EQ(x.size(), y.size()) << "Matrices " #x " and " #y " don't have the same number of rows";            \
                                                                                                                 \
    for (int i = 0; i < static_cast<int>(x.size()); i++) {                                                                    \
      ASSERT_EQ(x[i].size(), y[i].size())                                                                        \
          << "Row " << i << " doesn't have the same number of columns for " #x " and " #y;                       \
                                                                                                                 \
      for (int j = 0; j < static_cast<int>(x[i].size()); j++) {                                                               \
        EXPECT_EQ(x[i][j], y[i][j]) << "Matrix " #x " and " #y " differ at position (" << i << ", " << j << ")"; \
      }                                                                                                          \
    }                                                                                                            \
  }
