#include <iostream>
#include <fstream>
#include <cstring>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <unordered_map>

using namespace std;

struct Book
{
    int id;
    char title[100];
    char author[100];
    int quantity;
    string category;
    Book *next;
};

struct BookInfo {
    int id;
    string title;
    int quantity;
    string category;

    BookInfo(int id, const string& title, int quantity, const string& category)
        : id(id), title(title), quantity(quantity), category(category) {}
};

struct TransactionNode {
    int bookID;
    string borrowerName;
    int quantity;
    string borrowDate;
    string returnDate;
    TransactionNode* next;

    TransactionNode(int id, const string& name, int qty, const string& bDate, const string& rDate)
        : bookID(id), borrowerName(name), quantity(qty), borrowDate(bDate), returnDate(rDate), next(nullptr) {}
};

const int HASH_SIZE = 3; // Ukuran hash table
const int MAX_ENTRIES_PER_BUCKET = 3;

struct HashEntry {
    TransactionNode* head;
    HashEntry() : head(nullptr) {}
};

class TransactionManager {
private:
    HashEntry* hashTable;

    int hashFunction(int key) {
        return key % HASH_SIZE;
    }

public:
    TransactionManager() {
        hashTable = new HashEntry[HASH_SIZE];
    }

void addTransaction(int bookID, const string& borrowerName, int quantity, const string& borrowDate, const string& returnDate) {
    int index = hashFunction(bookID);
    int startIndex = index;

    do {
        if (hashTable[index].head == nullptr) {
            // Bucket kosong, tambahkan entri baru
            TransactionNode* newNode = new TransactionNode(bookID, borrowerName, quantity, borrowDate, returnDate);
            hashTable[index].head = newNode;
            return;
        }

        // Coba bucket berikutnya
        index = (index + 1) % HASH_SIZE;
    } while (index != startIndex);

    // Jika semua bucket penuh, tampilkan pesan kesalahan
    cout << "SLOT PENUH, TIDAK BISA MEMINJAM BUKU" << endl;
}

    void displayTransactions() {
        for (int i = 0; i < HASH_SIZE; i++) {
            cout << "Data " << i << ": ";
            TransactionNode* current = hashTable[i].head;
            while (current != nullptr) {
                cout << "\n" << "ID Buku: " << current->bookID << ", Peminjam: " << current->borrowerName << ", Jumlah: " << current->quantity << ", Tanggal Pinjam: " << current->borrowDate << ", Tanggal Kembali: " << current->returnDate << "\n";
                current = current->next;
            }
            cout << endl;
        }
    }

    ~TransactionManager() {
        for (int i = 0; i < HASH_SIZE; i++) {
            TransactionNode* current = hashTable[i].head;
            while (current != nullptr) {
                TransactionNode* temp = current;
                current = current->next;
                delete temp;
            }
        }
        delete[] hashTable;
    }
};

// Graph structure for library locations
class Graph {
private:
    unordered_map<string, vector<string>> adjList;
    unordered_map<string, unordered_map<string, vector<BookInfo>>> bookInfoMapByCategory;

public:
    void addEdge(const string& location1, const string& location2) {
        adjList[location1].push_back(location2);
        adjList[location2].push_back(location1); // Assuming undirected graph
    }

    void addBookInfo(const string& location, int id, const string& title, int quantity, const string& category) {
        if (bookInfoMapByCategory.find(location) == bookInfoMapByCategory.end()) {
            bookInfoMapByCategory[location] = unordered_map<string, vector<BookInfo>>();
        }

        if (bookInfoMapByCategory[location].find(category) == bookInfoMapByCategory[location].end()) {
            bookInfoMapByCategory[location][category].emplace_back(id, title, quantity, category);
        } else {
            bookInfoMapByCategory[location][category].emplace_back(id, title, quantity, category);
        }
    }

void displayGraphByCategory() {
        for (const auto& pair : bookInfoMapByCategory) {
            cout << "Lokasi: " << pair.first << "\n";
            for (const auto& categoryPair : pair.second) {
                cout << "Kategori: " << categoryPair.first << "\n";
                cout << "Buku:\n";
                for (const BookInfo& book : categoryPair.second) {
                    cout << "ID: " << book.id << ", Judul: " << book.title << ", Kuantitas: " << book.quantity << "\n";
                }
                cout << "\n";
            }
        }
    }
};

