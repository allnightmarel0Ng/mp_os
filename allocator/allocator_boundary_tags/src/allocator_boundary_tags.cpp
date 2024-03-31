#include "../include/allocator_boundary_tags.h"

allocator_boundary_tags::~allocator_boundary_tags()
{
    automatic_logger auto_log(logger::severity::trace, "destructor", get_typename(), get_logger());
    delete_trusted_memory();
}

allocator_boundary_tags::allocator_boundary_tags(
    allocator_boundary_tags const &other)
{
    automatic_logger auto_log(logger::severity::trace, "copy constructor", get_typename(), get_logger());
    copy_trusted_memory(other);
}

allocator_boundary_tags &allocator_boundary_tags::operator=(
    allocator_boundary_tags const &other)
{
    automatic_logger auto_log(logger::severity::trace, "copy assignment", get_typename(), get_logger());
    
    if (this == &other)
    {
        return *this;
    }
    
    delete_trusted_memory();
    copy_trusted_memory(other);

    return *this;
}

allocator_boundary_tags::allocator_boundary_tags(
    allocator_boundary_tags &&other) noexcept
{
    automatic_logger auto_log(logger::severity::trace, "move constructor", get_typename(), get_logger());
    
    copy_trusted_memory(other);

    ::operator delete(other._trusted_memory);
}

allocator_boundary_tags &allocator_boundary_tags::operator=(
    allocator_boundary_tags &&other) noexcept
{
    automatic_logger auto_log(logger::severity::trace, "move assignment", get_typename(), get_logger());

    if (this == &other)
    {
        return *this;
    }
    
    delete_trusted_memory();
    copy_trusted_memory(other);

    ::operator delete(other._trusted_memory);
    return *this;
}

allocator_boundary_tags::allocator_boundary_tags(size_t space_size,
    allocator *parent_allocator,
    logger *logger_instance,
    allocator_with_fit_mode::fit_mode allocate_fit_mode)
{
    automatic_logger auto_log(logger::severity::trace, "constructor", get_typename(), logger_instance);

    block_size_t meta_size = get_big_meta_size();
    
    if (meta_size > space_size)
    {
        if (logger_instance)
        {
            logger_instance->error("Invalid space size in constructor");
        }
        throw std::logic_error("Invalid space size");
    }

    block_size_t full_size = meta_size + space_size;
    
    try 
    {
        _trusted_memory = parent_allocator == nullptr 
            ? ::operator new(full_size)
            : parent_allocator->allocate(full_size, 1);
    }
    catch (std::bad_alloc &exception)
    {
        if (logger_instance)
        {
            logger_instance->error("Allocation error");
        }
        throw exception;
    }

    uint8_t *serialization = reinterpret_cast<uint8_t *>(_trusted_memory);

    *reinterpret_cast<block_size_t *>(serialization) = full_size;
    serialization += sizeof(block_size_t);

    *reinterpret_cast<block_size_t *>(serialization) = space_size;
    serialization += sizeof(block_size_t);

    *reinterpret_cast<allocator_with_fit_mode::fit_mode *>(serialization) = allocate_fit_mode;
    serialization += sizeof(allocator_with_fit_mode::fit_mode);
    
    *reinterpret_cast<logger **>(serialization) = logger_instance;
    serialization += sizeof(logger *);

    *reinterpret_cast<allocator **>(serialization) = parent_allocator;
    serialization += sizeof(allocator *);

    *reinterpret_cast<block_pointer_t *>(serialization) = nullptr;
}

