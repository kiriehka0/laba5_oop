#include "block_memory_resource.h"
#include <new>
#include <algorithm>

BlockMemoryResource::~BlockMemoryResource() {
    for (const auto& block : allocated_blocks) {
        ::operator delete(block.ptr);
    }
    for (const auto& block : free_blocks) {
        ::operator delete(block.ptr);
    }
}

void* BlockMemoryResource::do_allocate(std::size_t bytes, std::size_t alignment) {
    for (auto it = free_blocks.begin(); it != free_blocks.end(); ++it) {
        if (it->size >= bytes) {  
            void* ptr = it->ptr;
            allocated_blocks.push_back({ptr, it->size});
            free_blocks.erase(it);
            return ptr;
        }
    }
    
    void* ptr = ::operator new(bytes);
    allocated_blocks.push_back({ptr, bytes});  
    return ptr;
}

void BlockMemoryResource::do_deallocate(void* ptr, std::size_t bytes, std::size_t alignment) {
    auto it = std::find_if(allocated_blocks.begin(), allocated_blocks.end(),
                          [ptr](const Block& block) { return block.ptr == ptr; });
    
    if (it != allocated_blocks.end()) {
        free_blocks.push_back({ptr, it->size}); 
        allocated_blocks.erase(it);  
    }
}

bool BlockMemoryResource::do_is_equal(const memory_resource& other) const noexcept {
    return this == &other;
}
