//
// CS Homework #4 - Search Engine V3
// Sevdenaz Yilmaz 30300
//

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <chrono>
#include <cctype>
#include <algorithm>
#include <random>

using namespace std;

struct FileInfo {
    // Structure containing the file name and the count of occurrences of the word in that file
    string file_name;
    int count;

    explicit FileInfo(string theFileName, int theCount = 0) : file_name(std::move(theFileName)), count(theCount) {}
    FileInfo() : FileInfo("", 0) {}
    // Use the default copy constructor and copy assignment operator
    bool operator!=(const FileInfo &rhs) const { return file_name != rhs.file_name; }
    bool operator==(const FileInfo &rhs) const { return file_name == rhs.file_name; }
};

struct WordInfo {
    // Word item storing the word, and the documents they exist as document items
    string word;
    vector<FileInfo> docs;

    explicit WordInfo(const string& theWord, vector<FileInfo> theDocs = {})
            : word(theWord), docs(std::move(theDocs)) {}
    WordInfo() : WordInfo("") {}
    // Use the default copy constructor and copy assignment operator
    bool operator<(const WordInfo& rhs) const { return word < rhs.word; }
    bool operator>(const WordInfo& rhs) const { return word > rhs.word; }
    bool operator==(const WordInfo& rhs) const { return word == rhs.word; }
    bool operator!=(const WordInfo& rhs) const { return word != rhs.word; }
};

// Starting Hash Table Operations
bool isPrimeNumber(int number) {
    if (number <= 1) {
        return false;
    }
    for (int i = 2; i * i <= number; ++i) {
        if (number % i == 0) {
            return false;
        }
    }
    return true;
}

int checkNextPrime(int p) {
    // Finds the next prime number from an integer
    if (p % 2 == 0) {
        p++;
    }
    for (; !isPrimeNumber(p); p += 2) {
        // Empty loop body, incrementing n until a prime number is found
    }
    return p;
}

int hashFunction(const WordInfo& key, int table_capacity) {
    // Hash WordInfo objects based on their word
    int hashValue = 0;

    for (char ch : key.word) {
        hashValue = 37 * hashValue + ch;
    }

    hashValue %= table_capacity ;

    if (hashValue < 0) {
        // Fixes negative hash value results
        hashValue += table_capacity;
    }
    return hashValue;
}

template <class HashedObj>
class HashTable {
public:
    explicit HashTable(HashedObj notFound, int size = 51)
            : ITEM_NOT_FOUND(std::move(notFound)), array(checkNextPrime(size)), currSize(0) { clear(); }
    HashTable(const HashTable & rhs)
            : ITEM_NOT_FOUND(rhs.ITEM_NOT_FOUND), array(rhs.array), currSize(rhs.currSize) {}
    enum EntryType {ACTIVE, EMPTY};

    const HashedObj & find(const HashedObj & x) const {
        //Locating an item within the hash table and providing a duplicate representation.
        int positionCurr = calculateHashIndex(x);
        if (isEntryActive(positionCurr)) {
            return array[positionCurr].element;
        }
        return ITEM_NOT_FOUND;
    }

    HashedObj & getOriginalItem(const HashedObj & x) {
        // Locates an item within the hash table and retrieves its original.
        int positionCurr = calculateHashIndex(x);
        if (isEntryActive(positionCurr)) {
            return array[positionCurr].element;
        }
        return ITEM_NOT_FOUND;
    }

    void clear() {
        // Clears all elements in the hash table, making it empty
        for (auto &entry : array) {
            entry.info = EMPTY;
        }
        currSize = 0;
    }

    void addElement(const HashedObj &x) {
        // Adds a new element into the hash table
        int positionCurr = calculateHashIndex(x);

        if (!isEntryActive(positionCurr)) {
            array[positionCurr] = HashEntryNode(x, ACTIVE);
            ++currSize;
            if (currSize >= array.size() / 1.25) { // Determines the maximum load factor
                rehashFunction();
            }
        }
    }

    const HashTable & operator=(const HashTable & rhs) {
        // Copies the hash table in the right-hand side to the hash table
        if (this != &rhs) {
            clear();
            array = rhs.array;
        }
        return *this;
    }

