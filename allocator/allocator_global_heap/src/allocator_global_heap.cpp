#include "../include/allocator_global_heap.h"

allocator_global_heap::allocator_global_heap(logger *logger)
    : _logger(logger)
{
    trace_with_guard("START: " + get_typename() + ": constructor\n");
    trace_with_guard("END: " + get_typename() + ": constructor\n");
}

allocator_global_heap::~allocator_global_heap() 
{
    trace_with_guard("START: " + get_typename() + ": destructor");
    trace_with_guard("END: " + get_typename() + ": destructor");
}

allocator_global_heap::allocator_global_heap(
    allocator_global_heap &&other) noexcept
    : _logger(std::exchange(other._logger, nullptr))
{
    trace_with_guard("START: " + get_typename() + ": move constructor");
    trace_with_guard("END: "  + get_typename() + ": move constructor");
}

allocator_global_heap &allocator_global_heap::operator=(
    allocator_global_heap &&other) noexcept
{
    trace_with_guard("START: " + get_typename() + ": move operator");

    if (this == &other) {
        trace_with_guard("END: " + get_typename() + ": move operator");
        return *this;
    }
    std::swap(_logger, other._logger);

    trace_with_guard("END: " + get_typename() + ": move operator");
    return *this;
}

[[nodiscard]] void *allocator_global_heap::allocate(size_t value_size,
    size_t values_count)
{
    debug_with_guard("START: " + get_typename() + ": allocate()");

    block_size_t block_size = value_size * values_count;
    block_size_t meta_size = sizeof(allocator *) + sizeof(size_t);
    
    block_pointer_t block;
    try 
    {
        block = ::operator new(block_size + meta_size);
    }
    catch (std::bad_alloc &exception)
    {
        error_with_guard(get_typename() + ": allocation error");
        throw exception;
    }
        
    uint8_t *tmp = reinterpret_cast<uint8_t *>(block);
    *reinterpret_cast<allocator **>(tmp) = this;
    tmp += sizeof(allocator *);
    *reinterpret_cast<size_t *>(tmp) = block_size;
    tmp += sizeof(size_t);

    debug_with_guard("END: " + get_typename() + ": allocate()");
    return reinterpret_cast<void *>(tmp);
}

void allocator_global_heap::deallocate(void *at)
{
    debug_with_guard("START: " + get_typename() + ": deallocate()");
    
    block_pointer_t block = reinterpret_cast<uint8_t *>(at) - 
        sizeof(allocator *) - sizeof(size_t);

    if (*reinterpret_cast<allocator **>(block) != this) {
        error_with_guard(get_typename() + " wrong allocator to deallocate memory");
        throw std::logic_error(get_typename() + " wrong allocator to deallocate");
    }

    block_size_t block_size = *(reinterpret_cast<uint8_t *>(at) - sizeof(size_t));

    int *bytes = reinterpret_cast<int *>(at);
    std::string bytes_array;
    for (block_size_t i = 0; i < block_size; ++i) {
        bytes_array += std::to_string(*bytes++);
        if (i != block_size - 1) {
            bytes_array += ' ';
        }
    }

    debug_with_guard(get_typename() + " bytes array: " + bytes_array);

    ::operator delete(block);

    debug_with_guard("END: " + get_typename() + ": deallocate()");
}

inline logger *allocator_global_heap::get_logger() const
{
    return _logger;
}

inline std::string allocator_global_heap::get_typename() const noexcept
{
    return "allocator_global_heap";
}