struct TreeNode {
    int id;
    char title[100];
    char author[100];
    int quantity;
    string category;
    string location;
    TreeNode* left, * right;

    TreeNode(int id, const char* title, const char* author, int quantity, const string& category, const string& location)
        : id(id), quantity(quantity), category(category), location(location), left(nullptr), right(nullptr) {
        strcpy(this->title, title);
        strcpy(this->author, author);
    }
};

class MetadataTree
{
private:
    TreeNode *root;
    Graph& libraryGraph;

    // Recursive helper function to insert a node
    TreeNode* insert(TreeNode* node, int id, const char* title, const char* author, int quantity, const string& category, const string& location) {
        if (node == nullptr) {
            TreeNode* newNode = new TreeNode(id, title, author, quantity, category, location);
            libraryGraph.addBookInfo(location, id, title, quantity, category); // Tambahkan informasi buku ke Graph
            return newNode;
        }
        if (id < node->id) {
            node->left = insert(node->left, id, title, author, quantity, category, location);
        }
        else {
            node->right = insert(node->right, id, title, author, quantity, category, location);
        }
        return node;
    }

    // Recursive helper function to search for a node
    TreeNode *search(TreeNode *node, int id)
    {
        if (node == nullptr || node->id == id)
        {
            return node;
        }
        if (id < node->id)
        {
            return search(node->left, id);
        }
        else
        {
            return search(node->right, id);
        }
    }

    // Recursive helper function for in-order traversal
    void inOrderTraversal(TreeNode *node)
    {
        if (node != nullptr)
        {
            inOrderTraversal(node->left);
            cout << "ID: " << node->id << ", Judul: " << node->title << ", Penulis: " << node->author << ", Kuantitas: " << node->quantity << ", Kategori: " << node->category << "\n";
            inOrderTraversal(node->right);
        }
    }

public:
    MetadataTree(Graph& graph) : root(nullptr), libraryGraph(graph) {}

    void insert(int id, const char* title, const char* author, int quantity, const string& category, const string& location) {
        root = insert(root, id, title, author, quantity, category, location);
    }

    void search(int id)
    {
        TreeNode *result = search(root, id);
        if (result != nullptr)
        {
            cout << "ID: " << result->id << ", Judul: " << result->title << ", Penulis: " << result->author << ", Kuantitas: " << result->quantity << ", Kategori: " << result->category << "\n";
        }
        else
        {
            cout << "Buku tidak ditemukan.\n";
        }
    }

    void display()
    {
        inOrderTraversal(root);
    }
};


class Library
{
private:
    Book *head;
    MetadataTree metadataTree;
    Graph libraryGraph;
    TransactionManager transactionManager;

    struct Transaction
    {
        int bookID;
        string borrowerName;
        int quantity;
        string borrowDate;
        string returnDate;
        Transaction *next;
    };
    Transaction *transactionsHead;

    int countBooksRecursive(Book *node)
    {
        if (node == NULL)
            return 0;
        return 1 + countBooksRecursive(node->next);
    }

    Book* searchBookByTitleRecursive(Book *node, const char *title)
    {
        if (node == NULL)
            return NULL;
        if (strcmp(node->title, title) == 0)
            return node;
        return searchBookByTitleRecursive(node->next, title);
    }

    void swapBooks(Book *a, Book *b)
    {
        int tempId = a->id;
        char tempTitle[100];
        char tempAuthor[100];
        int tempQuantity = a->quantity;
        string tempCategory = a->category;

        strcpy(tempTitle, a->title);
        strcpy(tempAuthor, a->author);

        a->id = b->id;
        strcpy(a->title, b->title);
        strcpy(a->author, b->author);
        a->quantity = b->quantity;
        a->category = b->category;

        b->id = tempId;
        strcpy(b->title, tempTitle);
        strcpy(b->author, tempAuthor);
        b->quantity = tempQuantity;
        b->category = tempCategory;
    }

