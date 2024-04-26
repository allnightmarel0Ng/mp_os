#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_RED_BLACK_TREE_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_RED_BLACK_TREE_H

#include <binary_search_tree.h>

template<typename tkey, typename tvalue>
class red_black_tree final:
    public binary_search_tree<tkey, tvalue>
{

public:
    
    enum class node_color
    {
        RED,
        BLACK
    };

private:
    
    struct node final:
        binary_search_tree<tkey, tvalue>::node
    {
    
    public:
        
        node_color color;
    
    public:

        explicit node(tkey const &key, tvalue const &value);

        explicit node(tkey const &key, tvalue &&value);
        
    };

public:
    
    struct iterator_data final:
        public binary_search_tree<tkey, tvalue>::iterator_data
    {
    
    public:
        
        node_color color;
    
    public:
        
        explicit iterator_data(unsigned int depth, tkey const &key, tvalue const &value, node_color color);
        
    };

private:
    
    class insertion_template_method final:
        public binary_search_tree<tkey, tvalue>::insertion_template_method
    {
    
    public:
        
        explicit insertion_template_method(red_black_tree<tkey, tvalue> *tree, typename binary_search_tree<tkey, tvalue>::insertion_of_existent_key_attempt_strategy insertion_strategy);
    
    private:
        
        void balance(std::stack<typename binary_search_tree<tkey, tvalue>::node **> &path) override;
        
    };
    
    class disposal_template_method final:
        public binary_search_tree<tkey, tvalue>::disposal_template_method
    {
    
    public:
        
        explicit disposal_template_method(red_black_tree<tkey, tvalue> *tree, typename binary_search_tree<tkey, tvalue>::disposal_of_nonexistent_key_attempt_strategy disposal_strategy);
        
    private:

        void balance(std::stack<typename binary_search_tree<tkey, tvalue>::node **> &path) override;

    };

private:

    inline size_t get_node_size() const noexcept override;

    inline void call_node_constructor(typename binary_search_tree<tkey, tvalue>::node *raw_space, tkey const &key, tvalue const &value) const override;

    inline void call_node_constructor(typename binary_search_tree<tkey, tvalue>::node *raw_space, tkey const &key, tvalue &&value) const override;

    inline void call_iterator_data_constructor(typename binary_search_tree<tkey, tvalue>::iterator_data *raw_space, typename binary_search_tree<tkey, tvalue>::node *subtree_root, unsigned int depth) const override;

    inline size_t get_iterator_data_size() const noexcept override;

    typename binary_search_tree<tkey, tvalue>::node *copy_node(typename binary_search_tree<tkey, tvalue>::node const *subtree_root) const override;

public:
    
    explicit red_black_tree(
        std::function<int(tkey const &, tkey const &)> keys_comparer = std::less<tkey>(),
        allocator *allocator = nullptr, 
        logger *logger = nullptr, 
        typename binary_search_tree<tkey, tvalue>::insertion_of_existent_key_attempt_strategy insertion_strategy = binary_search_tree<tkey, tvalue>::insertion_of_existent_key_attempt_strategy::throw_an_exception, 
        typename binary_search_tree<tkey, tvalue>::disposal_of_nonexistent_key_attempt_strategy disposal_strategy = binary_search_tree<tkey, tvalue>::disposal_of_nonexistent_key_attempt_strategy::throw_an_exception);

public:
    
    ~red_black_tree() noexcept final;
    
    red_black_tree(red_black_tree<tkey, tvalue> const &other);
    
    red_black_tree<tkey, tvalue> &operator=(red_black_tree<tkey, tvalue> const &other);
    
    red_black_tree(red_black_tree<tkey, tvalue> &&other) noexcept;
    
    red_black_tree<tkey, tvalue> &operator=(red_black_tree<tkey, tvalue> &&other) noexcept;
    
};


template<typename tkey, typename tvalue>
red_black_tree<tkey, tvalue>::node::node(tkey const &key, tvalue const &value):
    binary_search_tree<tkey, tvalue>::node(key, value),
    color(node_color::RED)
{

}

template<typename tkey, typename tvalue>
red_black_tree<tkey, tvalue>::node::node(tkey const &key, tvalue &&value):
    binary_search_tree<tkey, tvalue>::node(key, std::move(value)),
    color(node_color::RED)
{

}

template<typename tkey, typename tvalue>
red_black_tree<tkey, tvalue>::iterator_data::iterator_data(unsigned int depth, tkey const &key, tvalue const &value, typename red_black_tree<tkey, tvalue>::node_color color):
    binary_search_tree<tkey, tvalue>::iterator_data(depth, key, value),
    color(color)
{

}

