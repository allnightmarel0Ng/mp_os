#include "../include/allocator_sorted_list.h"

allocator_sorted_list::~allocator_sorted_list()
{
    automatic_logger auto_log(logger::severity::trace, "destructor", get_typename(), get_logger());
    clear_trusted_memory(*this);
}

allocator_sorted_list::allocator_sorted_list(allocator_sorted_list const &other)
{
    automatic_logger auto_log(logger::severity::trace, "copy constructor", get_typename(), other.get_logger());
    try
    {
        _trusted_memory = copy_trusted_memory(other);
    }
    catch(std::bad_alloc const &exception)
    {
        throw exception;
    }
}

allocator_sorted_list &allocator_sorted_list::operator=(allocator_sorted_list const &other)
{
    automatic_logger auto_log(logger::severity::trace, "copy assignment", get_typename(), get_logger());
    
    if (this == &other)
    {
        return *this;
    }
    
    clear_trusted_memory(*this);
    try
    {
        _trusted_memory = copy_trusted_memory(other);
    }
    catch (std::bad_alloc const &exception)
    {
        throw exception;
    }

    return *this;
}

allocator_sorted_list::allocator_sorted_list(allocator_sorted_list &&other) noexcept
{
    automatic_logger auto_log(logger::severity::trace, "move constructor", get_typename(), other.get_logger());

    _trusted_memory = std::exchange(other._trusted_memory, nullptr);
}

allocator_sorted_list &allocator_sorted_list::operator=(allocator_sorted_list &&other) noexcept
{
    automatic_logger auto_log(logger::severity::trace, "move assignment", get_typename(), get_logger());
    
    if (this == &other)
    {
        return *this;
    }

    clear_trusted_memory(*this);
    _trusted_memory = std::exchange(other._trusted_memory, nullptr);

    return *this;
}

void allocator_sorted_list::clear_trusted_memory(allocator_sorted_list &at) noexcept
{
    at.deallocate_with_guard(at._trusted_memory);
    at._trusted_memory = nullptr;
}

void *allocator_sorted_list::copy_trusted_memory(allocator_sorted_list const &from)
{
    block_size_t size = from.get_full_size();
    void *result;
    try
    {
        result = from.allocate_with_guard(size);

    }
    catch (std::bad_alloc const &exception)
    {
        throw exception;
    }

    memcpy(result, from._trusted_memory, size);
    return result;
}

allocator_sorted_list::allocator_sorted_list(size_t space_size, allocator *parent_allocator, logger *logger_instance, allocator_with_fit_mode::fit_mode allocate_fit_mode)
{
    automatic_logger auto_log(logger::severity::trace, "constructor", get_typename(), logger_instance);

    if (space_size < get_available_meta_size())
    {
        std::string message = "Unable to create allocator on such small space size";
        if (logger_instance != nullptr)
        {
            logger_instance->log(message, logger::severity::error);
        }
        throw std::logic_error(message);
    }
    
    block_size_t full_size = space_size + get_main_meta_size();

    try
    {
        _trusted_memory = parent_allocator == nullptr
            ? ::operator new(full_size)
            : parent_allocator->allocate(full_size, 1);
    }
    catch (std::bad_alloc const &exception)
    {
        if (logger_instance != nullptr)
        {
            logger_instance->log(get_typename() + ": constructor: allocation error", logger::severity::error);
        }
        throw exception;
    }

    uint8_t *serialization = reinterpret_cast<uint8_t *>(_trusted_memory);

    *reinterpret_cast<block_size_t *>(serialization) = full_size;
    serialization += sizeof(block_size_t);

    *reinterpret_cast<block_size_t *>(serialization) = space_size;
    serialization += sizeof(block_size_t);

    *reinterpret_cast<logger **>(serialization) = logger_instance;
    serialization += sizeof(logger *);

    *reinterpret_cast<allocator **>(serialization) = parent_allocator;
    serialization += sizeof(allocator *);

    allocator::construct(reinterpret_cast<std::mutex *>(serialization));
    serialization += sizeof(std::mutex);

    *reinterpret_cast<fit_mode *>(serialization) = allocate_fit_mode;
    serialization += sizeof(fit_mode);

    *reinterpret_cast<block_pointer_t *>(serialization) = reinterpret_cast<block_pointer_t>(serialization + sizeof(block_pointer_t));
    serialization += sizeof(block_pointer_t);

    available_meta_serialization(serialization, space_size, nullptr);
}

