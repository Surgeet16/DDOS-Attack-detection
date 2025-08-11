#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <unordered_map>

// Trie Node for the Aho-Corasick automaton
struct TrieNode {
    std::unordered_map<char, TrieNode*> children;
    TrieNode* failure_link = nullptr;
    std::vector<std::string> output; // Patterns that end at this node
    TrieNode() = default;
};

class AhoCorasick {
private:
    TrieNode* root;

public:
    AhoCorasick(const std::vector<std::string>& patterns) {
        root = new TrieNode();
        build_trie(patterns);
        build_failure_links();
    }

    ~AhoCorasick() {
        // Proper memory cleanup for the trie is needed for production code
    }

    // Build the initial trie from the dictionary of patterns
    void build_trie(const std::vector<std::string>& patterns) {
        for (const auto& pattern : patterns) {
            TrieNode* current = root;
            for (char ch : pattern) {
                if (current->children.find(ch) == current->children.end()) {
                    current->children[ch] = new TrieNode();
                }
                current = current->children[ch];
            }
            current->output.push_back(pattern);
        }
    }

    // Build the failure links using a breadth-first search (BFS)
    void build_failure_links() {
        std::queue<TrieNode*> q;
        for (auto& pair : root->children) {
            q.push(pair.second);
            pair.second->failure_link = root;
        }

        while (!q.empty()) {
            TrieNode* current = q.front();
            q.pop();

            for (auto& pair : current->children) {
                char ch = pair.first;
                TrieNode* child = pair.second;
                TrieNode* temp = current->failure_link;

                while (temp != nullptr && temp->children.find(ch) == temp->children.end()) {
                    temp = temp->failure_link;
                }

                if (temp == nullptr) {
                    child->failure_link = root;
                } else {
                    child->failure_link = temp->children[ch];
                }

                // Append output of failure link to the current child's output
                child->output.insert(child->output.end(), child->failure_link->output.begin(), child->failure_link->output.end());
                q.push(child);
            }
        }
    }

    // Search for all patterns in a given text
    std::vector<std::string> search(const std::string& text) {
        std::vector<std::string> found_patterns;
        TrieNode* current = root;

        for (char ch : text) {
            while (current != nullptr && current->children.find(ch) == current->children.end()) {
                current = current->failure_link;
            }

            if (current == nullptr) {
                current = root;
                continue;
            }

            current = current->children[ch];
            if (!current->output.empty()) {
                found_patterns.insert(found_patterns.end(), current->output.begin(), current->output.end());
            }
        }
        return found_patterns;
    }
};
