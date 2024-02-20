//
// Sevdenaz Yılmaz 30300
//
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

using namespace std;

// Struct to store document information
struct DocumentItem {
    string documentName;
    int count;
};

// Struct to store word information
struct WordItem {
    string word;
    int totalOccurrence; // Total occurrence of the word in all documents
    vector<DocumentItem> documentCounts; // Vector to store document counts for each word
};

// AVL Search Tree class
template <class Key, class Value>
class AVLSearchTree {
private:
    struct Node {
        Key key;
        Value data;
        Node* left;
        Node* right;
        int height;
    };

    Node* root;

    int getHeight(Node* node) {
        if (!node)
            return 0;
        return node->height;
    }

    int getBalance(Node* node) {
        if (!node)
            return 0;
        return getHeight(node->left) - getHeight(node->right);
    }

    Node* rotateRight(Node* y) {
        Node* x = y->left;
        Node* T2 = x->right;

        x->right = y;
        y->left = T2;

        y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
        x->height = max(getHeight(x->left), getHeight(x->right)) + 1;

        return x;
    }

    Node* rotateLeft(Node* x) {
        Node* y = x->right;
        Node* T2 = y->left;

        y->left = x;
        x->right = T2;

        x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
        y->height = max(getHeight(y->left), getHeight(y->right)) + 1;

        return y;
    }

    Node* insert(Node* node, Key key, Value data) {
        if (!node)
            return new Node{key, data, nullptr, nullptr, 1};

        if (key < node->key)
            node->left = insert(node->left, key, data);
        else if (key > node->key)
            node->right = insert(node->right, key, data);
        else {
            // Key already exists, update data
            node->data = data;
            return node;
        }

        node->height = 1 + max(getHeight(node->left), getHeight(node->right));

        int balance = getBalance(node);

        // Left-Left Case
        if (balance > 1 && key < node->left->key)
            return rotateRight(node);

        // Right-Right Case
        if (balance < -1 && key > node->right->key)
            return rotateLeft(node);

        // Left-Right Case
        if (balance > 1 && key > node->left->key) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }

        // Right-Left Case
        if (balance < -1 && key < node->right->key) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }

        return node;
    }

    Node* remove(Node* node, Key key) {
        if (!node)
            return nullptr;

        if (key < node->key)
            node->left = remove(node->left, key);
        else if (key > node->key)
            node->right = remove(node->right, key);
        else {
            // Node with only one child or no child
            if (!node->left || !node->right) {
                Node* temp = node->left ? node->left : node->right;

                // No child case
                if (!temp) {
                    temp = node;
                    node = nullptr;
                } else // One child case
                    *node = *temp; // Copy the contents of the non-empty child

                delete temp;
            } else {
                // Node with two children: Get the inorder successor (smallest in the right subtree)
                Node* temp = minValueNode(node->right);

                // Copy the inorder successor's data to this node
                node->key = temp->key;
                node->data = temp->data;

                // Delete the inorder successor
                node->right = remove(node->right, temp->key);
            }
        }

        // If the tree had only one node, then return
        if (!node)
            return nullptr;

        // Update height of the current node
        node->height = 1 + max(getHeight(node->left), getHeight(node->right));

        // Get the balance factor of this node (to check whether this node became unbalanced)
        int balance = getBalance(node);

        // Left-Left Case
        if (balance > 1 && getBalance(node->left) >= 0)
            return rotateRight(node);

        // Left-Right Case
        if (balance > 1 && getBalance(node->left) < 0) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }

        // Right-Right Case
        if (balance < -1 && getBalance(node->right) <= 0)
            return rotateLeft(node);

        // Right-Left Case
        if (balance < -1 && getBalance(node->right) > 0) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }

        return node;
    }

    Node* minValueNode(Node* node) {
        Node* current = node;

        // Loop down to find the leftmost leaf
        while (current->left)
            current = current->left;

        return current;
    }

    Value search(Node* node, Key key) {
        if (!node)
            return Value(); // Return default-constructed Value when key is not found

        if (key < node->key)
            return search(node->left, key);
        else if (key > node->key)
            return search(node->right, key);
        else
            return node->data;
    }