[[nodiscard]] void *allocator_sorted_list::allocate(size_t value_size, size_t values_count)
{
    std::lock_guard<std::mutex> lock(get_mutex());
    automatic_logger auto_log(logger::severity::debug, "allocate", get_typename(), get_logger());

    block_size_t requested_size = value_size * values_count + get_occupied_meta_size();
    if (requested_size < get_available_meta_size())
    {
        requested_size = get_available_meta_size();
        warning_with_guard(get_typename() + ": allocate: allocation size was changed to " + std::to_string(requested_size));
    }
    
    block_size_t *available_space = get_available_space_address();
    if (requested_size > *available_space)
    {
        error_with_guard(get_typename() + ": allocate: unable to allocate " + std::to_string(values_count) + " elements of " + std::to_string(value_size) + " size");
        throw std::bad_alloc{};
    }

    fit_mode mode = get_fit_mode();

    block_pointer_t first_available_block = get_first_available_block();

    block_pointer_t result;
    try
    {
        if (mode == fit_mode::first_fit)
        {
            result = allocate_first_fit(first_available_block, requested_size);
        }
        else
        {
            result = allocate_worst_best_fit(first_available_block, requested_size, mode);
        }
    }
    catch (std::bad_alloc const &exception)
    {
        error_with_guard(get_typename() + ": allocate: unable to allocate " + std::to_string(values_count) + " elements of " + std::to_string(value_size) + " size" );
        throw exception;
    }

    *available_space -= requested_size;

    information_with_guard(get_typename() + ": allocate: available space in trusted memory after allocation of " + std::to_string(values_count * value_size) + " bytes is " + std::to_string(*available_space));

    debug_with_guard(get_typename() + ": allocate: blocks information after allocation is " + get_blocks_visualization(get_blocks_info()));

    return result;
}

allocator::block_pointer_t allocator_sorted_list::allocate_first_fit(block_pointer_t first_available_block, block_size_t requested_size)
{
    block_meta_t current(reinterpret_cast<uint8_t *>(first_available_block)), prev;

    while (true)
    {
        if (current._size >= requested_size)
        {
            return create_occupied_block(prev, current, requested_size);
        }

        if (current._next == nullptr)
        {
            break;
        }

        prev = current;
        current = block_meta_t(reinterpret_cast<uint8_t *>(current._next));
    }

    throw std::bad_alloc{};
}

allocator::block_pointer_t allocator_sorted_list::allocate_worst_best_fit(block_pointer_t first_available_block, block_size_t requested_size, fit_mode mode)
{
    block_meta_t current(reinterpret_cast<uint8_t *>(first_available_block)), prev, best, best_prev;
    best._size = mode == fit_mode::the_best_fit
        ? get_full_size()
        : 0;

    bool block_found = false;
    
    while (true)
    {
        bool compare = mode == fit_mode::the_best_fit
            ? current._size <= best._size
            : current._size >= best._size;
        
        if (current._size >= requested_size && compare)
        {
            best_prev = prev;
            best = current;
            block_found = true;
        }

        if (current._next == nullptr)
        {
            break;
        }

        prev = current;
        current = block_meta_t(reinterpret_cast<uint8_t *>(current._next));
    }

    if (!block_found)
    {
        throw std::bad_alloc{};
    }

    return create_occupied_block(best_prev, best, requested_size);
}

