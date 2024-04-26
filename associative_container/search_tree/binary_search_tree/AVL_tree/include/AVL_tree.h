#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_AVL_TREE_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_AVL_TREE_H

#include <binary_search_tree.h>

template<typename tkey, typename tvalue>
class AVL_tree final:
    public binary_search_tree<tkey, tvalue>
{

private:
    
    struct node final:
        binary_search_tree<tkey, tvalue>::node
    {

    public:

        size_t subtree_height;

    public:

        explicit node(tkey const &key, tvalue const &value);  

        explicit node(tkey const &key, tvalue &&value);   

    };

public:
    
    struct iterator_data final:
        public binary_search_tree<tkey, tvalue>::iterator_data
    {
    
    public:
        
        size_t subtree_height;
    
    public:
        
        explicit iterator_data(unsigned int depth, tkey const &key, tvalue const &value, size_t subtree_height);

    };

private:

    class balancer
    {

    public:

        void balance_processing(std::stack<typename binary_search_tree<tkey, tvalue>::node **> &path, AVL_tree<tkey, tvalue> const *tree);
    
    private:

        inline int get_node_difference(typename AVL_tree<tkey, tvalue>::node *subtree_root);
        
        inline size_t get_node_height(typename AVL_tree<tkey, tvalue>::node *subtree_root);

        void update_node_height(typename AVL_tree<tkey, tvalue>::node *subtree_root);

    };
    
    class insertion_template_method final:
        public binary_search_tree<tkey, tvalue>::insertion_template_method,
        public balancer
    {
    
    public:
        
        explicit insertion_template_method(AVL_tree<tkey, tvalue> *tree, typename binary_search_tree<tkey, tvalue>::insertion_of_existent_key_attempt_strategy insertion_strategy);
    
    private:
        
        void balance(std::stack<typename binary_search_tree<tkey, tvalue>::node **> &path) override;
        
    };
    
    class disposal_template_method final:
        public binary_search_tree<tkey, tvalue>::disposal_template_method,
        public balancer
    {
    
    public:
        
        explicit disposal_template_method(AVL_tree<tkey, tvalue> *tree, typename binary_search_tree<tkey, tvalue>::disposal_of_nonexistent_key_attempt_strategy disposal_strategy);
        
    private:

        void balance(std::stack<typename binary_search_tree<tkey, tvalue>::node **> &path) override;
        
    };

public:
    
    explicit AVL_tree(
        std::function<int(tkey const &, tkey const &)> keys_comparer = std::less<tkey>(), 
        allocator *allocator = nullptr, 
        logger *logger = nullptr, 
        typename binary_search_tree<tkey, tvalue>::insertion_of_existent_key_attempt_strategy insertion_strategy = binary_search_tree<tkey, tvalue>::insertion_of_existent_key_attempt_strategy::throw_an_exception, 
        typename binary_search_tree<tkey, tvalue>::disposal_of_nonexistent_key_attempt_strategy disposal_strategy = binary_search_tree<tkey, tvalue>::disposal_of_nonexistent_key_attempt_strategy::throw_an_exception);

private:

    inline size_t get_node_size() const noexcept override;

    inline void call_node_constructor(typename binary_search_tree<tkey, tvalue>::node *raw_space, tkey const &key, tvalue const &value) const override;

    inline void call_node_constructor(typename binary_search_tree<tkey, tvalue>::node *raw_space, tkey const &key, tvalue &&value) const override;

    inline void call_iterator_data_constructor(typename binary_search_tree<tkey, tvalue>::iterator_data *raw_space, typename binary_search_tree<tkey, tvalue>::node *subtree_root, unsigned int depth) const override;

    inline size_t get_iterator_data_size() const noexcept override;

    typename binary_search_tree<tkey, tvalue>::node *copy_node(typename binary_search_tree<tkey, tvalue>::node const *subtree_root) const override;

public:
    
    ~AVL_tree() noexcept final;
    
    AVL_tree(AVL_tree<tkey, tvalue> const &other);
    
    AVL_tree<tkey, tvalue> &operator=(AVL_tree<tkey, tvalue> const &other);
    
    AVL_tree(AVL_tree<tkey, tvalue> &&other) noexcept;
    
    AVL_tree<tkey, tvalue> &operator=(AVL_tree<tkey, tvalue> &&other) noexcept;
    
};

template<typename tkey, typename tvalue>
AVL_tree<tkey, tvalue>::node::node(tkey const &key, tvalue const &value):
    binary_search_tree<tkey, tvalue>::node(key, value),
    subtree_height(1)
{

}

template<typename tkey, typename tvalue>
AVL_tree<tkey, tvalue>::node::node(tkey const &key, tvalue &&value):
    binary_search_tree<tkey, tvalue>::node(key, std::move(value)),
    subtree_height(1)
{

}

