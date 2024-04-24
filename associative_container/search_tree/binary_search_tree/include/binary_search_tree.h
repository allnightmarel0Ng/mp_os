#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_BINARY_SEARCH_TREE_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_BINARY_SEARCH_TREE_H

#include <list>
#include <stack>
#include <vector>
#include <logger.h>
#include <logger_guardant.h>
#include <allocator.h>
#include <allocator_guardant.h>
#include <search_tree.h>
#include <typename_holder.h>
#include <automatic_logger.h>

template<typename tkey, typename tvalue>
class binary_search_tree:
    public search_tree<tkey, tvalue>
{

protected:
    
    struct node
    {
    
    public:
        
        tkey key;
        
        tvalue value;
        
        node *left_subtree;
        
        node *right_subtree;
    
    public:
        
        explicit node(tkey const &key, tvalue const &value);
        
        explicit node(tkey const &key, tvalue &&value);
        
    };

private:

    class iterator:
        public allocator_guardant
    {
    
    public:

        iterator(binary_search_tree<tkey, tvalue> const *holder);

    public:

        template<typename T>
        bool check_equality(std::stack<T> const &one, std::stack<T> const &another) const noexcept;
            
    protected:

        typename binary_search_tree<tkey, tvalue>::iterator_data *create_iterator_data() const;

        void iterator_destructor(typename binary_search_tree<tkey, tvalue>::iterator_data *&data, bool &flag) const noexcept;

        template<typename T>
        void iterator_copy_constructor(typename binary_search_tree<tkey, tvalue>::iterator_data *&this_data, std::stack<T> &this_state, bool &this_flag, typename binary_search_tree<tkey, tvalue>::iterator_data const *other_data, std::stack<T> const &other_state, bool other_flag) const;

        template<typename T>
        void iterator_copy_assignment(typename binary_search_tree<tkey, tvalue>::iterator_data *&this_data, std::stack<T> &this_state, bool &this_flag, typename binary_search_tree<tkey, tvalue>::iterator_data const *other_data, std::stack<T> const &other_state, bool other_flag) const;

        template<typename T>
        void iterator_move_constructor(typename binary_search_tree<tkey, tvalue>::iterator_data *&this_data, std::stack<T> &this_state, bool &this_flag, typename binary_search_tree<tkey, tvalue>::iterator_data *&other_data, std::stack<T> &other_state, bool &other_flag) const noexcept;
        
        template<typename T>
        void iterator_move_assignment(typename binary_search_tree<tkey, tvalue>::iterator_data *&this_data, std::stack<T> &this_state, bool &this_flag, typename binary_search_tree<tkey, tvalue>::iterator_data *&other_data, std::stack<T> &other_state, bool &other_flag) const noexcept;
    
    protected:

        void call_iterator_data_constructor(typename binary_search_tree<tkey, tvalue>::iterator_data *raw_space, node *subtree_root, unsigned int depth) const;

        size_t get_iterator_data_size() const noexcept;
    
    private:

        [[nodiscard]] allocator *get_allocator() const noexcept final;
    
    protected:

        binary_search_tree<tkey, tvalue> *_holder;

    };

public:
    
    // region iterators definition
    
    struct iterator_data
    {
    
    public:
        
        unsigned int depth;
        
        tkey key;
        
        tvalue value;
    
    public:

        inline tkey const &get_key() const noexcept;

        inline tvalue const &get_value() const noexcept;
    
    public:
    
        explicit iterator_data(unsigned int depth, tkey const &key, tvalue const &value);
        
    };
    
    class prefix_iterator final:
        private iterator
    {

        friend class binary_search_tree<tkey, tvalue>;
    
    private:

        iterator_data *_data;

        bool _is_data_constructed;
        
        std::stack<std::pair<node *, unsigned int>> _state;
    
    private:
        
        explicit prefix_iterator(typename binary_search_tree<tkey, tvalue>::node *subtree_root, binary_search_tree<tkey, tvalue> const *holder);
    
    public:

        virtual ~prefix_iterator() noexcept;

        prefix_iterator(prefix_iterator const &other);

        prefix_iterator &operator=(prefix_iterator const &other);

        prefix_iterator(prefix_iterator &&other) noexcept;

        prefix_iterator &operator=(prefix_iterator &&other) noexcept;
    
    public:
        
        bool operator==(prefix_iterator const &other) const noexcept;
        
        bool operator!=(prefix_iterator const &other) const noexcept;
        
        prefix_iterator &operator++();
        
        prefix_iterator const operator++(int not_used);
        
        iterator_data *operator*() const;
        
    };
    
    class prefix_const_iterator final
    {
        
        friend class binary_search_tree<tkey, tvalue>;
    
    private:

        prefix_iterator _it;
    
    private:
        
        explicit prefix_const_iterator(typename binary_search_tree<tkey, tvalue>::node *subtree_root, binary_search_tree<tkey, tvalue> const *holder);
    
    public:
        
        bool operator==(prefix_const_iterator const &other) const noexcept;
        
        bool operator!=(prefix_const_iterator const &other) const noexcept;
        
        prefix_const_iterator &operator++();
        
        prefix_const_iterator const operator++(int not_used);
        
        iterator_data const *operator*() const;
        
    };
    
    class prefix_reverse_iterator final:
        private iterator
    {

        friend class binary_search_tree<tkey, tvalue>;
    
    private:

        iterator_data *_data;

        bool _is_data_constructed;
        
        std::stack<std::pair<node *, unsigned int>> _state;
    
    private:
        
        explicit prefix_reverse_iterator(typename binary_search_tree<tkey, tvalue>::node *subtree_root, binary_search_tree<tkey, tvalue> const *holder);

    public:

        virtual ~prefix_reverse_iterator() noexcept;

        prefix_reverse_iterator(prefix_reverse_iterator const &other);

        prefix_reverse_iterator &operator=(prefix_reverse_iterator const &other);

        prefix_reverse_iterator(prefix_reverse_iterator &&other) noexcept;

        prefix_reverse_iterator &operator=(prefix_reverse_iterator &&other) noexcept;
    
    public:
        
        bool operator==(prefix_reverse_iterator const &other) const noexcept;
        
        bool operator!=(prefix_reverse_iterator const &other) const noexcept;
        
        prefix_reverse_iterator &operator++();
        
        prefix_reverse_iterator const operator++(int not_used);
        
        iterator_data *operator*() const;
        
    };
    
    class prefix_const_reverse_iterator final
    {

        friend class binary_search_tree<tkey, tvalue>;
    
    private:

        prefix_reverse_iterator _it;
    
    private:
        
        explicit prefix_const_reverse_iterator(typename binary_search_tree<tkey, tvalue>::node *subtree_root, binary_search_tree<tkey, tvalue> const *holder);
    
    public:
        
        bool operator==(prefix_const_reverse_iterator const &other) const noexcept;
        
        bool operator!=(prefix_const_reverse_iterator const &other) const noexcept;
        
        prefix_const_reverse_iterator &operator++();
        
        prefix_const_reverse_iterator const operator++(int not_used);
        
        iterator_data const *operator*() const;
        
    };
    
    class infix_iterator final:
        private iterator
    {

        friend class binary_search_tree<tkey, tvalue>;
    
    private:

        iterator_data *_data;

        bool _is_data_constructed;
        
        std::stack<node *> _state;
    
    private:
        
        explicit infix_iterator(typename binary_search_tree<tkey, tvalue>::node *subtree_root, binary_search_tree<tkey,tvalue> const *holder);

        explicit infix_iterator(
            binary_search_tree<tkey,tvalue> const *holder,
            std::stack<node *> state);


    public:

        virtual ~infix_iterator() noexcept;

        infix_iterator(infix_iterator const &other);

        infix_iterator &operator=(infix_iterator const &other);

        infix_iterator(infix_iterator &&other) noexcept;

        infix_iterator &operator=(infix_iterator &&other) noexcept;
   
    public:
        
        bool operator==(infix_iterator const &other) const noexcept;
        
        bool operator!=(infix_iterator const &other) const noexcept;
        
        infix_iterator &operator++();
        
        infix_iterator const operator++(int not_used);
        
        iterator_data *operator*() const;
        
    };
    
    class infix_const_iterator final
    {

        friend class binary_search_tree<tkey, tvalue>;

    private:

        infix_iterator _it;
    
    private:
        
        explicit infix_const_iterator(typename binary_search_tree<tkey, tvalue>::node *subtree_root, binary_search_tree<tkey, tvalue> const *holder);
    
    public:
        
        bool operator==(infix_const_iterator const &other) const noexcept;
        
        bool operator!=(infix_const_iterator const &other) const noexcept;
        
        infix_const_iterator &operator++();
        
        infix_const_iterator const operator++(int not_used);
        
        iterator_data const *operator*() const;
        
    };
    
    class infix_reverse_iterator final:
        private iterator
    {

        friend class binary_search_tree<tkey, tvalue>;

    private:

        iterator_data *_data;

        bool _is_data_constructed;

        std::stack<node *> _state;
    
    private:
        
        explicit infix_reverse_iterator(typename binary_search_tree<tkey, tvalue>::node *subtree_root, binary_search_tree<tkey, tvalue> const *holder);
    
    public:

        virtual ~infix_reverse_iterator() noexcept;

        infix_reverse_iterator(infix_reverse_iterator const &other);

        infix_reverse_iterator &operator=(infix_reverse_iterator const &other);

        infix_reverse_iterator(infix_reverse_iterator &&other) noexcept;

        infix_reverse_iterator &operator=(infix_reverse_iterator &&other) noexcept;
    
    public:
        
        bool operator==(infix_reverse_iterator const &other) const noexcept;
        
        bool operator!=(infix_reverse_iterator const &other) const noexcept;
        
        infix_reverse_iterator &operator++();
        
        infix_reverse_iterator const operator++(int not_used);
        
        iterator_data *operator*() const;
        
    };
    
    class infix_const_reverse_iterator final
    {

        friend class binary_search_tree<tkey, tvalue>;

    private:

        infix_reverse_iterator _it;
    
    private:
        
        explicit infix_const_reverse_iterator(typename binary_search_tree<tkey, tvalue>::node *subtree_root, binary_search_tree<tkey, tvalue> const *holder);
    
    public:
        
        bool operator==(infix_const_reverse_iterator const &other) const noexcept;
        
        bool operator!=(infix_const_reverse_iterator const &other) const noexcept;
        
        infix_const_reverse_iterator &operator++();
        
        infix_const_reverse_iterator const operator++(int not_used);
        
        iterator_data const *operator*() const;
        
    };
    
    class postfix_iterator final:
        private iterator
    {

        friend class binary_search_tree<tkey, tvalue>;

    private:

        iterator_data *_data;

        bool _is_data_constructed;
        
        std::stack<std::pair<node *, unsigned int>> _state;
    
    private:
        
        explicit postfix_iterator(typename binary_search_tree<tkey, tvalue>::node *subtree_root, binary_search_tree<tkey, tvalue> const *holder);

        void iterate(typename binary_search_tree<tkey, tvalue>::node *subtree_root, unsigned int subtree_root_depth);
    
    public:

        virtual ~postfix_iterator() noexcept;

        postfix_iterator(postfix_iterator const &other);

        postfix_iterator &operator=(postfix_iterator const &other);

        postfix_iterator(postfix_iterator &&other) noexcept;

        postfix_iterator &operator=(postfix_iterator &&other) noexcept;
    
    public:
        
        bool operator==(postfix_iterator const &other) const noexcept;
        
        bool operator!=(postfix_iterator const &other) const noexcept;
        
        postfix_iterator &operator++();
        
        postfix_iterator const operator++(int not_used);
        
        iterator_data *operator*() const;
        
    };
    
    class postfix_const_iterator final
    {

        friend class binary_search_tree<tkey, tvalue>;
    
    private:

        postfix_iterator _it;
    
    private:
        
        explicit postfix_const_iterator(typename binary_search_tree<tkey, tvalue>::node *subtree_root, binary_search_tree<tkey, tvalue> const *holder);
    
    public:
        
        bool operator==(postfix_const_iterator const &other) const noexcept;
        
        bool operator!=(postfix_const_iterator const &other) const noexcept;
        
        postfix_const_iterator &operator++();
        
        postfix_const_iterator const operator++(int not_used);
        
        iterator_data const *operator*() const;
        
    };
    
    class postfix_reverse_iterator final:
        private iterator
    {

        friend class binary_search_tree<tkey, tvalue>;
    
    private:

        iterator_data *_data;

        bool _is_data_constructed;

        std::stack<std::pair<node *, unsigned int>> _state;
    
    private:
        
        explicit postfix_reverse_iterator(typename binary_search_tree<tkey, tvalue>::node *subtree_root, binary_search_tree<tkey, tvalue> const *holder);

        void iterate(typename binary_search_tree<tkey, tvalue>::node *subtree_root, unsigned int depth);

    public:

        virtual ~postfix_reverse_iterator() noexcept;

        postfix_reverse_iterator(postfix_reverse_iterator const &other);

        postfix_reverse_iterator &operator=(postfix_reverse_iterator const &other);

        postfix_reverse_iterator(postfix_reverse_iterator &&other) noexcept;

        postfix_reverse_iterator &operator=(postfix_reverse_iterator &&other) noexcept;
    
    public:

        bool operator==(postfix_reverse_iterator const &other) const noexcept;

        bool operator!=(postfix_reverse_iterator const &other) const noexcept;
        
        postfix_reverse_iterator &operator++();
        
        postfix_reverse_iterator const operator++(int not_used);
        
        iterator_data *operator*() const;
        
    };
    
    class postfix_const_reverse_iterator final
    {

        friend class binary_search_tree<tkey, tvalue>;
    
    private:

        postfix_reverse_iterator _it;
    
    private:
        
        explicit postfix_const_reverse_iterator(typename binary_search_tree<tkey, tvalue>::node *subtree_root, binary_search_tree<tkey, tvalue> const *holder);
    
    public:
        
        bool operator==(postfix_const_reverse_iterator const &other) const noexcept;
        
        bool operator!=(postfix_const_reverse_iterator const &other) const noexcept;
        
        postfix_const_reverse_iterator &operator++();
        
        postfix_const_reverse_iterator const operator++(int not_used);
        
        iterator_data const *operator*() const;
        
    };
    
    // endregion iterators definition

protected:
    
    // region target operations strategies definition
    
    enum class insertion_of_existent_key_attempt_strategy
    {
        update_value,
        throw_an_exception
    };
    
    enum class disposal_of_nonexistent_key_attempt_strategy
    {
        do_nothing,
        throw_an_exception
    };
    
    // endregion target operations strategies definition
    
    // region target operations associated exception types
    
    class insertion_of_existent_key_attempt_exception final:
        public std::logic_error
    {
    
    private:
        
        tkey _key;
    
    public:
        
        explicit insertion_of_existent_key_attempt_exception(tkey const &key);
        
    public:
        
        tkey const &get_key() const noexcept;
    
    };
    
    class obtaining_of_nonexistent_key_attempt_exception final:
        public std::logic_error
    {
    
    private:
        
        tkey _key;
        
    public:
        
        explicit obtaining_of_nonexistent_key_attempt_exception(tkey const &key);
        
    public:
        
        tkey const &get_key() const noexcept;
        
    };
    
    class disposal_of_nonexistent_key_attempt_exception final:
        public std::logic_error
    {
    
    private:
        
        tkey _key;
    
    public:
        
        explicit disposal_of_nonexistent_key_attempt_exception(tkey const &key);
        
    public:
        
        tkey const &get_key() const noexcept;
    
    };
    
    // endregion target operations associated exception types
    
    // region template methods definition
    
    class template_method_basics:
        public logger_guardant,
        public allocator_guardant
    {

        friend class binary_search_tree<tkey, tvalue>;
    
    protected:
    
        binary_search_tree<tkey, tvalue> *_tree;
        
    public:
    
        explicit template_method_basics(binary_search_tree<tkey, tvalue> *tree);
        
    protected:
        
        std::stack<node **> find_path(tkey const &key) const noexcept;

        virtual void balance(std::stack<node **> &path);

    protected:
    
        [[nodiscard]] logger *get_logger() const noexcept final;

        [[nodiscard]] allocator *get_allocator() const noexcept final;
        
    };
    
    class insertion_template_method:
        public template_method_basics
    {
    
    private:
        
        binary_search_tree<tkey, tvalue>::insertion_of_existent_key_attempt_strategy _strategy;
    
    public:
        
        explicit insertion_template_method(binary_search_tree<tkey, tvalue> *tree, typename binary_search_tree<tkey, tvalue>::insertion_of_existent_key_attempt_strategy insertion_strategy);
        
    public:
        
        void insert(tkey const &key, tvalue const &value);
        
        void insert(tkey const &key, tvalue &&value);
    
        void set_insertion_strategy(typename binary_search_tree<tkey, tvalue>::insertion_of_existent_key_attempt_strategy insertion_strategy) noexcept;
        
    };
    
    class obtaining_template_method:
        public template_method_basics
    {
    
    public:
        
        explicit obtaining_template_method(binary_search_tree<tkey, tvalue> *tree);
    
    public:
        
        tvalue const &obtain(tkey const &key);

        std::vector<typename associative_container<tkey, tvalue>::key_value_pair> obtain_between(tkey const &lower_bound, tkey const &upper_bound, bool lower_bound_inclusive, bool upper_bound_inclusive);
        
    };
    
    class disposal_template_method:
        public template_method_basics
    {
    
    private:

        binary_search_tree<tkey, tvalue>::disposal_of_nonexistent_key_attempt_strategy _strategy;
    
    public:
        
        explicit disposal_template_method(binary_search_tree<tkey, tvalue> *tree, typename binary_search_tree<tkey, tvalue>::disposal_of_nonexistent_key_attempt_strategy disposal_strategy);
        
    public:
        
        tvalue dispose(tkey const &key);
        
        void set_disposal_strategy(typename binary_search_tree<tkey, tvalue>::disposal_of_nonexistent_key_attempt_strategy disposal_strategy) noexcept;
    
    protected:
        
        template<typename T>
        void swap(T &&one, T &&another) const noexcept;
    
    };
    
    // endregion template methods definition

protected:
    
    node *_root;
    
    insertion_template_method *_insertion_template;
    
    obtaining_template_method *_obtaining_template;
    
    disposal_template_method *_disposal_template;

protected:
    
    explicit binary_search_tree(typename binary_search_tree<tkey, tvalue>::insertion_template_method *insertion_template, typename binary_search_tree<tkey, tvalue>::obtaining_template_method *obtaining_template, typename binary_search_tree<tkey, tvalue>::disposal_template_method *disposal_template, std::function<int(tkey const &, tkey const &)>, allocator *allocator, logger *logger);

public:
    
    explicit binary_search_tree(std::function<int(tkey const &, tkey const &)> comparer = std::less<tkey>(), allocator *allocator = nullptr, logger *logger = nullptr, typename binary_search_tree<tkey, tvalue>::insertion_of_existent_key_attempt_strategy insertion_strategy = binary_search_tree<tkey, tvalue>::insertion_of_existent_key_attempt_strategy::throw_an_exception, typename binary_search_tree<tkey, tvalue>::disposal_of_nonexistent_key_attempt_strategy disposal_strategy = binary_search_tree<tkey, tvalue>::disposal_of_nonexistent_key_attempt_strategy::throw_an_exception);

private:

    inline std::string get_typename() const noexcept;

protected:

    virtual void clear_node(node *&subtree_root) const noexcept;

    virtual node *copy_node(node const *subtree_root) const;

    virtual void clear_tree(binary_search_tree<tkey, tvalue> &tree) noexcept;

    virtual void copy_tree(binary_search_tree<tkey, tvalue> const &tree);

    virtual void move_tree(binary_search_tree<tkey, tvalue> &tree) noexcept;

    inline virtual size_t get_node_size() const noexcept;

    inline virtual void call_node_constructor(node *raw_space, tkey const &key, tvalue const &value) const;

    inline virtual void call_node_constructor(node *raw_space, tkey const &key, tvalue &&value) const;

    inline virtual void call_iterator_data_constructor(iterator_data *raw_space, node *subtree_root, unsigned int depth) const;

    inline virtual size_t get_iterator_data_size() const noexcept;

public:
    
    binary_search_tree(binary_search_tree<tkey, tvalue> const &other);
    
    binary_search_tree(binary_search_tree<tkey, tvalue> &&other) noexcept;
    
    binary_search_tree<tkey, tvalue> &operator=(binary_search_tree<tkey, tvalue> const &other);
    
    binary_search_tree<tkey, tvalue> &operator=(binary_search_tree<tkey, tvalue> &&other) noexcept;
    
    ~binary_search_tree() override;

public:
    
    void insert(tkey const &key, tvalue const &value) final;
    
    void insert(tkey const &key, tvalue &&value) final;
    
    tvalue const &obtain(tkey const &key) final;
    
    std::vector<typename associative_container<tkey, tvalue>::key_value_pair> obtain_between(tkey const &lower_bound, tkey const &upper_bound, bool lower_bound_inclusive, bool upper_bound_inclusive) final;
    
    tvalue dispose(tkey const &key) final;
    
public:
    
    void set_insertion_strategy(typename binary_search_tree<tkey, tvalue>::insertion_of_existent_key_attempt_strategy insertion_strategy) noexcept;
    
    void set_removal_strategy(typename binary_search_tree<tkey, tvalue>::disposal_of_nonexistent_key_attempt_strategy disposal_strategy) noexcept;

public:
    
    // region iterators requests definition
    
    prefix_iterator begin_prefix() const noexcept;
    
    prefix_iterator end_prefix() const noexcept;
    
    prefix_const_iterator cbegin_prefix() const noexcept;
    
    prefix_const_iterator cend_prefix() const noexcept;
    
    prefix_reverse_iterator rbegin_prefix() const noexcept;
    
    prefix_reverse_iterator rend_prefix() const noexcept;
    
    prefix_const_reverse_iterator crbegin_prefix() const noexcept;
    
    prefix_const_reverse_iterator crend_prefix() const noexcept;
    
    infix_iterator begin_infix() const noexcept;
    
    infix_iterator end_infix() const noexcept;
    
    infix_const_iterator cbegin_infix() const noexcept;
    
    infix_const_iterator cend_infix() const noexcept;
    
    infix_reverse_iterator rbegin_infix() const noexcept;
    
    infix_reverse_iterator rend_infix() const noexcept;
    
    infix_const_reverse_iterator crbegin_infix() const noexcept;
    
    infix_const_reverse_iterator crend_infix() const noexcept;
    
    postfix_iterator begin_postfix() const noexcept;
    
    postfix_iterator end_postfix() const noexcept;
    
    postfix_const_iterator cbegin_postfix() const noexcept;
    
    postfix_const_iterator cend_postfix() const noexcept;
    
    postfix_reverse_iterator rbegin_postfix() const noexcept;
    
    postfix_reverse_iterator rend_postfix() const noexcept;
    
    postfix_const_reverse_iterator crbegin_postfix() const noexcept;
    
    postfix_const_reverse_iterator crend_postfix() const noexcept;
    
    // endregion iterators requests definition

protected:
    
    // region subtree rotations definition
    
    void small_left_rotation(typename binary_search_tree<tkey, tvalue>::node *&subtree_root, bool validate = true) const;
    
    void small_right_rotation(typename binary_search_tree<tkey, tvalue>::node *&subtree_root, bool validate = true) const;
    
    void big_left_rotation(typename binary_search_tree<tkey, tvalue>::node *&subtree_root, bool validate = true) const;
    
    void big_right_rotation(typename binary_search_tree<tkey, tvalue>::node *&subtree_root, bool validate = true) const;
    
    void double_left_rotation(typename binary_search_tree<tkey, tvalue>::node *&subtree_root, bool at_grandparent_first,bool validate = true) const;
    
    void double_right_rotation(typename binary_search_tree<tkey, tvalue>::node *&subtree_root, bool at_grandparent_first, bool validate = true) const;
    
    // endregion subtree rotations definition
    
};