template<typename tkey, typename tvalue>
red_black_tree<tkey, tvalue>::insertion_template_method::insertion_template_method(red_black_tree<tkey, tvalue> *tree, typename binary_search_tree<tkey, tvalue>::insertion_of_existent_key_attempt_strategy insertion_strategy):
    binary_search_tree<tkey, tvalue>::insertion_template_method(tree, insertion_strategy)
{

}

template<typename tkey, typename tvalue>
void red_black_tree<tkey, tvalue>::insertion_template_method::balance(std::stack<typename binary_search_tree<tkey, tvalue>::node **> &path)
{
    red_black_tree<tkey, tvalue> *tree = reinterpret_cast<red_black_tree<tkey, tvalue> *>(this->_tree);
    automatic_logger auto_log(logger::severity::debug, "insertion balance", "red_black_tree", tree->get_logger());
    
    red_black_tree<tkey, tvalue>::node **current = reinterpret_cast<red_black_tree<tkey, tvalue>::node **>(path.top());
    
    path.pop();
    if (path.empty())
    {
        (*current)->color = node_color::BLACK;
        return;
    }

    red_black_tree<tkey, tvalue>::node **parent = reinterpret_cast<red_black_tree<tkey, tvalue>::node **>(path.top());
    while ((*parent)->color == node_color::RED)
    {
        if (path.empty())
        {
            break;
        }

        path.pop();
        red_black_tree<tkey, tvalue>::node **grandparent = reinterpret_cast<red_black_tree<tkey, tvalue>::node **>(path.top());

        if ((*grandparent)->left_subtree == *parent)
        {
            red_black_tree<tkey, tvalue>::node **uncle = reinterpret_cast<red_black_tree<tkey, tvalue>::node **>(&((*grandparent)->right_subtree));
            if (*uncle != nullptr && (*uncle)->color == node_color::RED)
            {
                (*parent)->color = node_color::BLACK;
                (*uncle)->color = node_color::BLACK;
                (*grandparent)->color = node_color::RED;
                current = grandparent;
                
                path.pop();
                if (path.empty())
                {
                    break;
                }

                parent = reinterpret_cast<red_black_tree<tkey, tvalue>::node **>(path.top());
            }
            else
            {
                if ((*parent)->right_subtree == *current)
                {
                    current = parent;
                    tree->small_left_rotation(*reinterpret_cast<typename binary_search_tree<tkey, tvalue>::node **>(current));
                }
                
                (*parent)->color = node_color::BLACK;
                (*grandparent)->color = node_color::RED;
                tree->small_right_rotation(*reinterpret_cast<typename binary_search_tree<tkey, tvalue>::node **>(grandparent));
                break;
            }
        }
        else
        {
            red_black_tree<tkey, tvalue>::node **uncle = reinterpret_cast<red_black_tree<tkey, tvalue>::node **>(&((*grandparent)->left_subtree));
            if (*uncle != nullptr && (*uncle)->color == node_color::RED)
            {
                (*parent)->color = node_color::BLACK;
                (*uncle)->color = node_color::BLACK;
                (*grandparent)->color = node_color::RED;
                current = grandparent;
                
                path.pop();
                if (path.empty())
                {
                    break;
                }

                parent = reinterpret_cast<red_black_tree<tkey, tvalue>::node **>(path.top());
            }
            else
            {
                if ((*parent)->left_subtree == *current)
                {
                    current = parent;
                    tree->small_right_rotation(*reinterpret_cast<typename binary_search_tree<tkey, tvalue>::node **>(current));
                }

                (*parent)->color = node_color::BLACK;
                (*grandparent)->color = node_color::RED;
                tree->small_left_rotation(*reinterpret_cast<typename binary_search_tree<tkey, tvalue>::node **>(grandparent));
                break;
            }
        }
    }
    (*reinterpret_cast<red_black_tree<tkey, tvalue>::node **>(&(tree->_root)))->color = node_color::BLACK;
}


template<typename tkey, typename tvalue>
red_black_tree<tkey, tvalue>::disposal_template_method::disposal_template_method(red_black_tree<tkey, tvalue> *tree, typename binary_search_tree<tkey, tvalue>::disposal_of_nonexistent_key_attempt_strategy disposal_strategy):
    binary_search_tree<tkey, tvalue>::disposal_template_method(tree, disposal_strategy)
{

}