[[nodiscard]] void *allocator_boundary_tags::allocate(size_t value_size, size_t values_count)
{
    std::mutex mutex;
    mutex.lock();
    
    automatic_logger auto_log(logger::severity::debug, "allocate", get_typename(), get_logger());
    
    block_size_t mini_meta_size = get_mini_meta_size();
    block_size_t requested_size = value_size * values_count + mini_meta_size;
    
    block_size_t full_size = get_full_size();

    block_size_t *free_space = get_available_size_address();

    uint8_t *allocatable_memory_start = get_allocatable_memory_start();
    uint8_t *allocatable_memory_end = get_allocatable_memory_end();

    allocator_with_fit_mode::fit_mode current_fit_mode = get_fit_mode();

    block_pointer_t first_occupied = get_first_available_block();

    block_pointer_t result;

    try 
    {
        if (first_occupied == nullptr)
        {
            if (allocatable_memory_end - allocatable_memory_start < requested_size)
            {
                throw std::bad_alloc{};
            }

            block_size_t remainder = allocatable_memory_end - allocatable_memory_start - requested_size;
            if (remainder < mini_meta_size)
            {
                requested_size += remainder;
                warning_with_guard(get_typename() + ": allocate: allocation size was changed to " + std::to_string(requested_size));
            }
            
            result = meta_serialization(allocatable_memory_start, nullptr, nullptr, requested_size);
            *reinterpret_cast<block_pointer_t *>(allocatable_memory_start - sizeof(block_pointer_t)) = allocatable_memory_start;
        }
        else if (current_fit_mode == allocator_with_fit_mode::fit_mode::first_fit)
        {
            result = allocate_first_fit(first_occupied, requested_size, allocatable_memory_start, allocatable_memory_end);
        }
        else 
        {
            result = allocate_worst_best_fit(first_occupied, requested_size, allocatable_memory_start, allocatable_memory_end, current_fit_mode);
        }
    }
    catch (std::bad_alloc &exception)
    {
        error_with_guard(get_typename() + ": allocate: unable to allocate " + std::to_string(values_count) + " elements of " + std::to_string(value_size) + " size" );
        mutex.unlock();
        throw exception;
    }

    *free_space -= requested_size;
    information_with_guard(get_typename() + ": allocate: free space in trusted memory after allocation of " + std::to_string(values_count * value_size) + " bytes is " + std::to_string(*free_space));

    debug_with_guard(get_typename() + ": allocate: blocks information after allocation is " + get_blocks_visualization(get_blocks_info()));

    mutex.unlock();

    return reinterpret_cast<uint8_t *>(result) + mini_meta_size;
}

void *allocator_boundary_tags::allocate_first_fit(block_pointer_t first_occupied, block_size_t desired_size, 
    uint8_t *allocatable_memory_start, uint8_t const *allocatable_memory_end)
{   
    block_meta_t current_block = meta_deserialization(first_occupied);
    block_size_t mini_meta_size = get_mini_meta_size();

    block_pointer_t result;
    block_size_t remainder;
    block_meta_t next_meta;

    if (current_block.start - allocatable_memory_start >= desired_size)
    {
        remainder = current_block.start - allocatable_memory_start - desired_size;
        if (remainder < mini_meta_size)
        {
            desired_size += remainder;
            warning_with_guard(get_typename() + ": allocate: allocation size was changed to " + std::to_string(desired_size));
        }
        
        result = meta_serialization(
            allocatable_memory_start, 
            nullptr, 
            first_occupied,
            desired_size);
        
        *reinterpret_cast<block_pointer_t *>(allocatable_memory_start - sizeof(block_pointer_t)) = result;
        meta_serialization(current_block.start, result, current_block.next, current_block.size);

        return result;
    }

    uint8_t *empty_start = current_block.start + current_block.size;

    while (current_block.next)
    {
        uint8_t *next = reinterpret_cast<uint8_t *>(current_block.next);

        remainder = next - empty_start - desired_size;
        if (remainder < mini_meta_size)
        {
            desired_size += remainder;
            warning_with_guard(get_typename() + ": allocate: allocation size was changed to " + std::to_string(desired_size));
        }

        if (next - empty_start >= desired_size)
        {
            result = meta_serialization(
                empty_start, 
                reinterpret_cast<block_pointer_t>(current_block.start + sizeof(block_pointer_t)),
                current_block.next,
                desired_size);
            
            current_block.prev = result;
            meta_serialization(current_block);

            next_meta = meta_deserialization(current_block.next);
            next_meta.prev = result;
            meta_serialization(next_meta);

            return result;
        }
        current_block = meta_deserialization(next);
        empty_start = current_block.start + current_block.size;
    }

    if (allocatable_memory_end - empty_start < desired_size)
    {    
        throw std::bad_alloc{};
    }

    remainder = allocatable_memory_end - empty_start - desired_size;
    if (remainder < mini_meta_size)
    {
        desired_size += remainder;
        warning_with_guard(get_typename() + ": allocate: allocation size was changed to " + std::to_string(desired_size));
    }
    
    result = meta_serialization(
        empty_start,
        reinterpret_cast<block_pointer_t>(current_block.start + sizeof(block_pointer_t)),
        nullptr,
        desired_size);
    
    current_block.next = result;
    meta_serialization(current_block);

    return result;
}

