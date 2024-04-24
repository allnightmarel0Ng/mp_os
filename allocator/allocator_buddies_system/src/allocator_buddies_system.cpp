#include "../include/allocator_buddies_system.h"

allocator_buddies_system::~allocator_buddies_system()
{
    automatic_logger auto_log(logger::severity::trace, "destructor", get_typename(), get_logger());
    
    clear_trusted_memory(*this);
}

allocator_buddies_system::allocator_buddies_system(allocator_buddies_system const &other)
{
    automatic_logger auto_log(logger::severity::trace, "copy constructor", get_typename(), other.get_logger());
    
    try
    {
        _trusted_memory = copy_trusted_memory(other);
    }
    catch (std::bad_alloc const &exception)
    {
        throw exception;
    }
}

allocator_buddies_system &allocator_buddies_system::operator=(allocator_buddies_system const &other)
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

allocator_buddies_system::allocator_buddies_system(allocator_buddies_system &&other) noexcept
{
    automatic_logger auto_log(logger::severity::trace, "move constructor", get_typename(), other.get_logger());
    
    _trusted_memory = std::exchange(other._trusted_memory, nullptr);
}
allocator_buddies_system &allocator_buddies_system::operator=(allocator_buddies_system &&other) noexcept
{
    automatic_logger(logger::severity::trace, "move assignment", get_typename(), get_logger());
    
    if (this == &other)
    {
        return *this;
    }
    
    clear_trusted_memory(*this);
    _trusted_memory = std::exchange(other._trusted_memory, nullptr);

    return *this;
}

void allocator_buddies_system::clear_trusted_memory(allocator_buddies_system &at) noexcept
{
    at.deallocate_with_guard(at._trusted_memory);
    at._trusted_memory = nullptr;
}

void *allocator_buddies_system::copy_trusted_memory(allocator_buddies_system const &from)
{
    block_size_t other_memory_size = (1 << from.get_allocatable_memory_order()) + get_main_meta_size();

    void *result;
    try
    {
        result = from.allocate_with_guard(other_memory_size);
    }
    catch (std::bad_alloc const &exception)
    {
        throw exception;
    }

    std::memcpy(result, from._trusted_memory, other_memory_size);

    return result;
}

allocator_buddies_system::allocator_buddies_system(size_t space_size, allocator *parent_allocator,logger *logger_instance, allocator_with_fit_mode::fit_mode allocate_fit_mode)
{
    automatic_logger auto_log(logger::severity::trace, "constructor", get_typename(), logger_instance);

    block_size_t main_meta_size = get_main_meta_size();
    if (space_size < get_order(2 * sizeof(block_pointer_t) + sizeof(int8_t)))
    {
        throw std::logic_error("Unable to create allocator on such small space size");
    }
    
    block_size_t allocatable_memory_size = 1 << space_size;
    block_size_t full_memory_size = allocatable_memory_size + main_meta_size;

    try
    {
        _trusted_memory = parent_allocator == nullptr
            ? ::operator new(full_memory_size)
            : parent_allocator->allocate(full_memory_size, 1);        
    }
    catch (std::bad_alloc const &exception)
    {
        throw exception;
    }

    uint8_t *serialization = reinterpret_cast<uint8_t *>(_trusted_memory);

    *serialization = static_cast<uint8_t>(space_size);
    ++serialization;

    *reinterpret_cast<block_size_t *>(serialization) = allocatable_memory_size;
    serialization += sizeof(block_size_t);

    *reinterpret_cast<allocator **>(serialization) = parent_allocator;
    serialization += sizeof(allocator *);

    *reinterpret_cast<logger **>(serialization) = logger_instance;
    serialization += sizeof(logger *);

    allocator::construct(reinterpret_cast<std::mutex *>(serialization));
    serialization += sizeof(std::mutex);

    *reinterpret_cast<fit_mode *>(serialization) = allocate_fit_mode;
    serialization += sizeof(fit_mode);

    *reinterpret_cast<block_pointer_t *>(serialization) = reinterpret_cast<block_pointer_t>(serialization + sizeof(block_pointer_t));
    serialization += sizeof(block_pointer_t);

    available_meta_serialization(serialization, static_cast<int8_t>(space_size), nullptr, nullptr);
}

