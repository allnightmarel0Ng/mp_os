#include "../include/database.h"

database::collection::collection(std::string const &filename, mode mode, allocator *allocator, logger *logger, container_variant variant, size_t t):
    _filename(filename), _mode(mode), _allocator(allocator), _logger(logger), _variant(variant), _t_for_b_trees(t), _container(nullptr)
{
    std::vector<std::function<int(tdata const &, tdata const &)>> comparers = { tdata::key_comparer, tdata::name_comparer, tdata::surname_comparer, tdata::birthday_comparer, tdata::marks_comparer };
    
    switch (_variant)
    {
        default:
            auto tree = reinterpret_cast<b_tree<index, search_tree<tdata, std::vector<std::shared_ptr<chain_of_responsibility>>> *> *>(allocate_with_guard(sizeof(b_tree<index, search_tree<tdata, std::vector<std::shared_ptr<chain_of_responsibility>>> *>)));
            allocator::construct(tree, _t_for_b_trees, index_comparer, allocator, logger);
            _container = tree;
            break;
    };

    for (size_t i = 0; i < INDEXES_COUNT; ++i)
    {
        _container->insert(static_cast<index>(i), get_tree_instance(t, comparers[i], allocator, logger));
    }
}

void database::collection::insert(tkey const &key, tvalue const &value)
{
    tdata to_insert(key, value);
    auto chain_custom_deleter = [this](chain_of_responsibility *raw_ptr)
        {
            allocator::destruct(raw_ptr);
            deallocate_with_guard(raw_ptr);
        };
    
    std::shared_ptr<chain_of_responsibility> chain(reinterpret_cast<chain_of_responsibility *>(allocate_with_guard(sizeof(chain_of_responsibility))), chain_custom_deleter);
    allocator::construct(chain.get(), _allocator, _logger);

    insertion_processing(to_insert, chain);
    chain->insert(to_insert);

    push_to_file(to_insert);
}

void database::collection::insert(tkey const &key, tvalue &&value)
{
    tdata to_insert(key, std::move(value));
    auto chain_custom_deleter = [this](chain_of_responsibility *raw_ptr)
        {
            allocator::destruct(raw_ptr);
            deallocate_with_guard(raw_ptr);
        };
    
    std::shared_ptr<chain_of_responsibility> chain(reinterpret_cast<chain_of_responsibility *>(allocate_with_guard(sizeof(chain_of_responsibility))), chain_custom_deleter);
    allocator::construct(chain.get(), _allocator, _logger);

    insertion_processing(to_insert, chain);
    chain->insert(to_insert);

    std::cout << std::boolalpha << (_mode == mode::file) << std::endl;
    push_to_file(to_insert);
}

void database::collection::update(tkey const &key, tvalue const &value)
{
    tdata update_with(key, value);
    update_processing(update_with);

    push_to_file(update_with);
}

void database::collection::update(tkey const &key, tvalue &&value)
{
    tdata update_with(key, std::move(value));
    update_processing(update_with);

    push_to_file(update_with);
}

std::vector<tdata> database::collection::obtain(tdata const &to_find, index where, date_time const &date_time_target)
{
    std::vector<tdata> result;

    auto &chains = _container->obtain(where)->obtain(to_find);

    for (auto const &chain: chains)
    {
        auto obtained = chain->obtain(date_time_target);
        if (obtained.has_value())
        {
            result.push_back(obtained.value());
        }
    }

    return result;
}

std::vector<tdata> database::collection::dispose(tdata const &to_dispose, index where)
{
    std::vector<tdata> result;

    auto &chains = _container->obtain(where)->obtain(to_dispose);

    for (auto const &chain: chains)
    {
        auto obtained = chain->obtain(date_time());
        if (obtained.has_value())
        {
            result.push_back(obtained.value());
        }
        chain->dispose();
    }

    std::ofstream os;
    os.open(_filename + ".txt");
    os << "deleted\n";
    os.close();

    return result;
}