template<typename tkey, typename tvalue>
void red_black_tree<tkey, tvalue>::disposal_template_method::balance(std::stack<typename binary_search_tree<tkey, tvalue>::node **> &path)
{
    red_black_tree<tkey, tvalue> *tree = reinterpret_cast<red_black_tree<tkey, tvalue> *>(this->_tree);
    automatic_logger auto_log(logger::severity::debug, "disposition balance", "red_black_tree", tree->get_logger());
            
    red_black_tree<tkey, tvalue>::node **current = reinterpret_cast<red_black_tree<tkey, tvalue>::node **>(path.top());
    if (*current == nullptr)
    {
        return;
    }
    while ((*current)->color == node_color::BLACK && (*current) != reinterpret_cast<red_black_tree<tkey, tvalue>::node *>(tree->_root))
    {
        path.pop();
        red_black_tree<tkey, tvalue>::node **parent = reinterpret_cast<red_black_tree<tkey, tvalue>::node **>(path.top());
        if ((*parent)->left_subtree == *current)
        {
            red_black_tree<tkey, tvalue>::node **brother = reinterpret_cast<red_black_tree<tkey, tvalue>::node **>(&((*parent)->right_subtree));
            if ((*brother)->color == node_color::RED)
            {
                (*brother)->color = node_color::BLACK;
                (*parent)->color = node_color::RED;
                tree->small_left_rotation(*reinterpret_cast<typename binary_search_tree<tkey, tvalue>::node **>(parent));
            }

            red_black_tree<tkey, tvalue>::node **brother_left_subtree = reinterpret_cast<red_black_tree<tkey, tvalue>::node **>(&((*brother)->left_subtree));
            red_black_tree<tkey, tvalue>::node **brother_right_subtree = reinterpret_cast<red_black_tree<tkey, tvalue>::node **>(&((*brother)->right_subtree));

            if (*brother_left_subtree != nullptr && (*brother_left_subtree)->color == node_color::BLACK && *brother_right_subtree != nullptr && (*brother_right_subtree)->color == node_color::BLACK)
            {
                (*brother)->color = node_color::RED;
            }
            else
            {
                if (*brother_right_subtree != nullptr && (*brother_right_subtree)->color == node_color::BLACK)
                {
                    (*brother_left_subtree)->color = node_color::BLACK;
                    (*brother)->color = node_color::RED;
                    tree->small_right_rotation(*reinterpret_cast<typename binary_search_tree<tkey, tvalue>::node **>(brother));
                }

                (*brother)->color = (*parent)->color;
                (*parent)->color = node_color::BLACK;

                if (*brother_right_subtree != nullptr)
                {
                    (*brother_right_subtree)->color = node_color::BLACK;
                }

                tree->small_left_rotation(*reinterpret_cast<typename binary_search_tree<tkey, tvalue>::node **>(parent));
                *current = reinterpret_cast<red_black_tree<tkey, tvalue>::node *>(tree->_root);
            }
        }
        else
        {
            red_black_tree<tkey, tvalue>::node **brother = reinterpret_cast<red_black_tree<tkey, tvalue>::node **>(&((*parent)->left_subtree));
            if ((*brother)->color == node_color::RED)
            {
                (*brother)->color = node_color::BLACK;
                (*parent)->color = node_color::RED;
                tree->small_right_rotation(*reinterpret_cast<typename binary_search_tree<tkey, tvalue>::node **>(parent));
            }

            red_black_tree<tkey, tvalue>::node **brother_left_subtree = reinterpret_cast<red_black_tree<tkey, tvalue>::node **>(&((*brother)->left_subtree));
            red_black_tree<tkey, tvalue>::node **brother_right_subtree = reinterpret_cast<red_black_tree<tkey, tvalue>::node **>(&((*brother)->right_subtree));

            if (*brother_left_subtree != nullptr && (*brother_left_subtree)->color == node_color::BLACK && *brother_right_subtree != nullptr && (*brother_right_subtree)->color == node_color::BLACK)
            {
                (*brother)->color = node_color::RED;
            }
            else
            {
                if (*brother_left_subtree != nullptr && (*brother_left_subtree)->color == node_color::BLACK)
                {
                    (*brother_left_subtree)->color = node_color::BLACK;
                    (*brother)->color = node_color::RED;
                    tree->small_left_rotation(*reinterpret_cast<typename binary_search_tree<tkey, tvalue>::node **>(brother));
                }

                (*brother)->color = (*parent)->color;
                (*parent)->color = node_color::BLACK;

                if (*brother_right_subtree != nullptr)
                {
                    (*brother_right_subtree)->color = node_color::BLACK;
                }

                tree->small_right_rotation(*reinterpret_cast<typename binary_search_tree<tkey, tvalue>::node **>(parent));
                *current = reinterpret_cast<red_black_tree<tkey, tvalue>::node *>(tree->_root);
            }
        }
    }

    (*current)->color = node_color::BLACK;
    (*reinterpret_cast<red_black_tree<tkey, tvalue>::node **>(&(tree->_root)))->color = node_color::BLACK;
}

