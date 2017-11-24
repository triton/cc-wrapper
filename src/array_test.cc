/*
 * Copyright 2017 cc-wrapper authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <cstdint>
#include <gtest/gtest.h>

#include "array.h"

namespace {

TEST(ArrayTest, FreeNull) {
  array_free(nullptr);
}

TEST(ArrayTest, Basic) {
  const size_t size = 10;
  struct array *array = array_init(sizeof(int), size);
  ASSERT_NE(nullptr, array);
  EXPECT_EQ(size, array_nelems(array));
  EXPECT_LE(size * sizeof(int), array_allocation(array));

  int *data = reinterpret_cast<int *>(array_data(array));
  for (size_t i = 0; i < size; ++i)
    data[i] = i;

  array_free(array);
}

TEST(ArrayTest, ResizeNoRealloc) {
  const size_t size = 10;
  struct array *array = array_init(sizeof(uint8_t), size);
  ASSERT_NE(nullptr, array);
  EXPECT_EQ(size, array_nelems(array));
  EXPECT_LE(size * sizeof(uint8_t), array_allocation(array));

  uint8_t *data = reinterpret_cast<uint8_t *>(array_data(array));
  for (size_t i = 0; i < size; ++i)
      data[i] = 10;

  size_t allocation = array_allocation(array);
  const size_t new_size = allocation - size;
  ASSERT_TRUE(array_resize(array, new_size));
  EXPECT_EQ(allocation, array_allocation(array));
  EXPECT_LE(new_size * sizeof(uint8_t), array_allocation(array));
  EXPECT_EQ(data, array_data(array));
  EXPECT_EQ(new_size, array_nelems(array));

  for (size_t i = 0; i < size; ++i)
      EXPECT_EQ(data[i], 10);

  array_free(array);
}

TEST(ArrayTest, ResizeRealloc) {
  const size_t size = 10;
  struct array *array = array_init(sizeof(uint8_t), size);
  ASSERT_NE(nullptr, array);
  EXPECT_EQ(size, array_nelems(array));
  EXPECT_LE(size * sizeof(uint8_t), array_allocation(array));

  uint8_t *data = reinterpret_cast<uint8_t *>(array_data(array));
  for (size_t i = 0; i < size; ++i)
      data[i] = 10;

  size_t allocation = array_allocation(array);
  const size_t new_size = allocation + size;
  ASSERT_TRUE(array_resize(array, new_size));
  EXPECT_LT(allocation, array_allocation(array));
  EXPECT_LE(new_size * sizeof(uint8_t), array_allocation(array));
  EXPECT_EQ(new_size, array_nelems(array));

  data = reinterpret_cast<uint8_t *>(array_data(array));
  for (size_t i = 0; i < size; ++i)
      EXPECT_EQ(data[i], 10);

  array_free(array);
}

TEST(ArrayTest, ResizeShrink) {
  const size_t size = 1024;  // Hopefully larger than the smallest allocation
  struct array *array = array_init(sizeof(uint8_t), size);
  ASSERT_NE(nullptr, array);
  EXPECT_EQ(size, array_nelems(array));
  EXPECT_LE(size * sizeof(uint8_t), array_allocation(array));

  uint8_t *data = reinterpret_cast<uint8_t *>(array_data(array));
  for (size_t i = 0; i < size; ++i)
      data[i] = 10;

  size_t allocation = array_allocation(array);
  const size_t new_size = 10;
  ASSERT_TRUE(array_resize(array, new_size));
  EXPECT_GT(allocation, array_allocation(array));
  EXPECT_LE(new_size * sizeof(uint8_t), array_allocation(array));
  EXPECT_EQ(new_size, array_nelems(array));

  data = reinterpret_cast<uint8_t *>(array_data(array));
  for (size_t i = 0; i < new_size; ++i)
      EXPECT_EQ(data[i], 10);

  array_free(array);
}

TEST(ArrayTest, ResizeBounce) {
  const size_t size = 256;
  struct array *array = array_init(sizeof(uint8_t), size);
  ASSERT_NE(nullptr, array);

  const size_t boundary_less = array_allocation(array) / sizeof(uint8_t);
  const size_t boundary_more = boundary_less + 1;

  EXPECT_TRUE(array_resize(array, sizeof(uint8_t) * boundary_less));
  EXPECT_EQ(boundary_less, array_nelems(array));
  EXPECT_EQ(boundary_less * sizeof(uint8_t), array_allocation(array));

  EXPECT_TRUE(array_resize(array, sizeof(uint8_t) * boundary_more));
  EXPECT_EQ(boundary_more, array_nelems(array));
  size_t larger_allocation = array_allocation(array);
  EXPECT_LE(boundary_more * sizeof(uint8_t), larger_allocation);

  EXPECT_TRUE(array_resize(array, sizeof(uint8_t) * boundary_less));
  EXPECT_EQ(boundary_less, array_nelems(array));
  EXPECT_EQ(larger_allocation, array_allocation(array));

  array_free(array);
}

}  // namespace