    void sortBooksByTitle()
    {
        if (!head || !head->next)
            return;

        bool swapped;
        Book *ptr1;
        Book *lptr = NULL;

        do
        {
            swapped = false;
            ptr1 = head;

            while (ptr1->next != lptr)
            {
                if (strcmp(ptr1->title, ptr1->next->title) > 0)
                {
                    swapBooks(ptr1, ptr1->next);
                    swapped = true;
                }
                ptr1 = ptr1->next;
            }
            lptr = ptr1;
        } while (swapped);
    }

    void sortBooksByID()
    {
        if (!head || !head->next)
            return;

        bool swapped;
        Book *ptr1;
        Book *lptr = NULL;

        do
        {
            swapped = false;
            ptr1 = head;

            while (ptr1->next != lptr)
            {
                if (ptr1->id > ptr1->next->id)
                {
                    swapBooks(ptr1, ptr1->next);
                    swapped = true;
                }
                ptr1 = ptr1->next;
            }
            lptr = ptr1;
        } while (swapped);
    }


public:
    Library() : head(nullptr), transactionsHead(nullptr), metadataTree(libraryGraph)
    {
        loadBooksFromFile();
    }

void addBook(int id, const char* title, const char* author, int quantity, const string& category, const string& location) {
    Book* newBook = new Book;
    newBook->id = id;
    strcpy(newBook->title, title);
    strcpy(newBook->author, author);
    newBook->quantity = quantity;
    newBook->category = category;
    newBook->next = head;
    head = newBook;

    // Insert metadata into tree and add book info to graph
    metadataTree.insert(id, title, author, quantity, category, location);

    cout << "Buku berhasil ditambahkan.\n";
}

    void displayBooks()
    {
        if (!head)
        {
            cout << "Tidak ada buku.\n";
            return;
        }

        Book *current = head;
        cout << "Daftar Buku:\n";
        while (current)
        {
            cout << "ID: " << current->id << ", Judul: " << current->title << ", Penulis: " << current->author << ", Kuantitas: " << current->quantity << ", Kategori: " << current->category << "\n";
            
            current = current->next;
        }
        
    }

    void searchBook(int id)
    {
        Book *current = head;
        while (current)
        {
            if (current->id == id)
            {
                cout << "ID ditemukan\n";
                cout << "ID: " << current->id << "\n";
                cout << "Judul: " << current->title << "\n";
                cout << "Penulis: " << current->author << "\n";
                cout << "Kuantitas: " << current->quantity << "\n";
                cout << "Kategori: " << current->category << "\n";
                return;
            }
            current = current->next;
        }
        cout << "Buku tidak ditemukan.\n";
    }

    void deleteBook(int id)
    {
        Book *current = head;
        Book *prev = NULL;

        while (current && current->id != id)
        {
            prev = current;
            current = current->next;
        }

        if (current)
        {
            if (prev)
            {
                prev->next = current->next;
            }
            else
            {
                head = current->next;
            }
            delete current;
            cout << "Buku berhasil dihapus.\n";
        }
        else
        {
            cout << "Buku tidak ditemukan.\n";
        }
    }

    void editBook(int id)
    {
        Book *current = head;

        while (current && current->id != id)
        {
            current = current->next;
        }

        if (current)
        {
            cout << "Masukkan judul baru: ";
            cin.ignore();
            cin.getline(current->title, sizeof(current->title));

            cout << "Masukkan penulis baru: ";
            cin.getline(current->author, sizeof(current->author));

            cout << "Masukkan kuantitas baru: ";
            cin >> current->quantity;

            cout << "Informasi buku berhasil diubah.\n";
        }
        else
        {
            cout << "Buku tidak ditemukan.\n";
        }
    }