std::vector<tdata> database::collection::obtain_between(tdata const &lower_bound, tdata const &upper_bound, index where, date_time date_time_target, bool lower_bound_inclusive, bool upper_bound_inclusive)
{
    std::vector<tdata> result;
    
    auto chains_vector = _container->obtain(where)->obtain_between(lower_bound, upper_bound, lower_bound_inclusive, upper_bound_inclusive);
    for (auto const &chains: chains_vector)
    {
        for (auto const &chain: chains.value)
        {
            auto obtained = chain->obtain(date_time_target);
            if (obtained.has_value())
            {
                result.push_back(obtained.value());
            }
        }
    }

    return result;
}

database::collection::~collection()
{
    clear();
}

database::collection::collection(collection const &other)
{
    copy_from(other);
}

database::collection &database::collection::operator=(collection const &other)
{
    if (this != &other)
    {
        clear();
        copy_from(other);
    }
    return *this;
}

database::collection::collection(collection &&other) noexcept
{
    move_from(std::move(other));
}

database::collection &database::collection::operator=(collection &&other) noexcept
{
    if (this != &other)
    {
        clear();
        move_from(std::move(other));
    }
    return *this;
}

void database::collection::clear() noexcept
{
    if (_container != nullptr)
    {
        switch(_variant)
        {
            default:
                auto tree = dynamic_cast<b_tree<index, search_tree<tdata, std::vector<std::shared_ptr<chain_of_responsibility>>> *> *>(_container);
                auto it = tree->cbegin_infix();
                auto end = tree->cend_infix();
                while (it != end)
                {
                    auto subtree = std::get<3>(*it);
                    allocator::destruct(subtree);
                    deallocate_with_guard(subtree);
                    subtree = nullptr;
                    ++it;
                }
        }

        allocator::destruct(_container);
        deallocate_with_guard(_container);
        _container = nullptr;
    }
    _os.close();
}

void database::collection::copy_from(collection const &other)
{
    _allocator = other._allocator;
    _logger = other._logger;
    _variant = other._variant;
    _t_for_b_trees = other._t_for_b_trees;
    _filename = other._filename;
    _mode = other._mode;

    switch (_variant)
    {
        default:
            auto tree = reinterpret_cast<b_tree<index, search_tree<tdata, std::vector<std::shared_ptr<chain_of_responsibility>>> *> *>(allocate_with_guard(sizeof(b_tree<index, search_tree<tdata, std::vector<std::shared_ptr<chain_of_responsibility>>> *>)));
            allocator::construct(tree, _t_for_b_trees, index_comparer, _allocator, _logger);
            _container = tree;
    }

    auto other_tree = dynamic_cast<b_tree<index, search_tree<tdata, std::vector<std::shared_ptr<chain_of_responsibility>>> *> *>(other._container);
    auto it = other_tree->cbegin_infix();
    auto end = other_tree->cend_infix();

    while (it != end)
    {
        auto subtree = reinterpret_cast<b_tree<tdata, std::vector<std::shared_ptr<chain_of_responsibility>>> *>(allocate_with_guard(sizeof(b_tree<tdata, std::vector<std::shared_ptr<chain_of_responsibility>>>)));
        allocator::construct(subtree, *dynamic_cast<b_tree<tdata, std::vector<std::shared_ptr<chain_of_responsibility>>> *>(std::get<3>(*it)));
        _container->insert(std::get<2>(*it), subtree);
        ++it;
    }
}

void database::collection::move_from(collection &&other) noexcept
{
    _container = std::exchange(other._container, nullptr);
    _allocator = std::exchange(other._allocator, nullptr);
    _logger = std::exchange(other._logger, nullptr);

    _variant = other._variant;
    _t_for_b_trees = other._t_for_b_trees;
    _mode = other._mode;
    _filename = std::move(other._filename);
}