// region binary_search_tree<tkey, tvalue>::node methods implementation

template<typename tkey, typename tvalue> 
binary_search_tree<tkey, tvalue>::node::node(tkey const &key, tvalue const &value):
    key(key), value(value), left_subtree(nullptr), right_subtree(nullptr) {}

template<typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::node::node(tkey const &key, tvalue &&value):
    key(key), value(std::move(value)), left_subtree(nullptr), right_subtree(nullptr) {}

// endregion binary_search_tree<tkey, tvalue>::node methods implementation

// region iterators implementation

// region iterator implementation

template<typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::iterator::iterator(binary_search_tree<tkey, tvalue> const *holder):
    _holder(const_cast<binary_search_tree<tkey, tvalue> *>(holder)) 
{

}

template<typename tkey, typename tvalue>
template<typename T>
bool binary_search_tree<tkey, tvalue>::iterator::check_equality(std::stack<T> const &one, std::stack<T> const &another) const noexcept
{
    if (one.empty() && another.empty())
    {
        return true;
    }
    if (one.size() ^ another.size())
    {
        return false;
    }
    return one.top() == another.top();
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::iterator_data *binary_search_tree<tkey, tvalue>::iterator::create_iterator_data() const
{
    iterator_data *output;
    try
    {
        output = reinterpret_cast<iterator_data *>(allocate_with_guard(get_iterator_data_size()));
    }
    catch(std::bad_alloc const &exception)
    {
        throw exception;
    }

    return output;
}

template<typename tkey, typename tvalue>
void binary_search_tree<tkey, tvalue>::iterator::iterator_destructor(typename binary_search_tree<tkey, tvalue>::iterator_data *&data, bool &flag) const noexcept
{
    if (flag)
    {
        allocator::destruct(data);
        flag = false;
    }

    deallocate_with_guard(data);
    data = nullptr;
}

template<typename tkey, typename tvalue>
template<typename T>
void binary_search_tree<tkey, tvalue>::iterator::iterator_copy_constructor(typename binary_search_tree<tkey, tvalue>::iterator_data *&this_data, std::stack<T> &this_state, bool &this_flag, typename binary_search_tree<tkey, tvalue>::iterator_data const *other_data, std::stack<T> const &other_state, bool other_flag) const
{
    try
    {
        this_data = create_iterator_data();
    }
    catch (std::bad_alloc const &exception)
    {
        throw exception;
    }
    
    if (other_flag && other_data != nullptr)
    {
        allocator::construct(this_data, *other_data);
        this_flag = true;
    }
    else
    {
        this_flag = false;
    }

    this_state = other_state;
}

template<typename tkey, typename tvalue>
template<typename T>
void binary_search_tree<tkey, tvalue>::iterator::iterator_copy_assignment(typename binary_search_tree<tkey, tvalue>::iterator_data *&this_data, std::stack<T> &this_state, bool &this_flag, typename binary_search_tree<tkey, tvalue>::iterator_data const *other_data, std::stack<T> const &other_state, bool other_flag) const
{
    allocator::destruct(this_data);
    deallocate_with_guard(this_data);
    this_data = nullptr;

    try
    {
        this_data = create_iterator_data();
    }
    catch (std::bad_alloc const &exception)
    {
        throw exception;
    }

    if (other_flag && other_data != nullptr)
    {
        allocator::construct(this_data, *other_data);
        this_flag = true;
    }
    else
    {
        this_flag = false;
    }

    this_state = other_state;
}

template<typename tkey, typename tvalue>
template<typename T>
void binary_search_tree<tkey, tvalue>::iterator::iterator_move_constructor(typename binary_search_tree<tkey, tvalue>::iterator_data *&this_data, std::stack<T> &this_state, bool &this_flag, typename binary_search_tree<tkey, tvalue>::iterator_data *&other_data, std::stack<T> &other_state, bool &other_flag) const noexcept
{
    this_data = std::exchange(other_data, nullptr);
    
    this_flag = other_flag;
    other_flag = false;

    this_state = std::move(other_state);
}

template<typename tkey, typename tvalue>
template<typename T>
void binary_search_tree<tkey, tvalue>::iterator::iterator_move_assignment(typename binary_search_tree<tkey, tvalue>::iterator_data *&this_data, std::stack<T> &this_state, bool &this_flag, typename binary_search_tree<tkey, tvalue>::iterator_data *&other_data, std::stack<T> &other_state, bool &other_flag) const noexcept
{
    allocator::destruct(this_data);
    deallocate_with_guard(this_data);
    this_data = nullptr;

    this_data = std::exchange(other_data, nullptr);
    
    this_flag = other_flag;
    other_flag = false;

    this_state = std::move(other_state);
}

template<typename tkey, typename tvalue>
void binary_search_tree<tkey, tvalue>::iterator::call_iterator_data_constructor(typename binary_search_tree<tkey, tvalue>::iterator_data *raw_space, node *subtree_root, unsigned int depth) const
{
    _holder->call_iterator_data_constructor(raw_space, subtree_root, depth);
}

template<typename tkey, typename tvalue>
size_t binary_search_tree<tkey, tvalue>::iterator::get_iterator_data_size() const noexcept
{
    return _holder->get_iterator_data_size();
}

template<typename tkey, typename tvalue>
allocator *binary_search_tree<tkey, tvalue>::iterator::get_allocator() const noexcept
{
    return _holder->get_allocator();
}

// region iterator data implementation

template<typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::iterator_data::iterator_data(unsigned int depth, tkey const &key, tvalue const &value):
    depth(depth), key(key), value(value) 
{

}

template<typename tkey, typename tvalue>
tkey const &binary_search_tree<tkey, tvalue>::iterator_data::get_key() const noexcept
{
    return key;
}

template<typename tkey, typename tvalue>
tvalue const &binary_search_tree<tkey, tvalue>::iterator_data::get_value() const noexcept
{
    return value;
}

// endregion iterator data implementation

// region prefix_iterator implementation

template<typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::prefix_iterator::prefix_iterator(typename binary_search_tree<tkey, tvalue>::node *subtree_root, binary_search_tree<tkey, tvalue> const *holder):
    iterator(holder), _is_data_constructed(false)
{    
    try
    {
        _data = this->create_iterator_data();
    }
    catch (std::bad_alloc const &exception)
    {
        throw exception;
    }
    
    if (subtree_root != nullptr)
    {
        _state.push(std::pair<node *, unsigned int>(subtree_root, 0));
    }
}

template<typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::prefix_iterator::~prefix_iterator() noexcept
{
    this->iterator_destructor(_data, _is_data_constructed);
}

template<typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::prefix_iterator::prefix_iterator(prefix_iterator const &other)
{    
    this->iterator_copy_constructor(_data, _state, _is_data_constructed, other._data, other._state, other._is_data_constructed);
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::prefix_iterator &binary_search_tree<tkey, tvalue>::prefix_iterator::operator=(prefix_iterator const &other)
{
    if (this == &other)
    {
        return *this;
    }

    this->iterator_copy_assignment(_data, _state, _is_data_constructed, other._data, other._state, other._is_data_constructed);

    return *this;
}

template<typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::prefix_iterator::prefix_iterator(prefix_iterator &&other) noexcept:
    iterator(std::exchange(other._holder, nullptr))
{
    this->iterator_move_constructor(_data, _state, _is_data_constructed, other._data, other._state, other._is_data_constructed);
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::prefix_iterator &binary_search_tree<tkey, tvalue>::prefix_iterator::operator=(prefix_iterator &&other) noexcept
{
    if (this == &other)
    {
        return *this;
    }
    
    this->iterator_move_assignment(_data, _state, _is_data_constructed, other._data, other._state, other._is_data_constructed);
    
    return *this;
}

template<typename tkey, typename tvalue>
bool binary_search_tree<tkey, tvalue>::prefix_iterator::operator==(typename binary_search_tree<tkey, tvalue>::prefix_iterator const &other) const noexcept
{
    return this->check_equality(_state, other._state);
}

template<typename tkey, typename tvalue>
bool binary_search_tree<tkey, tvalue>::prefix_iterator::operator!=(typename binary_search_tree<tkey, tvalue>::prefix_iterator const &other) const noexcept
{
    return !(*this == other);
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::prefix_iterator &binary_search_tree<tkey, tvalue>::prefix_iterator::operator++()
{
    if (_state.empty())
    {
        return *this;
    }

    std::pair<node *, unsigned int> current = _state.top();
    _state.pop();

    if (current.first->right_subtree)
    {
        _state.push(std::pair<node *, unsigned int>(current.first->right_subtree, current.second + 1));
    }
    if (current.first->left_subtree)
    {
        _state.push(std::pair<node *, unsigned int>(current.first->left_subtree, current.second + 1));
    }

    return *this;
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::prefix_iterator const binary_search_tree<tkey, tvalue>::prefix_iterator::operator++(int not_used)
{
    auto it = *this;
    ++*this;
    return it;
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::iterator_data *binary_search_tree<tkey, tvalue>::prefix_iterator::operator*() const
{
    if (_state.empty())
    {
        throw std::logic_error("Cannot get data from iterator");
    }

    if (_is_data_constructed)
    {
        allocator::destruct(_data);
    }
    this->call_iterator_data_constructor(_data, _state.top().first, _state.top().second);

    return _data;
}

// endregion prefix_iterator implementation

// region prefix_const_iterator implementation

template<typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::prefix_const_iterator::prefix_const_iterator(typename binary_search_tree<tkey, tvalue>::node *subtree_root, binary_search_tree<tkey, tvalue> const *holder):
    _it(subtree_root, holder) 
{

}

template<typename tkey,typename tvalue>
bool binary_search_tree<tkey, tvalue>::prefix_const_iterator::operator==(typename binary_search_tree<tkey, tvalue>::prefix_const_iterator const &other) const noexcept
{
    return _it == other._it;
}

template<typename tkey,typename tvalue>
bool binary_search_tree<tkey, tvalue>::prefix_const_iterator::operator!=(typename binary_search_tree<tkey, tvalue>::prefix_const_iterator const &other) const noexcept
{
    return _it != other._it;
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::prefix_const_iterator &binary_search_tree<tkey, tvalue>::prefix_const_iterator::operator++()
{
    ++_it;
    return *this;
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::prefix_const_iterator const binary_search_tree<tkey, tvalue>::prefix_const_iterator::operator++(int not_used)
{
    auto it = *this;
    ++*this;
    return it;
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::iterator_data const *binary_search_tree<tkey, tvalue>::prefix_const_iterator::operator*() const
{
    return *_it;
}

// endregion prefix_const_iterator implementation

// region prefix_reverse_iterator implementation

template<typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::prefix_reverse_iterator::prefix_reverse_iterator(typename binary_search_tree<tkey, tvalue>::node *subtree_root, binary_search_tree<tkey, tvalue> const *holder):
    iterator(holder), _is_data_constructed(false)
{    
    try
    {
        _data = this->create_iterator_data();
    }
    catch (std::bad_alloc const &exception)
    {
        throw exception;
    }
    
    if (subtree_root != nullptr)
    {
        _state.push(std::pair<node *, unsigned int>(subtree_root, 0));
    }
}

template<typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::prefix_reverse_iterator::~prefix_reverse_iterator() noexcept
{
    this->iterator_destructor(_data, _is_data_constructed);
}

template<typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::prefix_reverse_iterator::prefix_reverse_iterator(prefix_reverse_iterator const &other)
{    
    this->iterator_copy_constructor(_data, _state, _is_data_constructed, other._data, other._state, other._is_data_constructed);
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::prefix_reverse_iterator &binary_search_tree<tkey, tvalue>::prefix_reverse_iterator::operator=(prefix_reverse_iterator const &other)
{
    if (this == &other)
    {
        return *this;
    }

    this->iterator_copy_assignment(_data, _state, _is_data_constructed, other._data, other._state, other._is_data_constructed);

    return *this;
}

template<typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::prefix_reverse_iterator::prefix_reverse_iterator(prefix_reverse_iterator &&other) noexcept
{
    this->iterator_move_constructor(_data, _state, _is_data_constructed, other._data, other._state, other._is_data_constructed);
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::prefix_reverse_iterator &binary_search_tree<tkey, tvalue>::prefix_reverse_iterator::operator=(prefix_reverse_iterator &&other) noexcept
{
    if (this == &other)
    {
        return *this;
    }
    
    this->iterator_move_assignment(_data, _state, _is_data_constructed, other._data, other._state, other._is_data_constructed);  

    return *this;
}

template<typename tkey, typename tvalue>
bool binary_search_tree<tkey, tvalue>::prefix_reverse_iterator::operator==(typename binary_search_tree<tkey, tvalue>::prefix_reverse_iterator const &other) const noexcept
{
    return this->check_equality(_state, other._state);
}

template<typename tkey, typename tvalue>
bool binary_search_tree<tkey, tvalue>::prefix_reverse_iterator::operator!=(typename binary_search_tree<tkey, tvalue>::prefix_reverse_iterator const &other) const noexcept
{
    return !(*this == other);
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::prefix_reverse_iterator &binary_search_tree<tkey, tvalue>::prefix_reverse_iterator::operator++()
{
    if (_state.empty())
    {
        return *this;
    }

    std::pair<node *, unsigned int> current = _state.top();
    _state.pop();

    if (current.first->left_subtree)
    {
        _state.push(std::pair<node *, unsigned int>(current.first->left_subtree, current.second + 1));
    }
    if (current.first->right_subtree)
    {
        _state.push(std::pair<node *, unsigned int>(current.first->right_subtree, current.second + 1));
    }

    return *this;
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::prefix_reverse_iterator const binary_search_tree<tkey, tvalue>::prefix_reverse_iterator::operator++(int not_used)
{
    auto it = *this;
    ++*this;
    return it;
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::iterator_data *binary_search_tree<tkey, tvalue>::prefix_reverse_iterator::operator*() const
{
    if (_state.empty())
    {
        throw std::logic_error("Cannot get data from iterator");
    }

    if (_is_data_constructed)
    {
        allocator::destruct(_data);
    }

    this->call_iterator_data_constructor(_data, _state.top().first, _state.top().second);

    return _data;
}

// endregion prefix_reverse_iterator implementation

// region prefix_const_reverse_iterator implementation

template<typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::prefix_const_reverse_iterator::prefix_const_reverse_iterator(typename binary_search_tree<tkey, tvalue>::node *subtree_root, binary_search_tree<tkey, tvalue> const *holder):
    _it(subtree_root, holder) 
{

}

template<typename tkey, typename tvalue>
bool binary_search_tree<tkey, tvalue>::prefix_const_reverse_iterator::operator==(typename binary_search_tree<tkey, tvalue>::prefix_const_reverse_iterator const &other) const noexcept
{
    return _it == other._it;
}

template<typename tkey, typename tvalue>
bool binary_search_tree<tkey, tvalue>::prefix_const_reverse_iterator::operator!=(typename binary_search_tree<tkey, tvalue>::prefix_const_reverse_iterator const &other) const noexcept
{
    return _it != other._it;
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::prefix_const_reverse_iterator &binary_search_tree<tkey, tvalue>::prefix_const_reverse_iterator::operator++()
{
    ++_it;
    return *this;
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::prefix_const_reverse_iterator const binary_search_tree<tkey, tvalue>::prefix_const_reverse_iterator::operator++(int not_used)
{
    auto it = *this;
    ++*this;
    return it;
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::iterator_data const *binary_search_tree<tkey, tvalue>::prefix_const_reverse_iterator::operator*() const
{
    return *_it;
}

// endregion prefix_const_reverse_iterator implementation

// region infix_iterator implementation

template<typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::infix_iterator::infix_iterator(typename binary_search_tree<tkey, tvalue>::node *subtree_root, binary_search_tree<tkey, tvalue> const *holder):
    iterator(holder), _is_data_constructed(false)
{    
    try
    {
        _data = this->create_iterator_data();
    }
    catch (std::bad_alloc const &exception)
    {
        throw exception;
    }
    
    while (subtree_root != nullptr)
    {
        _state.push(subtree_root);
        subtree_root = subtree_root->left_subtree;
    }
}

template<typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::infix_iterator::infix_iterator(binary_search_tree<tkey, tvalue> const *holder, std::stack<node *> state):
    iterator(holder), _is_data_constructed(false), _state(state) 
{
    try
    {
        _data = this->create_iterator_data();
    }
    catch (std::bad_alloc const &exception)
    {
        throw exception;
    }
}

template<typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::infix_iterator::~infix_iterator() noexcept
{
    this->iterator_destructor(_data, _is_data_constructed);
}

template<typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::infix_iterator::infix_iterator(infix_iterator const &other)
{    
    this->iterator_copy_constructor(_data, _state, _is_data_constructed, other._data, other._state, other._is_data_constructed);
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::infix_iterator &binary_search_tree<tkey, tvalue>::infix_iterator::operator=(infix_iterator const &other)
{
    if (this == &other)
    {
        return *this;
    }

    this->iterator_copy_assignment(_data, _state, _is_data_constructed, other._data, other._state, other._is_data_constructed);

    return *this;
}

template<typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::infix_iterator::infix_iterator(infix_iterator &&other) noexcept:
    iterator(std::exchange(other._holder, nullptr))
{
    this->iterator_move_constructor(_data, _state, _is_data_constructed, other._data, other._state, other._is_data_constructed);
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::infix_iterator &binary_search_tree<tkey, tvalue>::infix_iterator::operator=(infix_iterator &&other) noexcept
{
    if (this == &other)
    {
        return *this;
    }
    
    this->iterator_move_assignment(_data, _state, _is_data_constructed, other._data, other._state, other._is_data_constructed);  

    return *this;
}

template<typename tkey, typename tvalue>
bool binary_search_tree<tkey, tvalue>::infix_iterator::operator==(typename binary_search_tree<tkey, tvalue>::infix_iterator const &other) const noexcept
{
    return this->check_equality(_state, other._state);
}

template<typename tkey, typename tvalue>
bool binary_search_tree<tkey, tvalue>::infix_iterator::operator!=(typename binary_search_tree<tkey, tvalue>::infix_iterator const &other) const noexcept
{
    return !(*this == other);
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::infix_iterator &binary_search_tree<tkey, tvalue>::infix_iterator::operator++()
{
    if (_state.empty())
    {
        return *this;
    }

    if (_state.top()->right_subtree != nullptr)
    {
        _state.push(_state.top()->right_subtree);
        while (_state.top()->left_subtree != nullptr)
        {
            _state.push(_state.top()->left_subtree);
        }
    }
    else
    {
        node *current = nullptr;
        do
        {
            current = _state.top();
            _state.pop();
        
            if (_state.empty() || _state.top()->left_subtree == current)
            {
                return *this;
            }
        } 
        while (true);
        
    }
    return *this;
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::infix_iterator const binary_search_tree<tkey, tvalue>::infix_iterator::operator++(int not_used)
{
    auto it = *this;
    ++*this;
    return it;
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::iterator_data *binary_search_tree<tkey, tvalue>::infix_iterator::operator*() const
{
    if (_state.empty())
    {
        throw std::logic_error("Cannot get data from iterator");
    }

    if (_is_data_constructed)
    {
        allocator::destruct(_data);
    }

    this->call_iterator_data_constructor(_data, _state.top() ,_state.size() - 1);

    return _data;
}

// endregion infix_iterator implementation

// region infix_const_iterator implementation

template<typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::infix_const_iterator::infix_const_iterator(typename binary_search_tree<tkey, tvalue>::node *subtree_root, binary_search_tree<tkey, tvalue> const *holder):
    _it(subtree_root, holder) 
{

}

template<typename tkey, typename tvalue>
bool binary_search_tree<tkey, tvalue>::infix_const_iterator::operator==(typename binary_search_tree<tkey, tvalue>::infix_const_iterator const &other) const noexcept
{
    return _it == other._it;
}

template<typename tkey, typename tvalue>
bool binary_search_tree<tkey, tvalue>::infix_const_iterator::operator!=(typename binary_search_tree<tkey, tvalue>::infix_const_iterator const &other) const noexcept
{
    return _it != other._it;
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::infix_const_iterator &binary_search_tree<tkey, tvalue>::infix_const_iterator::operator++()
{
    ++_it;
    return *this;
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::infix_const_iterator const binary_search_tree<tkey, tvalue>::infix_const_iterator::operator++(int not_used)
{
    auto it = *this;
    ++*this;
    return it;
}

template<typename tkey,typename tvalue>
typename binary_search_tree<tkey, tvalue>::iterator_data const *binary_search_tree<tkey, tvalue>::infix_const_iterator::operator*() const
{
    return *_it;
}

// endregion infix_const_iterator implementation

// region infix_reverse_iterator implementation

template<typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::infix_reverse_iterator::infix_reverse_iterator(typename binary_search_tree<tkey, tvalue>::node *subtree_root, binary_search_tree<tkey, tvalue> const *holder):
    iterator(holder), _is_data_constructed(false)
{
    try
    {
        _data = this->create_iterator_data();
    }
    catch (std::bad_alloc const &exception)
    {
        throw exception;
    }
    
    while (subtree_root != nullptr)
    {
        _state.push(subtree_root);
        subtree_root = subtree_root->right_subtree;
    }
}

template<typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::infix_reverse_iterator::~infix_reverse_iterator() noexcept
{
    this->iterator_destructor(_data, _is_data_constructed);
}

template<typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::infix_reverse_iterator::infix_reverse_iterator(infix_reverse_iterator const &other)
{    
    this->iterator_copy_constructor(_data, _state, _is_data_constructed, other._data, other._state, other._is_data_constructed);
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::infix_reverse_iterator &binary_search_tree<tkey, tvalue>::infix_reverse_iterator::operator=(infix_reverse_iterator const &other)
{
    if (this == &other)
    {
        return *this;
    }

    this->iterator_copy_assignment(_data, _state, _is_data_constructed, other._data, other._state, other._is_data_constructed);

    return *this;
}

template<typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::infix_reverse_iterator::infix_reverse_iterator(infix_reverse_iterator &&other) noexcept
{
    this->iterator_move_constructor(_data, _state, _is_data_constructed, other._data, other._state, other._is_data_constructed);
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::infix_reverse_iterator &binary_search_tree<tkey, tvalue>::infix_reverse_iterator::operator=(infix_reverse_iterator &&other) noexcept
{
    if (this == &other)
    {
        return *this;
    }
    
    this->iterator_move_assignment(_data, _state, _is_data_constructed, other._data, other._state, other._is_data_constructed);  

    return *this;
}

template<typename tkey, typename tvalue>
bool binary_search_tree<tkey, tvalue>::infix_reverse_iterator::operator==(typename binary_search_tree<tkey, tvalue>::infix_reverse_iterator const &other) const noexcept
{
    return this->check_equality(_state, other._state);
}

template<typename tkey,typename tvalue>
bool binary_search_tree<tkey, tvalue>::infix_reverse_iterator::operator!=(typename binary_search_tree<tkey, tvalue>::infix_reverse_iterator const &other) const noexcept
{
    return !(*this == other);
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::infix_reverse_iterator &binary_search_tree<tkey, tvalue>::infix_reverse_iterator::operator++()
{
    if (_state.empty())
    {
        return *this;
    }

    if (_state.top()->left_subtree != nullptr)
    {
        _state.push(_state.top()->left_subtree);
        while (_state.top()->right_subtree != nullptr)
        {
            _state.push(_state.top()->right_subtree);
        }
    }
    else
    {
        node *current = nullptr;
        do
        {
            current = _state.top();
            _state.pop();
        
            if (_state.empty() || _state.top()->right_subtree == current)
            {
                return *this;
            }
        } 
        while (true);
        
    }
}

template<typename tkey,typename tvalue>
typename binary_search_tree<tkey, tvalue>::infix_reverse_iterator const binary_search_tree<tkey, tvalue>::infix_reverse_iterator::operator++(int not_used)
{
    auto it = *this;
    ++*this;
    return it;
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::iterator_data *binary_search_tree<tkey, tvalue>::infix_reverse_iterator::operator*() const
{
    if (_state.empty())
    {
        throw std::logic_error("Cannot get data from iterator");
    }

    if (_is_data_constructed)
    {
        allocator::destruct(_data);
    }

    this->call_iterator_data_constructor(_data, _state.top() ,_state.size() - 1);

    return _data;
}

// endregion infix_reverse_iterator implementation

// region infix_const_reverse_iterator implementation

template<typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::infix_const_reverse_iterator::infix_const_reverse_iterator(typename binary_search_tree<tkey, tvalue>::node *subtree_root, binary_search_tree<tkey, tvalue> const *holder):
    _it(subtree_root, holder) 
{

}

template<typename tkey, typename tvalue>
bool binary_search_tree<tkey, tvalue>::infix_const_reverse_iterator::operator==(typename binary_search_tree<tkey, tvalue>::infix_const_reverse_iterator const &other) const noexcept
{
    return _it == other._it;
}

template<typename tkey, typename tvalue>
bool binary_search_tree<tkey, tvalue>::infix_const_reverse_iterator::operator!=(typename binary_search_tree<tkey, tvalue>::infix_const_reverse_iterator const &other) const noexcept
{
    return _it != other._it;
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::infix_const_reverse_iterator &binary_search_tree<tkey, tvalue>::infix_const_reverse_iterator::operator++()
{
    ++_it;
    return *this;
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::infix_const_reverse_iterator const binary_search_tree<tkey, tvalue>::infix_const_reverse_iterator::operator++(int not_used)
{
    auto it = *this;
    ++*this;
    return it;
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::iterator_data const *binary_search_tree<tkey, tvalue>::infix_const_reverse_iterator::operator*() const
{
    return *_it;
}

// endregion infix_const_reverse_iterator implementation

// region postfix_iterator implementation

template<typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::postfix_iterator::postfix_iterator(typename binary_search_tree<tkey, tvalue>::node *subtree_root, binary_search_tree<tkey, tvalue> const *holder):
    iterator(holder), _is_data_constructed(false) 
{
    try
    {
        _data = this->create_iterator_data();
    }
    catch (std::bad_alloc const &exception)
    {
        throw exception;
    }
    
    iterate(subtree_root, 0);
}

template<typename tkey, typename tvalue>
void binary_search_tree<tkey, tvalue>::postfix_iterator::iterate(typename binary_search_tree<tkey, tvalue>::node *subtree_root, unsigned int depth)
{    
    while (subtree_root != nullptr)
    {
        _state.push(std::pair<node *, unsigned int>(subtree_root, depth));
        subtree_root = subtree_root->left_subtree != nullptr
            ? subtree_root->left_subtree
            : subtree_root->right_subtree;
        ++depth;
    }
}

template<typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::postfix_iterator::~postfix_iterator() noexcept
{
    this->iterator_destructor(_data, _is_data_constructed);
}

template<typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::postfix_iterator::postfix_iterator(postfix_iterator const &other)
{    
    this->iterator_copy_constructor(_data, _state, _is_data_constructed, other._data, other._state, other._is_data_constructed);
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::postfix_iterator &binary_search_tree<tkey, tvalue>::postfix_iterator::operator=(postfix_iterator const &other)
{
    if (this == &other)
    {
        return *this;
    }

    this->iterator_copy_assignment(_data, _state, _is_data_constructed, other._data, other._state, other._is_data_constructed);

    return *this;
}

template<typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::postfix_iterator::postfix_iterator(postfix_iterator &&other) noexcept:
    iterator(std::exchange(other._holder, nullptr))
{
    this->iterator_move_constructor(_data, _state, _is_data_constructed, other._data, other._state, other._is_data_constructed);
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::postfix_iterator &binary_search_tree<tkey, tvalue>::postfix_iterator::operator=(postfix_iterator &&other) noexcept
{
    if (this == &other)
    {
        return *this;
    }
    
    this->iterator_move_assignment(_data, _state, _is_data_constructed, other._data, other._state, other._is_data_constructed);  

    return *this;
}

template<typename tkey, typename tvalue>
bool binary_search_tree<tkey, tvalue>::postfix_iterator::operator==(typename binary_search_tree<tkey, tvalue>::postfix_iterator const &other) const noexcept
{
    return this->check_equality(_state, other._state);
}

template<typename tkey, typename tvalue>
bool binary_search_tree<tkey, tvalue>::postfix_iterator::operator!=(typename binary_search_tree<tkey, tvalue>::postfix_iterator const &other) const noexcept
{
    return !(*this == other);
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::postfix_iterator &binary_search_tree<tkey, tvalue>::postfix_iterator::operator++()
{
    if (_state.empty())
    {
        return *this;
    }
    
    node *current = _state.top().first;
    _state.pop();

    if (_state.empty())
    {
        return *this;
    }
    
    node *parent = _state.top().first;
    if (parent->right_subtree != nullptr && current == parent->left_subtree)
    {
        iterate(parent->right_subtree, _state.top().second + 1);
    }

    return *this;
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::postfix_iterator const binary_search_tree<tkey, tvalue>::postfix_iterator::operator++(int not_used)
{
    auto it = *this;
    ++*this;
    return it;
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::iterator_data *binary_search_tree<tkey, tvalue>::postfix_iterator::operator*() const
{
    if (_state.empty())
    {
        throw std::logic_error("Cannot get data from iterator");
    }

    if (_is_data_constructed)
    {
        allocator::destruct(_data);
    }

    this->call_iterator_data_constructor(_data, _state.top().first ,_state.top().second);

    return _data;
}

// endregion postfix_iterator implementation

// region postfix_const_iterator implementation

template<typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::postfix_const_iterator::postfix_const_iterator(typename binary_search_tree<tkey, tvalue>::node *subtree_root, binary_search_tree<tkey, tvalue> const *holder):
    _it(subtree_root, holder)
{
    
}

template<typename tkey, typename tvalue>
bool binary_search_tree<tkey, tvalue>::postfix_const_iterator::operator==(typename binary_search_tree<tkey, tvalue>::postfix_const_iterator const &other) const noexcept
{
    return _it == other._it;
}

template<typename tkey, typename tvalue>
bool binary_search_tree<tkey, tvalue>::postfix_const_iterator::operator!=(typename binary_search_tree<tkey, tvalue>::postfix_const_iterator const &other) const noexcept
{
    return _it != other._it;
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::postfix_const_iterator &binary_search_tree<tkey, tvalue>::postfix_const_iterator::operator++()
{
    ++_it;
    return *this;
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::postfix_const_iterator const binary_search_tree<tkey, tvalue>::postfix_const_iterator::operator++(int not_used)
{
    auto it = *this;
    ++*this;
    return it;
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::iterator_data const *binary_search_tree<tkey, tvalue>::postfix_const_iterator::operator*() const
{
    return *_it;
}

// endregion postfix_const_iterator implementation

// region postfix_reverse_iterator implementation

template<typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::postfix_reverse_iterator::postfix_reverse_iterator(typename binary_search_tree<tkey, tvalue>::node *subtree_root, binary_search_tree<tkey, tvalue> const *holder):
    iterator(holder), _is_data_constructed(false)
{
    try
    {
        _data = this->create_iterator_data();
    }
    catch (std::bad_alloc const &exception)
    {
        throw exception;
    }
    
    iterate(subtree_root, 0);
}

template<typename tkey, typename tvalue>
void binary_search_tree<tkey, tvalue>::postfix_reverse_iterator::iterate(typename binary_search_tree<tkey, tvalue>::node *subtree_root, unsigned int depth)
{
    while (subtree_root != nullptr)
    {
        _state.push(std::pair<node *, unsigned int>(subtree_root, depth));
        subtree_root = subtree_root->right_subtree != nullptr
            ? subtree_root->right_subtree
            : subtree_root->left_subtree;
        ++depth;
    }
}

template<typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::postfix_reverse_iterator::~postfix_reverse_iterator() noexcept
{
    this->iterator_destructor(_data, _is_data_constructed);
}

template<typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::postfix_reverse_iterator::postfix_reverse_iterator(postfix_reverse_iterator const &other)
{    
    this->iterator_copy_constructor(_data, _state, _is_data_constructed, other._data, other._state, other._is_data_constructed);
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::postfix_reverse_iterator &binary_search_tree<tkey, tvalue>::postfix_reverse_iterator::operator=(postfix_reverse_iterator const &other)
{
    if (this == &other)
    {
        return *this;
    }

    this->iterator_copy_assignment(_data, _state, _is_data_constructed, other._data, other._state, other._is_data_constructed);

    return *this;
}

template<typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::postfix_reverse_iterator::postfix_reverse_iterator(postfix_reverse_iterator &&other) noexcept
{
    this->iterator_move_constructor(_data, _state, _is_data_constructed, other._data, other._state, other._is_data_constructed);
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::postfix_reverse_iterator &binary_search_tree<tkey, tvalue>::postfix_reverse_iterator::operator=(postfix_reverse_iterator &&other) noexcept
{
    if (this == &other)
    {
        return *this;
    }
    
    this->iterator_move_assignment(_data, _state, _is_data_constructed, other._data, other._state, other._is_data_constructed);  

    return *this;
}

template<typename tkey, typename tvalue>
bool binary_search_tree<tkey, tvalue>::postfix_reverse_iterator::operator==(typename binary_search_tree<tkey, tvalue>::postfix_reverse_iterator const &other) const noexcept
{
    return this->check_equality(_state, other._state);
}

template<typename tkey, typename tvalue>
bool binary_search_tree<tkey, tvalue>::postfix_reverse_iterator::operator!=(typename binary_search_tree<tkey, tvalue>::postfix_reverse_iterator const &other) const noexcept
{
    return !(*this == other);
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::postfix_reverse_iterator &binary_search_tree<tkey, tvalue>::postfix_reverse_iterator::operator++()
{
    if (_state.empty())
    {
        return *this;
    }
    
    node *current = _state.top().first;
    _state.pop();

    if (_state.empty())
    {
        return *this;
    }
    
    node *parent = _state.top().first;
    if (parent->left_subtree != nullptr && current == parent->right_subtree)
    {
        iterate(parent->left_subtree, _state.top().second + 1);
    }

    return *this;
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::postfix_reverse_iterator const binary_search_tree<tkey, tvalue>::postfix_reverse_iterator::operator++(int not_used)
{
    auto it = *this;
    ++*this;
    return it;
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::iterator_data *binary_search_tree<tkey, tvalue>::postfix_reverse_iterator::operator*() const
{
    if (_state.empty())
    {
        throw std::logic_error("Cannot get data from iterator");
    }

    if (_is_data_constructed)
    {
        allocator::destruct(_data);
    }

    this->call_iterator_data_constructor(_data, _state.top().first ,_state.top().second);
    
    return _data;
}

// endregion postfix_reverse_iterator implementation

// region postfix_const_reverse_iterator implementation

template<typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::postfix_const_reverse_iterator::postfix_const_reverse_iterator(typename binary_search_tree<tkey, tvalue>::node *subtree_root, binary_search_tree<tkey, tvalue> const *holder):
    _it(subtree_root, holder) 
{

}

template<typename tkey, typename tvalue>
bool binary_search_tree<tkey, tvalue>::postfix_const_reverse_iterator::operator==(typename binary_search_tree<tkey, tvalue>::postfix_const_reverse_iterator const &other) const noexcept
{
    return _it == other._it;
}

template<typename tkey, typename tvalue>
bool binary_search_tree<tkey, tvalue>::postfix_const_reverse_iterator::operator!=(typename binary_search_tree<tkey, tvalue>::postfix_const_reverse_iterator const &other) const noexcept
{
    return _it != other._it;
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::postfix_const_reverse_iterator &binary_search_tree<tkey, tvalue>::postfix_const_reverse_iterator::operator++()
{
    ++_it;
    return *this;
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::postfix_const_reverse_iterator const binary_search_tree<tkey, tvalue>::postfix_const_reverse_iterator::operator++(int not_used)
{
    auto it = *this;
    ++*this;
    return it;
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::iterator_data const *binary_search_tree<tkey, tvalue>::postfix_const_reverse_iterator::operator*() const
{
    return *_it;
}

// endregion postfix_const_reverse_iterator implementation

// endregion iterators implementation

// region target operations associated exception types implementation

template<typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::insertion_of_existent_key_attempt_exception::insertion_of_existent_key_attempt_exception(tkey const &key):
    std::logic_error("Attempt to insert already existing key inside the tree.")
{

}

template<typename tkey, typename tvalue>
tkey const &binary_search_tree<tkey, tvalue>::insertion_of_existent_key_attempt_exception::get_key() const noexcept
{
    return _key;
}

template<typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::obtaining_of_nonexistent_key_attempt_exception::obtaining_of_nonexistent_key_attempt_exception(tkey const &key):
    std::logic_error("Attempt to obtain a value by non-existing key from the tree.")
{

}

template<typename tkey, typename tvalue>
tkey const &binary_search_tree<tkey, tvalue>::obtaining_of_nonexistent_key_attempt_exception::get_key() const noexcept
{
    return _key;
}

template<typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::disposal_of_nonexistent_key_attempt_exception::disposal_of_nonexistent_key_attempt_exception(tkey const &key):
    std::logic_error("Attempt to dispose a value by non-existing key from the tree.")
{

}

template<typename tkey, typename tvalue>
tkey const &binary_search_tree<tkey, tvalue>::disposal_of_nonexistent_key_attempt_exception::get_key() const noexcept
{
    return _key;
}

// endregion target operations associated exception types implementation

// region template methods implementation

// region binary_search_tree<tkey, tvalue>::template_method_basics implementation

template<typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::template_method_basics::template_method_basics(binary_search_tree<tkey, tvalue> *tree):
    _tree(tree) 
{

}

template<typename tkey, typename tvalue>
std::stack<typename binary_search_tree<tkey, tvalue>::node **> binary_search_tree<tkey, tvalue>::template_method_basics::find_path(tkey const &key) const noexcept
{
    std::stack<node **> result_path;

    node **path_finder = &(_tree->_root);
    auto const &comparer = _tree->_keys_comparer;

    while (true)
    {
        result_path.push(path_finder);

        if (*path_finder == nullptr)
        {
            break;
        }

        auto comparison_result = comparer(key, (*path_finder)->key);
        if (comparison_result == 0)
        {
            break;
        }

        path_finder = comparison_result < 0
            ? &((*path_finder)->left_subtree)
            : &((*path_finder)->right_subtree);
    }

    return result_path;
}

template<typename tkey, typename tvalue>
void binary_search_tree<tkey, tvalue>::template_method_basics::balance(std::stack<node **> &path)
{

}

template<typename tkey, typename tvalue>
[[nodiscard]] inline logger *binary_search_tree<tkey, tvalue>::template_method_basics::get_logger() const noexcept
{
    return _tree->get_logger();
}

template<typename tkey, typename tvalue>
[[nodiscard]] inline allocator *binary_search_tree<tkey, tvalue>::template_method_basics::get_allocator() const noexcept
{
    return _tree->get_allocator();
}

// endregion binary_search_tree<tkey, tvalue>::template_method_basics implementation

// region search_tree<tkey, tvalue>::insertion_template_method implementation

template<typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::insertion_template_method::insertion_template_method(binary_search_tree<tkey, tvalue> *tree, typename binary_search_tree<tkey, tvalue>::insertion_of_existent_key_attempt_strategy insertion_strategy):
    binary_search_tree<tkey, tvalue>::template_method_basics::template_method_basics(tree), _strategy(insertion_strategy) 
{

}

template<typename tkey, typename tvalue>
void binary_search_tree<tkey, tvalue>::insertion_template_method::insert(tkey const &key, tvalue const &value)
{
    auto path = this->find_path(key);
    if (*(path.top()) != nullptr)
    {
        switch (_strategy)
        {
            case insertion_of_existent_key_attempt_strategy::throw_an_exception:
                throw insertion_of_existent_key_attempt_exception(key);
            case insertion_of_existent_key_attempt_strategy::update_value:
                (*(path.top()))->value = value;
                break;
        }

        return;
    }

    this->_tree->call_node_constructor(*(path.top()) = reinterpret_cast<node *>(allocator_guardant::allocate_with_guard(this->_tree->get_node_size())), key, value);

    this->balance(path);
}

template<typename tkey, typename tvalue>
void binary_search_tree<tkey, tvalue>::insertion_template_method::insert(tkey const &key, tvalue &&value)
{
    auto path = this->find_path(key);
    if (*(path.top()) != nullptr)
    {
        switch (_strategy)
        {
            case insertion_of_existent_key_attempt_strategy::throw_an_exception:
                throw insertion_of_existent_key_attempt_exception(key);
            case insertion_of_existent_key_attempt_strategy::update_value:
                (*(path.top()))->value = std::move(value);
                break;
        }

        return;
    }

    *(path.top()) = reinterpret_cast<node *>(allocator_guardant::allocate_with_guard(this->_tree->get_node_size()));

    this->_tree->call_node_constructor(*(path.top()), key, std::move(value));

    this->balance(path);
}

template<typename tkey, typename tvalue>
void binary_search_tree<tkey, tvalue>::insertion_template_method::set_insertion_strategy(typename binary_search_tree<tkey, tvalue>::insertion_of_existent_key_attempt_strategy insertion_strategy) noexcept
{
    _strategy = insertion_strategy;
}

// endregion search_tree<tkey, tvalue>::insertion_template_method implementation

// region search_tree<tkey, tvalue>::obtaining_template_method implementation

template<typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::obtaining_template_method::obtaining_template_method(binary_search_tree<tkey, tvalue> *tree):
    binary_search_tree<tkey, tvalue>::template_method_basics::template_method_basics(tree)
{

}

template<typename tkey, typename tvalue>
tvalue const &binary_search_tree<tkey, tvalue>::obtaining_template_method::obtain(tkey const &key)
{
    auto path = this->find_path(key);
    if (*(path.top()) == nullptr)
    {
        throw obtaining_of_nonexistent_key_attempt_exception(key);
    }

    tvalue const &got_value = (*(path.top()))->value;

    this->balance(path);

    return got_value;
}

template<typename tkey, typename tvalue>
std::vector<typename associative_container<tkey, tvalue>::key_value_pair> binary_search_tree<tkey, tvalue>::obtaining_template_method::obtain_between(tkey const &lower_bound, tkey const &upper_bound, bool lower_bound_inclusive, bool upper_bound_inclusive)
{
    std::vector<typename associative_container<tkey, tvalue>::key_value_pair> range;

    std::stack<node *> path;
    node *current = this->_tree->_root;
    while (true)
    {
        if (current == nullptr)
        {
            break;
        }

        auto comparison_result = this->_tree->_keys_comparer(lower_bound, current->key);
        path.push(current);

        if (comparison_result == 0)
        {
            if (lower_bound_inclusive)
            {
                break;
            }

            current = current->right_subtree;
        }
        else if (comparison_result < 0)
        {
            current = current->left_subtree;
        }
        else
        {
            current = current->right_subtree;
        }

        if (current == nullptr && this->_tree->_keys_comparer(path.top()->key, lower_bound) < (lower_bound_inclusive ? 0 : 1))
        {
            path = std::move(std::stack<node *>());
        }
    }

    auto it = infix_iterator(this->_tree, path);
    while ((it != this->_tree->end_infix()) && (this->_tree->_keys_comparer(upper_bound, (*it)->get_key()) > (upper_bound_inclusive ? -1 : 0)))
    {
        range.push_back(std::move(typename associative_container<tkey, tvalue>::key_value_pair((*it)->get_key(), (*it)->get_value())));
        ++it;
    }

    return range;
}

// endregion search_tree<tkey, tvalue>::obtaining_template_method implementation

// region search_tree<tkey, tvalue>::disposal_template_method implementation

template<typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::disposal_template_method::disposal_template_method(binary_search_tree<tkey, tvalue> *tree, typename binary_search_tree<tkey, tvalue>::disposal_of_nonexistent_key_attempt_strategy disposal_strategy):
    binary_search_tree<tkey, tvalue>::template_method_basics(tree), _strategy(disposal_strategy) 
{

}

template<typename tkey, typename tvalue>
template<typename T>
void binary_search_tree<tkey, tvalue>::disposal_template_method::swap(T &&one, T &&another) const noexcept
{
    T tmp = std::move(one);
    one = std::move(another);
    another = std::move(tmp);
}

template<typename tkey, typename tvalue>
tvalue binary_search_tree<tkey, tvalue>::disposal_template_method::dispose(tkey const &key)
{
    auto path = this->find_path(key);
    if (*(path.top()) == nullptr)
    {
        switch (_strategy)
        {
            case disposal_of_nonexistent_key_attempt_strategy::throw_an_exception:
                throw disposal_of_nonexistent_key_attempt_exception(key);
            case disposal_of_nonexistent_key_attempt_strategy::do_nothing:
                return tvalue();
        }
    }

    if ((*(path.top()))->left_subtree != nullptr && (*(path.top()))->right_subtree != nullptr)
    {
        auto *target_to_swap = *(path.top());
        auto **current = &((*(path.top()))->left_subtree);

        while (*current != nullptr)
        {
            path.push(current);
            current = &((*current)->right_subtree);
        }

        swap(std::move(target_to_swap->key), std::move((*(path.top()))->key));
        swap(std::move(target_to_swap->value), std::move((*(path.top()))->value));
    }

    tvalue value = std::move((*(path.top()))->value);
    
    node *leftover_subtree = (*(path.top()))->left_subtree == nullptr
        ? (*(path.top()))->right_subtree
        : (*(path.top()))->left_subtree;
    
    allocator::destruct(*(path.top()));
    allocator_guardant::deallocate_with_guard(reinterpret_cast<void *>(*(path.top())));
    
    *(path.top()) = leftover_subtree;

    this->balance(path);

    return value;
}

template<typename tkey, typename tvalue>
void binary_search_tree<tkey, tvalue>::disposal_template_method::set_disposal_strategy(typename binary_search_tree<tkey, tvalue>::disposal_of_nonexistent_key_attempt_strategy disposal_strategy) noexcept
{
    _strategy = disposal_strategy;
}

// endregion search_tree<tkey, tvalue>::disposal_template_method implementation

// endregion template methods

// region construction, assignment, destruction implementation

template<typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::binary_search_tree(typename binary_search_tree<tkey, tvalue>::insertion_template_method *insertion_template, typename binary_search_tree<tkey, tvalue>::obtaining_template_method *obtaining_template, typename binary_search_tree<tkey, tvalue>::disposal_template_method *disposal_template, std::function<int(tkey const &, tkey const &)> comparer, allocator *allocator, logger *logger):
    search_tree<tkey, tvalue>(comparer, logger, allocator),
    _insertion_template(insertion_template),
    _obtaining_template(obtaining_template),
    _disposal_template(disposal_template),
    _root(nullptr) 
{
    automatic_logger(logger::severity::trace, "constructor", get_typename(), this->get_logger());
}

template<typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::binary_search_tree(std::function<int(tkey const &, tkey const &)> keys_comparer, allocator *allocator, logger *logger, typename binary_search_tree<tkey, tvalue>::insertion_of_existent_key_attempt_strategy insertion_strategy, typename binary_search_tree<tkey, tvalue>::disposal_of_nonexistent_key_attempt_strategy disposal_strategy):
    binary_search_tree(new binary_search_tree<tkey, tvalue>::insertion_template_method(this, insertion_strategy), new binary_search_tree<tkey, tvalue>::obtaining_template_method(this), new binary_search_tree<tkey, tvalue>::disposal_template_method(this, disposal_strategy), keys_comparer, allocator, logger) 
{

}

template<typename tkey, typename tvalue>
inline std::string binary_search_tree<tkey, tvalue>::get_typename() const noexcept
{
    return "binary_search_tree";
}

template<typename tkey, typename tvalue>
void binary_search_tree<tkey, tvalue>::clear_node(node *&subtree_root) const noexcept
{
    if (subtree_root == nullptr)
    {
        return;
    }

    clear_node(subtree_root->left_subtree);
    clear_node(subtree_root->right_subtree);
    subtree_root->~node();
    this->deallocate_with_guard(subtree_root);

    subtree_root = nullptr;
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::node *binary_search_tree<tkey, tvalue>::copy_node(node const *subtree_root) const
{   
    if (subtree_root == nullptr)
    {
        return nullptr;
    }

    node *subtree_root_copied;
    try
    {
        subtree_root_copied = reinterpret_cast<node *>(this->allocate_with_guard(get_node_size()));
    }
    catch (std::bad_alloc const &exception)
    {
        throw exception;
    }

    call_node_constructor(subtree_root_copied, subtree_root->key, subtree_root->value);
    subtree_root_copied->left_subtree = copy_node(subtree_root->left_subtree);
    subtree_root_copied->right_subtree = copy_node(subtree_root->right_subtree);

    return subtree_root_copied;
}

template<typename tkey, typename tvalue>
void binary_search_tree<tkey, tvalue>::clear_tree(binary_search_tree<tkey, tvalue> &tree) noexcept
{
    clear_node(tree._root);

    delete tree._insertion_template;
    tree._insertion_template = nullptr;

    delete tree._obtaining_template;
    tree._obtaining_template = nullptr;

    delete tree._disposal_template;
    tree._disposal_template = nullptr;
}

template<typename tkey, typename tvalue>
void binary_search_tree<tkey, tvalue>::copy_tree(binary_search_tree<tkey, tvalue> const &other)
{
    try
    {
        _root = this->copy_node(other._root);
    }
    catch (std::bad_alloc const &exception)
    {
        throw exception;
    }

    _insertion_template = other._insertion_template != nullptr
        ? new binary_search_tree<tkey, tvalue>::insertion_template_method(*other._insertion_template)
        : nullptr;

    _obtaining_template = other._obtaining_template != nullptr
        ? new binary_search_tree<tkey, tvalue>::obtaining_template_method(*other._obtaining_template)
        : nullptr;
    
    _disposal_template = other._disposal_template != nullptr
        ? new binary_search_tree<tkey, tvalue>::disposal_template_method(*other._disposal_template)
        : nullptr;
}

template<typename tkey, typename tvalue>
void binary_search_tree<tkey, tvalue>::move_tree(binary_search_tree<tkey, tvalue> &tree) noexcept
{
    _root = std::exchange(tree._root, nullptr);
    _insertion_template = std::exchange(tree._insertion_template, nullptr);
    _obtaining_template = std::exchange(tree._obtaining_template, nullptr);
    _disposal_template = std::exchange(tree._disposal_template, nullptr);
}

template<typename tkey, typename tvalue>
inline size_t binary_search_tree<tkey, tvalue>::get_node_size() const noexcept
{
    return sizeof(typename binary_search_tree<tkey, tvalue>::node);
}

template<typename tkey, typename tvalue>
inline void binary_search_tree<tkey, tvalue>::call_node_constructor(node *raw_space, tkey const &key, tvalue const &value) const 
{
    allocator::construct(raw_space, key, value);
}

template<typename tkey, typename tvalue>
inline void binary_search_tree<tkey, tvalue>::call_node_constructor(node *raw_space, tkey const &key, tvalue &&value) const 
{
    allocator::construct(raw_space, key, std::move(value));
}

template<typename tkey, typename tvalue>
inline void binary_search_tree<tkey, tvalue>::call_iterator_data_constructor(iterator_data *raw_space, node *subtree_root, unsigned int depth) const
{
    allocator::construct(raw_space, depth, subtree_root->key, subtree_root->value);
}

template<typename tkey, typename tvalue>
inline size_t binary_search_tree<tkey, tvalue>::get_iterator_data_size() const noexcept
{
    return sizeof(iterator_data);
}

template<typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::binary_search_tree(binary_search_tree<tkey, tvalue> const &other)
{
    automatic_logger(logger::severity::trace, "copy constructor", get_typename(), this->get_logger());
    copy_tree(other);
}

template<typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::binary_search_tree(binary_search_tree<tkey, tvalue> &&other) noexcept
{    
    automatic_logger(logger::severity::trace, "move constructor", get_typename(), this->get_logger());
    move_tree(other);
}

template<typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue> &binary_search_tree<tkey, tvalue>::operator=(binary_search_tree<tkey, tvalue> const &other)
{    
    automatic_logger(logger::severity::trace, "copy assignment", get_typename(), this->get_logger());
    if (this == &other)
    {
        return *this;
    }

    clear_tree(*this);
    copy_tree(other);

    return *this;
}

template<typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue> &binary_search_tree<tkey, tvalue>::operator=(binary_search_tree<tkey, tvalue> &&other) noexcept
{
    automatic_logger(logger::severity::trace, "move assignment", get_typename(), this->get_logger());
    if (this == &other)
    {
        return *this;
    }
    
    clear_tree(*this);
    move_tree(other);

    return *this;
}

template<typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::~binary_search_tree()
{
    clear_tree(*this);
}

// endregion construction, assignment, destruction implementation

// region associative_container<tkey, tvalue> contract implementation

template<typename tkey, typename tvalue>
void binary_search_tree<tkey, tvalue>::insert(tkey const &key, tvalue const &value)
{
    automatic_logger(logger::severity::debug, "copy insert", get_typename(), this->get_logger());
    _insertion_template->insert(key, value);
}

template<typename tkey, typename tvalue>
void binary_search_tree<tkey, tvalue>::insert(tkey const &key, tvalue &&value)
{
    automatic_logger(logger::severity::debug, "move insert", get_typename(), this->get_logger());
    _insertion_template->insert(key, std::move(value));
}

template<typename tkey, typename tvalue>
tvalue const &binary_search_tree<tkey, tvalue>::obtain(tkey const &key)
{
    automatic_logger(logger::severity::debug, "obtain", get_typename(), this->get_logger());
    return _obtaining_template->obtain(key);
}

template<typename tkey, typename tvalue>
std::vector<typename associative_container<tkey, tvalue>::key_value_pair> binary_search_tree<tkey, tvalue>::obtain_between(tkey const &lower_bound, tkey const &upper_bound, bool lower_bound_inclusive, bool upper_bound_inclusive)
{
    automatic_logger(logger::severity::debug, "obtain between", get_typename(), this->get_logger());    
    return _obtaining_template->obtain_between(lower_bound, upper_bound, lower_bound_inclusive, upper_bound_inclusive);
}

template<typename tkey, typename tvalue>
tvalue binary_search_tree<tkey, tvalue>::dispose(tkey const &key)
{
    automatic_logger(logger::severity::debug, "dispose", get_typename(), this->get_logger());
    return _disposal_template->dispose(key);
}

// endregion associative_containers contract implementations

template<typename tkey, typename tvalue>
void binary_search_tree<tkey, tvalue>::set_insertion_strategy(typename binary_search_tree<tkey, tvalue>::insertion_of_existent_key_attempt_strategy insertion_strategy) noexcept
{
    information_with_guard(get_typename() + ": insertion strategy was changed");
    _insertion_template->set_insertion_strategy(insertion_strategy);
}

template<typename tkey, typename tvalue>
void binary_search_tree<tkey, tvalue>::set_removal_strategy(typename binary_search_tree<tkey, tvalue>::disposal_of_nonexistent_key_attempt_strategy disposal_strategy) noexcept
{
    information_with_guard(get_typename() + ": disposal strategy was changed");
    _disposal_template->set_disposal_strategy(disposal_strategy);
}

// region iterators requesting implementation

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::prefix_iterator binary_search_tree<tkey, tvalue>::begin_prefix() const noexcept
{
    return binary_search_tree<tkey, tvalue>::prefix_iterator(dynamic_cast<typename binary_search_tree<tkey, tvalue>::node *>(_root), this);
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::prefix_iterator binary_search_tree<tkey, tvalue>::end_prefix() const noexcept
{
    return binary_search_tree<tkey, tvalue>::prefix_iterator(nullptr, this);
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::prefix_const_iterator binary_search_tree<tkey, tvalue>::cbegin_prefix() const noexcept
{
    return binary_search_tree<tkey, tvalue>::prefix_const_iterator(dynamic_cast<typename binary_search_tree<tkey, tvalue>::node *>(_root), this);
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::prefix_const_iterator binary_search_tree<tkey, tvalue>::cend_prefix() const noexcept
{
    return binary_search_tree<tkey, tvalue>::prefix_const_iterator(nullptr, this);
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::prefix_reverse_iterator binary_search_tree<tkey, tvalue>::rbegin_prefix() const noexcept
{
    return binary_search_tree<tkey, tvalue>::prefix_reverse_iterator(dynamic_cast<typename binary_search_tree<tkey, tvalue>::node *>(_root), this);
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::prefix_reverse_iterator binary_search_tree<tkey, tvalue>::rend_prefix() const noexcept
{
    return binary_search_tree<tkey, tvalue>::prefix_reverse_iterator(nullptr, this);
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::prefix_const_reverse_iterator binary_search_tree<tkey, tvalue>::crbegin_prefix() const noexcept
{
    return binary_search_tree<tkey, tvalue>::prefix_const_reverse_iterator(dynamic_cast<typename binary_search_tree<tkey, tvalue>::node *>(_root), this);
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::prefix_const_reverse_iterator binary_search_tree<tkey, tvalue>::crend_prefix() const noexcept
{
    return binary_search_tree<tkey, tvalue>::prefix_const_reverse_iterator(nullptr, this);
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::infix_iterator binary_search_tree<tkey, tvalue>::begin_infix() const noexcept
{
    return binary_search_tree<tkey, tvalue>::infix_iterator(dynamic_cast<typename binary_search_tree<tkey, tvalue>::node *>(_root), this);
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::infix_iterator binary_search_tree<tkey, tvalue>::end_infix() const noexcept
{
    return binary_search_tree<tkey, tvalue>::infix_iterator(nullptr, this);
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::infix_const_iterator binary_search_tree<tkey, tvalue>::cbegin_infix() const noexcept
{
    return binary_search_tree<tkey, tvalue>::infix_const_iterator(dynamic_cast<typename binary_search_tree<tkey, tvalue>::node *>(_root), this);
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::infix_const_iterator binary_search_tree<tkey, tvalue>::cend_infix() const noexcept
{
    return binary_search_tree<tkey, tvalue>::infix_const_iterator(nullptr, this);
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::infix_reverse_iterator binary_search_tree<tkey, tvalue>::rbegin_infix() const noexcept
{
    return binary_search_tree<tkey, tvalue>::infix_reverse_iterator(dynamic_cast<typename binary_search_tree<tkey, tvalue>::node *>(_root), this);
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::infix_reverse_iterator binary_search_tree<tkey, tvalue>::rend_infix() const noexcept
{
    return binary_search_tree<tkey, tvalue>::infix_reverse_iterator(nullptr, this);
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::infix_const_reverse_iterator binary_search_tree<tkey, tvalue>::crbegin_infix() const noexcept
{
    return binary_search_tree<tkey, tvalue>::infix_const_reverse_iterator(dynamic_cast<typename binary_search_tree<tkey, tvalue>::node *>(_root), this);
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::infix_const_reverse_iterator binary_search_tree<tkey, tvalue>::crend_infix() const noexcept
{
    return binary_search_tree<tkey, tvalue>::infix_const_reverse_iterator(nullptr, this);
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::postfix_iterator binary_search_tree<tkey, tvalue>::begin_postfix() const noexcept
{
    return binary_search_tree<tkey, tvalue>::postfix_iterator(dynamic_cast<typename binary_search_tree<tkey, tvalue>::node *>(_root), this);
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::postfix_iterator binary_search_tree<tkey, tvalue>::end_postfix() const noexcept
{
    return binary_search_tree<tkey, tvalue>::postfix_iterator(nullptr, this);
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::postfix_const_iterator binary_search_tree<tkey, tvalue>::cbegin_postfix() const noexcept
{
    return binary_search_tree<tkey, tvalue>::postfix_const_iterator(dynamic_cast<typename binary_search_tree<tkey, tvalue>::node *>(_root), this);
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::postfix_const_iterator binary_search_tree<tkey, tvalue>::cend_postfix() const noexcept
{
    return binary_search_tree<tkey, tvalue>::postfix_const_iterator(nullptr, this);
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::postfix_reverse_iterator binary_search_tree<tkey, tvalue>::rbegin_postfix() const noexcept
{
    return binary_search_tree<tkey, tvalue>::postfix_reverse_iterator(dynamic_cast<typename binary_search_tree<tkey, tvalue>::node *>(_root), this);
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::postfix_reverse_iterator binary_search_tree<tkey, tvalue>::rend_postfix() const noexcept
{
    return binary_search_tree<tkey, tvalue>::postfix_reverse_iterator(nullptr, this);
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::postfix_const_reverse_iterator binary_search_tree<tkey, tvalue>::crbegin_postfix() const noexcept
{
    return binary_search_tree<tkey, tvalue>::postfix_const_reverse_iterator(dynamic_cast<typename binary_search_tree<tkey, tvalue>::node *>(_root, this));
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::postfix_const_reverse_iterator binary_search_tree<tkey, tvalue>::crend_postfix() const noexcept
{
    return binary_search_tree<tkey, tvalue>::postfix_const_reverse_iterator(nullptr, this);
}

// endregion iterators request implementation

// region subtree rotations implementation

template<typename tkey, typename tvalue>
void binary_search_tree<tkey, tvalue>::small_left_rotation(binary_search_tree<tkey, tvalue>::node *&subtree_root, bool validate) const
{
    if (subtree_root == nullptr)
    {
        return;
    }
   
    if (validate && subtree_root->right_subtree == nullptr)
    {
        throw std::logic_error("Tree is not valid before rotation");
    }

    binary_search_tree<tkey, tvalue>::node *tmp = subtree_root->right_subtree;
    subtree_root->right_subtree = tmp->left_subtree;
    tmp->left_subtree = subtree_root;
    subtree_root = tmp;
}

template<typename tkey, typename tvalue>
void binary_search_tree<tkey, tvalue>::small_right_rotation(binary_search_tree<tkey, tvalue>::node *&subtree_root, bool validate) const
{
    if (subtree_root == nullptr)
    {
        return;
    }
    
    if (validate && subtree_root->left_subtree == nullptr)
    {
        throw std::logic_error("Tree is not valid before rotation");
    }

    binary_search_tree<tkey, tvalue>::node *tmp = subtree_root->left_subtree;
    subtree_root->left_subtree = tmp->right_subtree;
    tmp->right_subtree = subtree_root;
    subtree_root = tmp;
}

template<typename tkey, typename tvalue>
void binary_search_tree<tkey, tvalue>::big_left_rotation(binary_search_tree<tkey, tvalue>::node *&subtree_root, bool validate) const
{
    if (subtree_root == nullptr)
    {
        return;
    }
    
    try 
    {
        small_right_rotation(subtree_root->right_subtree, validate);
        small_left_rotation(subtree_root, validate);
    }
    catch (std::logic_error const &exception)
    {
        throw exception;
    }
}

template<typename tkey, typename tvalue>
void binary_search_tree<tkey, tvalue>::big_right_rotation(binary_search_tree<tkey, tvalue>::node *&subtree_root, bool validate) const
{
    if (subtree_root == nullptr)
    {
        return;
    }
    
    try
    {
        small_left_rotation(subtree_root->left_subtree, validate);
        small_right_rotation(subtree_root, validate);
    }
    catch (std::logic_error const &exception)
    {
        throw exception;
    }    
}

template<typename tkey, typename tvalue>
void binary_search_tree<tkey, tvalue>::double_left_rotation(binary_search_tree<tkey, tvalue>::node *&subtree_root, bool at_grandparent_first, bool validate) const
{
    try 
    {
        if (at_grandparent_first)
        {
            big_right_rotation(subtree_root, validate);
        }
        else
        {
            big_left_rotation(subtree_root, validate);
        }
    }
    catch (std::logic_error const &exception)
    {
        throw exception;
    }
}

template<typename tkey, typename tvalue>
void binary_search_tree<tkey, tvalue>::double_right_rotation(binary_search_tree<tkey, tvalue>::node *&subtree_root, bool at_grandparent_first, bool validate) const
{
    try 
    {
        if (at_grandparent_first)
        {
            big_left_rotation(subtree_root, validate);
        }
        else
        {
            big_right_rotation(subtree_root, validate);
        }
    }
    catch (std::logic_error const &exception)
    {
        throw exception;
    }
}

// endregion subtree rotations implementation

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_BINARY_SEARCH_TREE_H