void allocator_sorted_list::deallocate(void *at)
{
    std::lock_guard<std::mutex> lock(get_mutex());
    automatic_logger auto_log(logger::severity::debug, "deallocate", get_typename(), get_logger());

    if (at == nullptr)
    {
        return;
    }

    block_meta_t at_meta(reinterpret_cast<uint8_t *>(at) - get_occupied_meta_size());
    if (at_meta._allocator != reinterpret_cast<allocator *>(this))
    {
        throw std::logic_error(get_typename() + ": deallocate: wrong allocator instance to deallocate memory");
    }

    uint8_t *allocatable_memory_end = get_casted_trusted_memory() + get_full_size();
    
    block_meta_t current(get_casted_trusted_memory() + get_main_meta_size());

    block_meta_t prev_available, prev;
    
    while (true)
    {
        if (at_meta._start == current._start)
        {
            at_meta._size = abs(at_meta._size);
            uint8_t *next = at_meta._start + at_meta._size;
            bool merged = false;

            if (next != allocatable_memory_end)
            {
                block_meta_t next_meta(next);
                if (next_meta._size > 0)
                {
                    merge(at_meta, next_meta);
                    at_meta._next = next_meta._next;
                    at_meta._size += next_meta._size;
                    merged = true;
                }
            }
            
            if (prev_available._start == nullptr)
            {
                if (!merged)
                {
                    available_meta_serialization(at_meta._start, at_meta._size, get_first_available_block());
                }
                set_first_available_block(at_meta._start);
                break;
            }

            if (prev_available._start == prev._start)
            {
                merge(prev_available, at_meta);
            }
            else
            {
                available_meta_serialization(at_meta._start, at_meta._size, merged ? at_meta._next : prev_available._next);
                available_meta_serialization(prev_available._start, prev_available._size, at_meta._start);
            }
            break;            
        }

        if (current._start + abs(current._size) == allocatable_memory_end)
        {
            break;
        }
        if (current._size > 0)
        {
            prev_available = current;
        }
        prev = current;
        current = block_meta_t(current._start + abs(current._size));
    }

    block_size_t *available_space = get_available_space_address();
    *available_space += at_meta._size;

    information_with_guard(get_typename() + ": deallocate: free space in trusted memory after deallocation of " + std::to_string(at_meta._size) + " is " + std::to_string(*available_space));

    int *bytes = reinterpret_cast<int *>(get_casted_trusted_memory() + get_main_meta_size());
    block_size_t workspace_size = get_full_size() - get_main_meta_size();
    std::string bytes_message = get_typename() + ": deallocate: array of bytes after deallocation: ";

    for (block_size_t i = 0; i < workspace_size; ++i)
    {
        bytes_message += std::to_string(*bytes++);
        bytes_message += ' ';
    }
    debug_with_guard(bytes_message);

    debug_with_guard(get_typename() + ": deallocate: blocks information after deallocation is " + get_blocks_visualization(get_blocks_info()));
}

allocator_sorted_list::block_meta_t::block_meta_t(uint8_t *at)
{
    block_size_t offset = 0;
    _start = at;
    
    if (_start == nullptr)
    {
        _size = 0;
        _allocator = nullptr;
        _next = nullptr;
        return;
    }

    _size = *reinterpret_cast<int *>(at);
    offset += sizeof(int);

    if (_size > 0)
    {
        _allocator = nullptr;
        _next = *reinterpret_cast<block_pointer_t *>(at + offset);
    }
    else
    {
        _next = nullptr;
        _allocator = *reinterpret_cast<allocator **>(at + offset);
    }
}

allocator_sorted_list::block_meta_t::block_meta_t():
    _size(0), _start(nullptr), _next(nullptr), _allocator(nullptr)
{

}

allocator::block_pointer_t allocator_sorted_list::create_occupied_block(block_meta_t prev, block_meta_t current, block_size_t requested_size) noexcept
{
    if (current._size - requested_size < get_available_meta_size())
    {
        requested_size += current._size - requested_size;
        warning_with_guard(get_typename() + ": allocate: allocation size was changed to " + std::to_string(requested_size));
        
        if (prev._start == nullptr)
        {
            set_first_available_block(current._next);
        }
        else
        {
            available_meta_serialization(prev._start, prev._size, current._next);
        }

        occupied_meta_serialization(current._start, requested_size);
        return current._start + get_occupied_meta_size();
    }

    uint8_t *next = current._start + requested_size == get_casted_trusted_memory() + get_full_size()
        ? nullptr
        : current._start + requested_size;

    if (prev._start == nullptr)
    {
        set_first_available_block(next);
    }
    else
    {
        available_meta_serialization(prev._start, prev._size, next);
    }

    if (next != nullptr)
    {
        available_meta_serialization(next, current._size - requested_size, current._next);
    }

    occupied_meta_serialization(current._start, requested_size);
    return current._start + get_occupied_meta_size();
}

void allocator_sorted_list::merge(block_meta_t one, block_meta_t another) const noexcept
{
    available_meta_serialization(one._start, one._size + another._size, another._next);
}

void allocator_sorted_list::available_meta_serialization(uint8_t *at, block_size_t size, block_pointer_t next) const noexcept
{
    *reinterpret_cast<int *>(at) = static_cast<int>(size);
    at += sizeof(int);

    *reinterpret_cast<block_pointer_t *>(at) = next;
}