search_tree<tdata, std::vector<std::shared_ptr<database::chain_of_responsibility>>> *database::collection::get_tree_instance(size_t t, std::function<int(tdata const &, tdata const &)> comparer, allocator *allocator, logger *logger) const
{    
    search_tree<tdata, std::vector<std::shared_ptr<chain_of_responsibility>>> *result = nullptr;
    switch (_variant)
    {
        default:
            auto tree = reinterpret_cast<b_tree<tdata, std::vector<std::shared_ptr<chain_of_responsibility>>> *>(allocate_with_guard(sizeof(b_tree<tdata, std::vector<std::shared_ptr<chain_of_responsibility>>>)));
            allocator::construct(tree, t, comparer, allocator, logger);
            result = tree;
    }

    return result;
}

void database::collection::insertion_processing(tdata const &to_insert, std::shared_ptr<chain_of_responsibility> const &chain)
{
    for (size_t i = 0; i < INDEXES_COUNT; ++i)
    {
        auto &subkey_tree = _container->obtain(static_cast<index>(i));

        try
        {
            auto chains = subkey_tree->obtain(to_insert);

            if (static_cast<index>(i) == index::main_key)
            {
                throw std::logic_error("This main key exists in collection");
            }

            chains.push_back(chain);
        }
        catch (std::logic_error const &exception)
        {
            subkey_tree->insert(to_insert, std::vector<std::shared_ptr<chain_of_responsibility>>{ chain });
        }
    }
}

void database::collection::update_processing(tdata const &data)
{
    auto &main_key_tree = _container->obtain(index::main_key);
    auto main_key_chains = main_key_tree->obtain(data);
    main_key_tree->dispose(data);
    
    auto obtained = main_key_chains[0]->obtain(date_time()).value();

    for (size_t i = 1; i < INDEXES_COUNT; ++i)
    {
        auto &secondary_key_tree = _container->obtain(static_cast<index>(i));
        auto &secondary_key_chains = secondary_key_tree->obtain(obtained);
        for (auto it = secondary_key_chains.cbegin(); it != secondary_key_chains.cend(); ++it)
        {
            if (*it == main_key_chains[0])
            {
                secondary_key_chains.erase(it);
                break;
            }
        }
        
        if (secondary_key_chains.empty())
        {
            secondary_key_tree->dispose(obtained);
        }
    }

    main_key_chains[0]->update(data);

    insertion_processing(data, main_key_chains[0]);
}

void database::collection::push_to_file(tdata const &data) const
{
    if (_mode != mode::file)
    {
        return;
    }

    std::ofstream os(_filename, std::ios::app);
    if (!os.is_open())
    {
        throw std::runtime_error("Unable to open file\n");
    }
    os << data.key.id << " " << data.value.name << " " << data.value.surname << " " << static_cast<std::string>(data.value.birthday) << " "; 
    for (auto const &mark: data.value.marks)
    {
        os << mark;
    }
    os << std::endl;
    os.close();
}


int database::collection::index_comparer(index const &one, index const &another) noexcept
{
    return static_cast<int>(one) - static_cast<int>(another);
}

inline database::index database::collection::get_next_index(index current) const noexcept
{
    return static_cast<index>(static_cast<uint8_t>(current) + 1);
}

inline allocator *database::collection::get_allocator() const noexcept
{
    return _allocator;
}

inline logger *database::collection::get_logger() const noexcept
{
    return _logger;
}

database::scheme::scheme(allocator *allocator, logger *logger, container_variant variant, size_t t):
    _allocator(allocator), _logger(logger), _variant(variant), _container(nullptr)
{
    switch (_variant)
    {
        default:
            auto tree = reinterpret_cast<b_tree<std::string, collection> *>(allocate_with_guard(sizeof(b_tree<std::string, collection>)));
            allocator::construct(tree, t, string_comparer, allocator, logger);
            _container = tree;
    }
}

void database::scheme::insert(std::string const &key, collection const &value)
{
    _container->insert(key, value);
}

