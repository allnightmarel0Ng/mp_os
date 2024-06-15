#ifndef MP_OS_WORKBENCH_HASH_TABLE_H
#define MP_OS_WORKBENCH_HASH_TABLE_H

#include <AVL_tree.h>
#include <associative_container.h>
#include <binary_search_tree.h>
#include <red_black_tree.h>

template<typename tkey, typename tvalue, typename tcontainer = binary_search_tree<tkey, tvalue>>
class hash_table final:
    public associative_container<tkey, tvalue>,
    private allocator_guardant,
    private logger_guardant
{

public:

    explicit hash_table(
        size_t table_size = 128,
        std::function<int(tkey const &)> hash_function = std::hash<tkey>(), 
        std::function<int(tkey const &, tkey const &)> keys_comparer = std::less<tkey>(), 
        allocator *allocator = nullptr,
        logger *logger = nullptr);

public:

    void insert(tkey const &key, tvalue const &value) override;

    void insert(tkey const &key, tvalue &&value) override;
    
    tvalue const &obtain(tkey const &key) override;

    tvalue dispose(tkey const &key) override;

public:

    ~hash_table();

    hash_table(hash_table const &other);

    hash_table &operator=(hash_table const &other);

    hash_table(hash_table &&other) noexcept;

    hash_table &operator=(hash_table &&other) noexcept;

private:

    std::function<int(tkey const &)> _hash_function;

    std::function<int(tkey const &, tkey const &)> _keys_comparer;

    tcontainer **_table;

    size_t const _table_size;

private:

    allocator *_allocator;

    logger *_logger;

private:

    [[nodiscard]] inline allocator *get_allocator() const noexcept override;

    [[nodiscard]] inline logger *get_logger() const noexcept override;

private:

    void clear_table(tcontainer **&table, size_t table_size) const noexcept;

    tcontainer **copy_table(tcontainer **from, size_t size) const;

};

template<typename tkey, typename tvalue, typename tcontainer>
hash_table<tkey, tvalue, tcontainer>::hash_table(size_t table_size, std::function<int(tkey const &)> hash_function, std::function<int(tkey const &, tkey const &)> keys_comparer, allocator *allocator, logger *logger):
    _table_size(table_size), _hash_function(hash_function), _keys_comparer(keys_comparer), _allocator(allocator), _logger(logger)
{
    if (typeid(tcontainer) != typeid(binary_search_tree<tkey, tvalue>) && typeid(tcontainer) != typeid(AVL_tree<tkey, tvalue>) && typeid(tcontainer) != typeid(red_black_tree<tkey, tvalue>))
    {
        throw std::logic_error("Hash Table works only with trees. Scapegoat tree and splay tree are not implemented yet");
    }
    
    _table = allocate_with_guard(sizeof(tcontainer *), _table_size);
}

template<typename tkey, typename tvalue, typename tcontainer>
void hash_table<tkey, tvalue, tcontainer>::insert(tkey const &key, tvalue const &value)
{
    size_t table_index = _hash_function(key) % _table_size;

    if (_table[table_index] == nullptr)
    {
        _table[table_index] = allocate_with_guard(sizeof(tcontainer));
        allocator::construct(_table[table_index], _keys_comparer, _allocator, _logger, binary_search_tree<tkey, tvalue>::insertion_of_existent_key_attempt_strategy::update_value);
    }

    _table[table_index]->insert(key, value);
}

template<typename tkey, typename tvalue, typename tcontainer>
void hash_table<tkey, tvalue, tcontainer>::insert(tkey const &key, tvalue &&value)
{
    size_t table_index = _hash_function(key) % _table_size;

    if (_table[table_index] == nullptr)
    {
        _table[table_index] = allocate_with_guard(sizeof(tcontainer));
        allocator::construct(_table[table_index], _keys_comparer, _allocator, _logger, binary_search_tree<tkey, tvalue>::insertion_of_existent_key_attempt_strategy::update_value);
    }

    _table[table_index]->insert(key, std::move(value));
}

template<typename tkey, typename tvalue, typename tcontainer>
tvalue const &hash_table<tkey, tvalue, tcontainer>::obtain(tkey const &key)
{
    size_t table_index = _hash_function(key) % _table_size;

    if (_table[table_index] == nullptr)
    {
        throw std::logic_error("Attempt to obtain the nonexistant key");
    }

    return _table[table_index]->obtain(key);
}

