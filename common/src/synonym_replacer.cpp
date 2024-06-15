#include "../include/synonym_replacer.h"

synonym_replacer::synonym_replacer(associative_container<std::string, std::string> *synonyms_holder, missing_word_replacement_strategy strategy):
    _synonyms_holder(synonyms_holder), _strategy(strategy)
{

}

void synonym_replacer::replace(std::string const &filepath)
{
    std::ifstream in;
    in.open(filepath);

    if (!in.is_open())
    {
        throw std::runtime_error("Unable to open file");
    }

    std::ostringstream changed, unchanged;
    char symbol;
    std::string word;
    while (in >> symbol)
    {
        if (symbol != ' ' && symbol != '\n')
        {
            word.push_back(symbol);
            continue;
        }
        
        changed << symbol;
        unchanged << symbol;

        if (word.empty())
        {
            continue;
        }

        std::string replace_with;
        try
        {
            replace_with = _synonyms_holder->obtain(word);
        }
        catch (std::logic_error const &exception)
        {
            switch (_strategy)
            {
            case missing_word_replacement_strategy::new_synonym:
                _synonyms_holder->insert(replace_with, _main_reference);
                break;
            
            case missing_word_replacement_strategy::new_reference:
                _synonyms_holder->insert(_main_synonym, replace_with);
                break;
            
            case missing_word_replacement_strategy::change_reference:
                const_cast<std::string &>(_synonyms_holder->obtain(_main_synonym)) = replace_with;
                break;
            
            default: 
                break;
            }
        }
            
        changed << replace_with;
        unchanged << word;

        word.clear();
    }

    in.close();
    
    std::ofstream out;
    out.open(filepath);
    if (!out.is_open())
    {
        throw std::runtime_error("Unable to open file");
    }
    out << changed.str();
    
    _unchanged_files.push(std::make_pair(filepath, unchanged.str()));
}

void synonym_replacer::add_synonyms_from_file(std::string const &filepath)
{
    std::ifstream in;
    in.open(filepath);

    if (!in.is_open())
    {
        throw std::runtime_error("Unable to open file");
    }

    std::string line;
    while (std::getline(in, line))
    {
        auto parsed_line = split(line);
        for (auto const &synonym: parsed_line.second)
        {
            _synonyms_holder->insert(synonym, parsed_line.first);
            _actions.push({synonym, parsed_line.first, action::insert});
        }
        _synonyms_holder->insert(parsed_line.first, parsed_line.first);
    }

    in.close();
}

void synonym_replacer::add_synonym(std::string const &reference_word, std::string const &synonym_word)
{
    _synonyms_holder->insert(synonym_word, reference_word);
    _actions.push({synonym_word, reference_word, action::insert});
}

void synonym_replacer::undo(size_t actions_amount)
{
    for (size_t i = 0; i < actions_amount; ++i)
    {
        auto top = _actions.top();
        switch (std::get<2>(top))
        {
        case action::insert:
            _synonyms_holder->dispose(std::get<0>(top));
            break;
        
        case action::remove:
            _synonyms_holder->insert(std::get<0>(top), std::get<1>(top));
            break;
        
        case action::change:
            std::ofstream out;
            out.open(_unchanged_files.top().first);
            if (!out.is_open())
            {
                throw std::runtime_error("Unable to open file for changes discarding");
            }
            out << _unchanged_files.top().second;
            _unchanged_files.pop();
            out.close();
            break;
        }
        _actions.pop();
    }
}

std::pair<std::string, std::vector<std::string>> synonym_replacer::split(std::string const &line)
{
    std::pair<std::string, std::vector<std::string>> result;
    std::istringstream ss(line);

    std::getline(ss, result.first, '{');
    result.first.erase(std::remove(result.first.begin(), result.first.end(), '<'), result.first.end());
    result.first.erase(std::remove(result.first.begin(), result.first.end(), '>'), result.first.end());
    result.first = result.first.substr(0, result.first.find_last_not_of(' ') + 1);

    std::string synonyms;
    std::getline(ss, synonyms, '}');
    synonyms = synonyms.substr(synonyms.find_first_not_of(' '));

    std::istringstream synonyms_stream(synonyms);
    std::string synonym;
    while (std::getline(synonyms_stream, synonym, ',')) {
        synonym = synonym.substr(0, synonym.find_last_not_of(' ') + 1);
        result.second.push_back(synonym);
    }

    return result;
}