[[nodiscard]] void *allocator_buddies_system::allocate(size_t value_size, size_t values_count)
{
    std::lock_guard<std::mutex> lock(get_mutex());
    automatic_logger auto_log(logger::severity::debug, "allocate", get_typename(), get_logger());
        
    block_size_t requested_size = value_size * values_count + get_occupied_meta_size();

    uint8_t requested_order = get_order(requested_size);
    uint8_t available_meta_order = get_order(get_available_meta_size());
    requested_order = requested_order >= available_meta_order
        ? requested_order
        : available_meta_order, warning_with_guard(get_typename() + ": allocate: allocation size was changed to " + std::to_string(1 << requested_order));
    
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
            result = allocate_first_fit(first_available_block, requested_order);
        }
        else
        {
            result = allocate_worst_best_fit(first_available_block, requested_order, mode);
        }
    }
    catch (std::bad_alloc const &exception)
    {
        error_with_guard(get_typename() + ": allocate: unable to allocate " + std::to_string(values_count) + " elements of " + std::to_string(value_size) + " size");
        throw exception;
    }

    *available_space -= requested_size;

    information_with_guard(get_typename() + ": allocate: available space in trusted memory after allocation of " + std::to_string(values_count * value_size) + " bytes is " + std::to_string(*available_space));

    debug_with_guard(get_typename() + ": allocate: blocks information after allocation is " + get_blocks_visualization(get_blocks_info()));

    return reinterpret_cast<uint8_t *>(result) + get_occupied_meta_size();

}

void *allocator_buddies_system::allocate_first_fit(block_pointer_t first_available, uint8_t required_order)
{
    block_meta_t current(reinterpret_cast<uint8_t *>(first_available));
    
    while (true)
    {   
        if (current._order >= required_order)
        {
            return split(current._start, required_order);
        }
        if (current._next == nullptr)
        {
            break;
        }
        current = block_meta_t(reinterpret_cast<uint8_t *>(current._next));
    }

    throw std::bad_alloc{};
}

void *allocator_buddies_system::allocate_worst_best_fit(block_pointer_t first_available, uint8_t required_order, fit_mode mode)
{
    block_meta_t best;
    best._order = mode == fit_mode::the_best_fit 
        ? get_allocatable_memory_order() 
        : 0;

    block_meta_t current(reinterpret_cast<uint8_t *>(first_available));

    bool block_found = false;

    while (true)
    {
        bool compare = mode == fit_mode::the_best_fit
            ? current._order <= best._order
            : current._order >= best._order;

        if (current._order >= required_order && compare)
        {
            best = current;
            block_found = true;
        }

        if (current._next == nullptr)
        {
            break;
        }
        current = block_meta_t(reinterpret_cast<uint8_t *>(current._next));
    }

    if (!block_found)
    {
        throw std::bad_alloc{};
    }

    return split(best._start, required_order);
}

void allocator_buddies_system::deallocate(void *at)
{
    std::lock_guard<std::mutex> lock(get_mutex());
    automatic_logger auto_log(logger::severity::debug, "deallocate", get_typename(), get_logger());
    
    block_meta_t at_meta(reinterpret_cast<uint8_t *>(at) - sizeof(allocator *) - sizeof(int8_t));
    
    if (at_meta._allocator != reinterpret_cast<allocator *>(this))
    {
        throw std::logic_error("Wrong allocator instance to deallocate memory");
    }

    merge(at_meta, get_casted_trusted_memory() + get_main_meta_size() + (1 << get_allocatable_memory_order()));

    block_size_t *available_space = get_available_space_address();
    *available_space += 1 << abs(at_meta._order);

    information_with_guard(get_typename() + ": deallocate: free space in trusted memory after deallocation of " + std::to_string(1 << abs(at_meta._order)) + " is " + std::to_string(*available_space));

    int *bytes = reinterpret_cast<int *>(get_casted_trusted_memory() + get_main_meta_size());
    block_size_t workspace_size = 1 << get_allocatable_memory_order();
    std::string bytes_message = get_typename() + ": deallocate: array of bytes after deallocation: ";

    for (block_size_t i = 0; i < workspace_size; ++i)
    {
        bytes_message += std::to_string(*bytes++);
        bytes_message += ' ';
    }
    debug_with_guard(bytes_message);

    debug_with_guard(get_typename() + ": deallocate: blocks information after deallocation is " + get_blocks_visualization(get_blocks_info()));
}

void allocator_buddies_system::create_available_block(uint8_t *current, int8_t current_order, uint8_t *prev) const noexcept
{
    block_meta_t prev_meta(prev);
    uint8_t *next = reinterpret_cast<uint8_t *>(prev_meta._next);
    if (next != nullptr)
    {
        block_meta_t next_meta(next);
        available_meta_serialization(next_meta._start, next_meta._order, current, next_meta._next);
    }

    available_meta_serialization(current, current_order, prev_meta._start, reinterpret_cast<block_pointer_t>(next));
    available_meta_serialization(prev_meta._start, current_order, prev_meta._prev, current);
}

