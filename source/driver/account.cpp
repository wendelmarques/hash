/*!
 * @file: account.cpp
 */
#include "account.h"

#include <utility>

/// Basic constructor.
Account::Account(std::string n, int bnc, int brc, int nmr, float bal)
    : m_name{std::move( n )}, m_bank_code{ bnc }, m_branch_code{ brc }, m_number{ nmr }, m_balance{ bal }
{ /* Empty */
}

/// Returns the account key.
Account::AcctKey Account::getKey() const
{
    return std::make_tuple(m_name, m_bank_code, m_branch_code, m_number);
}

std::ostream& operator<<(std::ostream& os_, const Account::AcctKey& ak_)
{
    const auto& [name, bkid, brid, accn] = ak_;
    return os_ << "K{" << name << "," << bkid << "," << brid << "," << accn << "}";
}

/// Stream extractor of the account information.
std::ostream& operator<<(std::ostream& os, const Account& acct)
{
    return os << "[ Client: <" << acct.m_name << "> Bank: <" << acct.m_bank_code << "> Branch: <"
               << acct.m_branch_code << "> Number: <" << acct.m_number << "> Balance: <"
               << acct.m_balance << "> ]";
}

/// Compare two accounts
bool operator==(const Account& a, const Account& b)
{
    return (a.m_name == b.m_name and a.m_bank_code == b.m_bank_code
            and a.m_branch_code == b.m_branch_code and a.m_number == b.m_number
            and a.m_balance == b.m_balance);
}

std::size_t KeyHash::operator()(const Account::AcctKey& k_) const
{
    const auto& [name, bkid, brid, accn] = k_;
    return std::hash<std::string>{}(name) xor std::hash<int>{}(bkid) xor std::hash<int>{}(brid)
           xor std::hash<int>{}(accn);
}

// Functor that test two keys for equality.
bool KeyEqual::operator()(const Account::AcctKey& k1_, const Account::AcctKey& k2_) const
{
    const auto& [name1, bkid1, brid1, accn1] = k1_;
    const auto& [name2, bkid2, brid2, accn2] = k2_;
    return name1 == name2 and bkid1 == bkid2 and brid1 == brid2 and accn1 == accn2;
}