void *allocator_boundary_tags::allocate_worst_best_fit(block_pointer_t first_occupied, block_size_t desired_size, uint8_t *allocatable_memory_start, uint8_t const *allocatable_memory_end, allocator_with_fit_mode::fit_mode fit_mode)
{
    block_meta_t current_block = meta_deserialization(first_occupied);

    block_size_t most_suitable_difference = (fit_mode == allocator_with_fit_mode::fit_mode::the_best_fit) 
        ? allocatable_memory_end - allocatable_memory_start : 0;
    
    bool block_found = false;
    block_meta_t best_block;
    block_size_t difference;

    if (current_block.start - allocatable_memory_start >= desired_size)
    {
        best_block.start = allocatable_memory_start;
        best_block.prev = nullptr;
        best_block.next = first_occupied;
        best_block.size = desired_size;

        most_suitable_difference = current_block.start - allocatable_memory_start - desired_size;
        block_found = true;
    }

    uint8_t *empty_start = current_block.start + current_block.size;
    bool compare;

    while (current_block.next)
    {
        uint8_t *next = reinterpret_cast<uint8_t *>(current_block.next);        
        block_size_t empty_size = next - empty_start;
        difference = empty_size - desired_size;

        compare = (fit_mode == allocator_with_fit_mode::fit_mode::the_best_fit) 
            ? difference < most_suitable_difference
            : difference > most_suitable_difference;
        
        if (empty_size >= desired_size && compare)
        {
            best_block.start = empty_start;
            best_block.prev = reinterpret_cast<block_pointer_t>(current_block.start + sizeof(block_pointer_t));
            best_block.next = current_block.next;
            best_block.size = desired_size;

            most_suitable_difference = difference;
            block_found = true;
        }
        current_block = meta_deserialization(next);
        empty_start = current_block.start + current_block.size;
    }

    difference = allocatable_memory_end - empty_start - desired_size;

    compare = (fit_mode == allocator_with_fit_mode::fit_mode::the_best_fit) 
        ? difference < most_suitable_difference
        : difference > most_suitable_difference;
            
    if (allocatable_memory_end - empty_start >= desired_size && compare)
    {
        best_block.start = empty_start;
        best_block.prev = reinterpret_cast<block_pointer_t>(current_block.start + sizeof(block_pointer_t));
        best_block.next = nullptr;
        best_block.size = desired_size;

        most_suitable_difference = difference;
        block_found = true;
    }
    else if (!block_found)
    {
        throw std::bad_alloc{};
    }
    
    block_size_t remainder = reinterpret_cast<uint8_t *>(best_block.next) - (best_block.start + best_block.size);
    if (remainder < get_mini_meta_size())
    {
        best_block.size += remainder;
        warning_with_guard(get_typename() + ": allocate: allocation size was changed to " + std::to_string(desired_size));
    }
    block_pointer_t result = meta_serialization(best_block);

    if (best_block.prev)
    {
        block_meta_t prev_meta = meta_deserialization(reinterpret_cast<uint8_t *>(best_block.prev) - sizeof(block_pointer_t));
        prev_meta.next = result;
        meta_serialization(prev_meta);
    }
    else
    {
        *reinterpret_cast<block_pointer_t *>(allocatable_memory_start - sizeof(block_pointer_t)) = result;
    }

    if (best_block.next)
    {
        block_meta_t next_meta = meta_deserialization(best_block.next);
        next_meta.prev = result;
        meta_serialization(next_meta);
    }

    return result;
}

