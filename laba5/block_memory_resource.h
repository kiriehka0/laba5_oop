#pragma once
#include <memory_resource>
#include <vector>
#include <list>
#include <cstddef>

class BlockMemoryResource : public std::pmr::memory_resource {
private:
    struct Block {
        void* ptr;
        std::size_t size;
    };
    
    std::vector<Block> allocated_blocks; 
    std::list<Block> free_blocks;        
    
public:
    BlockMemoryResource() = default;
    ~BlockMemoryResource() override;
    
    // Для тестирования
    std::size_t get_allocated_blocks_count() const { return allocated_blocks.size(); }
    std::size_t get_free_blocks_count() const { return free_blocks.size(); }
    
protected:
    void* do_allocate(std::size_t bytes, std::size_t alignment) override;
    void do_deallocate(void* ptr, std::size_t bytes, std::size_t alignment) override;
    bool do_is_equal(const memory_resource& other) const noexcept override;
};