template<typename tkey, typename tvalue>
void AVL_tree<tkey, tvalue>::balancer::balance_processing(std::stack<typename binary_search_tree<tkey, tvalue>::node **> &path, AVL_tree<tkey, tvalue> const *tree)
{
    if (path.empty())
    {
        return;
    }

    AVL_tree<tkey, tvalue>::node **current = reinterpret_cast<AVL_tree<tkey, tvalue>:: node **>(path.top());
    path.pop();
    int difference = get_node_difference(*current);
    
    if (difference > 1 && get_node_difference(reinterpret_cast<AVL_tree<tkey, tvalue>:: node *>((*current)->left_subtree)) >= 0)
    {
        tree->small_right_rotation(*reinterpret_cast<typename binary_search_tree<tkey, tvalue>::node **>(current));
    }
    else if (difference > 1 && get_node_difference(reinterpret_cast<AVL_tree<tkey, tvalue>::node *>((*current)->left_subtree)) < 0)
    {
        tree->big_right_rotation(*reinterpret_cast<typename binary_search_tree<tkey, tvalue>::node **>(current));
    }
    else if (difference < -1 && get_node_difference(reinterpret_cast<AVL_tree<tkey, tvalue>::node *>((*current)->right_subtree)) <= 0)
    {
        tree->small_left_rotation(*reinterpret_cast<typename binary_search_tree<tkey, tvalue>::node **>(current));
    }
    else if (difference < -1 && get_node_difference(reinterpret_cast<AVL_tree<tkey, tvalue>::node *>((*current)->right_subtree)) > 0)
    {
        tree->big_left_rotation(*reinterpret_cast<typename binary_search_tree<tkey, tvalue>::node **>(current));
    }
    
    if (*current != nullptr)
    {
        update_node_height(reinterpret_cast<AVL_tree<tkey, tvalue>::node *>((*current)->left_subtree));
        update_node_height(reinterpret_cast<AVL_tree<tkey, tvalue>::node *>((*current)->right_subtree));
    }
    update_node_height(*current);
    balance_processing(path, tree);
}

template<typename tkey, typename tvalue>
inline int AVL_tree<tkey, tvalue>::balancer::get_node_difference(typename AVL_tree<tkey, tvalue>::node *subtree_root)
{
    return subtree_root != nullptr
        ? get_node_height(reinterpret_cast<typename AVL_tree<tkey, tvalue>::node *>(subtree_root->left_subtree)) - get_node_height(reinterpret_cast<typename AVL_tree<tkey, tvalue>::node *>(subtree_root->right_subtree))
        : 0;
}

template<typename tkey, typename tvalue>
inline size_t AVL_tree<tkey, tvalue>::balancer::get_node_height(typename AVL_tree<tkey, tvalue>::node *subtree_root)
{
    return subtree_root != nullptr
        ? subtree_root->subtree_height
        : 0;
}

template<typename tkey, typename tvalue>
void AVL_tree<tkey, tvalue>::balancer::update_node_height(typename AVL_tree<tkey, tvalue>::node *subtree_root)
{
    if (subtree_root == nullptr)
    {
        return;
    }
    
    subtree_root->subtree_height = std::max(get_node_height(reinterpret_cast<typename AVL_tree<tkey, tvalue>::node *>(subtree_root->left_subtree)), get_node_height(reinterpret_cast<typename AVL_tree<tkey, tvalue>::node *>(subtree_root->right_subtree))) + 1;
}

template<typename tkey, typename tvalue>
AVL_tree<tkey, tvalue>::iterator_data::iterator_data(unsigned int depth, tkey const &key,tvalue const &value, size_t subtree_height):
    binary_search_tree<tkey, tvalue>::iterator_data(depth, key, value),
    subtree_height(subtree_height)
{

}

template<typename tkey, typename tvalue>
AVL_tree<tkey, tvalue>::insertion_template_method::insertion_template_method(AVL_tree<tkey, tvalue> *tree, typename binary_search_tree<tkey, tvalue>::insertion_of_existent_key_attempt_strategy insertion_strategy):
    binary_search_tree<tkey, tvalue>::insertion_template_method(tree, insertion_strategy)
{
    
}

template<typename tkey, typename tvalue>
void AVL_tree<tkey, tvalue>::insertion_template_method::balance(std::stack<typename binary_search_tree<tkey, tvalue>::node **> &path)
{
    automatic_logger auto_log(logger::severity::debug, "balance", "AVL_tree", dynamic_cast<AVL_tree<tkey, tvalue> const *>(this->_tree)->get_logger());
    this->balance_processing(path, dynamic_cast<AVL_tree<tkey, tvalue> const *>(this->_tree));
}

template<typename tkey, typename tvalue>
AVL_tree<tkey, tvalue>::disposal_template_method::disposal_template_method(AVL_tree<tkey, tvalue> *tree, typename binary_search_tree<tkey, tvalue>::disposal_of_nonexistent_key_attempt_strategy disposal_strategy):
    binary_search_tree<tkey, tvalue>::disposal_template_method(tree, disposal_strategy)
{

}

template<typename tkey, typename tvalue>
void AVL_tree<tkey, tvalue>::disposal_template_method::balance(std::stack<typename binary_search_tree<tkey, tvalue>::node **> &path)
{
    automatic_logger auto_log(logger::severity::debug, "balance", "AVL_tree", dynamic_cast<AVL_tree<tkey, tvalue> const *>(this->_tree)->get_logger());
    this->balance_processing(path, dynamic_cast<AVL_tree<tkey, tvalue> const *>(this->_tree));
}

