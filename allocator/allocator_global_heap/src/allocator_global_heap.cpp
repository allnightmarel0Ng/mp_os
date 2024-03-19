#include "../include/allocator_global_heap.h"

allocator_global_heap::allocator_global_heap(logger *logger)
{
    trace_with_guard("START: allocator_global_heap constructor\n");
    _logger = logger;
    trace_with_guard("END: allocator_global_heap constructor\n");
}

allocator_global_heap::~allocator_global_heap() 
{
    trace_with_guard("START: allocator_global_heap destructor\n");
    trace_with_guard("END: allocator_global_heap destructor\n");
}

allocator_global_heap::allocator_global_heap(
    allocator_global_heap &&other) noexcept
{
    trace_with_guard("START: allocator_global_heap move constructor\n");
    _logger = std::exchange(other._logger, nullptr);
    trace_with_guard("END: allocator_global_heap move constructor\n");
}

allocator_global_heap &allocator_global_heap::operator=(
    allocator_global_heap &&other) noexcept
{
    trace_with_guard("START: allocator_global_heap move operator\n");

    if (this == &other) {
        return *this;
    }
    std::swap(_logger, other._logger);

    trace_with_guard("END: allocator_global_heap move operator\n");
    return *this;
}

[[nodiscard]] void *allocator_global_heap::allocate(size_t value_size,
    size_t values_count)
{
    debug_with_guard("START: allocator_global_heap: allocate()\n");

    block_size_t block_size = value_size * values_count;
    block_size_t meta_size = sizeof(allocator *) + sizeof(size_t);
    
    block_pointer_t block = ::operator new(block_size + meta_size);
    if (block == nullptr) {
        error_with_guard("allocation error\n");
        throw std::bad_alloc{};
    }
        
    block_pointer_t tmp = block;
    *reinterpret_cast<allocator **>(tmp) = this;
    tmp += sizeof(allocator *);
    *reinterpret_cast<size_t *>(tmp) = block_size;

    block_pointer_t out = reinterpret_cast<uint8_t *>(block) + meta_size;

    debug_with_guard("END: allocator_global_heap: allocate()\n");
    return out;
}

void allocator_global_heap::deallocate(void *at)
{
    debug_with_guard("START: allocator_global_heap: deallocate()\n");
    
    block_pointer_t block = reinterpret_cast<uint8_t *>(at) - 
        sizeof(allocator *) - sizeof(size_t);

    if (*reinterpret_cast<allocator **>(block) != this) {
        error_with_guard("wrong allocator to deallocate memory\n");
        throw std::logic_error("wrong allocator to deallocate\n");
    }

    block_size_t block_size = *(reinterpret_cast<uint8_t *>(at) - sizeof(size_t));

    int *bytes = reinterpret_cast<int *>(at);
    std::string bytes_array;
    for (block_size_t i = 0; i < block_size; i++) {
        bytes_array += std::to_string(*bytes++);
        if (i != block_size - 1) {
            bytes_array += ' ';
        }
    }

    debug_with_guard("bytes array: " + bytes_array);

    ::operator delete(block);

    debug_with_guard("END: allocator_global_heap: deallocate()\n");
}

inline logger *allocator_global_heap::get_logger() const
{
    return _logger;
}

inline std::string allocator_global_heap::get_typename() const noexcept
{
    trace_with_guard("START: allocator_global_heap: get_typename()\n");
    trace_with_guard("END: allocator_global_heap: get_typename()\n");
    return "allocator_global_heap";
}