void allocator_buddies_system::connect(uint8_t *left, uint8_t *right) const noexcept
{   
    if (left != nullptr)
    {        
        block_meta_t left_meta(left);
        available_meta_serialization(left_meta._start, left_meta._order, left_meta._prev, right);
    }
    else
    {
        *reinterpret_cast<block_pointer_t *>(get_casted_trusted_memory() + get_main_meta_size() - sizeof(block_pointer_t)) = right;
    }

    if (right != nullptr)
    {
        block_meta_t right_meta(right);
        available_meta_serialization(right_meta._start, right_meta._order, left, right_meta._next);
    }
}

allocator::block_pointer_t allocator_buddies_system::split(uint8_t *block, uint8_t requested_order) noexcept
{    
    block_meta_t current_meta(block);

    uint8_t *buddy = nullptr;
    
    while (current_meta._order > requested_order)
    {
        buddy = get_buddy(current_meta._start, 1 << --current_meta._order);
        
        create_available_block(buddy, current_meta._order, current_meta._start);
        current_meta = block_meta_t(current_meta._start);
    }

    connect(reinterpret_cast<uint8_t *>(current_meta._prev), reinterpret_cast<uint8_t *>(current_meta._next));

    occupied_meta_serialization(current_meta._start, current_meta._order);

    return block;
}

uint8_t *allocator_buddies_system::find_block(uint8_t *to_find, uint8_t const *allocatable_memory_end) const noexcept
{
    uint8_t *current = get_casted_trusted_memory() + get_main_meta_size();
    uint8_t *prev = nullptr;

    while (current != allocatable_memory_end)
    {
        block_meta_t meta(current);
        if (meta._start == to_find)
        {
            return prev;
        }
        if (meta._order > 0)
        {
            prev = meta._start;
        }
        meta = block_meta_t(meta._start + (1 << abs(meta._order)));
    }
    
    return nullptr;
}

void allocator_buddies_system::merge(block_meta_t block_meta, uint8_t const *allocatable_memory_end) const noexcept
{
    block_meta._order = abs(block_meta._order);

    uint8_t *buddy = get_buddy(block_meta._start, 1 << block_meta._order);

    if (buddy == allocatable_memory_end)
    {
        return;
    }
    
    block_meta_t buddy_meta(buddy);

    if (buddy_meta._order < 0)
    {
        uint8_t *prev_available = find_block(block_meta._start, allocatable_memory_end);
        block_pointer_t next = nullptr;
        if (prev_available == nullptr)
        {
            next = get_first_available_block();
            set_first_available_block(block_meta._start);
        }
        else
        {
            block_meta_t prev_available_meta(prev_available);
            next = prev_available_meta._next;
            available_meta_serialization(prev_available_meta._start, prev_available_meta._order, prev_available_meta._prev, block_meta._start);    
        }

        block_meta_t next_available_meta(reinterpret_cast<uint8_t *>(next));
        available_meta_serialization(next_available_meta._start, next_available_meta._order, block_meta._start, next_available_meta._next);
        available_meta_serialization(block_meta._start, block_meta._order, prev_available, next);

        return;
    }

    if (buddy < block_meta._start)
    {
        available_meta_serialization(buddy_meta._start, ++buddy_meta._order, buddy_meta._prev, buddy_meta._next);
        merge(buddy_meta, allocatable_memory_end);
    }
    else
    {
        available_meta_serialization(block_meta._start, ++block_meta._order, buddy_meta._prev, buddy_meta._next);
        merge(block_meta, allocatable_memory_end);
    }
}

allocator_buddies_system::block_meta_t::block_meta_t(uint8_t *block)
{
    _start = block;
    block_size_t offset = 0;

    _order = *reinterpret_cast<int8_t *>(_start);
    offset += sizeof(int8_t);

    if (_order > 0)
    {
        _prev = *reinterpret_cast<block_pointer_t *>(block + offset);
        offset += sizeof(block_pointer_t);

        _next = *reinterpret_cast<block_pointer_t *>(block + offset);
    }
    else
    {
        _allocator = *reinterpret_cast<allocator **>(block + offset);
    }
}

allocator_buddies_system::block_meta_t::block_meta_t():
    _start(nullptr), _order(0), _prev(nullptr), _next(nullptr), _allocator(nullptr)
{
    
}

allocator::block_pointer_t allocator_buddies_system::available_meta_serialization(uint8_t *start, int8_t order, block_pointer_t prev, block_pointer_t next) const noexcept
{
    block_size_t offset = 0;
    *reinterpret_cast<int8_t *>(start) = order;
    offset += sizeof(int8_t);

    *reinterpret_cast<block_pointer_t *>(start + offset) = prev;
    offset += sizeof(block_pointer_t);

    *reinterpret_cast<block_pointer_t *>(start + offset) = next;

    return start;
}