template<typename tkey, typename tvalue>
inline size_t AVL_tree<tkey, tvalue>::get_node_size() const noexcept
{
    return sizeof(AVL_tree<tkey, tvalue>::node);
}

template<typename tkey, typename tvalue>
inline void AVL_tree<tkey, tvalue>::call_node_constructor(typename binary_search_tree<tkey, tvalue>::node *raw_space, tkey const &key, tvalue const &value) const
{
    allocator::construct(reinterpret_cast<AVL_tree<tkey, tvalue>::node *>(raw_space), key, value);
}

template<typename tkey, typename tvalue>
inline void AVL_tree<tkey, tvalue>::call_node_constructor(typename binary_search_tree<tkey, tvalue>::node *raw_space, tkey const &key, tvalue &&value) const
{
    allocator::construct(reinterpret_cast<AVL_tree<tkey, tvalue>::node *>(raw_space), key, std::move(value));
}

template<typename tkey, typename tvalue>
inline void AVL_tree<tkey, tvalue>::call_iterator_data_constructor(typename binary_search_tree<tkey, tvalue>::iterator_data *raw_space, typename binary_search_tree<tkey, tvalue>::node *subtree_root, unsigned int depth) const
{
    allocator::construct(reinterpret_cast<typename AVL_tree<tkey, tvalue>::iterator_data *>(raw_space), depth, subtree_root->key, subtree_root->value, reinterpret_cast<AVL_tree<tkey, tvalue>::node *>(subtree_root)->subtree_height);
}

template<typename tkey, typename tvalue>
inline size_t AVL_tree<tkey, tvalue>::get_iterator_data_size() const noexcept
{
    return sizeof(AVL_tree<tkey, tvalue>::iterator_data);
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::node *AVL_tree<tkey, tvalue>::copy_node(typename binary_search_tree<tkey, tvalue>::node const *subtree_root) const
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
    reinterpret_cast<AVL_tree<tkey, tvalue>::node *>(subtree_root_copied)->subtree_height = reinterpret_cast<AVL_tree<tkey, tvalue>::node const *>(subtree_root)->subtree_height;

    subtree_root_copied->left_subtree = copy_node(subtree_root->left_subtree);
    subtree_root_copied->right_subtree = copy_node(subtree_root->right_subtree);
    return subtree_root_copied;
}

template<typename tkey,typename tvalue>
AVL_tree<tkey, tvalue>::AVL_tree(
    std::function<int(tkey const &, tkey const &)> keys_comparer, 
    allocator *allocator, 
    logger *logger, 
    typename binary_search_tree<tkey, tvalue>::insertion_of_existent_key_attempt_strategy insertion_strategy, 
    typename binary_search_tree<tkey, tvalue>::disposal_of_nonexistent_key_attempt_strategy disposal_strategy):
    binary_search_tree<tkey, tvalue>(
        new AVL_tree<tkey, tvalue>::insertion_template_method(this, insertion_strategy), 
        new typename binary_search_tree<tkey, tvalue>::obtaining_template_method(dynamic_cast<binary_search_tree<tkey, tvalue> *>(this)),
        new AVL_tree<tkey, tvalue>::disposal_template_method(this, disposal_strategy), 
        keys_comparer, allocator, logger)
{

}

template<typename tkey, typename tvalue>
AVL_tree<tkey, tvalue>::~AVL_tree() noexcept
{
    this->clear_tree(*reinterpret_cast<binary_search_tree<tkey, tvalue> *>(this));
}

template<typename tkey, typename tvalue>
AVL_tree<tkey, tvalue>::AVL_tree(AVL_tree<tkey, tvalue> const &other)
{
    this->copy_tree(*reinterpret_cast<binary_search_tree<tkey, tvalue> *>(&other));
}

template<typename tkey, typename tvalue>
AVL_tree<tkey, tvalue> &AVL_tree<tkey, tvalue>::operator=(AVL_tree<tkey, tvalue> const &other)
{
    if (this == &other)
    {
        return *this;
    }

    this->clear_tree(*reinterpret_cast<binary_search_tree<tkey, tvalue> *>(this));
    this->copy_tree(*reinterpret_cast<binary_search_tree<tkey, tvalue> *>(&other));

    return *this;
}

template<typename tkey, typename tvalue>
AVL_tree<tkey, tvalue>::AVL_tree(AVL_tree<tkey, tvalue> &&other) noexcept
{
    this->move_tree(*reinterpret_cast<binary_search_tree<tkey, tvalue> *>(&other));
}

template<typename tkey, typename tvalue>
AVL_tree<tkey, tvalue> &AVL_tree<tkey, tvalue>::operator=(AVL_tree<tkey, tvalue> &&other) noexcept
{
    if (this == &other)
    {
        return *this;
    }

    this->clear_tree(*reinterpret_cast<binary_search_tree<tkey, tvalue> *>(this));
    this->move_tree(*reinterpret_cast<binary_search_tree<tkey, tvalue> *>(&other));

    return *this;
}

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_AVL_TREE_H