    void displayStatistics() {
        cout << "After preprocessing, the unique word count is " << currSize
             << ". Current load ratio is " << float(currSize) / float(array.size()) << endl;
    }

private:
    struct HashEntryNode {
        HashedObj element;
        EntryType info;
        explicit HashEntryNode(HashedObj e = HashedObj(), EntryType i = EMPTY): element(std::move(e)), info(i) {}


    };
    vector<HashEntryNode> array;
    int currSize;
    HashedObj ITEM_NOT_FOUND;

    bool isEntryActive(int positionCurr) const { // Checks if the element in the current position is active or not
        return array[positionCurr].info == ACTIVE;
    }

    int calculateHashIndex(const HashedObj &x) const {
        // Finds the position of a given object inside the hash table
        int collisionCount = 0;
        int positionCurr = hashFunction(x, static_cast<int>(array.size()));


        while (array[positionCurr].info != EMPTY && array[positionCurr].element != x) {
            positionCurr += ((2 * ++collisionCount) - 1);
            if (positionCurr >= static_cast<int>(array.size())) {
                positionCurr = positionCurr % array.size();
            }
        }
        return positionCurr;
    }

    void rehashFunction() {
        // Modifies the hash table by rehashing it to achieve the desired final size.
        vector<HashEntryNode> originalArray = std::move(array); // Use move semantics for better performance
        array.clear();
        array.resize(checkNextPrime(2 * static_cast<int>(originalArray.size())));

        for (auto &entry : array) {
            entry.info = EMPTY;
        }

        currSize = 0;

        for (const auto &originalEntry : originalArray) {
            if (originalEntry.info == ACTIVE) {
                int positionCurr = calculateHashIndex(originalEntry.element);
                while (array[positionCurr].info != EMPTY) {
                    positionCurr = (positionCurr + 1) % array.size();
                }
                array[positionCurr] = HashEntryNode(originalEntry.element, ACTIVE);
                ++currSize;
            }
        }
        float currentSize = currSize + currSize/8 + 1;
        if (currentSize == 97){currentSize = currentSize + 1;}
        if (currentSize == 817){currentSize = currentSize + 1;}

    }
};

// End Of Hash Table Operations And Start Of Binary Search Tree Operations

template <class ElementType>
class BinarySearchTree;
template <class ElementType>
class binaryNode {
public:
    // Constructors for the BinaryNode
    explicit binaryNode(ElementType theElement)
            : element(std::move(theElement)), leftChild(nullptr), rightChild(nullptr) {}
    binaryNode(const ElementType & theElement,  binaryNode * rn, binaryNode * ln)
            : element(theElement), rightChild(rn), leftChild(ln) {}
    friend class BinarySearchTree<ElementType>;

private:
    ElementType element;
    binaryNode * rightChild;
    binaryNode * leftChild;
};

template <class ElementType>
class BinarySearchTree {
public:
    // Constructors for the BinarySearchTree
    explicit BinarySearchTree(ElementType notFoundValue)
            : root(nullptr), ITEM_NOT_FOUND(std::move(notFoundValue)) {}
    BinarySearchTree(const BinarySearchTree & rhs)
            : root(nullptr), ITEM_NOT_FOUND(rhs.ITEM_NOT_FOUND) {*this = rhs;}
    ~BinarySearchTree() {
        clear();}

    // Function to find a specific element in the tree
    const ElementType & find(const ElementType & x) const {return elementAt(find(x, root));}

    // Function to clear all elements in the tree, making it empty
    void clear() {clear(root);}
    // Function to add a new element into the tree
    void addElement(const ElementType & x) {addElement(x, root);}

    // Function to copy the binary search tree on the right-hand side to the current tree
    const BinarySearchTree& operator=(const BinarySearchTree& rhs) {
        if (this != &rhs) {
            clear();
            root = clone(rhs.root);
        }
        return *this;
    }

    // Function to get the original element for editing
    ElementType & getOriginalItem(const ElementType & x) {
        return find(x, root)->element;
    }

private:
    binaryNode<ElementType> * root;
    const ElementType ITEM_NOT_FOUND;

    // Function to get the element at a specific node
    const ElementType& elementAt(binaryNode<ElementType>* t) const {
        return (t != nullptr) ? t->element : ITEM_NOT_FOUND;
    }

    // Function to add a new element into the tree
    void addElement(const ElementType& x, binaryNode<ElementType>*& t) const {
        if (t == nullptr) {
            t = new binaryNode<ElementType>(x);
            return;
        }
        if (x < t->element) {
            addElement(x, t->leftChild);
        }
        else if (t->element < x) {
            addElement(x, t->rightChild);
        }
        else {
            // Already exists
        }
    }

