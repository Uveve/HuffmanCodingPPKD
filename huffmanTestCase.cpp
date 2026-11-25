
#include <iostream>
#include <queue>
#include <unordered_map>
#include <vector>
#include <string>
#include <stdexcept>
#include <cstdio>
using namespace std;

// Huffman Tree Node
struct Node {
    char ch;
    int freq;
    Node* left;
    Node* right;

    Node(char ch, int freq, Node* left = nullptr,
         Node* right = nullptr)
        : ch(ch), freq(freq), left(left), right(right) {}
};

// Mengatur prioritas berdasarkan frekuensi terkecil
struct compare {
    bool operator()(Node* l, Node* r) {
        return l->freq > r->freq;
    }
};

// Membuat kode Huffman untuk setiap karakter
void printCodes(Node* root, string str,
                unordered_map<char, string>& huffmanCode) {
    if (root == nullptr)
        return;

    // Jika sudah mencapai leaf node
    if (!root->left && !root->right) {
        // Menangani kasus hanya satu karakter unik
        huffmanCode[root->ch] = str.empty() ? "0" : str;
        return;
    }

    printCodes(root->left, str + "0", huffmanCode);
    printCodes(root->right, str + "1", huffmanCode);
}

// Menghapus seluruh node untuk mencegah memory leak
void freeTree(Node* root) {
    if (root == nullptr)
        return;

    freeTree(root->left);
    freeTree(root->right);
    delete root;
}

// Mengubah kembali encoded string menjadi teks asli
string decodeHuffman(Node* root, const string& bits) {
    if (root == nullptr)
        throw runtime_error("Huffman Tree tidak tersedia");

    string result;
    Node* curr = root;

    bool single = !root->left && !root->right;

    for (char bit : bits) {
        // Memastikan input hanya berisi 0 dan 1
        if (bit != '0' && bit != '1') {
            throw invalid_argument("Bit tidak valid");
        }

        // Jika hanya ada satu karakter unik
        if (single) {
            result += root->ch;
            continue;
        }

        // Mengikuti jalur pohon berdasarkan bit
        curr = (bit == '0') ? curr->left : curr->right;

        if (curr == nullptr)
            throw invalid_argument("Jalur Huffman tidak valid");

        // Jika mencapai leaf node
        if (!curr->left && !curr->right) {
            result += curr->ch;
            curr = root;
        }
    }

    // Memeriksa apakah bitstring berakhir di tengah kode
    if (!single && curr != root) {
        throw runtime_error("Bitstring terpotong");
    }

    return result;
}

// Membangun Huffman Tree dan melakukan pengujian
void buildHuffmanTree(string text, int mode = 0) {
    // Menangani input kosong
    if (text.empty()) {
        throw invalid_argument("Teks kosong");
    }

    // Menghitung frekuensi setiap karakter
    unordered_map<char, int> freq;

    for (char ch : text) {
        freq[ch]++;
    }

    // Membuat priority queue
    priority_queue<Node*, vector<Node*>, compare> pq;

    // Membuat leaf node berdasarkan frekuensi karakter
    for (auto pair : freq) {
        pq.push(new Node(pair.first, pair.second));
    }

    // Membentuk Huffman Tree
    while (pq.size() > 1) {
        Node* left = pq.top();
        pq.pop();

        Node* right = pq.top();
        pq.pop();

        int sum = left->freq + right->freq;

        pq.push(new Node('\0', sum, left, right));
    }

    // Mengambil root dari Huffman Tree
    Node* root = pq.top();

    try {
        unordered_map<char, string> huffmanCode;

        printCodes(root, "", huffmanCode);

        // Menampilkan Huffman Codes
        cout << "Huffman Codes:\n";

        for (auto pair : huffmanCode) {
            cout << pair.first << " " << pair.second << "\n";
        }

        // Menampilkan teks asli
        cout << "\nOriginal string:\n" << text << "\n";

        // Melakukan encoding
        string encoded;

        for (char ch : text) {
            encoded += huffmanCode[ch];
        }

        // Mode khusus untuk pengujian input rusak
        if (mode == 1) {
            encoded.insert(2, "x");
        }
        else if (mode == 2) {
            encoded.pop_back();
        }

        cout << "\nEncoded string:\n" << encoded << "\n";

        // Melakukan decoding
        string decoded = decodeHuffman(root, encoded);

        cout << "\nDecoded string:\n" << decoded << "\n";

        // Menghitung ukuran data
        int originalBits = text.size() * 8;
        int encodedBits = encoded.size();

        double saving = (1.0 -
            (double)encodedBits / originalBits) * 100;

        cout << "\nUkuran: " << originalBits
             << " -> " << encodedBits
             << " bit (hemat " << saving << "%)\n";

        // Memastikan hasil decoding sama dengan teks asli
        if (decoded != text) {
            throw runtime_error("Hasil decoding tidak sesuai");
        }

    }
    catch (...) {
        freeTree(root);
        throw;
    }

    freeTree(root);
}

// Main program dan daftar test case
int main() {
    vector<pair<string, int>> tests = {
        {"HUFFMAN", 0},
        {"Halo, besok tugas alpro dikumpul jam 10!", 0},
        {"AAAAAAAA", 0},
        {"", 0},
        {"ABCD", 0},
        {"Aa Bb 123 !?", 0},
        {"HUFFMAN", 1},
        {"HUFFMAN", 2}
    };

    int no = 1;

    for (auto& t : tests) {
        cout << "========== TEST " << no++ 
             << " ==========\n";

        try {
            buildHuffmanTree(t.first, t.second);
        }
        catch (const exception& e) {
            cout << "EXCEPTION: " << e.what() << "\n";
        }

        cout << "\n";
    }

    return 0;
}