void database::scheme::insert(std::string const &key, collection &&value)
{
    _container->insert(key, std::move(value));
}

database::collection &database::scheme::obtain(std::string const &key)
{
    return _container->obtain(key);
}

database::collection database::scheme::dispose(std::string const &key)
{    
    return _container->dispose(key);
}

database::scheme::~scheme()
{
    clear();
}

database::scheme::scheme(scheme const &other)
{
    copy_from(other);
}

database::scheme &database::scheme::operator=(scheme const &other)
{
    if (this != &other)
    {
        clear();
        copy_from(other);
    }

    return *this;
}

database::scheme::scheme(scheme &&other) noexcept
{
    move_from(std::move(other));
}

database::scheme &database::scheme::operator=(scheme &&other) noexcept
{
    if (this != &other)
    {
        clear();
        move_from(std::move(other));
    }

    return *this;
}

void database::scheme::clear() noexcept
{
    if (_container != nullptr)
    {
        allocator::destruct(_container);
        deallocate_with_guard(_container);
        _container = nullptr;
    }
}

void database::scheme::copy_from(scheme const &other)
{
    _allocator = other._allocator;
    _logger = other._logger;
    _variant = other._variant;

    switch (_variant)
    {
        default:
            auto tree = reinterpret_cast<b_tree<std::string, collection> *>(allocate_with_guard(sizeof(b_tree<std::string, collection>)));
            allocator::construct(tree, *dynamic_cast<b_tree<std::string, collection> *>(other._container));
            _container = tree;
    }
}

void database::scheme::move_from(scheme &&other) noexcept
{
    _allocator = std::exchange(other._allocator, nullptr);
    _logger = std::exchange(other._logger, nullptr);
    _variant = other._variant;
    _container = std::exchange(other._container, nullptr);
}

inline allocator *database::scheme::get_allocator() const noexcept
{
    return _allocator;
}

inline logger *database::scheme::get_logger() const noexcept
{
    return _logger;
}

database::pool::pool(allocator *allocator, logger *logger, container_variant variant, size_t t):
    _allocator(allocator), _logger(logger), _variant(variant), _container(nullptr)
{   
    switch (_variant)
    {
        default:
            auto tree = reinterpret_cast<b_tree<std::string, scheme> *>(allocate_with_guard(sizeof(b_tree<std::string, scheme>)));
            allocator::construct(tree, t, string_comparer, allocator, logger);
            _container = tree;
    }
}

void database::pool::insert(std::string const &key, scheme const &value)
{
    _container->insert(key, value);
}

void database::pool::insert(std::string const &key, scheme &&value)
{   
    _container->insert(key, std::move(value));
}

database::scheme &database::pool::obtain(std::string const &key)
{
    return _container->obtain(key);
}

database::scheme database::pool::dispose(std::string const &key)
{
    return _container->dispose(key);
}

database::pool::~pool()
{
    clear();
}

database::pool::pool(pool const &other)
{
    copy_from(other);
}

database::pool &database::pool::operator=(pool const &other)
{
    if (this != &other)
    {
        clear();
        copy_from(other);
    }

    return *this;
}

database::pool::pool(pool &&other) noexcept
{
    move_from(std::move(other));
}

database::pool &database::pool::operator=(pool &&other) noexcept
{
    if (this != &other)
    {
        clear();
        move_from(std::move(other));
    }

    return *this;
}

void database::pool::clear() noexcept
{
    if (_container != nullptr)
    {
        allocator::destruct(_container);
        deallocate_with_guard(_container);
        _container = nullptr;
    }
    
    // if (_mutex != nullptr)
    // {
    //     allocator::destruct(_mutex);
    //     deallocate_with_guard(_mutex);
    //     _mutex = nullptr;
    // }
}

