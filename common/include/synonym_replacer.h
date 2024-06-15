#ifndef SYNONYM_REPLACER_H
#define SYNONYM_REPLACER_H

#include <algorithm>
#include <string>
#include <sstream>

#include <associative_container.h>
#include <hash_table.h>

class synonym_replacer final
{

public:

    enum class missing_word_replacement_strategy
    {
        ignore,
        new_synonym,
        new_reference,
        change_reference
    };

public:

    explicit synonym_replacer(
        associative_container<std::string, std::string> *synonyms_holder,
        missing_word_replacement_strategy strategy = missing_word_replacement_strategy::ignore);

public:

    void replace(std::string const &filepath);
    
    void add_synonyms_from_file(std::string const &filepath);

    void add_synonym(std::string const &reference_word, std::string const &synonym_word);

    void undo(size_t actions_amount);

    void set_main_reference(std::string const &word);

    void set_main_synonym(std::string const &word);

public:

    ~synonym_replacer();

    synonym_replacer(synonym_replacer const &other);

    synonym_replacer& operator=(synonym_replacer const &other);

    synonym_replacer(synonym_replacer &&other) noexcept;

    synonym_replacer& operator=(synonym_replacer &&other) noexcept;

private:

    std::pair<std::string, std::vector<std::string>> split(std::string const &line);

private:

    enum class action
    {
        insert,
        remove,
        change
    };

private:

    associative_container<std::string, std::string> *_synonyms_holder;

    missing_word_replacement_strategy _strategy;

    std::stack<std::tuple<std::string, std::string, action>> _actions;

    std::stack<std::pair<std::string, std::string>> _unchanged_files;

    std::string _main_reference;

    std::string _main_synonym;

};

#endif