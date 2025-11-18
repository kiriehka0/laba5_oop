#include <gtest/gtest.h>
#include "block_memory_resource.h"
#include "dynamic_array.h"

// Тесты для BlockMemoryResource
TEST(BlockMemoryResourceTest, BasicAllocation) {
    BlockMemoryResource mr;
    
    void* ptr1 = mr.allocate(100);
    EXPECT_NE(ptr1, nullptr);
    EXPECT_EQ(mr.get_allocated_blocks_count(), 1);
    
    void* ptr2 = mr.allocate(200);
    EXPECT_NE(ptr2, nullptr);
    EXPECT_EQ(mr.get_allocated_blocks_count(), 2);
    
    mr.deallocate(ptr1, 100);
    EXPECT_EQ(mr.get_allocated_blocks_count(), 1);  
    
    mr.deallocate(ptr2, 200);
    EXPECT_EQ(mr.get_allocated_blocks_count(), 0); 
}

TEST(BlockMemoryResourceTest, SimpleDeallocation) {
    BlockMemoryResource mr;
    
    void* ptr1 = mr.allocate(100);
    EXPECT_EQ(mr.get_allocated_blocks_count(), 1);
    
    mr.deallocate(ptr1, 100);
    EXPECT_EQ(mr.get_allocated_blocks_count(), 0);
}

// Тесты для DynamicArray с простыми типами
TEST(DynamicArrayTest, BasicOperations) {
    BlockMemoryResource mr;
    DynamicArray<int> array(0, &mr);
    
    EXPECT_TRUE(array.empty());
    EXPECT_EQ(array.size(), 0);
    
    array.push_back(1);
    array.push_back(2);
    array.push_back(3);
    
    EXPECT_FALSE(array.empty());
    EXPECT_EQ(array.size(), 3);
    EXPECT_EQ(array[0], 1);
    EXPECT_EQ(array[1], 2);
    EXPECT_EQ(array[2], 3);
}

TEST(DynamicArrayTest, Iterators) {
    DynamicArray<int> array;
    array.push_back(1);
    array.push_back(2);
    array.push_back(3);
    
    int sum = 0;
    for (auto it = array.begin(); it != array.end(); ++it) {
        sum += *it;
    }
    EXPECT_EQ(sum, 6);
    sum = 0;
    for (int value : array) {
        sum += value;
    }
    EXPECT_EQ(sum, 6);
}

TEST(DynamicArrayTest, CopyConstructor) {
    DynamicArray<int> original;
    original.push_back(1);
    original.push_back(2);
    original.push_back(3);
    
    DynamicArray<int> copy = original;
    
    EXPECT_EQ(original.size(), copy.size());
    for (size_t i = 0; i < original.size(); ++i) {
        EXPECT_EQ(original[i], copy[i]);
    }
}

TEST(DynamicArrayTest, MoveConstructor) {
    DynamicArray<int> original;
    original.push_back(1);
    original.push_back(2);
    
    DynamicArray<int> moved = std::move(original);
    
    EXPECT_EQ(original.size(), 0);
    EXPECT_EQ(moved.size(), 2);
    EXPECT_EQ(moved[0], 1);
    EXPECT_EQ(moved[1], 2);
}

TEST(DynamicArrayTest, PopBack) {
    DynamicArray<int> array;
    array.push_back(1);
    array.push_back(2);
    array.push_back(3);
    
    array.pop_back();
    EXPECT_EQ(array.size(), 2);
    EXPECT_EQ(array[0], 1);
    EXPECT_EQ(array[1], 2);
    
    array.pop_back();
    EXPECT_EQ(array.size(), 1);
    EXPECT_EQ(array[0], 1);
    
    array.pop_back();
    EXPECT_TRUE(array.empty());
}

TEST(DynamicArrayTest, Reserve) {
    DynamicArray<int> array;
    EXPECT_EQ(array.capacity(), 0);
    
    array.reserve(10);
    EXPECT_GE(array.capacity(), 10);
    EXPECT_TRUE(array.empty());
    
    array.push_back(1);
    array.push_back(2);
    EXPECT_EQ(array.size(), 2);
    EXPECT_GE(array.capacity(), 10);
}

TEST(DynamicArrayTest, AtMethod) {
    DynamicArray<int> array;
    array.push_back(1);
    array.push_back(2);
    
    EXPECT_EQ(array.at(0), 1);
    EXPECT_EQ(array.at(1), 2);
    
    EXPECT_THROW(array.at(2), std::out_of_range);
    EXPECT_THROW(array.at(10), std::out_of_range);
}

// Тесты для DynamicArray со сложными типами
TEST(DynamicArrayComplexTest, ComplexTypeOperations) {
    DynamicArray<ComplexType> array;
    
    array.push_back(ComplexType(1, 1.0, "First"));
    array.push_back(ComplexType(2, 2.0, "Second"));
    
    EXPECT_EQ(array.size(), 2);
    EXPECT_EQ(array[0].id, 1);
    EXPECT_EQ(array[0].name, "First");
    EXPECT_EQ(array[1].id, 2);
    EXPECT_EQ(array[1].name, "Second");
}

TEST(DynamicArrayComplexTest, ComplexTypeCopy) {
    DynamicArray<ComplexType> original;
    original.push_back(ComplexType(1, 1.5, "Test"));
    
    DynamicArray<ComplexType> copy = original;
    
    EXPECT_EQ(original.size(), copy.size());
    EXPECT_EQ(original[0].id, copy[0].id);
    EXPECT_EQ(original[0].value, copy[0].value);
    EXPECT_EQ(original[0].name, copy[0].name);
}

TEST(DynamicArrayComplexTest, IteratorWithComplexType) {
    DynamicArray<ComplexType> array;
    array.push_back(ComplexType(1, 1.0, "A"));
    array.push_back(ComplexType(2, 2.0, "B"));
    array.push_back(ComplexType(3, 3.0, "C"));
    
    int count = 0;
    for (auto it = array.begin(); it != array.end(); ++it) {
        EXPECT_EQ(it->id, count + 1);
        ++count;
    }
    EXPECT_EQ(count, 3);
}

// Интеграционный тест
TEST(IntegrationTest, MemoryResourceWithDynamicArray) {
    BlockMemoryResource mr;
    
    {
        DynamicArray<int> array(0, &mr);
        for (int i = 0; i < 10; ++i) {
            array.push_back(i);
        }
        
        EXPECT_EQ(array.size(), 10);
        for (int i = 0; i < 10; ++i) {
            EXPECT_EQ(array[i], i);
        }
    }
    EXPECT_EQ(mr.get_allocated_blocks_count(), 0);
}

TEST(MemoryTest, NoMemoryLeaks) {
    BlockMemoryResource mr;
    
    auto initial_allocated = mr.get_allocated_blocks_count();
    
    {
        DynamicArray<int> array1(0, &mr);
        DynamicArray<std::string> array2(0, &mr);
        
        for (int i = 0; i < 10; ++i) {
            array1.push_back(i);
            array2.push_back("test" + std::to_string(i));
        }
    }
    
    EXPECT_EQ(mr.get_allocated_blocks_count(), 0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