void allocator_boundary_tags::deallocate(void *at)
{
    std::mutex mutex;
    mutex.lock();
    
    automatic_logger auto_log(logger::severity::debug, "deallocate", get_typename(), get_logger());
    
    block_meta_t at_meta = meta_deserialization(
        reinterpret_cast<uint8_t *>(at) - 2 * sizeof(block_pointer_t) - sizeof(block_size_t) - sizeof(allocator *));
    
    if (at_meta.alc != reinterpret_cast<allocator *>(this))
    {
        std::string message = get_typename() + ": deallocate: wrong allocator instance called to deallocate memory";
        error_with_guard(message);
        mutex.unlock();
        throw std::logic_error(message);
    }

    block_size_t meta_size = get_big_meta_size();

    if (!at_meta.prev)
    {
        *reinterpret_cast<block_pointer_t *>(get_allocatable_memory_start() - sizeof(block_pointer_t)) = at_meta.next;
    }
    else 
    {
        block_meta_t prev = meta_deserialization(at_meta.prev);
        meta_serialization(prev.start, prev.prev, at_meta.next, prev.size);
    }

    if (at_meta.next)
    {
        block_meta_t next = meta_deserialization(at_meta.next);
        meta_serialization(next.start, at_meta.prev, next.next, next.size);
    }

    block_size_t full_size = get_full_size();

    block_size_t *free_space = get_available_size_address();
    *free_space += at_meta.size;

    information_with_guard(get_typename() + ": deallocate: free space in trusted memory after deallocation of " + std::to_string(at_meta.size) + " is " + std::to_string(*free_space));

    int *bytes = reinterpret_cast<int *>(get_allocatable_memory_start());
    block_size_t workspace_size = full_size - meta_size;
    std::string bytes_message = get_typename() + ": deallocate: array of bytes after deallocation: ";

    for (block_size_t i = 0; i < workspace_size; ++i)
    {
        bytes_message += std::to_string(*bytes++);
        bytes_message += ' ';
    }
    debug_with_guard(bytes_message);

    debug_with_guard(get_typename() + ": deallocate: blocks information after deallocation is " + get_blocks_visualization(get_blocks_info()));

    mutex.unlock();
}

void *allocator_boundary_tags::meta_serialization(uint8_t *start, block_pointer_t prev, block_pointer_t next, block_size_t size) noexcept
{
    block_size_t offset = 0;
    
    *reinterpret_cast<block_pointer_t *>(start) = prev;
    offset += sizeof(block_pointer_t);

    *reinterpret_cast<block_pointer_t *>(start + offset) = next;
    offset += sizeof(block_pointer_t);

    *reinterpret_cast<block_size_t *>(start + offset) = size;
    offset += sizeof(block_size_t);

    *reinterpret_cast<allocator **>(start + offset) = reinterpret_cast<allocator *>(this);

    return start;
}

void *allocator_boundary_tags::meta_serialization(block_meta_t block) noexcept
{
    return meta_serialization(block.start, block.prev, block.next, block.size);
}

allocator_boundary_tags::block_meta_t allocator_boundary_tags::meta_deserialization(uint8_t *start) const noexcept
{
    block_meta_t result;
    block_size_t offset = 0;

    result.start = start;

    result.prev = *reinterpret_cast<block_pointer_t *>(start);
    offset += sizeof(block_pointer_t);

    result.next = *reinterpret_cast<block_pointer_t *>(start + offset);
    offset += sizeof(block_pointer_t);

    result.size = *reinterpret_cast<block_size_t *>(start + offset);
    offset += sizeof(block_size_t);

    result.alc = *reinterpret_cast<allocator **>(start + offset);
    
    return result;
}

allocator_boundary_tags::block_meta_t allocator_boundary_tags::meta_deserialization(block_pointer_t start) const noexcept
{
    return meta_deserialization(reinterpret_cast<uint8_t *>(start));   
}

inline void allocator_boundary_tags::set_fit_mode(
    allocator_with_fit_mode::fit_mode mode)
{    
    trace_with_guard(get_typename() + ": fit mode has been changed\n");

    *reinterpret_cast<allocator_with_fit_mode::fit_mode *>(cast_trusted_memory() + 2 * sizeof(block_size_t)) = mode;
}

inline allocator *allocator_boundary_tags::get_allocator() const
{    
    return *reinterpret_cast<allocator **>(reinterpret_cast<uint8_t *>(_trusted_memory) + 2 * sizeof(block_size_t) +
        sizeof(allocator_with_fit_mode::fit_mode) + sizeof(logger *));
}