    void saveBooksToFile()
    {
        ofstream file("Data Buku.txt");

        if (file.is_open())
        {
            Book *current = head;
            while (current)
            {
                file << current->id << " "
                     << current->title << " "
                     << current->author << " "
                     << current->quantity << " "
                     << current->category << "\n";
                current = current->next;
            }

            file.close();
            cout << "Successfully Saved to File.\n";
        }
        else
        {
            cout << "Gagal menyimpan data buku ke dalam file.\n";
        }
    }

    void loadBooksFromFile()
    {
        ifstream file("Data Buku.txt");

        if (file.is_open())
        {
            int id, quantity;
            char title[100], author[100];
            string category, location = "Main Library"; // Change category type to string

            while (file >> id >> title >> author >> quantity >> category)
            {
                addBook(id, title, author, quantity, category, location);
            }

            file.close();
            cout << "Data buku berhasil dimuat dari file.\n";
        }
        else
        {
            cout << "File data buku tidak ditemukan. Membuat file baru.\n";
        }
    }

    void borrowBook(int id, const string &borrowerName, int quantity)
    {
        Book *current = head;

        while (current && current->id != id)
        {
            current = current->next;
        }

        if (current && current->quantity >= quantity)
        {
            current->quantity -= quantity;

            Transaction *newTransaction = new Transaction;
            newTransaction->bookID = id;
            newTransaction->borrowerName = borrowerName;
            newTransaction->quantity = quantity;

            // Mendapatkan tanggal saat ini (misalnya, menggunakan format "YYYY-MM-DD")
            time_t now = time(0);
            tm *currentTime = localtime(&now);
            char dateBuffer[80];
            strftime(dateBuffer, 80, "%Y-%m-%d", currentTime);

            newTransaction->borrowDate = dateBuffer;

            // Menghitung tanggal pengembalian (contoh: 7 hari dari peminjaman)
            tm returnTime = *currentTime;
            returnTime.tm_mday += 7;
            mktime(&returnTime);
            strftime(dateBuffer, 80, "%Y-%m-%d", &returnTime);

            newTransaction->returnDate = dateBuffer;

            newTransaction->next = transactionsHead;
            transactionsHead = newTransaction;

            cout << "Buku berhasil dipinjam.\n";
        }
        else
        {
            cout << "Buku tidak tersedia atau tidak ditemukan.\n";
        }
    }

    void borrowBookV2(int id, const string& borrowerName, int quantity) {
    Book* current = head;

    while (current && current->id != id) {
        current = current->next;
    }

    if (current && current->quantity >= quantity) {
        current->quantity -= quantity;

        // Mendapatkan tanggal saat ini (misalnya, menggunakan format "YYYY-MM-DD")
        time_t now = time(0);
        tm* currentTime = localtime(&now);
        char dateBuffer[80];
        strftime(dateBuffer, 80, "%Y-%m-%d", currentTime);
        string borrowDate = dateBuffer;

        // Menghitung tanggal pengembalian (contoh: 7 hari dari peminjaman)
        tm returnTime = *currentTime;
        returnTime.tm_mday += 7;
        mktime(&returnTime);
        strftime(dateBuffer, 80, "%Y-%m-%d", &returnTime);
        string returnDate = dateBuffer;

        // Menambahkan transaksi ke hash table
        transactionManager.addTransaction(id, borrowerName, quantity, borrowDate, returnDate);

        cout << "Buku berhasil dipinjam." << endl;
    } else {
        cout << "Buku tidak tersedia atau tidak ditemukan." << endl;
    }
}

void displayTransactions() {
    transactionManager.displayTransactions();
}

    void displayBooksByCategory(const string &category)
    {
        if (!head)
        {
            cout << "Tidak ada buku.\n";
            return;
        }

        Book *current = head;
        cout << "Daftar Buku Kategori " << category << ":\n";
        while (current)
        {
            if (current->category == category)
            {
                cout << "ID: " << current->id << "\n";
                cout << "Judul: " << current->title << "\n";
                cout << "Penulis: " << current->author << "\n";
                cout << "Kuantitas: " << current->quantity << "\n";
                cout << "Kategori: " << current->category << "\n\n";
            }
            current = current->next;
        }
    }