template<typename tkey, typename tvalue, typename tcontainer>
tvalue hash_table<tkey, tvalue, tcontainer>::dispose(tkey const &key)
{
    size_t table_index = _hash_function(key) % _table_size;

    if (_table[table_index] == nullptr)
    {
        throw std::logic_error("Attempt to dispose the nonexistant key");
    }

    return _table[table_index]->dispose(key);
}

template<typename tkey, typename tvalue, typename tcontainer>
hash_table<tkey, tvalue, tcontainer>::~hash_table()
{
    clear_table(_table, _table_size);
}

template<typename tkey, typename tvalue, typename tcontainer>
hash_table<tkey, tvalue, tcontainer>::hash_table(hash_table const &other):
    _hash_function(other._hash_function), _keys_comparer(other._keys_comparer), _table_size(other._table_size), _allocator(other._allocator), _logger(other._logger)
{
    _table = copy_table(other._table, other._table_size);
}

template<typename tkey, typename tvalue, typename tcontainer>
hash_table<tkey, tvalue, tcontainer> &hash_table<tkey, tvalue, tcontainer>::operator=(hash_table const &other)
{
    if (this == &other)
    {
        return *this;
    }

    for (size_t i = 0; i < _table_size; ++i)
    {
        deallocate_with_guard(_table[i]);
        _table[i] = nullptr;
    }

    deallocate_with_guard(_table);
    _table = nullptr;

    _hash_function = other._hash_function;
    _keys_comparer = other._keys_comparer;
    _table_size = other._table_size;
    _allocator = other._allocator;
    _logger = other._logger;

    _table = copy_table(other._table, other._table_size);

    return *this;
}

template<typename tkey, typename tvalue, typename tcontainer>
hash_table<tkey, tvalue, tcontainer>::hash_table(hash_table &&other) noexcept:
    _hash_function(std::move(other._hash_function)), _keys_comparer(std::move(other._keys_comparer)), _table_size(std::exchange(other._table_size, 0)), _allocator(std::move(other._allocator)), _logger(std::move(other._logger)), _table(std::exchange(other._table, nullptr))
{

}

template<typename tkey, typename tvalue, typename tcontainer>
hash_table<tkey, tvalue, tcontainer> &hash_table<tkey, tvalue, tcontainer>::operator=(hash_table &&other) noexcept
{
    if (this == &other)
    {
        return *this;
    }
    
    deallocate_with_guard(_table);
    for (size_t i = 0; i < _table_size; ++i)
    {
        deallocate_with_guard(_table[i]);
    }

    _hash_function = std::move(other._hash_function);
    _keys_comparer = std::move(other._keys_comparer);
    _table_size = std::exchange(other._table_size, 0);
    _allocator = std::move(other._allocator);
    _logger = std::move(other._logger);

    _table = std::exchange(other._table, nullptr);

    return *this;
}

template<typename tkey, typename tvalue, typename tcontainer>
[[nodiscard]] inline allocator *hash_table<tkey, tvalue, tcontainer>::get_allocator() const noexcept
{
    return _allocator;
}

template<typename tkey, typename tvalue, typename tcontainer>
[[nodiscard]] inline logger *hash_table<tkey, tvalue, tcontainer>::get_logger() const noexcept
{
    return _logger;
}

template<typename tkey, typename tvalue, typename tcontainer>
void hash_table<tkey, tvalue, tcontainer>::clear_table(tcontainer **&table, size_t table_size) const noexcept
{
    for (size_t i = 0; i < table_size; ++i)
    {
        deallocate_with_guard(table[i]);
        table[i] = nullptr;
    }
    
    deallocate_with_guard(table);
    table = nullptr;
}

template<typename tkey, typename tvalue, typename tcontainer>
tcontainer **hash_table<tkey, tvalue, tcontainer>::copy_table(tcontainer **from, size_t size) const
{
    tcontainer **result = allocate_with_guard(sizeof(tcontainer *), size);
    for (size_t i = 0; i < size; ++i)
    {
        *result[i] = *from[i];
    }

    return result;
}

#endif //MP_OS_WORKBENCH_HASH_TABLE_H