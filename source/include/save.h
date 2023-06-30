// @author: Selan
//
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
        HashEntry( KeyType kt_, DataType dt_ ) : m_key{kt_} , m_data{dt_}
        {/*Empty*/}

        friend std::ostream & operator<<( std::ostream & os_, const HashEntry & he_ ) {
            os_ << "{" << he_.m_key << "," << he_.m_data << "}";
            // os_ << he_.m_data;
            return os_;
        }
    };

	template< class KeyType,
		      class DataType,
		      class KeyHash = std::hash< KeyType >,
		      class KeyEqual = std::equal_to< KeyType > >
	class HashTbl {
        public:
            // Aliases
            using entry_type = HashEntry<KeyType,DataType>;
            using list_type  = std::forward_list< entry_type >;
            using size_type  = std::size_t;

            
            explicit HashTbl( size_type table_sz_ = DEFAULT_SIZE ) {
                m_size = table_sz_;
                m_table = new list_type[m_size];
            };

            HashTbl( const HashTbl& other) {
                m_size = other.m_size;
                m_table = other.m_table;
            };

            HashTbl( const std::initializer_list< entry_type > & init_list) {
                m_size = init_list.size();
                m_table = new list_type[m_size];
                
                for (const auto& entry : init_list) {
                    insert(entry.m_key, entry.m_data);
                }
            };

            HashTbl& operator=( const HashTbl& other) {
                if (this == &other) 
                    return *this;
                
                m_size = other.m_size;
                m_table = other.m_table;
                
                return *this;
            };

            HashTbl& operator=( const std::initializer_list< entry_type > & init_list) {
                m_size = init_list.size();
                delete[] m_table; // Libera a memória alocada anteriormente
                m_table = new list_type[m_size];

                for (const auto& entry : init_list) {
                    insert(entry.m_key, entry.m_data);
                }

                return *this;
            };

            virtual ~HashTbl() {
                clear();
            };

            bool insert( const KeyType & key, const DataType & data) {
                size_type i = KeyHash()(key) % m_size;
                list_type& guarda = m_table[i];
                
                // Verifica se a chave já existe na lista
                auto iter = std::find_if(guarda.begin(), guarda.end(), [&key](const entry_type& entry) {
                    return KeyEqual()(entry.m_key, key);
                });
                
                if (iter != guarda.end()) {
                    return false; // A chave já existe, não pode inserir novamente
                }
                
                // Insere a nova entrada na lista
                guarda.push_front(entry_type(key, data));
                
                return true;
            };
            bool retrieve( const KeyType & key, DataType & data) const {
                size_type i = KeyHash()(key) % m_size;
                const list_type& guarda = m_table[i];
                
                // Procura pela chave na lista
                auto iter = std::find_if(guarda.begin(), guarda.end(), [&key](const entry_type& entry) {
                    return KeyEqual()(entry.m_key, key);
                });
                
                if (iter != guarda.end()) {
                    data = iter->m_data; // Armazena o dado encontrado na variável de saída
                    return true; // A chave foi encontrada
                }
                
                return false; // A chave não foi encontrada
            };
            bool erase( const KeyType & key) {
                size_type i = KeyHash()(key) % m_size;
                list_type& guarda = m_table[i];

                auto prev = guarda.before_begin();
                auto curr = guarda.begin();

                while (curr != guarda.end()) {
                    if (KeyEqual()(curr->m_key, key)) {
                        guarda.erase_after(prev);
                        --m_count;
                        return true;
                    }
                    ++prev;
                    ++curr;
                }

                return false;
            };
            void clear() {
                for (size_type i = 0; i < m_size; ++i) {
                    m_table[i].clear();
                }
                m_count = 0;
            };
            bool empty() const {
                for (const auto& guarda : m_table) {
                    if (!guarda.empty()) {
                        return false; // A tabela hash não está vazia se alguma lista de colisão não estiver vazia
                    }
                }
            
                return true; // Todas as listas de colisão estão vazias, portanto, a tabela hash está vazia
            };
            inline size_type size() const { return m_count; };
            DataType& at( const KeyType& key) {
                size_type i = KeyHash()(key) % m_size;
                list_type& guarda = m_table[i];

                // Procura pela chave na lista
                auto iter = std::find_if(guarda.begin(), guarda.end(), [&key](const entry_type& entry) {
                    return KeyEqual()(entry.m_key, key);
                });

                if (iter != guarda.end()) {
                    return iter->m_data; // Retorna uma referência para o dado encontrado
                }

                throw std::out_of_range("Key not found in HashTbl");
            };
            DataType& operator[]( const KeyType& key) {
                size_type i = KeyHash()(key) % m_size;
                list_type& guarda = m_table[i];

                // Procura pela chave na lista
                auto iter = std::find_if(guarda.begin(), guarda.end(), [&key](const entry_type& entry) {
                    return KeyEqual()(entry.m_key, key);
                });

                if (iter != guarda.end()) {
                    return iter->m_data; // Retorna uma referência para o dado encontrado
                }

                // Insere uma nova entrada com a chave e um valor padrão para o dado
                guarda.push_front(entry_type(key, DataType()));
                ++m_count;

                return guarda.front().m_data;
            };
            size_type count( const KeyType& key) const {
                size_type i = KeyHash()(key) % m_size;
                const list_type& guarda = m_table[i];

                // Verifica se a chave está presente na lista
                auto iter = std::find_if(guarda.begin(), guarda.end(), [&key](const entry_type& entry) {
                    return KeyEqual()(entry.m_key, key);
                });

                if (iter != guarda.end()) {
                    return 1; // A chave foi encontrada, retorna 1
                }

                return 0; // A chave não foi encontrada, retorna 0
            };
            float max_load_factor() const { return max_load_factor; };
            void max_load_factor(float mlf) { max_load_factor = mlf; };

            friend std::ostream & operator<<( std::ostream & os_, const HashTbl & ht_ ) {
                for (size_type i = 0; i < ht_.m_size; ++i) {
                        const list_type& guarda = ht_.m_table[i];
                        for (const auto& entry : guarda) {
                            os_ << entry << " "; // Imprime cada entrada da tabela hash
                        }
                    }
                return os_;
            }

        private:
            static size_type find_next_prime( size_type n) {
                    if (n <= 2) 
                        return 2;

                    bool prime = false;
                    size_type next_prime = n;

                    while (!prime) {
                        ++next_prime;
                        prime = true;

                        for (size_type i = 2; i <= std::sqrt(next_prime); ++i) {
                            if (next_prime % i == 0) {
                                prime = false;
                                break;
                            }
                        }
                    }

                    return next_prime;
            };
            void rehash( void ) {
                size_type new_table_size = find_next_prime(m_size * 2);
                list_type* new_table = new list_type[new_table_size];

                for (size_type i = 0; i < m_size; ++i) {
                    for (const auto& entry : m_table[i]) {
                        size_type new_index = KeyHash()(entry.m_key) % new_table_size;
                        new_table[new_index].push_front(entry);
                    }
                }

                delete[] m_table;
                m_table = new_table;
                m_size = new_table_size;
            };

        private:
            size_type m_size; //!< Tamanho da tabela.
            size_type m_count;//!< Numero de elementos na tabel.
            // std::unique_ptr< std::forward_list< entry_type > [] > m_table;
            std::forward_list< entry_type > *m_table; //!< Tabela de listas para entradas de tabela.
            static const short DEFAULT_SIZE = 10;
    };

} // MyHashTable
#include "hashtbl.inl"
#endif
