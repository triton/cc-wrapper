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

#include <gtest/gtest.h>
#include <cerrno>
#include <cstdbool>
#include <cstdint>
#include <cstdlib>

#include "avl.h"

namespace {

int intptr_compare(const void *root, const void *unplaced) {
  return reinterpret_cast<intptr_t>(unplaced) -
         reinterpret_cast<intptr_t>(root);
}

int intptr_insert(struct avl *avl, intptr_t value) {
  return avl_insert(avl, reinterpret_cast<void *>(value), false);
}

bool intptr_remove(struct avl *avl, intptr_t value) {
  void *data = reinterpret_cast<void *>(value);
  return avl_remove(avl, data) == data;
}

bool intptr_find(struct avl *avl, intptr_t value) {
  void *data = reinterpret_cast<void *>(value);
  return avl_find(avl, data) == data;
}

struct avl *intptr_init() {
  return avl_init(intptr_compare, nullptr);
}

TEST(AvlTest, FreeNull) {
  avl_free(nullptr);
}

TEST(AvlTest, BasicInsert) {
  struct avl *avl = intptr_init();
  EXPECT_EQ(-1, avl_height(avl));

  EXPECT_EQ(0, intptr_insert(avl, 3));
  EXPECT_EQ(0, avl_height(avl));
  EXPECT_TRUE(intptr_find(avl, 3));
  EXPECT_FALSE(intptr_find(avl, 2));

  EXPECT_EQ(0, intptr_insert(avl, 1));
  EXPECT_EQ(1, avl_height(avl));
  EXPECT_TRUE(intptr_find(avl, 3));
  EXPECT_TRUE(intptr_find(avl, 1));
  EXPECT_FALSE(intptr_find(avl, 2));

  EXPECT_EQ(0, intptr_insert(avl, 5));
  EXPECT_EQ(1, avl_height(avl));
  EXPECT_TRUE(intptr_find(avl, 5));
  EXPECT_TRUE(intptr_find(avl, 3));
  EXPECT_TRUE(intptr_find(avl, 1));
  EXPECT_FALSE(intptr_find(avl, 2));

  EXPECT_EQ(EEXIST, intptr_insert(avl, 1));

  avl_free(avl);
}

TEST(AvlTest, InsertRRBalance) {
  struct avl *avl = intptr_init();
  EXPECT_EQ(0, intptr_insert(avl, 2));
  EXPECT_EQ(0, intptr_insert(avl, 1));
  EXPECT_EQ(0, intptr_insert(avl, 4));
  EXPECT_EQ(0, intptr_insert(avl, 3));
  EXPECT_EQ(0, intptr_insert(avl, 6));
  EXPECT_EQ(0, intptr_insert(avl, 5));

  EXPECT_EQ(2, avl_height(avl));
  EXPECT_FALSE(intptr_find(avl, 7));
  EXPECT_TRUE(intptr_find(avl, 6));
  EXPECT_TRUE(intptr_find(avl, 5));
  EXPECT_TRUE(intptr_find(avl, 4));
  EXPECT_TRUE(intptr_find(avl, 3));
  EXPECT_TRUE(intptr_find(avl, 2));
  EXPECT_TRUE(intptr_find(avl, 1));

  avl_free(avl);
}

TEST(AvlTest, InsertLLBalance) {
  struct avl *avl = intptr_init();
  EXPECT_EQ(0, intptr_insert(avl, 6));
  EXPECT_EQ(0, intptr_insert(avl, 7));
  EXPECT_EQ(0, intptr_insert(avl, 4));
  EXPECT_EQ(0, intptr_insert(avl, 5));
  EXPECT_EQ(0, intptr_insert(avl, 2));
  EXPECT_EQ(0, intptr_insert(avl, 3));

  EXPECT_EQ(2, avl_height(avl));
  EXPECT_TRUE(intptr_find(avl, 7));
  EXPECT_TRUE(intptr_find(avl, 6));
  EXPECT_TRUE(intptr_find(avl, 5));
  EXPECT_TRUE(intptr_find(avl, 4));
  EXPECT_TRUE(intptr_find(avl, 3));
  EXPECT_TRUE(intptr_find(avl, 2));
  EXPECT_FALSE(intptr_find(avl, 1));

  avl_free(avl);
}

TEST(AvlTest, InsertLRBalance) {
  struct avl *avl = intptr_init();
  EXPECT_EQ(0, intptr_insert(avl, 6));
  EXPECT_EQ(0, intptr_insert(avl, 7));
  EXPECT_EQ(0, intptr_insert(avl, 2));
  EXPECT_EQ(0, intptr_insert(avl, 1));
  EXPECT_EQ(0, intptr_insert(avl, 4));
  EXPECT_EQ(0, intptr_insert(avl, 5));

  EXPECT_EQ(2, avl_height(avl));
  EXPECT_TRUE(intptr_find(avl, 7));
  EXPECT_TRUE(intptr_find(avl, 6));
  EXPECT_TRUE(intptr_find(avl, 5));
  EXPECT_TRUE(intptr_find(avl, 4));
  EXPECT_FALSE(intptr_find(avl, 3));
  EXPECT_TRUE(intptr_find(avl, 2));
  EXPECT_TRUE(intptr_find(avl, 1));

  avl_free(avl);
}

TEST(AvlTest, InsertRLBalance) {
  struct avl *avl = intptr_init();
  EXPECT_EQ(0, intptr_insert(avl, 2));
  EXPECT_EQ(0, intptr_insert(avl, 1));
  EXPECT_EQ(0, intptr_insert(avl, 6));
  EXPECT_EQ(0, intptr_insert(avl, 4));
  EXPECT_EQ(0, intptr_insert(avl, 7));
  EXPECT_EQ(0, intptr_insert(avl, 3));

  EXPECT_EQ(2, avl_height(avl));
  EXPECT_TRUE(intptr_find(avl, 7));
  EXPECT_TRUE(intptr_find(avl, 6));
  EXPECT_FALSE(intptr_find(avl, 5));
  EXPECT_TRUE(intptr_find(avl, 4));
  EXPECT_TRUE(intptr_find(avl, 3));
  EXPECT_TRUE(intptr_find(avl, 2));
  EXPECT_TRUE(intptr_find(avl, 1));

  avl_free(avl);
}

TEST(AvlTest, BasicRemove) {
  struct avl *avl = intptr_init();
  EXPECT_EQ(0, intptr_insert(avl, 3));
  EXPECT_TRUE(intptr_find(avl, 3));
  EXPECT_FALSE(intptr_find(avl, 2));

  EXPECT_FALSE(intptr_remove(avl, 2));
  EXPECT_TRUE(intptr_find(avl, 3));
  EXPECT_FALSE(intptr_find(avl, 2));

  EXPECT_TRUE(intptr_remove(avl, 3));
  EXPECT_FALSE(intptr_find(avl, 3));
  EXPECT_FALSE(intptr_find(avl, 2));

  avl_free(avl);
}

TEST(AvlTest, RemoveLeaf) {
  struct avl *avl = intptr_init();
  EXPECT_EQ(0, intptr_insert(avl, 2));
  EXPECT_EQ(0, intptr_insert(avl, 1));
  EXPECT_EQ(0, intptr_insert(avl, 3));

  EXPECT_TRUE(intptr_remove(avl, 3));

  EXPECT_TRUE(intptr_find(avl, 1));
  EXPECT_TRUE(intptr_find(avl, 2));
  EXPECT_FALSE(intptr_find(avl, 3));

  avl_free(avl);
}

TEST(AvlTest, RemoveOneR) {
  struct avl *avl = intptr_init();
  EXPECT_EQ(0, intptr_insert(avl, 2));
  EXPECT_EQ(0, intptr_insert(avl, 1));
  EXPECT_EQ(0, intptr_insert(avl, 3));
  EXPECT_EQ(0, intptr_insert(avl, 4));

  EXPECT_TRUE(intptr_remove(avl, 3));

  EXPECT_TRUE(intptr_find(avl, 1));
  EXPECT_TRUE(intptr_find(avl, 2));
  EXPECT_FALSE(intptr_find(avl, 3));
  EXPECT_TRUE(intptr_find(avl, 4));

  avl_free(avl);
}

TEST(AvlTest, RemoveOneL) {
  struct avl *avl = intptr_init();
  EXPECT_EQ(0, intptr_insert(avl, 3));
  EXPECT_EQ(0, intptr_insert(avl, 2));
  EXPECT_EQ(0, intptr_insert(avl, 4));
  EXPECT_EQ(0, intptr_insert(avl, 1));

  EXPECT_TRUE(intptr_remove(avl, 2));

  EXPECT_TRUE(intptr_find(avl, 1));
  EXPECT_FALSE(intptr_find(avl, 2));
  EXPECT_TRUE(intptr_find(avl, 3));
  EXPECT_TRUE(intptr_find(avl, 4));

  avl_free(avl);
}

TEST(AvlTest, RemoveTwoLeaves) {
  struct avl *avl = intptr_init();
  EXPECT_EQ(0, intptr_insert(avl, 2));
  EXPECT_EQ(0, intptr_insert(avl, 3));
  EXPECT_EQ(0, intptr_insert(avl, 1));

  EXPECT_TRUE(intptr_remove(avl, 2));

  EXPECT_TRUE(intptr_find(avl, 1));
  EXPECT_FALSE(intptr_find(avl, 2));
  EXPECT_TRUE(intptr_find(avl, 3));

  avl_free(avl);
}

TEST(AvlTest, RemoveTwoSubtrees) {
  struct avl *avl = intptr_init();
  EXPECT_EQ(0, intptr_insert(avl, 8));
  EXPECT_EQ(0, intptr_insert(avl, 3));
  EXPECT_EQ(0, intptr_insert(avl, 10));
  EXPECT_EQ(0, intptr_insert(avl, 2));
  EXPECT_EQ(0, intptr_insert(avl, 5));
  EXPECT_EQ(0, intptr_insert(avl, 9));
  EXPECT_EQ(0, intptr_insert(avl, 11));
  EXPECT_EQ(0, intptr_insert(avl, 1));
  EXPECT_EQ(0, intptr_insert(avl, 4));
  EXPECT_EQ(0, intptr_insert(avl, 7));
  EXPECT_EQ(0, intptr_insert(avl, 12));
  EXPECT_EQ(0, intptr_insert(avl, 6));

  EXPECT_TRUE(intptr_remove(avl, 8));

  EXPECT_TRUE(intptr_find(avl, 1));
  EXPECT_TRUE(intptr_find(avl, 2));
  EXPECT_TRUE(intptr_find(avl, 3));
  EXPECT_TRUE(intptr_find(avl, 4));
  EXPECT_TRUE(intptr_find(avl, 5));
  EXPECT_TRUE(intptr_find(avl, 6));
  EXPECT_TRUE(intptr_find(avl, 7));
  EXPECT_FALSE(intptr_find(avl, 8));
  EXPECT_TRUE(intptr_find(avl, 9));
  EXPECT_TRUE(intptr_find(avl, 10));
  EXPECT_TRUE(intptr_find(avl, 11));
  EXPECT_TRUE(intptr_find(avl, 12));

  avl_free(avl);
}

struct avl_arr {
  intptr_t data[6];
  size_t i;
};

void avl_to_arr(const void *data, void *priv) {
  auto arr_p = reinterpret_cast<struct avl_arr *>(priv);
  arr_p->data[arr_p->i++] = reinterpret_cast<intptr_t>(data);
}

TEST(AvlTest, InOrderTraverse) {
  struct avl *avl = intptr_init();
  EXPECT_EQ(0, intptr_insert(avl, 6));
  EXPECT_EQ(0, intptr_insert(avl, 1));
  EXPECT_EQ(0, intptr_insert(avl, 3));
  EXPECT_EQ(0, intptr_insert(avl, 2));
  EXPECT_EQ(0, intptr_insert(avl, 4));
  EXPECT_EQ(0, intptr_insert(avl, 5));

  struct avl_arr arr;
  arr.i = 0;
  avl_order_traverse(avl, avl_to_arr, &arr);
  EXPECT_EQ(1, arr.data[0]);
  EXPECT_EQ(2, arr.data[1]);
  EXPECT_EQ(3, arr.data[2]);
  EXPECT_EQ(4, arr.data[3]);
  EXPECT_EQ(5, arr.data[4]);
  EXPECT_EQ(6, arr.data[5]);
  avl_free(avl);
}

TEST(AvlTest, LargerSample) {
  struct avl *avl = intptr_init();

  for (intptr_t i = 25; i < 75; ++i)
    EXPECT_EQ(0, intptr_insert(avl, i));
  for (intptr_t i = 50; i >= 25; i -= 3)
    EXPECT_TRUE(intptr_remove(avl, i));
  for (intptr_t i = 0; i < 25; ++i)
    EXPECT_EQ(0, intptr_insert(avl, i));
  for (intptr_t i = 75; i < 100; ++i)
    EXPECT_EQ(0, intptr_insert(avl, i));

  for (intptr_t i = 0; i < 25; ++i)
    EXPECT_TRUE(intptr_find(avl, i));
  for (intptr_t i = 25; i <= 50; ++i)
    if ((i - 50) % 3 == 0)
      EXPECT_FALSE(intptr_find(avl, i));
    else
      EXPECT_TRUE(intptr_find(avl, i));
  for (intptr_t i = 51; i < 100; ++i)
    EXPECT_TRUE(intptr_find(avl, i));

  for (intptr_t i = 50; i >= 25; i -= 3)
    EXPECT_EQ(0, intptr_insert(avl, i));

  avl_free(avl);
}

int int_block_compare(const void *root, const void *unplaced) {
  return *reinterpret_cast<const int *>(unplaced) -
         *reinterpret_cast<const int *>(root);
}

int *int_block_alloc(int value) {
  int *ret = reinterpret_cast<int *>(malloc(sizeof(int)));
  *ret = value;
  return ret;
}

bool int_block_remove(struct avl *avl, int value) {
  int *removed = reinterpret_cast<int *>(avl_remove(avl, &value));
  bool ret = removed != nullptr && value == *removed;
  free(removed);
  return ret;
}

TEST(AvlTest, Ownership) {
  struct avl *avl = avl_init(int_block_compare, free);
  int *our_own = int_block_alloc(5);
  EXPECT_EQ(0, avl_insert(avl, our_own, false));
  EXPECT_EQ(0, avl_insert(avl, int_block_alloc(3), true));
  EXPECT_EQ(0, avl_insert(avl, int_block_alloc(4), true));
  EXPECT_EQ(0, avl_insert(avl, int_block_alloc(7), false));
  EXPECT_EQ(0, avl_insert(avl, int_block_alloc(9), true));

  EXPECT_TRUE(int_block_remove(avl, 3));
  EXPECT_TRUE(int_block_remove(avl, 7));

  avl_free(avl);

  EXPECT_EQ(5, *our_own);
  free(our_own);
}

}  // namespace
