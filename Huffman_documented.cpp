/**
 * @file huffmandokumentasi.cpp
 * @brief Program implementasi algoritma Huffman Coding.
 *
 * Program ini digunakan untuk melakukan kompresi data menggunakan
 * algoritma Huffman Coding. Setiap karakter diberikan kode biner
 * berdasarkan frekuensi kemunculannya dalam teks.
 *
 * Karakter dengan frekuensi kemunculan lebih tinggi akan mendapatkan
 * kode biner yang lebih pendek, sedangkan karakter dengan frekuensi
 * kemunculan lebih rendah akan mendapatkan kode biner yang lebih panjang.
 *
 * Program membangun Huffman Tree berdasarkan frekuensi setiap karakter,
 * menghasilkan kode Huffman, melakukan Encoding, dan melakukan Decoding
 * untuk memperoleh kembali teks asli.
 *
 * Program juga dilengkapi dengan penanganan kesalahan, penghapusan
 * Node untuk mencegah memory leak, serta beberapa test case untuk
 * menguji keberhasilan dan penanganan kesalahan proses Huffman Coding.
 */

#include <iostream>
#include <queue>
#include <unordered_map>
#include <vector>
#include <string>
#include <stdexcept>
#include <cstdio>
using namespace std;

// Huffman Tree Node

/**
 * @struct Node
 * @brief Merepresentasikan sebuah Node pada Huffman Tree.
 *
 * Setiap Node memiliki empat komponen:
 * - ch: Menyimpan karakter yang direpresentasikan oleh Node.
 * - freq: Menyimpan frekuensi kemunculan karakter atau jumlah
 *   frekuensi dari kedua Node anak.
 * - left: Pointer menuju Left Child.
 * - right: Pointer menuju Right Child.
 *
 * Leaf Node menyimpan karakter asli, sedangkan Internal Node
 * digunakan untuk menghubungkan dua Node anak.
 */
struct Node {
    char ch;
    int freq;
    Node* left;
    Node* right;

    /**
     * @brief Constructor untuk membuat Node.
     *
     * Menginisialisasi karakter, frekuensi, serta Pointer
     * menuju Left Child dan Right Child.
     *
     * @param ch Karakter yang disimpan dalam Node.
     * @param freq Frekuensi kemunculan karakter.
     * @param left Pointer menuju Left Child.
     * @param right Pointer menuju Right Child.
     *
     * @note Parameter left dan right memiliki nilai awal nullptr
     *       sehingga dapat digunakan untuk membuat Leaf Node
     *       tanpa memiliki anak.
     */
    Node(char ch, int freq, Node* left = nullptr, Node* right = nullptr)
        : ch(ch), freq(freq), left(left), right(right) {}
};

// Mengatur prioritas berdasarkan frekuensi terkecil

/**
 * @struct compare
 * @brief Mengatur urutan prioritas Node pada Priority Queue.
 *
 * Struct ini digunakan sebagai Comparison Object untuk mengatur
 * Node berdasarkan frekuensi kemunculannya.
 *
 * Node dengan frekuensi terkecil akan memiliki prioritas tertinggi
 * sehingga dapat diambil terlebih dahulu dari Priority Queue.
 */
struct compare {

    /**
     * @brief Membandingkan frekuensi antara dua Node.
     *
     * Operator ini membandingkan frekuensi Node l dan Node r
     * untuk menentukan urutan prioritas di dalam Priority Queue.
     *
     * @param l Pointer menuju Node pertama yang dibandingkan.
     * @param r Pointer menuju Node kedua yang dibandingkan.
     *
     * @return true jika frekuensi Node l lebih besar daripada Node r,
     *         dan false jika sebaliknya.
     */
    bool operator()(Node* l, Node* r) {
        return l->freq > r->freq;
    }
};

// Membuat kode Huffman untuk setiap karakter

/**
 * @brief Menghasilkan kode Huffman untuk setiap karakter.
 *
 * Fungsi melakukan Traversal terhadap Huffman Tree menggunakan
 * rekursi. Traversal ke Left Child menambahkan bit 0, sedangkan
 * Traversal ke Right Child menambahkan bit 1.
 *
 * Ketika mencapai Leaf Node, rangkaian bit yang terbentuk
 * disimpan ke dalam unordered_map sebagai kode Huffman
 * dari karakter tersebut.
 *
 * Jika hanya terdapat satu karakter unik, karakter tersebut
 * diberikan kode "0" agar tetap memiliki representasi biner.
 *
 * @param root Pointer menuju Node yang sedang diperiksa.
 * @param str String yang menyimpan rangkaian bit sementara
 *            selama proses Traversal.
 * @param huffmanCode unordered_map untuk menyimpan pasangan
 *                    karakter dan kode Huffman.
 *
 * @note Fungsi menggunakan rekursi untuk menelusuri Left Child
 *       dan Right Child hingga mencapai Leaf Node.
 */
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

/**
 * @brief Menghapus seluruh Node pada Huffman Tree.
 *
 * Fungsi melakukan Traversal secara rekursif untuk menghapus
 * seluruh Node yang telah dialokasikan menggunakan operator new.
 *
 * Penghapusan dilakukan terhadap Left Child dan Right Child
 * terlebih dahulu sebelum menghapus Node saat ini.
 *
 * Proses ini bertujuan membebaskan memori yang digunakan oleh
 * Huffman Tree sehingga dapat mencegah memory leak.
 *
 * @param root Pointer menuju Root atau Node yang akan dihapus.
 *
 * @note Jika root bernilai nullptr, fungsi langsung dihentikan.
 */