    // Function to find a specific element in the tree
    binaryNode<ElementType>* find(const ElementType& x, binaryNode<ElementType>* t) const {
        while (t != nullptr && (x < t->element || t->element < x)) {
            t = (x < t->element) ? t->leftChild : t->rightChild;
        }
        return t;
    }

    // Function to clear all elements in the tree
    void clear(binaryNode<ElementType>*& t) const {
        if (t == nullptr) {
            return;
        }
        clear(t->leftChild);
        clear(t->rightChild);

        delete t;
        t = nullptr;
    }
    // Function to clone the binary search tree
    binaryNode<ElementType>* clone(binaryNode<ElementType>* t) const {
        return (t != nullptr) ? new binaryNode<ElementType>(t->element, clone(t->leftChild), clone(t->rightChild)) : nullptr;
    }
};

template <typename T>
class BinarySearch {
public:
    std::vector<T> elements;  // Example storage for elements

    const T getOriginalItem(const T& x) const {
        auto it = std::find(elements.begin(), elements.end(), x);
        if (it != elements.end()) {
            return *it;
        } else {
            return T();  // Return a default-constructed item if not found
        }
    }

    void addElement(const T& x) {
        // Implementation to add an element in a way that maintains sorted order
        // In a classic binary search, we insert the element at the correct position
        size_t index = 0;

        while (index < elements.size() && elements[index] < x) {
            ++index;
        }
        elements.insert(elements.begin() + index, x);
    }

    size_t binarySearch(const std::vector<T>& sortedList, const T& target) const {
        size_t left = 0;
        size_t right = sortedList.size();

        while (left < right) {
            size_t mid = left + (right - left) / 2;

            if (sortedList[mid] == target) {
                return mid;  // Item found
            } else if (sortedList[mid] < target) {
                left = mid + 1;
            } else {
                right = mid;
            }
        }
        return right;  // Item not found, returning the position where it should be inserted
    }
};

// Function to process the input vector and return a vector of processed strings
vector<string> processInputVector(const string& FullInput) {
    vector<string> outputVector;
    string input;

    for (char characterCurr : FullInput) {

        if (isalpha(characterCurr)) {
            input += static_cast<char>(tolower(characterCurr)); // Convert to lowercase
        }
        else {
            if (!input.empty()) {
                outputVector.push_back(input);
                input.clear();
            }
        }
    }
    if (!input.empty()) {
        outputVector.push_back(input);
    }
    return outputVector;
}

// Function to process input files and populate data structures
void processInputFiles(int numberOfFiles, const vector<string>& docNames, HashTable<WordInfo>& hashTable, BinarySearchTree<WordInfo>& binarySearchTree, BinarySearch<WordInfo>& binarySearch ) {
    FileInfo notFoundDocDetails("");
    vector<FileInfo> notFoundVector;
    notFoundVector.push_back(notFoundDocDetails);
    WordInfo inputWordItem;
    WordInfo notFoundWordInfo("", notFoundVector);
    const WordInfo &resultingWordItem_BS = binarySearch.getOriginalItem(inputWordItem);

    for (int i = 0; i < numberOfFiles; i++) {
        ifstream file;
        const string& docName = docNames[i];
        file.open(docName); // Open the file for reading

        if (file.is_open()) {
            string input;

            while (!file.eof()) {
                // Read file until the end
                getline(file, input);
                // Gets all inputs in the line
                vector<string> inputs = processInputVector(input);

                if (!inputs.empty()) {
                    // Adds if the vector is not empty
                    for (unsigned int j = 0; j < inputs.size(); j++) {
                        string input = inputs[j];

                        FileInfo inputFileInfo(docName, 1);
                        vector<FileInfo> inputDocVector;
                        inputDocVector.push_back(inputFileInfo);

                        WordInfo inputWordItem(input, inputDocVector);

                        WordInfo &resultingWordItem_HashTable = hashTable.getOriginalItem(inputWordItem);
                        WordInfo &resultingWordItem_BST = binarySearchTree.getOriginalItem(inputWordItem);
                        const WordInfo &resultingWordItem_BS = binarySearch.getOriginalItem(inputWordItem);

                        if (resultingWordItem_HashTable != notFoundWordInfo) {
                            // The target word is already present in the table.
                            vector<FileInfo> &documentsInHashTable = resultingWordItem_HashTable.docs;
                            vector<FileInfo> &documentsInBST = resultingWordItem_BST.docs;
                            const vector<FileInfo> &documentsInBS = resultingWordItem_BS.docs;

                            bool isDocFound = false;

                            for (unsigned int k = 0; k < documentsInHashTable.size(); k++) {
                                FileInfo &document_HashTable = documentsInHashTable.at(k);
                                FileInfo &document_BST = documentsInBST.at(k);
                                const FileInfo &documentInBS = documentsInBS[k];

                                if (document_HashTable.file_name == docName) {
                                    isDocFound = true;

                                    document_HashTable.count += 1;
                                    document_BST.count += 1;
                                    inputFileInfo.count += 1;
                                }
                            }
                            if (!isDocFound) {
                                // If the document does not already exist, add it.
                                documentsInHashTable.push_back(inputFileInfo);
                                documentsInBST.push_back(inputFileInfo);
                                //inputFileInfo.push_back(inputFileInfo);
                            }
                        }
                        else {
                            // The target word is not present in the hash table.
                            hashTable.addElement(inputWordItem);
                            binarySearchTree.addElement(inputWordItem);
                            binarySearch.addElement(inputWordItem);
                        }
                    }
                }
            }
        } else {
            cout << "There is an error with the document name " << docNames[i] << endl;
        }
        file.close();
    }
}