public:
    AVLSearchTree() : root(nullptr) {}

    void insert(Key key, Value data) {
        root = insert(root, key, data);
    }

    void remove(Key key) {
        root = remove(root, key);
    }

    Value search(Key key) {
        return search(root, key);
    }

    // Function to search query in the AVL Search Tree
    void searchQuery(const string& query) {
        istringstream iss(query);
        string word;
        vector<pair<string, vector<pair<string, int>>>> result;

        while (iss >> word) {
            transform(word.begin(), word.end(), word.begin(), ::tolower);

            WordItem foundWordItem = search(word);
            if (!foundWordItem.word.empty()) {
                vector<pair<string, int>> documentData;
                for (const auto& documentCount : foundWordItem.documentCounts) {
                    documentData.emplace_back(documentCount.documentName, documentCount.count);
                }
                result.emplace_back(word, documentData);
            } else {
                cout << "No document contains the given query" << endl;
                return;
            }
        }

        // Print the result in the specified format
        for (const auto& documentData : result[0].second) {
            cout << "in Document " << documentData.first << ",";

            bool isFirstWord = true; // Flag to check if it's the first word for comma placement

            for (const auto& wordData : result) {
                for (const auto& wordDocumentCount : wordData.second) {
                    if (wordDocumentCount.first == documentData.first) {
                        // If it's not the first word, add a comma
                        if (!isFirstWord) {
                            cout << ",";
                        } else {
                            isFirstWord = false;
                        }

                        cout << " " << wordData.first << " found " << wordDocumentCount.second << " times";
                        break;
                    }
                }
            }
            cout << "." << endl;
        }
    }
};

// Function to preprocess text and update the AVL Search Tree
void preprocessText(const string& documentName, const string& text, AVLSearchTree<string, WordItem>& tree) {
    istringstream iss(text);
    string word;

    while (iss >> word) {
        // Remove non-alphabetic characters and convert to lowercase
        word.erase(remove_if(word.begin(), word.end(), [](char c) { return !isalpha(c); }), word.end());
        transform(word.begin(), word.end(), word.begin(), ::tolower);

        WordItem existingWordItem = tree.search(word);
        if (!existingWordItem.word.empty()) {
            existingWordItem.totalOccurrence++;
            bool documentFound = false;
            for (auto& documentCount : existingWordItem.documentCounts) {
                if (documentCount.documentName == documentName) {
                    documentCount.count++;
                    documentFound = true;
                    break;
                }
            }
            if (!documentFound) {
                existingWordItem.documentCounts.push_back({documentName, 1});
            }
            tree.insert(word, existingWordItem);
        } else {
            WordItem newWordItem = {word, 1, {{documentName, 1}}};
            tree.insert(word, newWordItem);
        }
    }
}

int main() {
    int numFiles;
    cout << "Enter number of input files: ";
    cin >> numFiles;

    AVLSearchTree<string, WordItem> myTree;

    const string BASE_PATH = "/Users/sevdeyilmaz/CLionProjects/CS300_hw2";

    for (int i = 0; i < numFiles; ++i) {
        string documentName;
        cout << "Enter " << i + 1 << ". file name: ";
        cin >> documentName;

        string documentPath = BASE_PATH + "/" + documentName;

        ifstream file(documentPath);
        if (file.is_open()) {
            stringstream buffer;
            buffer << file.rdbuf();
            preprocessText(documentName, buffer.str(), myTree);
        } else {
            cerr << "Unable to open file: " << documentPath << endl;
            return 1;
        }
    }
    cin.ignore(); // Ignore newline character left in the buffer

    string query;
    bool printMessage = true; // Flag to check whether to print the "No document contains the given query" message

    while (true) {
        cout << "Enter queried words in one line: ";
        getline(cin, query);

        if (query == "ENDOFINPUT") {
            break; // Exit the loop if "ENDOFINPUT" is entered
        } else if (query.find("REMOVE") == 0) {
            // Remove the specified word
            string wordToRemove = query.substr(7);
            transform(wordToRemove.begin(), wordToRemove.end(), wordToRemove.begin(), ::tolower);
            myTree.remove(wordToRemove); // Remove the specified word
            cout << wordToRemove << " has been REMOVED" << endl;
        } else {
            // Search the query
            myTree.searchQuery(query);

            // Check if the "No document contains the given query" message was printed
            printMessage = myTree.search(query).word.empty();
        }
    }

    return 0;
}

