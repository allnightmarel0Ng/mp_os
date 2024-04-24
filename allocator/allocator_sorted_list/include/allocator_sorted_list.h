#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_ALLOCATOR_ALLOCATOR_SORTED_LIST_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_ALLOCATOR_ALLOCATOR_SORTED_LIST_H

#include <allocator_guardant.h>
#include <allocator_test_utils.h>
#include <allocator_with_fit_mode.h>
#include <logger_guardant.h>
#include <typename_holder.h>
#include <automatic_logger.h>

class allocator_sorted_list final:
    private allocator_guardant,
    public allocator_test_utils,
    public allocator_with_fit_mode,
    private logger_guardant,
    private typename_holder
{

private:
    
    void *_trusted_memory;

public:
    
    ~allocator_sorted_list() override;
    
    allocator_sorted_list(allocator_sorted_list const &other);
    
    allocator_sorted_list &operator=(allocator_sorted_list const &other);
    
    allocator_sorted_list(allocator_sorted_list &&other) noexcept;
    
    allocator_sorted_list &operator=(allocator_sorted_list &&other) noexcept;

private:

    void clear_trusted_memory(allocator_sorted_list &at) noexcept;
    
    void *copy_trusted_memory(allocator_sorted_list const &from);

public:
    
    explicit allocator_sorted_list(
        size_t space_size,
        allocator *parent_allocator = nullptr,
        logger *logger = nullptr,
        allocator_with_fit_mode::fit_mode allocate_fit_mode = allocator_with_fit_mode::fit_mode::first_fit);

public:
    
    [[nodiscard]] void *allocate(
        size_t value_size,
        size_t values_count) override;
    
    void deallocate(
        void *at) override;

private:

    block_pointer_t allocate_first_fit(block_pointer_t first_available, block_size_t requested_size);

    block_pointer_t allocate_worst_best_fit(block_pointer_t first_available, block_size_t requested_size, fit_mode mode);

private:

    struct block_meta_t final
    {
        
        uint8_t *_start;
        
        int _size;

        block_pointer_t _next;

        allocator *_allocator;
    
    public:

        block_meta_t();

        block_meta_t(uint8_t *at);

    };

private:

    block_pointer_t create_occupied_block(block_meta_t prev, block_meta_t current, block_size_t requested_size) noexcept;

    void merge(block_meta_t one, block_meta_t another) const noexcept;
    
    void available_meta_serialization(uint8_t *at, block_size_t size, block_pointer_t next) const noexcept;

    void occupied_meta_serialization(uint8_t *at, int size) noexcept;


public:
    
    inline void set_fit_mode(allocator_with_fit_mode::fit_mode mode) override;

public:
    
    std::vector<allocator_test_utils::block_info> get_blocks_info() const noexcept override;

private:
    
    inline block_size_t get_main_meta_size() const noexcept;

    inline block_size_t get_occupied_meta_size() const noexcept;

    inline block_size_t get_available_meta_size() const noexcept;
    
    inline uint8_t *get_casted_trusted_memory() const noexcept;

    inline block_size_t get_full_size() const noexcept;

    inline block_size_t *get_available_space_address() const noexcept;
    
    inline std::mutex &get_mutex() const noexcept;

    inline fit_mode get_fit_mode() const noexcept;

    inline block_pointer_t get_first_available_block() const noexcept;

    inline allocator *get_allocator() const override;
    
    inline logger *get_logger() const override;

    inline void set_first_available_block(block_pointer_t at) const noexcept;

    std::string get_blocks_visualization(std::vector<allocator_test_utils::block_info> const &blocks_information) const noexcept;

private:
    
    inline std::string get_typename() const noexcept override;
    
};

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_ALLOCATOR_ALLOCATOR_SORTED_LIST_H