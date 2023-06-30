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
#include <tuple>

namespace ac // Associative container
{
	template<class KeyType, class DataType>
	struct HashEntry {
        KeyType m_key;   //! Data key
        DataType m_data; //! The data

        // Regular constructor.
        HashEntry( KeyType kt_, DataType dt_ ) : m_key{kt_} , m_data{dt_} {/*Empty*/}

        /*friend std::ostream & operator<<( std::ostream & os_, const HashEntry & he_ ) {
            os_ << "{" << he_.m_key << "," << he_.m_data << "}";
            // os_ << he_.m_data;
            return os_;
        }*/

        friend std::ostream& operator<<(std::ostream& os, const std::tuple<KeyType, DataType>& tuple) {
            os << "{" << std::get<0>(tuple) << "," << std::get<1>(tuple) << "}";
            return os;
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

            explicit HashTbl( size_type table_sz_ = DEFAULT_SIZE );
            HashTbl( const HashTbl& );
            HashTbl( const std::initializer_list< entry_type > & );
            HashTbl& operator=( const HashTbl& );
            HashTbl& operator=( const std::initializer_list< entry_type > & );

            virtual ~HashTbl();

            bool insert( const KeyType &, const DataType &  );
            bool retrieve( const KeyType &, DataType & ) const;
            bool erase( const KeyType & );
            void clear();
            bool empty() const;
            inline size_type size() const { return m_count; };
            DataType& at( const KeyType& );
            DataType& operator[]( const KeyType& );
            size_type count( const KeyType& ) const;
            float max_load_factor() const;
            void max_load_factor(float mlf);

            friend std::ostream & operator<<( std::ostream & os_, const HashTbl & ht_ ) {
                for (size_type i = 0; i < ht_.m_size; ++i) {
                    for (const auto& entry : ht_.m_table[i]) {
                        os_ << "{" << entry.m_key << "," << entry.m_data << "} ";
                    }
                }
                return os_;
            }

        private:
            static size_type find_next_prime( size_type );
            void rehash( void );

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