#include "hashtbl.h"

namespace ac
{
    template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
    HashTbl<KeyType, DataType, KeyHash, KeyEqual>::HashTbl(size_type sz)
    {
        m_size = sz;
        m_table = new list_type[m_size];
    }

    template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
    HashTbl<KeyType, DataType, KeyHash, KeyEqual>::HashTbl(const HashTbl &source)
    {
        m_size = source.m_size;
        m_table = source.m_table;
    }

    template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
    HashTbl<KeyType, DataType, KeyHash, KeyEqual>::HashTbl(const std::initializer_list<entry_type> &ilist)
    {
        m_size = ilist.size();
        m_table = new list_type[m_size];

        for (const auto &entry : ilist)
        {
            insert(entry.m_key, entry.m_data);
        }
    }

    template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
    HashTbl<KeyType, DataType, KeyHash, KeyEqual> &
    HashTbl<KeyType, DataType, KeyHash, KeyEqual>::operator=(const HashTbl &clone)
    {
        if (this == &clone)
            return *this;

        m_size = clone.m_size;
        m_table = clone.m_table;

        return *this;
    }

    template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
    HashTbl<KeyType, DataType, KeyHash, KeyEqual> &
    HashTbl<KeyType, DataType, KeyHash, KeyEqual>::operator=(const std::initializer_list<entry_type> &ilist)
    {
        m_size = ilist.size();
        delete[] m_table; // Libera a memória alocada anteriormente
        m_table = new list_type[m_size];

        for (const auto &entry : ilist)
        {
            insert(entry.m_key, entry.m_data);
        }

        return *this;
    }

    template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
    HashTbl<KeyType, DataType, KeyHash, KeyEqual>::~HashTbl()
    {
        clear();
    }

    template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
    bool HashTbl<KeyType, DataType, KeyHash, KeyEqual>::insert(const KeyType &key_, const DataType &new_data_)
    {
        size_type i = KeyHash()(key_) % m_size;
        list_type &guarda = m_table[i];

        // Verifica se a chave já existe na lista
        auto iter = std::find_if(guarda.begin(), guarda.end(), [&key_](const entry_type &entry)
                                 { return KeyEqual()(entry.m_key, key_); });

        if (iter != guarda.end())
        {
            return false; // A chave já existe, não pode inserir novamente
        }

        // Insere a nova entrada na lista
        guarda.push_front(entry_type(key_, new_data_));

        return true;
    }

    template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
    void HashTbl<KeyType, DataType, KeyHash, KeyEqual>::clear()
    {
        for (size_type i = 0; i < m_size; ++i)
        {
            m_table[i].clear();
        }
        m_count = 0;
    }

    template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
    bool HashTbl<KeyType, DataType, KeyHash, KeyEqual>::empty() const
    {
        for (size_type i = 0; i < m_size; ++i)
        {
            if (!m_table[i].empty())
            {
                return false;
            }
        }
        return true;
    }

    template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
    bool HashTbl<KeyType, DataType, KeyHash, KeyEqual>::retrieve(const KeyType &key_, DataType &data_item_) const
    {
        size_type i = KeyHash()(key_) % m_size;
        const list_type &guarda = m_table[i];

        // Procura pela chave na lista
        auto iter = std::find_if(guarda.begin(), guarda.end(), [&key_](const entry_type &entry)
                                 { return KeyEqual()(entry.m_key, key_); });

        if (iter != guarda.end())
        {
            data_item_ = iter->m_data; // Armazena o dado encontrado na variável de saída
            return true;               // A chave foi encontrada
        }

        return false; // A chave não foi encontrada
    }

    template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
    void HashTbl<KeyType, DataType, KeyHash, KeyEqual>::rehash(void)
    {
        size_type new_table_size = find_next_prime(m_size * 2);
        list_type *new_table = new list_type[new_table_size];

        for (size_type i = 0; i < m_size; ++i)
        {
            for (const auto &entry : m_table[i])
            {
                size_type new_index = KeyHash()(entry.m_key) % new_table_size;
                new_table[new_index].push_front(entry);
            }
        }

        delete[] m_table;
        m_table = new_table;
        m_size = new_table_size;
    }

    template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
    bool HashTbl<KeyType, DataType, KeyHash, KeyEqual>::erase(const KeyType &key_)
    {
        size_type i = KeyHash()(key_) % m_size;
        list_type &guarda = m_table[i];

        auto prev = guarda.before_begin();
        auto curr = guarda.begin();

        while (curr != guarda.end())
        {
            if (KeyEqual()(curr->m_key, key_))
            {
                guarda.erase_after(prev);
                --m_count;
                return true;
            }
            ++prev;
            ++curr;
        }

        return false;
    }

    template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
    std::size_t HashTbl<KeyType, DataType, KeyHash, KeyEqual>::find_next_prime(size_type n_)
    {
        if (n_ <= 2)
            return 2;

        bool prime = false;
        size_type next_prime = n_;

        while (!prime)
        {
            ++next_prime;
            prime = true;

            for (size_type i = 2; i <= std::sqrt(next_prime); ++i)
            {
                if (next_prime % i == 0)
                {
                    prime = false;
                    break;
                }
            }
        }

        return next_prime;
    }

    template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
    typename HashTbl<KeyType, DataType, KeyHash, KeyEqual>::size_type
    HashTbl<KeyType, DataType, KeyHash, KeyEqual>::count(const KeyType &key_) const
    {
        size_type i = KeyHash()(key_) % m_size;
        const list_type &guarda = m_table[i];

        // Verifica se a chave está presente na lista
        auto iter = std::find_if(guarda.begin(), guarda.end(), [&key_](const entry_type &entry)
                                 { return KeyEqual()(entry.m_key, key_); });

        if (iter != guarda.end())
        {
            return 1; // A chave foi encontrada, retorna 1
        }

        return 0; // A chave não foi encontrada, retorna 0
    }

    template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
    DataType &HashTbl<KeyType, DataType, KeyHash, KeyEqual>::at(const KeyType &key_)
    {
        size_type i = KeyHash()(key_) % m_size;
        list_type &guarda = m_table[i];

        // Procura pela chave na lista
        auto iter = std::find_if(guarda.begin(), guarda.end(), [&key_](const entry_type &entry)
                                 { return KeyEqual()(entry.m_key, key_); });

        if (iter != guarda.end())
        {
            return iter->m_data; // Retorna uma referência para o dado encontrado
        }

        throw std::out_of_range("Key not found in HashTbl");
    }

    template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
    DataType &HashTbl<KeyType, DataType, KeyHash, KeyEqual>::operator[](const KeyType &key_)
    {
        size_type i = KeyHash()(key_) % m_size;
        list_type &guarda = m_table[i];

        // Procura pela chave na lista
        auto iter = std::find_if(guarda.begin(), guarda.end(), [&key_](const entry_type &entry)
                                 { return KeyEqual()(entry.m_key, key_); });

        if (iter != guarda.end())
        {
            return iter->m_data; // Retorna uma referência para o dado encontrado
        }

        // Insere uma nova entrada com a chave e um valor padrão para o dado
        guarda.push_front(entry_type(key_, DataType()));
        ++m_count;

        return guarda.front().m_data;
    }
} // Namespace ac.