void database::pool::copy_from(pool const &other)
{
    _allocator = other._allocator;
    _logger = other._logger;
    _variant = other._variant;

    switch (_variant)
    {
        default:
            auto tree = reinterpret_cast<b_tree<std::string, scheme> *>(allocate_with_guard(sizeof(b_tree<std::string, scheme>)));
            allocator::construct(tree, *dynamic_cast<b_tree<std::string, scheme> *>(other._container));
            _container = tree;
    }

    // _mutex = reinterpret_cast<std::mutex *>(allocate_with_guard(sizeof(std::mutex)));
    // allocator::construct(_mutex);
}

void database::pool::move_from(pool &&other) noexcept
{
    _allocator = std::exchange(other._allocator, nullptr);
    _logger = std::exchange(other._logger, nullptr);
    _variant = other._variant;
    _container = std::exchange(other._container, nullptr);
    // _mutex = std::exchange(other._mutex, nullptr);
}

inline allocator *database::pool::get_allocator() const noexcept
{
    return _allocator;
}

inline logger *database::pool::get_logger() const noexcept
{
    return _logger;
}

database::insert_command::insert_command(tdata const &initial_version):
    _initial_version(initial_version)
{

}

void database::insert_command::execute(bool &data_exists, std::optional<tdata> &data_to_modify) const
{
    if (data_exists)
    {
        throw std::logic_error("Attempt to insert already existent data");
    }

    if (data_to_modify.has_value())
    {
        data_to_modify.value() = _initial_version;
    }

    data_exists = true;
}

database::update_command::update_command(std::string const &update_expression):
    _update_expression(update_expression)
{

}

void database::update_command::execute(bool &data_exists, std::optional<tdata> &data_to_modify) const
{
    if (!data_exists)
    {
        throw std::logic_error("Attempt to modify non-existent data");
    }

    if (!data_to_modify.has_value())
    {
        return;
    }

    std::string iso8601;
    size_t marks_array_size = 0;
    
    std::istringstream ss(_update_expression);
    ss.read(reinterpret_cast<char *>(&data_to_modify.value().key.id), sizeof(data_to_modify.value().key.id));
    std::getline(ss, data_to_modify.value().value.name, '\0');
    std::getline(ss, data_to_modify.value().value.surname, '\0');
    std::getline(ss, iso8601, '\0');
    ss.read(reinterpret_cast<char *>(&marks_array_size), sizeof(marks_array_size));
    data_to_modify.value().value.marks = std::move(std::vector<unsigned char>(marks_array_size, 0));
    for (auto &mark: data_to_modify.value().value.marks)
    {
        ss.read(reinterpret_cast<char *>(&mark), sizeof(mark));
    }
    data_to_modify.value().value.birthday = date_time(iso8601);
}

void database::dispose_command::execute(bool &data_exists, std::optional<tdata> &data_to_modify) const
{
    if (!data_exists)
    {
        throw std::logic_error("attempt to dispose non-existent data");
    }

    data_exists = false;
}

database::chain_of_responsibility::handler::handler(command *command, int64_t datetime_activity_started):
    _command(command), _datetime_activity_started(datetime_activity_started), _next(nullptr)
{
    
}

void database::chain_of_responsibility::handler::handle(bool &data_exists, std::optional<tdata> &data_to_modify, int64_t datetime_target) const
{
    if (datetime_target <= _datetime_activity_started)
    {
        return;
    }

    _command->execute(data_exists, data_to_modify);

    if (_next != nullptr)
    {
        _next->handle(data_exists, data_to_modify, datetime_target);
    }
}

database::chain_of_responsibility::chain_of_responsibility(allocator *allocator, logger *logger):
    _first(nullptr), _last(nullptr), _allocator(allocator), _logger(logger)
{

}

void database::chain_of_responsibility::add_handler(command *command)
{
    int64_t date_time_activity_started = date_time();

    auto new_handler = reinterpret_cast<handler *>(allocate_with_guard(sizeof(handler)));
    allocator::construct(new_handler, command, date_time_activity_started);

    if (_last == nullptr)
    {
        _first = _last = new_handler;
        return;
    }

    _last->_next = new_handler;
    _last = new_handler;
}