allocator::block_pointer_t allocator_buddies_system::occupied_meta_serialization(uint8_t *start, int8_t order) noexcept
{    
    block_size_t offset = 0;
    *reinterpret_cast<int8_t *>(start) = -order;
    offset += sizeof(int8_t);

    *reinterpret_cast<allocator **>(start + offset) = reinterpret_cast<allocator *>(this);

    return start;
}

inline void allocator_buddies_system::set_fit_mode(allocator_with_fit_mode::fit_mode mode)
{
    *reinterpret_cast<fit_mode *>(get_casted_trusted_memory() + get_main_meta_size() - sizeof(block_pointer_t) - sizeof(fit_mode)) = mode;
}

inline allocator *allocator_buddies_system::get_allocator() const
{
    return *reinterpret_cast<allocator **>(get_casted_trusted_memory() + sizeof(uint8_t) + sizeof(block_size_t));
}

std::string allocator_buddies_system::get_blocks_visualization(std::vector<allocator_test_utils::block_info> const &blocks_information) const noexcept
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

std::vector<allocator_test_utils::block_info> allocator_buddies_system::get_blocks_info() const noexcept
{
    std::vector<allocator_test_utils::block_info> result;

    uint8_t *current = get_casted_trusted_memory() + get_main_meta_size();

    uint8_t *allocatable_memory_end = current + (1 << get_allocatable_memory_order());

    block_info info;
    
    while (current != allocatable_memory_end)
    {
        block_meta_t meta(current);

        info.is_block_occupied = meta._order > 0
            ? false
            : true;
        info.block_size = 1 << abs(meta._order);
        result.push_back(info);

        current += 1 << abs(meta._order);
    }

    return result;
}

inline logger *allocator_buddies_system::get_logger() const
{
    return *reinterpret_cast<logger **>(get_casted_trusted_memory() + sizeof(uint8_t) + sizeof(block_size_t) + sizeof(allocator *));
}

inline std::string allocator_buddies_system::get_typename() const noexcept
{
    return "allocator_buddies_system";
}

inline uint8_t *allocator_buddies_system::get_casted_trusted_memory() const noexcept
{
    return reinterpret_cast<uint8_t *>(_trusted_memory);
}

inline allocator::block_size_t allocator_buddies_system::get_main_meta_size() const noexcept
{
    return sizeof(uint8_t) + sizeof(block_size_t) + sizeof(allocator *) + sizeof(logger *) + sizeof(std::mutex) + sizeof(allocator_with_fit_mode::fit_mode) + sizeof(block_pointer_t);
}

inline allocator::block_size_t allocator_buddies_system::get_available_meta_size() const noexcept
{
    return sizeof(int8_t) + 2 * sizeof(block_pointer_t);
}

inline allocator::block_size_t allocator_buddies_system::get_occupied_meta_size() const noexcept
{
    return sizeof(int8_t) + sizeof(allocator *);
}

inline uint8_t allocator_buddies_system::get_allocatable_memory_order() const noexcept
{
    return *get_casted_trusted_memory();
}

inline allocator::block_size_t *allocator_buddies_system::get_available_space_address() const noexcept
{
    return reinterpret_cast<block_size_t *>(get_casted_trusted_memory() + sizeof(uint8_t));
}

inline std::mutex &allocator_buddies_system::get_mutex() const noexcept
{
    return *reinterpret_cast<std::mutex *>(get_casted_trusted_memory() + sizeof(uint8_t) + sizeof(block_size_t) + sizeof(allocator *) + sizeof(logger *));
}

inline allocator_with_fit_mode::fit_mode allocator_buddies_system::get_fit_mode() const noexcept
{
    return *reinterpret_cast<fit_mode *>(get_casted_trusted_memory() + get_main_meta_size() - sizeof(block_pointer_t) - sizeof(fit_mode));
}


inline allocator::block_pointer_t allocator_buddies_system::get_first_available_block() const noexcept
{
    return *reinterpret_cast<block_pointer_t *>(get_casted_trusted_memory() + get_main_meta_size() - sizeof(block_pointer_t));
}

inline void allocator_buddies_system::set_first_available_block(block_pointer_t block) const noexcept
{
    *reinterpret_cast<block_pointer_t *>(get_casted_trusted_memory() + get_main_meta_size() - sizeof(block_pointer_t)) = block;
}

uint8_t *allocator_buddies_system::get_buddy(uint8_t *block, block_size_t size) const noexcept
{
    uintptr_t allocatable_memory_start = reinterpret_cast<uintptr_t>(get_casted_trusted_memory() + get_main_meta_size());
    return reinterpret_cast<uint8_t *>(((reinterpret_cast<uintptr_t>(block) - allocatable_memory_start) ^ (size)) + allocatable_memory_start);
}

uint8_t allocator_buddies_system::get_order(block_size_t size) const noexcept
{
    uint8_t result = 0;
    while (size >>= 1)
    {
        ++result;
    }
    return ++result;
}