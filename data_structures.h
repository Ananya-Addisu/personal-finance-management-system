#pragma once
#include <queue>
#include <unordered_map>
#include <memory>
#include <functional>
#include <string>
#include <vector>
#include "date.h"

// Forward declarations
class Date;
class Transaction;

// Priority Queue for upcoming payments/investments
struct UpcomingPayment {
    Date dueDate;
    std::string description;
    double amount;
    bool isInvestment;
    
    UpcomingPayment(const Date& date, const std::string& desc, double amt, bool inv = false)
        : dueDate(date), description(desc), amount(amt), isInvestment(inv) {}
};

struct PaymentCompare {
    bool operator()(const UpcomingPayment& a, const UpcomingPayment& b) const {
        // Compare dates (implement proper date comparison)
        if (a.dueDate.year != b.dueDate.year) return a.dueDate.year > b.dueDate.year;
        if (a.dueDate.month != b.dueDate.month) return a.dueDate.month > b.dueDate.month;
        return a.dueDate.day > b.dueDate.day;
    }
};

// Trie for autocomplete
class TrieNode {
public:
    std::unordered_map<char, std::unique_ptr<TrieNode>> children;
    bool isEndOfWord;
    
    TrieNode() : isEndOfWord(false) {}
};

class Trie {
private:
    std::unique_ptr<TrieNode> root;
    
    void getSuggestionsRecursive(TrieNode* node, std::string& prefix, std::vector<std::string>& result) {
        if (node->isEndOfWord) {
            result.push_back(prefix);
        }
        
        for (const auto& pair : node->children) {
            prefix.push_back(pair.first);
            getSuggestionsRecursive(pair.second.get(), prefix, result);
            prefix.pop_back();
        }
    }

public:
    Trie() : root(std::make_unique<TrieNode>()) {}
    
    void insert(const std::string& word) {
        TrieNode* current = root.get();
        for (char c : word) {
            if (!current->children[c]) {
                current->children[c] = std::make_unique<TrieNode>();
            }
            current = current->children[c].get();
        }
        current->isEndOfWord = true;
    }
    
    std::vector<std::string> getSuggestions(const std::string& prefix) {
        std::vector<std::string> suggestions;
        TrieNode* current = root.get();
        
        // Traverse to prefix node
        for (char c : prefix) {
            if (!current->children[c]) {
                return suggestions;
            }
            current = current->children[c].get();
        }
        
        // Get all words with this prefix
        std::string currentPrefix = prefix;
        getSuggestionsRecursive(current, currentPrefix, suggestions);
        return suggestions;
    }
};

// Transaction ID generator and hash table
class TransactionIndex {
private:
    std::unordered_map<std::string, Transaction*> transactionMap;
    int nextId;
    
    std::string generateId() {
        return "TXN" + std::to_string(++nextId);
    }

public:
    TransactionIndex() : nextId(0) {}
    
    std::string addTransaction(Transaction* transaction) {
        std::string id = generateId();
        transactionMap[id] = transaction;
        return id;
    }
    
    Transaction* getTransaction(const std::string& id) {
        auto it = transactionMap.find(id);
        return it != transactionMap.end() ? it->second : nullptr;
    }
}; 