void database::chain_of_responsibility::handle(bool &data_exists, std::optional<tdata> &data_to_modify, int64_t date_time_target)
{
    if (_first != nullptr)
    {
        _first->handle(data_exists, data_to_modify, date_time_target);
    }
}

void database::chain_of_responsibility::insert(tdata const &data_to_insert)
{
    bool data_exists = false;
    std::optional<tdata> empty_optional;

    int64_t current_date_time = date_time();

    handle(data_exists, empty_optional, current_date_time);
    if (data_exists)
    {
        throw std::logic_error("Attempt to add already existent data");
    }

    auto new_insert_command = reinterpret_cast<insert_command *>(allocate_with_guard(sizeof(insert_command)));
    allocator::construct(new_insert_command, data_to_insert);

    add_handler(new_insert_command); 
}

std::optional<tdata> database::chain_of_responsibility::obtain(int64_t date_time_of_activity)
{
    bool data_exists;
    tdata data_to_modify;

    auto data_to_modify_optional = std::make_optional(data_to_modify);
    handle(data_exists, data_to_modify_optional, date_time_of_activity);
    
    return data_exists
        ? data_to_modify_optional
        : std::optional<tdata>();
}

void database::chain_of_responsibility::update(tdata const &data)
{
    bool data_exists = false;
    std::optional<tdata> empty_optional;

    int64_t current_date_time = date_time();

    handle(data_exists, empty_optional, current_date_time);
    if (!data_exists)
    {
        throw std::logic_error("Attempt to update non-existent data");
    }

    std::string iso8601 = static_cast<std::string>(data.value.birthday);
    size_t marks_array_size = data.value.marks.size();

    std::ostringstream oss;
    oss.write(reinterpret_cast<char const *>(&data.key.id), sizeof(data.key.id));
    oss.write(data.value.name.c_str(), data.value.name.size() + 1);
    oss.write(data.value.surname.c_str(), data.value.surname.size() + 1);
    oss.write(iso8601.c_str(), iso8601.size() + 1);
    oss.write(reinterpret_cast<char const *>(&marks_array_size), sizeof(marks_array_size));
    for (auto const &mark: data.value.marks)
    {
        oss.write(reinterpret_cast<char const *>(&mark), sizeof(mark));
    }

    auto new_update_command = reinterpret_cast<update_command *>(allocate_with_guard(sizeof(update_command)));
    allocator::construct(new_update_command, oss.str());
    add_handler(new_update_command);
}

void database::chain_of_responsibility::dispose()
{
    bool data_exists = false;
    std::optional<tdata> empty_optional;

    int64_t current_date_time = date_time();
    handle(data_exists, empty_optional, current_date_time);
    if (!data_exists)
    {
        throw std::logic_error("attempt to add data dispose handler while data not exists");
    }

    auto new_dispose_command = reinterpret_cast<dispose_command *>(allocate_with_guard(sizeof(dispose_command)));
    allocator::construct(new_dispose_command);
    
    add_handler(new dispose_command);
}

database::chain_of_responsibility::~chain_of_responsibility()
{
    clear();
}

database::chain_of_responsibility::chain_of_responsibility(chain_of_responsibility const &other)
{
    copy_from(other);
}

database::chain_of_responsibility &database::chain_of_responsibility::operator=(chain_of_responsibility const &other)
{
    if (this != &other)
    {
        clear();
        copy_from(other);
    }

    return *this;
}

database::chain_of_responsibility::chain_of_responsibility(chain_of_responsibility &&other) noexcept
{
    move_from(std::move(other));
}

database::chain_of_responsibility &database::chain_of_responsibility::operator=(chain_of_responsibility &&other) noexcept
{
    if (this != &other)
    {
        clear();
        move_from(std::move(other));
    }

    return *this;
}

