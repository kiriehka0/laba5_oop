#include <iostream>
#include "block_memory_resource.h"
#include "dynamic_array.h"

int main() {
    BlockMemoryResource block_mr;
    
    std::cout << "=== Демонстрация с простыми типами (int) ===" << std::endl;
    {
        DynamicArray<int> int_array(0, &block_mr);
        for (int i = 0; i < 10; ++i) {
            int_array.push_back(i * i);
        }
        std::cout << "Элементы массива: ";
        for (auto it = int_array.begin(); it != int_array.end(); ++it) {
            std::cout << *it << " ";
        }
        std::cout << std::endl;
        std::cout << "int_array[5] = " << int_array[5] << std::endl;
        
        std::cout << "Размер: " << int_array.size() 
                  << ", Емкость: " << int_array.capacity() << std::endl;
    }
    
    std::cout << "\n=== Демонстрация со сложными типами ===" << std::endl;
    {
        DynamicArray<ComplexType> complex_array(0, &block_mr);
        complex_array.push_back(ComplexType(1, 3.14, "Pi"));
        complex_array.push_back(ComplexType(2, 2.71, "Euler"));
        complex_array.push_back(ComplexType(3, 1.41, "Square root of 2"));
        std::cout << "Сложные объекты:" << std::endl;
        for (auto it = complex_array.begin(); it != complex_array.end(); ++it) {
            std::cout << "  " << *it << std::endl;
        }

        complex_array[0].value = 3.14159;
        complex_array.push_back(ComplexType(4, 0.0, "Zero"));
        
        std::cout << "\nПосле модификации:" << std::endl;
        for (const auto& item : complex_array) {
            std::cout << "  " << item << std::endl;
        }
        
        std::cout << "Размер: " << complex_array.size() 
                  << ", Емкость: " << complex_array.capacity() << std::endl;
    }
    
    return 0;
}