void freeTree(Node* root) {
    if (root == nullptr)
        return;

    freeTree(root->left);
    freeTree(root->right);
    delete root;
}

// Mengubah kembali encoded string menjadi teks asli

/**
 * @brief Mengubah encoded string menjadi teks asli.
 *
 * Fungsi melakukan Decoding dengan membaca setiap bit pada
 * encoded string dan mengikuti jalur Huffman Tree.
 *
 * Bit 0 mengarahkan Traversal ke Left Child, sedangkan bit 1
 * mengarahkan Traversal ke Right Child.
 *
 * Ketika mencapai Leaf Node, karakter ditambahkan ke dalam
 * result dan Traversal dimulai kembali dari Root.
 *
 * Fungsi juga menangani beberapa kondisi kesalahan, yaitu
 * Huffman Tree yang tidak tersedia, bit yang tidak valid,
 * jalur Huffman yang tidak valid, dan bitstring yang terpotong.
 *
 * Jika Huffman Tree hanya memiliki satu Leaf Node, setiap bit
 * yang valid akan menghasilkan karakter tersebut.
 *
 * @param root Pointer menuju Root dari Huffman Tree.
 * @param bits String yang berisi kode biner hasil Encoding.
 *
 * @return String yang berisi teks hasil Decoding.
 *
 * @throws runtime_error Jika Huffman Tree tidak tersedia
 *         atau bitstring berakhir sebelum Traversal kembali
 *         ke Root.
 *
 * @throws invalid_argument Jika terdapat bit selain 0 dan 1
 *         atau jalur Huffman yang dituju tidak valid.
 */
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

/**
 * @brief Membangun Huffman Tree dan melakukan pengujian.
 *
 * Fungsi ini merupakan proses utama dalam implementasi
 * algoritma Huffman Coding.
 *
 * Proses dimulai dengan memeriksa teks masukan dan menghitung
 * frekuensi kemunculan setiap karakter menggunakan unordered_map.
 *
 * Setiap karakter dibuat menjadi Leaf Node dan dimasukkan
 * ke dalam Priority Queue. Selanjutnya, dua Node dengan
 * frekuensi terkecil diambil dan digabungkan menjadi Internal Node.
 *
 * Proses penggabungan dilakukan berulang kali hingga hanya
 * tersisa satu Node yang menjadi Root dari Huffman Tree.
 *
 * Setelah Huffman Tree terbentuk, fungsi printCodes() dipanggil
 * untuk menghasilkan kode Huffman setiap karakter.
 *
 * Kode Huffman digunakan untuk melakukan Encoding terhadap
 * teks asli. Hasil Encoding kemudian diproses menggunakan
 * decodeHuffman() untuk memperoleh kembali teks asli.
 *
 * Fungsi juga menghitung ukuran data sebelum dan sesudah
 * Encoding serta persentase penghematan ukuran data.
 *
 * Parameter mode digunakan untuk menjalankan pengujian khusus:
 * - Nilai 0 menjalankan proses normal.
 * - Nilai 1 menambahkan karakter tidak valid pada encoded string.
 * - Nilai 2 menghapus bit terakhir dari encoded string.
 *
 * Seluruh Node yang telah dibuat akan dihapus menggunakan
 * freeTree(), baik ketika proses berhasil maupun ketika
 * terjadi pengecualian di dalam blok try.
 *
 * @param text Teks asli yang akan diproses menggunakan
 *             algoritma Huffman Coding.
 * @param mode Mode pengujian dengan nilai awal 0.
 *
 * @throws invalid_argument Jika teks masukan kosong,
 *         terdapat bit yang tidak valid, atau jalur Huffman
 *         yang dituju tidak valid.
 *
 * @throws runtime_error Jika Huffman Tree tidak tersedia,
 *         bitstring terpotong, atau hasil Decoding tidak
 *         sesuai dengan teks asli.
 *
 * @note Fungsi menggunakan try-catch untuk memastikan
 *       seluruh Node dihapus ketika terjadi pengecualian
 *       selama proses Encoding dan Decoding.
 */
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

/**
 * @brief Fungsi utama program dan pelaksanaan pengujian.
 *
 * Fungsi main() merupakan titik awal eksekusi program.
 *
 * Fungsi membuat vector bernama tests yang berisi pasangan
 * teks dan mode pengujian. Setiap pasangan data diproses
 * menggunakan perulangan for dengan memanggil buildHuffmanTree().
 *
 * Setiap pengujian dibungkus menggunakan try-catch agar
 * pengecualian dapat ditangani dan pengujian berikutnya
 * tetap dapat dijalankan.
 *
 * Test case mencakup teks normal, teks dengan satu karakter unik,
 * teks kosong, karakter campuran, serta kondisi encoded string
 * yang mengandung bit tidak valid dan bitstring yang terpotong.
 *
 * Variabel no digunakan untuk memberikan nomor pada setiap
 * pengujian yang dijalankan.
 *
 * @return 0 jika seluruh proses pengujian selesai dijalankan.
 *
 * @note Setiap elemen tests menyimpan teks pada first
 *       dan mode pengujian pada second.
 */
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