void database::chain_of_responsibility::clear() noexcept
{   
    while (_first != nullptr)
    {
        handler *to_deallocate = _first;
        _first = _first->_next;

        allocator::destruct(to_deallocate->_command);
        deallocate_with_guard(to_deallocate->_command);

        allocator::destruct(to_deallocate);
        deallocate_with_guard(to_deallocate);
    }
}

void database::chain_of_responsibility::copy_from(chain_of_responsibility const &other)
{
    _allocator = other._allocator;
    _logger = other._logger;

    _first = nullptr;
    _last = other._last;

    handler *iterator = other._first;
    while (iterator != nullptr)
    {
        _first = reinterpret_cast<handler *>(allocate_with_guard(sizeof(handler)));
        allocator::construct(_first, *iterator);
        
        iterator = iterator->_next;
    }
}

void database::chain_of_responsibility::move_from(chain_of_responsibility &&other) noexcept
{
    _allocator = std::exchange(other._allocator, nullptr);
    _logger = std::exchange(other._logger, nullptr);

    _first = std::exchange(other._first, nullptr);
    _last = std::exchange(other._last, nullptr);
}

inline allocator *database::chain_of_responsibility::get_allocator() const noexcept
{
    return _allocator;
}

inline logger *database::chain_of_responsibility::get_logger() const noexcept
{
    return _logger;
}

std::shared_ptr<database> database::get_instance(size_t t_for_b_trees, mode mode, container_variant variant, allocator *allocator, logger *logger)
{
    static std::shared_ptr<database> instance(new database(t_for_b_trees, mode, variant, allocator, logger));
    return instance;
}

database::~database()
{
    if (_container != nullptr)
    {
        allocator::destruct(_container);
        deallocate_with_guard(_container);
        _container = nullptr;
    } 
}

database *database::add_pool(std::string const &pool_name, container_variant variant, size_t t_for_b_trees)
{
    insert(pool_name, std::move(pool(_allocator, _logger, variant, t_for_b_trees)));
    return this;
}

database *database::add_scheme(std::string const &pool_name, std::string const &scheme_name, container_variant variant, size_t t_for_b_trees)
{
    obtain(pool_name).insert(scheme_name, std::move(scheme(_allocator, _logger, variant, t_for_b_trees)));
    return this;
}

database *database::add_collection(std::string const &pool_name, std::string const &scheme_name, std::string const &collection_name, container_variant variant, size_t t_for_b_trees)
{
    obtain(pool_name).obtain(scheme_name).insert(collection_name, std::move(collection(pool_name + '_' + scheme_name + '_' + collection_name + ".txt", _mode, _allocator, _logger, variant, t_for_b_trees)));
   return this;
}

database *database::dispose_pool(std::string const &pool_name)
{
    dispose(pool_name);
    return this;
}

database *database::dispose_scheme(std::string const &pool_name, std::string const &scheme_name)
{
    obtain(pool_name).dispose(scheme_name);
    return this;
}

database *database::dispose_collection(std::string const &pool_name, std::string const &scheme_name, std::string const &collection_name)
{
    obtain(pool_name).obtain(scheme_name).dispose(collection_name);
    return this;
}

database *database::insert(std::string const &pool_name, std::string const &scheme_name, std::string const &collection_name, tkey const &key, tvalue const &value)
{
    obtain(pool_name).obtain(scheme_name).obtain(collection_name).insert(key, value);
    
    return this;
}

database *database::insert(std::string const &pool_name, std::string const &scheme_name, std::string const &collection_name, tkey const &key, tvalue &&value)
{
    obtain(pool_name).obtain(scheme_name).obtain(collection_name).insert(key, std::move(value));
    return this;
}

database *database::update(std::string const &pool_name, std::string const &scheme_name, std::string const &collection_name, tkey const &key, tvalue const &value)
{
    obtain(pool_name).obtain(scheme_name).obtain(collection_name).update(key, value);
    return this;
}

