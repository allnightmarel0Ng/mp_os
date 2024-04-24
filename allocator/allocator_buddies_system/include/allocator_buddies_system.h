#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_ALLOCATOR_ALLOCATOR_BUDDIES_SYSTEM_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_ALLOCATOR_ALLOCATOR_BUDDIES_SYSTEM_H

#include <allocator_guardant.h>
#include <allocator_test_utils.h>
#include <allocator_with_fit_mode.h>
#include <logger_guardant.h>
#include <typename_holder.h>
#include <automatic_logger.h>

class allocator_buddies_system final:
    private allocator_guardant,
    public allocator_test_utils,
    public allocator_with_fit_mode,
    private logger_guardant,
    private typename_holder
{

private:
    
    void *_trusted_memory;

public:
    
    ~allocator_buddies_system() override;
    
    allocator_buddies_system(allocator_buddies_system const &other);
    
    allocator_buddies_system &operator=(allocator_buddies_system const &other);
    
    allocator_buddies_system(allocator_buddies_system &&other) noexcept;
    
    allocator_buddies_system &operator=(allocator_buddies_system &&other) noexcept;

private:

    void clear_trusted_memory(allocator_buddies_system &at) noexcept;

    void *copy_trusted_memory(allocator_buddies_system const &from);

public:
    
    explicit allocator_buddies_system(size_t space_size_power_of_two, allocator *parent_allocator = nullptr, logger *logger = nullptr, allocator_with_fit_mode::fit_mode allocate_fit_mode = allocator_with_fit_mode::fit_mode::first_fit);

public:
    
    [[nodiscard]] void *allocate(size_t value_size, size_t values_count) override;
    
    void deallocate(void *at) override;

private:

    void *allocate_first_fit(block_pointer_t first_available, uint8_t required_order);

    void *allocate_worst_best_fit(block_pointer_t first_available, uint8_t required_order, fit_mode mode);

private:

    struct block_meta_t
    {    
        
        uint8_t *_start;

        int8_t _order;

        block_pointer_t _prev, _next;

        allocator *_allocator;

    public:

        block_meta_t(uint8_t *block);

        block_meta_t();

    };

private:

    void create_available_block(uint8_t *current, int8_t current_order, uint8_t *prev) const noexcept;
    
    void connect(uint8_t *left, uint8_t *right) const noexcept;
    
    block_pointer_t split(uint8_t *block, uint8_t requested_order) noexcept;

    uint8_t *find_block(uint8_t *to_find, uint8_t const *allocatable_memory_end) const noexcept;

    void merge(block_meta_t block_meta, uint8_t const *allocatable_memory_end) const noexcept;

private:

    block_pointer_t available_meta_serialization(uint8_t *start, int8_t order, block_pointer_t prev, block_pointer_t next) const noexcept;

    block_pointer_t occupied_meta_serialization(uint8_t *start, int8_t order) noexcept;

public:
    
    inline void set_fit_mode(allocator_with_fit_mode::fit_mode mode) override;

private:

    std::string get_blocks_visualization(std::vector<allocator_test_utils::block_info> const &blocks_information) const noexcept;

public:
    
    std::vector<allocator_test_utils::block_info> get_blocks_info() const noexcept override;

private:
    
    inline allocator *get_allocator() const override;
    
    inline logger *get_logger() const override;
    
    inline std::string get_typename() const noexcept override;

private:

    inline uint8_t *get_casted_trusted_memory() const noexcept;
    
    inline block_size_t get_main_meta_size() const noexcept;

    inline block_size_t get_available_meta_size() const noexcept;

    inline block_size_t get_occupied_meta_size() const noexcept;

    inline uint8_t get_allocatable_memory_order() const noexcept;

    inline block_size_t *get_available_space_address() const noexcept;

    inline std::mutex &get_mutex() const noexcept;

    inline fit_mode get_fit_mode() const noexcept;

    inline block_pointer_t get_first_available_block() const noexcept;

    inline void set_first_available_block(block_pointer_t block) const noexcept;

private:

    uint8_t *get_buddy(uint8_t *block, block_size_t size) const noexcept;

    uint8_t get_order(block_size_t size) const noexcept;
    
};

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_ALLOCATOR_ALLOCATOR_BUDDIES_SYSTEM_H