    void printTransactionReceipt()
    {
        if (!transactionsHead)
        {
            cout << "Tidak ada transaksi.\n";
            return;
        }

        Transaction *current = transactionsHead;
        cout << "Bukti Transaksi:\n";
        while (current)
        {
            cout << "ID Buku: " << current->bookID << "\n";
            cout << "Nama Peminjam: " << current->borrowerName << "\n";
            cout << "Jumlah Dipinjam: " << current->quantity << "\n";
            cout << "Tanggal Peminjaman: " << current->borrowDate << "\n";
            cout << "Tanggal Pengembalian: " << current->returnDate << "\n\n";
            current = current->next;
        }
    }

    int countBooks()
    {
        return countBooksRecursive(head);
    }

    void searchBookByTitle(const char *title)
    {
        Book *result = searchBookByTitleRecursive(head, title);
        if (result)
        {
            cout << "Buku ditemukan:\n";
            cout << "ID: " << result->id << "\n";
            cout << "Judul: " << result->title << "\n";
            cout << "Penulis: " << result->author << "\n";
            cout << "Kuantitas: " << result->quantity << "\n";
            cout << "Kategori: " << result->category << "\n";
        }
        else
        {
            cout << "Buku tidak ditemukan.\n";
        }
    }

    void sortBooksTitle()
    {
        sortBooksByTitle();
        cout << "Buku telah diurutkan berdasarkan judul.\n";
    }

    void sortBooksID()
    {
        sortBooksByID();
        cout << "Buku telah diurutkan berdasarkan ID.\n";
    }

    void displayMetadataTree()
    {
        cout << "Metadata Buku :\n";
        metadataTree.display();
    }

    void addLibraryLocation(const string &location1, const string &location2)
    {
        libraryGraph.addEdge(location1, location2);
        cout << "Edge between " << location1 << " and " << location2 << " added.\n";
    }

    void displayLibraryGraph()
    {
        cout << "Library Locations Graph:\n";
        libraryGraph.displayGraphByCategory();
    }

    void menuDataLibrary() {
        int choice;
        system("cls");
        while (true) {
        cout << "Menu Data Library\n";
        cout << "1. Tampilkan Semua Buku\n";
        cout << "2. Display Metadata Tree\n";
        cout << "3. Display buku dari kategori\n";
        cout << "4. Urutkan Buku Berdasarkan Judul\n";
        cout << "5. Urutkan Buku Berdasarkan ID\n";
        cout << "0. Exit\n";
        cout << "> "; 
        cin >> choice;
            if (choice == 1)
            {
                system("cls");
                displayBooks();
                cout << "0. Back to Menu Data Buku\n";
                cout << "> "; 
                cin.ignore();
                cin.get();
                system("cls");
                
            }
            else if (choice == 2)
            {
                system("cls");
                displayMetadataTree();
                cout << "\n0. Back to Menu Data Buku\n";
                cout << "> "; 
                cin.ignore();
                cin.get();
                system("cls");
                
            }
            else if (choice == 3) {
                system("cls");
                string category;
                cout << "Masukkan Kategori Buku untuk Ditampilkan: ";
                cin >> category;
                displayBooksByCategory(category);
                cout << "0. Exit\n";
                cout << "> ";
                cin.ignore();
                cin.get();
                system("cls");
            }
            else if (choice == 4) {
                system("cls");
                sortBooksTitle();
                cout << "0. Exit\n";
                cout << "> ";
                cin.ignore();
                cin.get();
                system("cls");

            }
            else if (choice == 5) {
                system("cls");
                sortBooksID();
                cout << "0. Exit\n";
                cout << "> ";
                cin.ignore();
                cin.get();
                system("cls");

            }
            else if (choice == 0) 
            {
                system("cls");
                break;
            }
            else 
            {
                cout << "Pilihan tidak valid. Silakan coba lagi.\n";
                cin.ignore();
                cout << "\n";
            }
        }
    }

