#ifndef HASHTBL_H
#define HASHTBL_H

#include <iostream>     // cout, endl, ostream
#include <forward_list> // forward_list
#include <algorithm>    // copy, find_if, for_each
#include <cmath>        // sqrt
#include <iterator>     // std::begin(), std::end()
#include <initializer_list>
#include <utility> // std::pair

namespace ac // Associative container
{
    template<class KeyType, class DataType>
    struct HashEntry {
        KeyType m_key;   //! Data key
        DataType m_data; //! The data -> dados associados a chave

        // Regular constructor.
        HashEntry(KeyType kt_, DataType dt_) : m_key{kt_}, m_data{dt_}
        {/*Empty*/}

        friend std::ostream& operator<<(std::ostream& os_, const HashEntry& he_) {
            os_ << "{" << he_.m_key << "," << he_.m_data << "}";
            // os_ << he_.m_data;
            return os_;
        }
    };

    template <class KeyType,
              class DataType,
              class KeyHash = std::hash<KeyType>,
              class KeyEqual = std::equal_to<KeyType>>
    class HashTbl {
    public:
        // Aliases
        using entry_type = HashEntry<KeyType, DataType>;
        using list_type = std::forward_list<entry_type>;
        using size_type = std::size_t;

        explicit HashTbl(size_type table_sz_ = DEFAULT_SIZE) {
            m_size = table_sz_;
            m_table = new list_type[m_size];
        }

        HashTbl(const HashTbl& other) {
            m_size = other.m_size;
            m_table = new list_type[m_size];

            for (size_type i = 0; i < m_size; ++i) {
                m_table[i] = other.m_table[i];
            }
        }

        HashTbl(const std::initializer_list<entry_type>& init_list) {
            m_size = init_list.size();
            m_table = new list_type[m_size];

            for (const auto& entry : init_list) {
                insert(entry.m_key, entry.m_data);
            }
        }

        HashTbl& operator=(const HashTbl& other) {
            if (this == &other)
                return *this;

            delete[] m_table;

            m_size = other.m_size;
            m_table = new list_type[m_size];

            for (size_type i = 0; i < m_size; ++i) {
                m_table[i] = other.m_table[i];
            }

            return *this;
        }

        HashTbl& operator=(const std::initializer_list<entry_type>& init_list) {
            m_size = init_list.size();
            delete[] m_table; // Libera a memória alocada anteriormente
            m_table = new list_type[m_size];

            for (const auto& entry : init_list) {
                insert(entry.m_key, entry.m_data);
            }

            return *this;
        }

        virtual ~HashTbl() {
            clear();
            delete[] m_table;
        }

        bool insert(const KeyType& key, const DataType& data) {
            size_type i = KeyHash()(key) % m_size;
            list_type& guarda = m_table[i];

            // Verifica se a chave já existe na lista
            auto iter = std::find_if(guarda.begin(), guarda.end(), [&key](const entry_type& entry) {
                return KeyEqual()(entry.m_key, key);
            });

            if (iter != guarda.end()) {
                return false; // Chave já existente
            }

            guarda.push_front(entry_type(key, data));
            return true;
        }

        bool remove(const KeyType& key) {
            size_type i = KeyHash()(key) % m_size;
            list_type& guarda = m_table[i];

            // Verifica se a chave existe na lista
            auto iter = std::find_if(guarda.begin(), guarda.end(), [&key](const entry_type& entry) {
                return KeyEqual()(entry.m_key, key);
            });

            if (iter != guarda.end()) {
                guarda.remove(*iter);
                return true;
            }

            return false;
        }

        DataType& find(const KeyType& key) {
            size_type i = KeyHash()(key) % m_size;
            list_type& guarda = m_table[i];

            // Verifica se a chave existe na lista
            auto iter = std::find_if(guarda.begin(), guarda.end(), [&key](const entry_type& entry) {
                return KeyEqual()(entry.m_key, key);
            });

            if (iter != guarda.end()) {
                return iter->m_data;
            }

            throw std::out_of_range("Key not found");
        }

        bool contains(const KeyType& key) const {
            size_type i = KeyHash()(key) % m_size;
            const list_type& guarda = m_table[i];

            // Verifica se a chave existe na lista
            auto iter = std::find_if(guarda.begin(), guarda.end(), [&key](const entry_type& entry) {
                return KeyEqual()(entry.m_key, key);
            });

            return iter != guarda.end();
        }

        void clear() {
            for (size_type i = 0; i < m_size; ++i) {
                m_table[i].clear();
            }
        }

        size_type size() const {
            size_type count = 0;
            for (size_type i = 0; i < m_size; ++i) {
                count += std::distance(m_table[i].begin(), m_table[i].end());
            }
            return count;
        }

        bool empty() const {
            for (size_type i = 0; i < m_size; ++i) {
                if (!m_table[i].empty()) {
                    return false;
                }
            }
            return true;
        }

        size_type bucket_count() const {
            return m_size;
        }

        float load_factor() const {
            return static_cast<float>(size()) / m_size;
        }

    private:
        static constexpr size_type DEFAULT_SIZE = 10;

        size_type m_size;
        list_type* m_table;
    };

} // namespace ac

#endif // HASHTBL_H