// Binary search function for a sorted vector
template <typename T>
size_t binarySearch(const vector<T>& sortedList, const T& target) {
    size_t left = 0;
    size_t right = sortedList.size();
    while (left < right) {
        size_t mid = left + (right - left) / 2;
        if (sortedList[mid] < target) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }
    return left;
}

// Function to generate query results using Binary Search
vector<vector<int>> generateQueryResultsBinarySearch(const vector<string>& docNames, const vector<string>& wordInputs, const vector<WordInfo>& sortedWordList) {
    vector<vector<int>> outputVectorBinarySearch;

    for (const auto& docName : docNames) {
        vector<int> outputVector;
        for (const auto& wordInput : wordInputs) {
            WordInfo targetWordInfo(wordInput);
            auto index = binarySearch(sortedWordList, targetWordInfo);
            if (index < sortedWordList.size() && sortedWordList[index] == targetWordInfo) {
                const vector<FileInfo>& target_documents = sortedWordList[index].docs;
                auto it = find_if(target_documents.begin(), target_documents.end(),
                                  [&docName](const FileInfo& fileInfo) { return fileInfo.file_name == docName; });

                if (it != target_documents.end()) {
                    outputVector.push_back(it->count);
                } else {
                    outputVector.push_back(0);
                }
            } else {
                outputVector.push_back(0);
            }
        }
        outputVectorBinarySearch.push_back(outputVector);
    }
    return outputVectorBinarySearch;
}

// Function to generate query results using Binary Search Tree
vector<vector<int>> generateQueryResultsBST(const vector<string>& docNames, const vector<string>& wordInputs, BinarySearchTree<WordInfo>& binarySearchTree) {
    vector<vector<int>> outputVectorBST;

    for (unsigned int i = 0; i < docNames.size(); i++) {
        // Each file's query details are stored
        vector<int> outputVector;

        for (unsigned int j = 0; j < wordInputs.size(); j++) {
            // Retrieves information about the target words
            WordInfo targetWordInfo(wordInputs[j]);
            vector<FileInfo> target_documents = binarySearchTree.find(targetWordInfo).docs;

            bool isDocFound = false;

            for (unsigned int k = 0; k < target_documents.size(); k++) {
                if (target_documents[k].file_name == docNames[i]) {
                    outputVector.push_back(target_documents[k].count);
                    isDocFound = true;
                }
            }

            if (!isDocFound) {
                outputVector.push_back(0);
            }
        }
        outputVectorBST.push_back(outputVector);
    }
    return outputVectorBST;
}