    void menuCariBuku() {
        int choice;
        system("cls");
        while (true) {
        cout << "Menu Searching Book\n";
        cout << "1. Cari Buku Berdasarkan ID\n";
        cout << "2. Cari Buku Berdasarkan Judul\n";
        cout << "0. Exit\n";
        cout << "> "; 
        cin >> choice;
            if (choice == 1)
            {
                system("cls");
                int id;
                cout << "Masukkan ID Buku yang dicari: ";
                cin >> id;
                searchBook(id);
                break;
            }
            else if (choice == 2)
            {
                system("cls");
                char title[100];
                cout << "Masukkan Judul Buku yang dicari: ";
                cin.ignore();
                cin.getline(title, sizeof(title));
                searchBookByTitle(title);
                break;
            }
            else if (choice == 0) 
            {
                system("cls");
                break;
            }
            else 
            {
                cout << "Pilihan tidak valid. Silakan coba lagi.\n";
                cin.ignore();
                cout << "\n";
            }
        }
    }

    void toolBooks() {
        system("cls");
        int  choice;
        string location = "Main Library";
        while (true) {
            cout << "Menu Tools Book\n";
            cout << "1. Tambah Buku\n";
            cout << "2. Hapus Buku dari Perpustakaan\n";
            cout << "3. Edit Informasi Buku\n";
            cout << "0. Exit\n";
            cout << "> \n";
            cin >> choice;
                if (choice == 1) {
                    system("cls");
                    int id, quantity;
                    char title[100], author[100];
                    string category;
                    cout << "Masukkan ID Buku: ";
                    cin >> id;
                    cout << "Masukkan Judul Buku: ";
                    cin.ignore();
                    cin.getline(title, sizeof(title));
                    cout << "Masukkan Nama Penulis: ";
                    cin.getline(author, sizeof(author));
                    cout << "Masukkan Kuantitas Buku: ";
                    cin >> quantity;
                    cout << "Masukkan Kategori Buku: ";
                    cin >> category;
                    addBook(id, title, author, quantity, category, location);
                    break;
                }
                else if (choice == 2) {
                    system("cls");
                    int id;
                    cout << "Masukkan ID Buku yang akan dihapus: ";
                    cin >> id;
                    deleteBook(id);
                    break;    
                }            
                else if (choice == 3) {
                    system("cls");
                    int id;
                    cout << "Masukkan ID Buku yang akan diubah: ";
                    cin >> id;
                    editBook(id);
                    break;
                }
                else if (choice == 0) 
                {
                    system("cls");
                    break;
                }
                else 
                {
                    cout << "Pilihan tidak valid. Silakan coba lagi.\n";
                    cin.ignore();
                    cout << "\n";
                }
       }
    }

    void pinjamBuku() {
        system("cls");
        int choice;
        while (true) {
            cout << "Menu Memimjam Buku\n";
            cout << "1. Pendaftaraan Pinjam Buku\n";
            cout << "2. Data Pemimjamnan Buku\n";
            cout << "0. Exit\n";
            cout << "> ";

            cin >> choice;

            if (choice == 1) {
                system("cls");
                int id, quantity;
                string borrowerName;
                cout << "Masukkan ID Buku yang akan dipinjam: ";
                cin >> id;
                cout << "Masukkan Nama Peminjam: ";
                cin.ignore();
                getline(cin, borrowerName);
                cout << "Masukkan Jumlah Buku yang akan dipinjam: ";
                cin >> quantity;
                borrowBookV2(id, borrowerName, quantity);
                break;
            }
            else if (choice == 2) {
                system("cls");
                cout << "Data Peminjaman Buku\n";
                displayTransactions();
                cin.ignore();
                cin.get();
                break;                    
            }
            else if (choice == 0) 
            {
                system("cls");
                break;
            }
            else 
            {
                cout << "Pilihan tidak valid. Silakan coba lagi.\n";
                cin.ignore();
                cout << "\n";
            }
        }
    }