void allocator_sorted_list::occupied_meta_serialization(uint8_t *at, int size) noexcept
{
    *reinterpret_cast<int *>(at) = -size;
    at += sizeof(int);

    *reinterpret_cast<allocator **>(at) = reinterpret_cast<allocator *>(this);
}

inline void allocator_sorted_list::set_fit_mode(allocator_with_fit_mode::fit_mode mode)
{
    *reinterpret_cast<fit_mode *>(get_casted_trusted_memory() + get_main_meta_size() - sizeof(block_pointer_t) - sizeof(fit_mode)) = mode;
}

inline allocator::block_size_t allocator_sorted_list::get_main_meta_size() const noexcept
{
    return 2 * sizeof(block_size_t) + sizeof(logger *) + sizeof(allocator *) + sizeof(std::mutex) + sizeof(fit_mode) + sizeof(block_pointer_t);
}

inline allocator::block_size_t allocator_sorted_list::get_occupied_meta_size() const noexcept
{
    return sizeof(int) + sizeof(allocator *);
}

inline allocator::block_size_t allocator_sorted_list::get_available_meta_size() const noexcept
{
    return sizeof(int) + sizeof(block_pointer_t);
}

inline uint8_t *allocator_sorted_list::get_casted_trusted_memory() const noexcept
{
    return reinterpret_cast<uint8_t *>(_trusted_memory);
}

inline allocator::block_size_t allocator_sorted_list::get_full_size() const noexcept
{
    return *reinterpret_cast<block_size_t *>(get_casted_trusted_memory());
}

inline allocator::block_size_t *allocator_sorted_list::get_available_space_address() const noexcept
{
    return reinterpret_cast<block_size_t *>(get_casted_trusted_memory() + sizeof(block_size_t));
}

inline std::mutex &allocator_sorted_list::get_mutex() const noexcept
{
    return *reinterpret_cast<std::mutex *>(get_casted_trusted_memory() + 2 * sizeof(block_pointer_t) + sizeof(logger *) + sizeof(allocator *));
}

inline allocator_with_fit_mode::fit_mode allocator_sorted_list::get_fit_mode() const noexcept
{
    return *reinterpret_cast<fit_mode *>(get_casted_trusted_memory() + get_main_meta_size() - sizeof(block_pointer_t) - sizeof(fit_mode));
}

inline allocator::block_pointer_t allocator_sorted_list::get_first_available_block() const noexcept
{
    return *reinterpret_cast<block_pointer_t *>(get_casted_trusted_memory() + get_main_meta_size() - sizeof(block_pointer_t));
}

inline allocator *allocator_sorted_list::get_allocator() const
{
    return *reinterpret_cast<allocator **>(get_casted_trusted_memory() + 2 * sizeof(block_pointer_t) + sizeof(logger *));
}

inline logger *allocator_sorted_list::get_logger() const
{
    return *reinterpret_cast<logger **>(get_casted_trusted_memory() + 2 * sizeof(block_pointer_t));
}

inline void allocator_sorted_list::set_first_available_block(block_pointer_t at) const noexcept
{
    *reinterpret_cast<block_pointer_t *>(get_casted_trusted_memory() + get_main_meta_size() - sizeof(block_pointer_t)) = at;
}

std::vector<allocator_test_utils::block_info> allocator_sorted_list::get_blocks_info() const noexcept
{
    std::vector<block_info> result;

    block_meta_t current(get_casted_trusted_memory() + get_main_meta_size());
    uint8_t *allocatable_memory_end = get_casted_trusted_memory() + get_full_size();
    block_info info;

    while (true)
    {
        info.is_block_occupied = current._size > 0
            ? false
            : true;
        info.block_size = abs(current._size);

        result.push_back(info);

        uint8_t *next = current._start + abs(current._size);
        if (next == allocatable_memory_end)
        {
            break;
        }

        current = block_meta_t(next);
    }

    return result;
}

std::string allocator_sorted_list::get_blocks_visualization(std::vector<allocator_test_utils::block_info> const &blocks_information) const noexcept
{
    std::string result = "| ";

    for (auto block_info : blocks_information)
    {
        result += (block_info.is_block_occupied)
            ? "occup "
            : "avail ";
        result += std::to_string(block_info.block_size);
        result += " | ";
    }

    return result;
}

inline std::string allocator_sorted_list::get_typename() const noexcept
{
    return "allocator_sorted_list";
}