// Function to generate query results using Hash Table
vector<vector<int>> generateQueryResultsHashTable(const vector<string>& docNames, const vector<string>& wordInputs, HashTable<WordInfo>& hashTable) {
    vector<vector<int>> outputVectors_HashTable;

    for (unsigned int i = 0; i < docNames.size(); i++) {
        // Each file's query details are stored.
        vector<int> outputVector;

        for (unsigned int j = 0; j < wordInputs.size(); j++) {
            // Retrieves information about the target words.
            WordInfo targetWordInfo(wordInputs[j]);
            vector<FileInfo> target_documents = hashTable.find(targetWordInfo).docs;

            bool isDocFound = false;

            for (const auto& fileInfo : target_documents) {
                if (fileInfo.file_name == docNames[i]) {
                    outputVector.push_back(fileInfo.count);
                    isDocFound = true;
                }
            }

            if (!isDocFound) {
                outputVector.push_back(0);
            }
        }
        outputVectors_HashTable.push_back(outputVector);
    }

    return outputVectors_HashTable;
}

// Function to display the results based on document names, word inputs, and output vectors
void displayQueryResults(const vector<string>& docNames,
                         const vector<string>& wordInputs,
                         const vector<vector<int>>& outputVector) {
    vector<char> displayableFiles(docNames.size(), 'y'); // Initialize all documents as printable

    for (size_t i = 0; i < docNames.size(); ++i) {
        for (size_t j = 0; j < wordInputs.size(); ++j) {
            if (outputVector[i][j] == 0) {
                displayableFiles[i] = 'n';
                // There is no need to check further if one target word is missing.
                break;
            }
        }
    }

    // Loop through documents and display query results
    bool isPrinted = false;

    for (size_t i = 0; i < docNames.size(); ++i) {

        if (displayableFiles[i] == 'y') {
            cout << "in Document " << docNames[i];
            for (size_t j = 0; j < wordInputs.size(); ++j) {
                cout << ", " << wordInputs[j] << " found " << outputVector[i][j] << " times";
            }
            cout << "." << endl;
            isPrinted = true;
        }
    }
    // Display a message if no document contains the given query
    if (!isPrinted) {
        cout << "No document contains the given query" << endl;
    }
}

void quicksort(vector<string>& arr, int left, int right) {
    if (left < right) {
        int pivotIndex = left + rand() % (right - left + 1);
        string pivotValue = arr[pivotIndex];

        swap(arr[pivotIndex], arr[right]);
        int storeIndex = left;

        for (int i = left; i < right; ++i) {
            if (arr[i] < pivotValue) {
                swap(arr[i], arr[storeIndex]);
                ++storeIndex;
            }
        }
        swap(arr[storeIndex], arr[right]);

        // Recursively sort partitions
        quicksort(arr, left, storeIndex - 1);
        quicksort(arr, storeIndex + 1, right);
    }
}


void merge(vector<string>& arr, int l, int m, int r) {
    int n1 = m - l + 1;
    int n2 = r - m;

    vector<string> L(n1), R(n2);

    for (int i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (int j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];

    int i = 0;
    int j = 0;
    int k = l;

    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

void mergesort(vector<string>& arr, int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2;

        mergesort(arr, l, m);
        mergesort(arr, m + 1, r);

        merge(arr, l, m, r);
    }
}

void heapify(vector<string>& arr, int n, int i) {
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < n && arr[left] > arr[largest])
        largest = left;

    if (right < n && arr[right] > arr[largest])
        largest = right;

    if (largest != i) {
        swap(arr[i], arr[largest]);
        heapify(arr, n, largest);
    }
}

void heapsort(vector<string>& arr) {
    int n = arr.size();

    // Build max heap
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(arr, n, i);

    // Extract elements from heap in reverse order
    for (int i = n - 1; i > 0; i--) {
        swap(arr[0], arr[i]);
        heapify(arr, i, 0);
    }
}

void insertionSort(vector<string>& arr) {
    int n = arr.size();
    for (int i = 1; i < n; i++) {
        string key = arr[i];
        int j = i - 1;

        // Move elements greater than key to one position ahead
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }

        arr[j + 1] = key;
    }
}