std::vector<allocator_test_utils::block_info> allocator_boundary_tags::get_blocks_info() const noexcept
{
    std::vector<allocator_test_utils::block_info> result;

    block_size_t full_size = get_full_size();
    
    block_size_t mini_meta_size = get_mini_meta_size();

    block_size_t meta_size = get_big_meta_size();

    block_pointer_t first_occupied = get_first_available_block();

    allocator_test_utils::block_info current_info;

    if (!first_occupied)
    {      
        current_info.is_block_occupied = 0;
        current_info.block_size = full_size - meta_size;
        result.push_back(current_info);
        return result;
    }

    block_meta_t current = meta_deserialization(first_occupied);

    current_info.block_size = current.start - (get_allocatable_memory_start());
    current_info.is_block_occupied = 0;

    if (current_info.block_size)
    {   
        result.push_back(current_info);
    }

    while (true)
    {
        current_info.is_block_occupied = 1;
        current_info.block_size = current.size - mini_meta_size;
        result.push_back(current_info);

        if (current.next == nullptr)
        {
            break;
        }

        uint8_t *next = reinterpret_cast<uint8_t *>(current.next); 

        current_info.is_block_occupied = 0;
        current_info.block_size = next - (current.start + current.size);
        
        if (current_info.block_size)
        {   
            result.push_back(current_info);
        }

        current = meta_deserialization(next);
    }
    
    current_info.is_block_occupied = 0;
    current_info.block_size = (get_allocatable_memory_end()) - (current.start + current.size);
    if (current_info.block_size)
    {
        result.push_back(current_info);
    }

    return result;
}

std::string allocator_boundary_tags::get_blocks_visualization(
    std::vector<allocator_test_utils::block_info> const &blocks_information) const noexcept
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


inline logger *allocator_boundary_tags::get_logger() const
{
    return *reinterpret_cast<logger **>(cast_trusted_memory() + 2 * sizeof(block_size_t) +
        sizeof(allocator_with_fit_mode::fit_mode));
}

inline std::string allocator_boundary_tags::get_typename() const noexcept
{
    return "allocator_boundary_tags";
}

void allocator_boundary_tags::delete_trusted_memory() noexcept
{
    ::operator delete(_trusted_memory);
}

void allocator_boundary_tags::copy_trusted_memory(allocator_boundary_tags const &other) noexcept
{
    uint8_t *other_deserialization = reinterpret_cast<uint8_t *>(other._trusted_memory);
    block_size_t other_size = *reinterpret_cast<block_size_t *>(other_deserialization);

    _trusted_memory = other.allocate_with_guard(other_size);
    memcpy(reinterpret_cast<uint8_t *>(_trusted_memory), other_deserialization, other_size);
}

allocator::block_size_t allocator_boundary_tags::get_big_meta_size() const noexcept
{
    return 2 * sizeof(block_size_t) + sizeof(logger *) + sizeof(allocator *) + sizeof(allocator_with_fit_mode::fit_mode) + sizeof(block_pointer_t);
}

allocator::block_size_t allocator_boundary_tags::get_mini_meta_size() const noexcept
{
    return 2 * sizeof(block_pointer_t) + sizeof(block_size_t) + sizeof(allocator *);
}

uint8_t *allocator_boundary_tags::cast_trusted_memory() const noexcept
{
    return reinterpret_cast<uint8_t *>(_trusted_memory);
}

allocator::block_pointer_t allocator_boundary_tags::get_first_available_block() const noexcept
{
    return *reinterpret_cast<block_pointer_t *>(cast_trusted_memory() + get_big_meta_size() - sizeof(block_pointer_t));
}

allocator::block_size_t allocator_boundary_tags::get_full_size() const noexcept
{
    return *reinterpret_cast<block_size_t *>(cast_trusted_memory());
}

allocator::block_size_t *allocator_boundary_tags::get_available_size_address() const noexcept
{
    return reinterpret_cast<block_size_t *>(cast_trusted_memory() + sizeof(block_size_t));
}

uint8_t *allocator_boundary_tags::get_allocatable_memory_start() const noexcept
{
    return reinterpret_cast<uint8_t *>(cast_trusted_memory() + get_big_meta_size());
}

uint8_t *allocator_boundary_tags::get_allocatable_memory_end() const noexcept
{
    return reinterpret_cast<uint8_t *>(cast_trusted_memory() + get_full_size());
}

allocator_with_fit_mode::fit_mode allocator_boundary_tags::get_fit_mode() const noexcept
{
    return *reinterpret_cast<allocator_with_fit_mode::fit_mode *>(cast_trusted_memory() + 2 * sizeof(block_size_t));
}