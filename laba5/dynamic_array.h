#pragma once
#include <memory_resource>
#include <iterator>
#include <stdexcept>
#include <cstddef>
#include <string>
#include <iostream>

// Сложная структура для демонстрации
struct ComplexType {
    int id;
    double value;
    std::string name;
    
    ComplexType(int i = 0, double v = 0.0, const std::string& n = "") 
        : id(i), value(v), name(n) {}
    
    bool operator==(const ComplexType& other) const {
        return id == other.id && value == other.value && name == other.name;
    }
    
    friend std::ostream& operator<<(std::ostream& os, const ComplexType& obj) {
        os << "ComplexType{id=" << obj.id << ", value=" << obj.value 
           << ", name=\"" << obj.name << "\"}";
        return os;
    }
};

template<typename T>
class DynamicArray {
private:
    T* data_;
    std::size_t size_;
    std::size_t capacity_;
    std::pmr::polymorphic_allocator<T> allocator_;
    
public:
    // Итератор
    class Iterator {
    private:
        T* ptr_;
        
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;
        
        Iterator(T* ptr) : ptr_(ptr) {}
        
        reference operator*() const { return *ptr_; }
        pointer operator->() const { return ptr_; }
        
        Iterator& operator++() {
            ++ptr_;
            return *this;
        }
        
        Iterator operator++(int) {
            Iterator temp = *this;
            ++ptr_;
            return temp;
        }
        
        bool operator==(const Iterator& other) const {
            return ptr_ == other.ptr_;
        }
        
        bool operator!=(const Iterator& other) const {
            return ptr_ != other.ptr_;
        }
    };
    
    // Конструкторы
    DynamicArray(std::pmr::memory_resource* mr = std::pmr::get_default_resource()) 
        : data_(nullptr), size_(0), capacity_(0), allocator_(mr) {}
    
    DynamicArray(std::size_t initial_capacity, 
                 std::pmr::memory_resource* mr = std::pmr::get_default_resource())
        : data_(nullptr), size_(0), capacity_(0), allocator_(mr) {
        reserve(initial_capacity);
    }
    
    // Деструктор
    ~DynamicArray() {
        clear();
        if (data_) {
            allocator_.deallocate(data_, capacity_);
        }
    }
    
    // Копирование
    DynamicArray(const DynamicArray& other) 
        : data_(nullptr), size_(0), capacity_(0), allocator_(other.allocator_) {
        reserve(other.capacity_);
        for (std::size_t i = 0; i < other.size_; ++i) {
            push_back(other.data_[i]);
        }
    }
    
    DynamicArray& operator=(const DynamicArray& other) {
        if (this != &other) {
            clear();
            if (capacity_ < other.capacity_) {
                if (data_) {
                    allocator_.deallocate(data_, capacity_);
                }
                capacity_ = 0;
                reserve(other.capacity_);
            }
            for (std::size_t i = 0; i < other.size_; ++i) {
                push_back(other.data_[i]);
            }
        }
        return *this;
    }
    
    // Перемещение
    DynamicArray(DynamicArray&& other) noexcept
        : data_(other.data_), size_(other.size_), capacity_(other.capacity_), 
          allocator_(std::move(other.allocator_)) {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }
    
    DynamicArray& operator=(DynamicArray&& other) noexcept {
        if (this != &other) {
            clear();
            if (data_) {
                allocator_.deallocate(data_, capacity_);
            }
            data_ = other.data_;
            size_ = other.size_;
            capacity_ = other.capacity_;
            allocator_ = std::move(other.allocator_);
            other.data_ = nullptr;
            other.size_ = 0;
            other.capacity_ = 0;
        }
        return *this;
    }
    
    // Методы контейнера
    void push_back(const T& value) {
        if (size_ >= capacity_) {
            reserve(capacity_ == 0 ? 1 : capacity_ * 2);
        }
        std::allocator_traits<std::pmr::polymorphic_allocator<T>>::construct(
            allocator_, data_ + size_, value);
        ++size_;
    }
    
    void push_back(T&& value) {
        if (size_ >= capacity_) {
            reserve(capacity_ == 0 ? 1 : capacity_ * 2);
        }
        std::allocator_traits<std::pmr::polymorphic_allocator<T>>::construct(
            allocator_, data_ + size_, std::move(value));
        ++size_;
    }
    
    void pop_back() {
        if (size_ > 0) {
            --size_;
            std::allocator_traits<std::pmr::polymorphic_allocator<T>>::destroy(
                allocator_, data_ + size_);
        }
    }
    
    T& operator[](std::size_t index) {
        return data_[index];
    }
    
    const T& operator[](std::size_t index) const {
        return data_[index];
    }
    
    T& at(std::size_t index) {
        if (index >= size_) {
            throw std::out_of_range("Index out of range");
        }
        return data_[index];
    }
    
    const T& at(std::size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("Index out of range");
        }
        return data_[index];
    }
    
    void reserve(std::size_t new_capacity) {
        if (new_capacity > capacity_) {
            T* new_data = allocator_.allocate(new_capacity);
            for (std::size_t i = 0; i < size_; ++i) {
                try {
                    std::allocator_traits<std::pmr::polymorphic_allocator<T>>::construct(
                        allocator_, new_data + i, std::move_if_noexcept(data_[i]));
                    std::allocator_traits<std::pmr::polymorphic_allocator<T>>::destroy(
                        allocator_, data_ + i);
                } catch (...) {
                    for (std::size_t j = 0; j < i; ++j) {
                        std::allocator_traits<std::pmr::polymorphic_allocator<T>>::destroy(
                            allocator_, new_data + j);
                    }
                    allocator_.deallocate(new_data, new_capacity);
                    throw;
                }
            }
            
            // Освобождаем старую память
            if (data_) {
                allocator_.deallocate(data_, capacity_);
            }
            
            data_ = new_data;
            capacity_ = new_capacity;
        }
    }
    
    void clear() {
        for (std::size_t i = 0; i < size_; ++i) {
            std::allocator_traits<std::pmr::polymorphic_allocator<T>>::destroy(
                allocator_, data_ + i);
        }
        size_ = 0;
    }
    
    std::size_t size() const { return size_; }
    std::size_t capacity() const { return capacity_; }
    bool empty() const { return size_ == 0; }
    
    // Итераторы
    Iterator begin() { return Iterator(data_); }
    Iterator end() { return Iterator(data_ + size_); }
    Iterator begin() const { return Iterator(data_); }
    Iterator end() const { return Iterator(data_ + size_); }
};