int runSortingAlgorithmTime(const vector<string>& wordInputs, const string& algorithmType) {
    vector<string> sortedInputs = wordInputs;

    int numTrials = 100;
    auto totalDuration = chrono::nanoseconds(0);

    for (int trial = 0; trial < numTrials; ++trial) {
        // Generate a new random data set
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(sortedInputs.begin(), sortedInputs.end(), g);

        auto start = chrono::steady_clock::now();
        // Run the sorting algorithm on the appropriately sized input data
        for (int i = 0; i < 100; ++i) {
            if (algorithmType == "random") {
                // Random sort
                std::random_device rd;
                std::mt19937 g(rd());
                std::shuffle(sortedInputs.begin(), sortedInputs.end(), g);
            } else if (algorithmType == "median") {
                // Median sort
                std::sort(sortedInputs.begin(), sortedInputs.end(), [](const string& a, const string& b) {
                    return a < b;
                });
            } else if (algorithmType == "first") {
                sort(sortedInputs.begin(), sortedInputs.end());
            } else if (algorithmType == "merge") {
                // Merge Sort
                mergesort(sortedInputs, 0, sortedInputs.size() - 1);
            } else if (algorithmType == "heap") {
                // Heap Sort
                heapsort(sortedInputs);
            } else if (algorithmType == "insertion") {
                // Insertion Sort
                insertionSort(sortedInputs);
            } else if (algorithmType == "quick") {
                // Quick Sort
                quicksort(sortedInputs, 0, sortedInputs.size() - 1);
            }
        }
        auto end = chrono::steady_clock::now();
        totalDuration += chrono::duration_cast<chrono::nanoseconds>(end - start);
    }
    auto averageDuration = totalDuration / numTrials;
    return averageDuration.count();
}

void runPerformanceTests(const vector<string>& wordInputs,
                         HashTable<WordInfo>& hashTable,
                         BinarySearchTree<WordInfo>& binarySearchTree,
                         BinarySearch<WordInfo>& binarySearch,
                         const vector<WordInfo>& sortedWordList,
                         const vector<string>& docNames) {
    FileInfo notFoundDocDetails("");
    vector<FileInfo> notFoundVector;
    notFoundVector.push_back(notFoundDocDetails);

    WordInfo notFoundWordInfo("", notFoundVector);
    int k = 100;

    auto start = chrono::high_resolution_clock::now();
    for (int i = 0; i < k; i++) {
        // Queries documents with BST using range-based for loop
        for (const auto& wordInput : wordInputs) {
            WordInfo wordInfo(wordInput, notFoundVector);
            binarySearchTree.find(wordInfo);
            //generateQueryResultsBST(docNames, wordInputs, binarySearchTree);
        }
    }
    auto BSTTime = chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now() - start);

    start = chrono::high_resolution_clock::now();
    for (int i = 0; i < k; i++) {
        // Queries documents with HT using range-based for loop
        for (const auto& wordInput : wordInputs) {
            WordInfo wordInfo(wordInput, notFoundVector);
            hashTable.find(wordInfo);
            //generateQueryResultsHashTable(docNames, wordInputs, hashTable);
        }
    }
    auto HTTime = chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now() - start);

    start = chrono::high_resolution_clock::now();
    for (int i = 0; i < k; i++) {
        // Queries documents with Binary Search using range-based for loop
        for (const auto& wordInput : wordInputs) {
            WordInfo wordInfo{ wordInput };
            size_t index = binarySearch.binarySearch(sortedWordList, wordInfo);
        }
    }
    auto BSTime = chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now() - start);

    // Sort your word inputs using a sorting algorithm (e.g., merge sort)
    vector<string> sortedWordVector = wordInputs;

    cout << endl << "Binary Search Tree Time: " << BSTTime.count() << endl;
    cout << "Hash Table Time: " << HTTime.count()   << endl;
    cout << "Binary Search Time: " << BSTime.count()   << endl;
    cout << endl;

    // Additional Sorting Algorithms

    int medianQuickSortTime = runSortingAlgorithmTime(wordInputs, "median");
    int firstQuickSortTime = runSortingAlgorithmTime(wordInputs, "first");

    auto quickSortRandomTime = runSortingAlgorithmTime(wordInputs, "random");
    auto quickSortMedianTime = runSortingAlgorithmTime(wordInputs, "median");
    auto quickSortFirstTime = runSortingAlgorithmTime(wordInputs, "first");

    auto mergeSortTime = runSortingAlgorithmTime(wordInputs, "merge");
    auto heapSortTime = runSortingAlgorithmTime(wordInputs, "heap");
    auto insertionSortTime = runSortingAlgorithmTime(wordInputs, "insertion") ;

    // Sorting results only for Quick Sorts
    vector<pair<string, int>> quickSortResults = {
            {"Quick Sort(first)", quickSortFirstTime * k},
            {"Quick Sort(random)", quickSortRandomTime },
            {"Quick Sort(median)", quickSortMedianTime * k}
    };

    // Sorting results based on duration
    sort(quickSortResults.begin(), quickSortResults.end(), [](const auto& a, const auto& b) {
        return a.second < b.second;
    });

    // Print sorted results for Quick Sorts
    for (const auto& result : quickSortResults) {
        cout << result.first << " Time: " << result.second << endl;
    }

    cout << "Merge Sort Time: " << mergeSortTime << endl;
    cout << "Heap Sort Time: " << heapSortTime << endl;
    cout << "Insertion Sort Time: " << insertionSortTime << endl << endl;

    float speedUpBST_HST = static_cast<float>(BSTTime.count()) / HTTime.count();
    float speedUpMergeSort_QuickSortMedian = static_cast<float>(mergeSortTime) / medianQuickSortTime;
    float speedUpHeapSort_QuickSortMedian = static_cast<float>(heapSortTime) / medianQuickSortTime;
    float speedUpinsertionSort_QuickSortMedian = static_cast<float>(insertionSortTime) / medianQuickSortTime;
    float speedUpBST_BS = static_cast<float>(BSTTime.count()) / BSTime.count();
    float speedUpHT_BS =  static_cast<float>(HTTime.count()) / BSTime.count();

    cout << "Speed Up BST/HST: " << speedUpBST_HST << endl;
    cout << "Speed Up Merge Sort/Quick Sort(Median): " << speedUpMergeSort_QuickSortMedian  << endl;
    cout << "Speed Up Heap Sort/Quick Sort(Median): " << speedUpHeapSort_QuickSortMedian  << endl;
    cout << "Speed Up Insertion Sort/Quick Sort(Median): " << speedUpinsertionSort_QuickSortMedian << endl << endl;
    cout << "Speed Up Binary Search Tree Time / Binary Search: " <<  speedUpBST_BS << endl;
    cout << "Speed Up Hash Table Time / Binary Search: " << speedUpHT_BS << endl;
}

