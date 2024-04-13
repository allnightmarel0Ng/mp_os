#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_ALLOCATOR_ALLOCATOR_BOUNDARY_TAGS_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_ALLOCATOR_ALLOCATOR_BOUNDARY_TAGS_H

#include <cstring>

#include <allocator_guardant.h>
#include <allocator_test_utils.h>
#include <allocator_with_fit_mode.h>
#include <logger_guardant.h>
#include <typename_holder.h>
#include <automatic_logger.h>

class allocator_boundary_tags final:
    private allocator_guardant,
    public allocator_test_utils,
    public allocator_with_fit_mode,
    private logger_guardant,
    private typename_holder
{

private:
    
    void *_trusted_memory;

public:
    
    ~allocator_boundary_tags() override;
    
    allocator_boundary_tags(allocator_boundary_tags const &other);
    
    allocator_boundary_tags &operator=(allocator_boundary_tags const &other);
    
    allocator_boundary_tags(allocator_boundary_tags &&other) noexcept;
    
    allocator_boundary_tags &operator=(allocator_boundary_tags &&other) noexcept;

public:
    
    explicit allocator_boundary_tags(size_t space_size,allocator *parent_allocator = nullptr, logger *logger = nullptr,
        allocator_with_fit_mode::fit_mode allocate_fit_mode = allocator_with_fit_mode::fit_mode::first_fit);

public:
    
    [[nodiscard]] void *allocate(size_t value_size, size_t values_count) override;
    
    void deallocate(void *at) override;

private:

    struct block_meta_t
    {
        uint8_t *start;
        block_pointer_t prev, next;
        block_size_t size;
        allocator *alc;
    };

private:

    void *meta_serialization(uint8_t *start, block_pointer_t prev, block_pointer_t next, block_size_t size) noexcept;

    void *meta_serialization(block_meta_t block) noexcept;

    block_meta_t meta_deserialization(uint8_t *start) const noexcept;

    block_meta_t meta_deserialization(block_pointer_t start) const noexcept;
    
    void *allocate_first_fit(block_pointer_t first_occupied, block_size_t desired_size, uint8_t *allocatable_memory_start, uint8_t const *allocatable_memory_end);

    void *allocate_worst_best_fit(block_pointer_t first_occupied, block_size_t desired_size, uint8_t *allocatable_memory_start, uint8_t const *allocatable_memory_end, allocator_with_fit_mode::fit_mode fit_mode);

private:

    void delete_trusted_memory() noexcept;

    void copy_trusted_memory(allocator_boundary_tags const &other) noexcept;

public:
    
    inline void set_fit_mode(allocator_with_fit_mode::fit_mode mode) override;

private:
    
    inline allocator *get_allocator() const override;

public:
    
    std::vector<allocator_test_utils::block_info> get_blocks_info() const noexcept override;

private:

    std::string get_blocks_visualization(std::vector<allocator_test_utils::block_info> const &blocks_information) const noexcept;

private:
    
    inline logger *get_logger() const override;
    
    inline std::string get_typename() const noexcept override;

    block_size_t get_big_meta_size() const noexcept;

    block_size_t get_mini_meta_size() const noexcept;

    uint8_t *cast_trusted_memory() const noexcept;

    block_pointer_t get_first_occupied_block() const noexcept;

    block_size_t get_full_size() const noexcept;

    block_size_t *get_available_size_address() const noexcept;

    uint8_t *get_allocatable_memory_start() const noexcept;

    uint8_t *get_allocatable_memory_end() const noexcept;

    allocator_with_fit_mode::fit_mode get_fit_mode() const noexcept;

    std::mutex &get_mutex() const noexcept;
};

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_ALLOCATOR_ALLOCATOR_BOUNDARY_TAGS_H