    void menuGraph() {
        system("cls");
        int choice;
        while (true) {
            cout << "Menu Graph\n";
            cout << "1. Tambah Lokasi Perpustakaan\n";
            cout << "2. Display Edge Perpustakaan\n";
            cout << "0. Exit\n";
            cout << "> ";
            cin >> choice;

            if (choice == 1) {
                system("cls");
                string location1, location2;
                cout << "Masukkan Nama Lokasi 1: ";
                cin.ignore();
                getline(cin, location1);
                cout << "Masukkan Nama Lokasi 2: ";
                getline(cin, location2);
                addLibraryLocation(location1, location2);
                break;
            }
            else if (choice == 2) {
                system("cls");
                displayLibraryGraph();
                cin.ignore();
            }
            else if (choice == 0) {
                system("cls");
                break;
            }
            else {
                cout << "Pilihan tidak valid. Silakan coba lagi.\n";
                cin.ignore();
                cout << "\n";
        }
    }
    }

    void printLibrary() {   
    cout << "        ____       _____      ____                          " << "\n";
    cout << "       |    |     |     |    |    |                         " << "\n";
    cout << "      _|    |_   _|     |_  _|    | _                       " << "\n";
    cout << "     |         ||         ||         |         __   __      " << "\n";
    cout << "     |         ||         ||    /|   |     ___/ /__/ |      " << "\n";
    cout << "     |         ||         ||   / |   |    (__/ /_____|      " << "\n";
    cout << "     |         ||   <3    ||   | |   |      /_/             " << "\n";
    cout << "     |          PERPUSTAKAAN   | |   |                      " << "\n";
    cout << "     |             HANFIK      | |   |                      " << "\n";
    cout << "     |         ||         ||   |_|   |                      " << "\n";
    cout << "     |         ||         ||    ||   |                      " << "\n";
    cout << "     |         ||         ||    ||   |                      " << "\n";
    cout << "     |         ||         ||         |        #HEWANQURBAN  " << "\n";
    cout << "     |_________||_________||_________|        #QURBANUNESA  " << "\n";
    cout << "     | |     | || |     | || |     | |       (__)           " << "\n";
    cout << "     | |     | || |     | || |     | |       (oo)_______    " << "\n";
    cout << "    _|_|_____|_||_|_____|_||_|_____|_|_      (__) UNESA )\\ " << "\n";
    cout << "   |           ||         ||            |       ||----||    " << "\n";
    cout << "   |___________||_________||____________|                   " << "\n";
    }

    ~Library()
    {
        saveBooksToFile();

        while (head)
        {
            Book *temp = head;
            head = head->next;
            delete temp;
        }
    }
};

int main()
{
    Library library;
    system("cls");
    int choice, id, quantity;
    char title[100], author[100];
    string category, borrowerName, borrowDate, returnDate;
    bool isRunning = true;

    do
    {
        library.printLibrary();
        cout << "\n";
        cout << "Perpustakaan Hanfik\n";
        cout << "Menu:\n";
        cout << "1. Tools Buku\n";
        cout << "2. Data Buku\n";
        cout << "3. Cari Buku\n";
        cout << "4. Pinjam buku\n";
        cout << "5. Menu Graph\n";
        cout << "6. Hitung Jumlah Buku\n";
        cout << "7. Testing Functions\n";
        cout << "8. Testing Functions\n";
        cout << "0. Keluar\n";
        cout << "> ";
        cin >> choice;

        switch (choice)
        { case 1: {
            system("cls");
            library.toolBooks();
            break;
        } case 2: {
            system("cls");
            library.menuDataLibrary();
            break;
        } case 3: {
            system("cls");
            library.menuCariBuku();
            break;
        } case 4: {
            system("cls");
            library.pinjamBuku(); 
            break;
        } case 5: {
            system("cls");
            library.menuGraph();
            break;
        } case 6: {
            system("cls");
            cout << "Jumlah Buku dalam Perpustakaan: " << library.countBooks() << "\n";
            cout << "\n";
            break;
        } case 7: {
            system("cls");
            cout << "Testing Functions\n";
            break;
        } case 8: {
            system("cls");
            cout << "Testing Functions\n";
            break;
        } case 0:
            system ("cls");
            isRunning = false;
            cout << "THANK YOU FOR USING THIS FUNCTION\n";
            break;
        default:
            cout << "Pilihan tidak valid.\n";
        }
    } while (choice != 0);

    return 0;
}