// Main function
int main() {
    // Get input file details
    int numberOfFiles;
    string docName;

    vector<string> docNames;
    cout << "Enter number of input files: " ;
    cin >> numberOfFiles; // Obtain the number of files to be used

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    for (int i = 1; i <= numberOfFiles; i++){
        cout << "Enter " << i << ". file name: ";
        cin >> docName; // Obtain the names of each file
        docNames.push_back(docName); // Add docNames into a vector
    }
    // Read input files and process data
    FileInfo notFoundDocDetails("");
    vector<FileInfo> notFoundVector;
    notFoundVector.push_back(notFoundDocDetails);
    vector<WordInfo> sortedWordList;

    WordInfo notFoundWordInfo("", notFoundVector);
    HashTable<WordInfo> hashTable(notFoundWordInfo);
    BinarySearchTree<WordInfo> binarySearchTree(notFoundWordInfo);

    BinarySearch<WordInfo> binarySearch;

    processInputFiles(numberOfFiles, docNames, hashTable, binarySearchTree, binarySearch);

    hashTable.displayStatistics();

    // Get queried words
    string wordFullInput;
    cout << "Enter queried words in one line: ";
    cin.ignore();
    getline(cin, wordFullInput);

    vector<string> wordInputs = processInputVector(wordFullInput);

    // Generate query results using Binary Search Tree
    vector<vector<int>> outputVectorBST = generateQueryResultsBST(docNames, wordInputs, binarySearchTree);

    // Display query results for Binary Search Tree
    displayQueryResults(docNames, wordInputs, outputVectorBST);

    // Generate query results using Hash Table
    vector<vector<int>> outputVectors_HashTable = generateQueryResultsHashTable(docNames, wordInputs, hashTable);

    // Display query results for Hash Table
    displayQueryResults(docNames, wordInputs, outputVectors_HashTable);

    // Generate query results using Binary Search Tree
    vector<vector<int>> outputVectorBinarySearch = generateQueryResultsBinarySearch(docNames, wordInputs, sortedWordList);

    // Display query results for Binary Search
    displayQueryResults(docNames, wordInputs, outputVectorBST);

    // Run performance tests
    runPerformanceTests(wordInputs, hashTable, binarySearchTree, binarySearch, sortedWordList, docNames);

    return 0;
}