database *database::update(std::string const &pool_name, std::string const &scheme_name, std::string const &collection_name, tkey const &key, tvalue &&value)
{
    obtain(pool_name).obtain(scheme_name).obtain(collection_name).update(key, std::move(value));
    return this;
}

std::vector<tdata> database::obtain(std::string const &pool_name, std::string const &scheme_name, std::string const &collection_name, tdata const &to_find, index where, date_time const &time)
{
    return obtain(pool_name).obtain(scheme_name).obtain(collection_name).obtain(to_find, where, time);
}

std::vector<tdata> database::obtain(std::string const &pool_name, std::string const &scheme_name, std::string const &collection_name, tdata const &to_find, std::string const &index_name, date_time const &time)
{
    return obtain(pool_name, scheme_name, collection_name, to_find, string_to_index(index_name), time);
}

std::vector<tdata> database::dispose(std::string const &pool_name, std::string const &scheme_name, std::string const &collection_name, tdata const &to_dispose, index where)
{
    return obtain(pool_name).obtain(scheme_name).obtain(collection_name).dispose(to_dispose, where);
}

std::vector<tdata> database::dispose(std::string const &pool_name, std::string const &scheme_name, std::string const &collection_name, tdata const &to_dispose, std::string const &index_name)
{
    return obtain(pool_name).obtain(scheme_name).obtain(collection_name).dispose(to_dispose, string_to_index(index_name));
}

std::vector<tdata> database::obtain_between(std::string const &pool_name, std::string const &scheme_name, std::string const &collection_name, tdata const &lower_bound, tdata const &upper_bound, index where, date_time date_time_target, bool lower_bound_inclusive, bool upper_bound_inclusive)
{
    return obtain(pool_name).obtain(scheme_name).obtain(collection_name).obtain_between(lower_bound, upper_bound, where, date_time_target, lower_bound_inclusive, upper_bound_inclusive);
}

std::vector<tdata> database::obtain_between(std::string const &pool_name, std::string const &scheme_name, std::string const &collection_name, tdata const &lower_bound, tdata const &upper_bound, std::string const &index_name, date_time date_time_target, bool lower_bound_inclusive, bool upper_bound_inclusive)
{
    return obtain(pool_name).obtain(scheme_name).obtain(collection_name).obtain_between(lower_bound, upper_bound, string_to_index(index_name), date_time_target, lower_bound_inclusive, upper_bound_inclusive);
}

database::database(size_t t, mode mode, container_variant variant, allocator *allocator, logger *logger):
    _mode(mode), _variant(variant), _allocator(allocator), _logger(logger)
{
    switch (_variant)
    {
        default:
            auto tree = reinterpret_cast<b_tree<std::string, pool> *>(allocate_with_guard(sizeof(b_tree<std::string, pool>)));
            allocator::construct(tree, t, string_comparer, _allocator, _logger);
            _container = tree;
    }
}

void database::insert(std::string const &key, pool const &value)
{
    _container->insert(key, value);
}

void database::insert(std::string const &key, pool &&value)
{
    _container->insert(key, std::move(value));
}

database::pool &database::obtain(std::string const &key)
{
    return _container->obtain(key);
}

database::pool database::dispose(std::string const &key)
{
    return _container->dispose(key);
}

database::index database::string_to_index(std::string const &to_convert) const
{
    index result;
    if (to_convert == "main_key")
    {
        result = index::main_key;
    }
    else if (to_convert == "name")
    {
        result = index::name;
    }
    else if (to_convert == "surname")
    {
        result = index::surname;
    }
    else if (to_convert == "birthday")
    {
        result = index::birthday;
    }
    else if (to_convert == "marks")
    {
        result = index::marks;
    }
    else
    {
        throw std::runtime_error("Can't convert" + to_convert + "to database::index");
    }

    return result;
}

inline allocator *database::get_allocator() const noexcept
{
    return _allocator;
}

inline logger *database::get_logger() const noexcept
{
    return _logger;
}