template<typename tkey, typename tvalue>
inline size_t red_black_tree<tkey, tvalue>::get_node_size() const noexcept
{
    return sizeof(red_black_tree<tkey, tvalue>::node);
}

template<typename tkey, typename tvalue>
inline void red_black_tree<tkey, tvalue>::call_node_constructor(typename binary_search_tree<tkey, tvalue>::node *raw_space, tkey const &key, tvalue const &value) const
{
    allocator::construct(reinterpret_cast<red_black_tree<tkey, tvalue>::node *>(raw_space), key, value);
}

template<typename tkey, typename tvalue>
inline void red_black_tree<tkey, tvalue>::call_node_constructor(typename binary_search_tree<tkey, tvalue>::node *raw_space, tkey const &key, tvalue &&value) const
{
    allocator::construct(reinterpret_cast<red_black_tree<tkey, tvalue>::node *>(raw_space), key, std::move(value));
}

template<typename tkey, typename tvalue>
inline void red_black_tree<tkey, tvalue>::call_iterator_data_constructor(typename binary_search_tree<tkey, tvalue>::iterator_data *raw_space, typename binary_search_tree<tkey, tvalue>::node *subtree_root, unsigned int depth) const
{
    allocator::construct(reinterpret_cast<red_black_tree<tkey, tvalue>::iterator_data *>(raw_space), depth, subtree_root->key, subtree_root->value, (reinterpret_cast<red_black_tree<tkey, tvalue>::node *>(subtree_root))->color);
}

template<typename tkey, typename tvalue>
inline size_t red_black_tree<tkey, tvalue>::get_iterator_data_size() const noexcept
{
    return sizeof(red_black_tree<tkey, tvalue>::iterator_data);
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::node *red_black_tree<tkey, tvalue>::copy_node(typename binary_search_tree<tkey, tvalue>::node const *subtree_root) const
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
    reinterpret_cast<red_black_tree<tkey, tvalue>::node *>(subtree_root_copied)->color = reinterpret_cast<red_black_tree<tkey, tvalue>::node const *>(subtree_root)->color;

    subtree_root_copied->left_subtree = copy_node(subtree_root->left_subtree);
    subtree_root_copied->right_subtree = copy_node(subtree_root->right_subtree);
    return subtree_root_copied;
}

template<typename tkey, typename tvalue>
red_black_tree<tkey, tvalue>::red_black_tree(
    std::function<int(tkey const &, tkey const &)> keys_comparer,
    allocator *allocator,
    logger *logger,
    typename binary_search_tree<tkey, tvalue>::insertion_of_existent_key_attempt_strategy insertion_strategy,
    typename binary_search_tree<tkey, tvalue>::disposal_of_nonexistent_key_attempt_strategy disposal_strategy):
    binary_search_tree<tkey, tvalue>(new red_black_tree<tkey, tvalue>::insertion_template_method(this, insertion_strategy), new typename binary_search_tree<tkey, tvalue>::obtaining_template_method(dynamic_cast<binary_search_tree<tkey, tvalue> *>(this)), new red_black_tree<tkey, tvalue>::disposal_template_method(this, disposal_strategy), keys_comparer, allocator, logger)
{

}

template<typename tkey, typename tvalue>
red_black_tree<tkey, tvalue>::~red_black_tree() noexcept
{
    this->clear_tree(*reinterpret_cast<binary_search_tree<tkey, tvalue> *>(this));
}

template<typename tkey, typename tvalue>
red_black_tree<tkey, tvalue>::red_black_tree(red_black_tree<tkey, tvalue> const &other)
{
    this->copy_tree(*reinterpret_cast<binary_search_tree<tkey, tvalue> *>(&other));
}

template<typename tkey, typename tvalue>
red_black_tree<tkey, tvalue> &red_black_tree<tkey, tvalue>::operator=(
    red_black_tree<tkey, tvalue> const &other)
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
red_black_tree<tkey, tvalue>::red_black_tree(red_black_tree<tkey, tvalue> &&other) noexcept
{
    this->move_tree(*reinterpret_cast<binary_search_tree<tkey, tvalue> *>(&other));
}

template<typename tkey, typename tvalue>
red_black_tree<tkey, tvalue> &red_black_tree<tkey, tvalue>::operator=(red_black_tree<tkey, tvalue> &&other) noexcept
{
    if (this == &other)
    {
        return *this;
    }

    this->clear_tree(*reinterpret_cast<binary_search_tree<tkey, tvalue> *>(this));
    this->move_tree(*reinterpret_cast<binary_search_tree<tkey, tvalue> *>(&other));

    return *this;
}

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_RED_BLACK_TREE_H
