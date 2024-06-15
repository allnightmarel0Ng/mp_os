#ifndef DATABASE_H
#define DATABASE_H

#include <optional>
#include <memory>
#include <sstream>
#include <fstream>
#include <filesystem>

#include <search_tree.h>
#include <b_tree.h>

#include "common.h"
#include "date_time.h"

class database final:
    private allocator_guardant,
    private logger_guardant
{

public:

    enum class mode
    {
        file,
        in_memory
    };

    enum class container_variant
    {
        b_tree,
        b_plus_tree,
        b_star_tree,
        b_star_plus_tree
    };

    enum class index
    {
        main_key,
        name,
        surname,
        birthday,
        marks
    };

private:

    class chain_of_responsibility;

    class collection final:
        private allocator_guardant,
        private logger_guardant
    {

    public:
    
        explicit collection(
            std::string const &filename,
            mode mode = mode::in_memory,
            allocator *allocator = nullptr,
            logger *logger = nullptr,
            container_variant variant = container_variant::b_tree,
            size_t t = 8);

    public:

        void insert(tkey const &key, tvalue const &value);

        void insert(tkey const &key, tvalue &&value);

        void update(tkey const &key, tvalue const &value);

        void update(tkey const &key, tvalue &&value);

        std::vector<tdata> obtain(tdata const &to_find, index where, date_time const &time);

        std::vector<tdata> dispose(tdata const &data, index where);
        
        std::vector<tdata> obtain_between(tdata const &lower_bound, tdata const &upper_bound, index where, date_time date_time_target, bool lower_bound_inclusive = true, bool upper_bound_inclusive = true);
    
    public:

        ~collection();

        collection(collection const &other);

        collection &operator=(collection const &other);

        collection(collection &&other) noexcept;

        collection &operator=(collection &&other) noexcept;

    private:

        void clear() noexcept;

        void copy_from(collection const &other);

        void move_from(collection &&other) noexcept;

    private: 

        search_tree<tdata, std::vector<std::shared_ptr<chain_of_responsibility>>> *get_tree_instance(size_t t, std::function<int(tdata const &, tdata const &)> comparer, allocator *allocator, logger *logger) const;

        void insertion_processing(tdata const &to_insert, std::shared_ptr<chain_of_responsibility> const &chain);

        void update_processing(tdata const &data);

    private:

        static int index_comparer(index const &one, index const &another) noexcept;

        inline index get_next_index(index current) const noexcept;

    private:

        void push_to_file(tdata const &data) const;
    
    private:

        [[nodiscard]] inline allocator *get_allocator() const noexcept override;

        [[nodiscard]] inline logger *get_logger() const noexcept override;
    
    private:

        allocator *_allocator;

        logger *_logger;
        
        search_tree<index, search_tree<tdata, std::vector<std::shared_ptr<chain_of_responsibility>>> *> *_container;
        
        container_variant _variant;

        size_t _t_for_b_trees;

        mode _mode;

        std::string _filename;

        std::ofstream _os;
    
    private:

        size_t const INDEXES_COUNT = 5;
    
    };

    class scheme final:
        private allocator_guardant,
        private logger_guardant
    {
    
    public:
    
        explicit scheme(
            allocator *allocator = nullptr,
            logger *logger = nullptr,
            container_variant variant = container_variant::b_tree,
            size_t t = 8);

    public:

        void insert(std::string const &key, collection const &value);

        void insert(std::string const &key, collection &&value);

        collection &obtain(std::string const &key);

        collection dispose(std::string const &key);
    
    public:

        ~scheme();

        scheme(scheme const &other);

        scheme &operator=(scheme const &other);

        scheme(scheme &&other) noexcept;

        scheme &operator=(scheme &&other) noexcept;

    private:

        void clear() noexcept;

        void copy_from(scheme const &other);

        void move_from(scheme &&other) noexcept;
    
    private:

        [[nodiscard]] inline allocator *get_allocator() const noexcept override;

        [[nodiscard]] inline logger *get_logger() const noexcept override;
    
    private:

        allocator *_allocator;

        logger *_logger;

        search_tree<std::string, collection> *_container;

        container_variant _variant;

    };

    class pool final:
        private allocator_guardant,
        private logger_guardant
    {
    
    public:
    
        explicit pool(
            allocator *allocator = nullptr,
            logger *logger = nullptr,
            container_variant variant = container_variant::b_tree,
            size_t t = 8);

    public:

        void insert(std::string const &key, scheme const &value);

        void insert(std::string const &key, scheme &&value);

        scheme &obtain(std::string const &key);

        scheme dispose(std::string const &key);
    
    public:

        ~pool();

        pool(pool const &other);

        pool &operator=(pool const &other);

        pool(pool &&other) noexcept;

        pool &operator=(pool &&other) noexcept;

    private:

        void clear() noexcept;

        void copy_from(pool const &other);

        void move_from(pool &&other) noexcept;
    
    private:

        [[nodiscard]] inline allocator *get_allocator() const noexcept override;

        [[nodiscard]] inline logger *get_logger() const noexcept override;
    
    private:

        allocator *_allocator;

        logger *_logger;

        search_tree<std::string, scheme> *_container;

        container_variant _variant;

    };

private:

    class command
    {

    public:

        virtual ~command() = default;
    
    public:

        virtual void execute(bool &data_exists, std::optional<tdata> &data_to_modify) const = 0;

    };

    class insert_command final:
        public command
    {

    private:

        tdata _initial_version;

    public:

        explicit insert_command(tdata const &initial_version);
    
    public:

        void execute(bool &data_exists, std::optional<tdata> &data_to_modify) const override;
    
    };

    class update_command final:
        public command
    {

    private:

        std::string _update_expression;

    public:

        explicit update_command(std::string const &update_expression);
    
    public:

        void execute(bool &data_exists, std::optional<tdata> &data_to_modify) const override;

    };

    class dispose_command final:
        public command
    {
    
    public:

        void execute(bool &data_exists, std::optional<tdata> &data_to_modify) const override;

    };

private:

    class chain_of_responsibility final:
        private allocator_guardant,
        private logger_guardant
    {
    
    public:

        explicit chain_of_responsibility(allocator *allocator, logger *logger);
    
    public:
        
        void add_handler(command *command);

        void handle(bool &data_exists, std::optional<tdata> &data_to_modify, int64_t date_time_target);
    
    public:

        void insert(tdata const &data);

        std::optional<tdata> obtain(std::int64_t date_time_of_activity);

        void update(tdata const &data);

        void dispose();

    public:

        ~chain_of_responsibility();

        chain_of_responsibility(chain_of_responsibility const &other);

        chain_of_responsibility &operator=(chain_of_responsibility const &other);

        chain_of_responsibility(chain_of_responsibility &&other) noexcept;

        chain_of_responsibility &operator=(chain_of_responsibility &&other) noexcept;
    
    private:

        class handler final
        {
            
            friend class chain_of_responsibility;
        
        private:

            command *_command;

            int64_t _datetime_activity_started;

            handler *_next;
        
        public:

            explicit handler(command *command, int64_t datetime_activity_started);

            void handle(bool &data_exists, std::optional<tdata> &data_to_modify, int64_t datetime_target) const;

        };

    private:

        handler *_first;

        handler *_last;

        allocator *_allocator;

        logger *_logger;
    
    private:

        void clear() noexcept;

        void copy_from(chain_of_responsibility const &other);

        void move_from(chain_of_responsibility &&other) noexcept;

    private:

        [[nodiscard]] inline allocator *get_allocator() const noexcept override;
        
        [[nodiscard]] inline logger *get_logger() const noexcept override;

    };

public:

    static std::shared_ptr<database> get_instance(size_t t_for_b_trees = 8, mode mode = mode::in_memory, container_variant variant = container_variant::b_tree, allocator *allocator = nullptr, logger *logger = nullptr);

public:

    ~database();

public:

    database(database const &) = delete;

    database(database &&) = delete;

public:

    database *add_pool(std::string const &pool_name, container_variant variant = container_variant::b_tree, size_t t_for_b_trees = 8);

    database *add_scheme(std::string const &pool_name, std::string const &scheme_name, container_variant variant = container_variant::b_tree, size_t t_for_b_trees = 8);

    database *add_collection(std::string const &pool_name, std::string const &scheme_name, std::string const &collection_name, container_variant variant = container_variant::b_tree, size_t t_for_b_trees = 8);

    database *dispose_pool(std::string const &pool_name);

    database *dispose_scheme(std::string const &pool_name, std::string const &scheme_name);

    database *dispose_collection(std::string const &pool_name, std::string const &scheme_name, std::string const &collection_name);

public:

    database *insert(std::string const &pool_name, std::string const &scheme_name, std::string const &collection_name, tkey const &key, tvalue const &value);

    database *insert(std::string const &pool_name, std::string const &scheme_name, std::string const &collection_name, tkey const &key, tvalue &&value);

    database *update(std::string const &pool_name, std::string const &scheme_name, std::string const &collection_name, tkey const &key, tvalue const &value);

    database *update(std::string const &pool_name, std::string const &scheme_name, std::string const &collection_name, tkey const &key, tvalue &&value);

    std::vector<tdata> obtain(std::string const &pool_name, std::string const &scheme_name, std::string const &collection_name, tdata const &to_find, index where, date_time const &time);

    std::vector<tdata> obtain(std::string const &pool_name, std::string const &scheme_name, std::string const &collection_name, tdata const &to_find, std::string const &index_name, date_time const &time);

    std::vector<tdata> dispose(std::string const &pool_name, std::string const &scheme_name, std::string const &collection_name, tdata const &to_dispose, index where);

    std::vector<tdata> dispose(std::string const &pool_name, std::string const &scheme_name, std::string const &collection_name, tdata const &to_dispose, std::string const &index_name);

    std::vector<tdata> obtain_between(std::string const &pool_name, std::string const &scheme_name, std::string const &collection_name, tdata const &lower_bound, tdata const &upper_bound, index where, date_time date_time_target, bool lower_bound_inclusive = true, bool upper_bound_inclusive = true);

    std::vector<tdata> obtain_between(std::string const &pool_name, std::string const &scheme_name, std::string const &collection_name, tdata const &lower_bound, tdata const &upper_bound, std::string const &index_name, date_time date_time_target, bool lower_bound_inclusive = true, bool upper_bound_inclusive = true);

private:

    search_tree<std::string, pool> *_container;

    mode _mode;

    container_variant _variant;

    allocator *_allocator;

    logger *_logger;

private:

    explicit database(
        size_t t,
        mode _mode, 
        container_variant variant, 
        allocator *allocator, 
        logger *logger);

private:

    void insert(std::string const &key, pool const &value);

    void insert(std::string const &key, pool &&data);

    pool &obtain(std::string const &key);

    pool dispose(std::string const &key);

private:

    index string_to_index(std::string const &to_convert) const;

private:

    [[nodiscard]] inline allocator *get_allocator() const noexcept override;

    [[nodiscard]] inline logger *get_logger() const noexcept override;

};

#endif