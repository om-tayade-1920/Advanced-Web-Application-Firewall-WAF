#include <bits/stdc++.h>
using namespace std;

// Huffman Tree Node
struct Node {
    char ch;
    int freq;
    Node *left, *right;

    Node(char c, int f) {
        ch = c;
        freq = f;
        left = right = nullptr;
    }
};

// Comparator for Min Heap
struct Compare {
    bool operator()(Node* l, Node* r) {
        return l->freq > r->freq;
    }
};

// Generate Huffman Codes
void generateCodes(Node* root, string code,
                   unordered_map<char, string>& huffmanCode) {
    if (!root)
        return;

    if (!root->left && !root->right)
        huffmanCode[root->ch] = code;

    generateCodes(root->left, code + "0", huffmanCode);
    generateCodes(root->right, code + "1", huffmanCode);
}


// Build Huffman Tree
Node* buildHuffmanTree(unordered_map<char, int>& freq) {
    priority_queue<Node*, vector<Node*>, Compare> pq;

    for (auto p : freq)
        pq.push(new Node(p.first, p.second));

    while (pq.size() > 1) {
        Node* left = pq.top(); pq.pop();
        Node* right = pq.top(); pq.pop();

        Node* merged = new Node('\0', left->freq + right->freq);
        merged->left = left;
        merged->right = right;

        pq.push(merged);
    }
    return pq.top();
}

// Get File Size
long getFileSize(const string& filename) {
    ifstream file(filename, ios::binary | ios::ate);
    return file.tellg();
}

// Compress File
void compressFile(const string& inputFile) {
    ifstream in(inputFile);
    string text((istreambuf_iterator<char>(in)),
                 istreambuf_iterator<char>());
    in.close();

    unordered_map<char, int> freq;
    for (char ch : text)
        freq[ch]++;

    Node* root = buildHuffmanTree(freq);

    unordered_map<char, string> huffmanCode;
    generateCodes(root, "", huffmanCode);

    // Display Codewords
    cout << "\nHuffman Code Table:\n";
    cout << "Char\tFreq\tCode\n";
    for (auto p : freq)
        cout << p.first << "\t" << p.second << "\t" << huffmanCode[p.first] << endl;

    // Save codes
    ofstream codeFile("codes.txt");
    for (auto p : huffmanCode)
        codeFile << p.first << " " << p.second << endl;
    codeFile.close();

    // Encode data
    string encoded = "";
    for (char ch : text)
        encoded += huffmanCode[ch];

    // Write compressed binary
    ofstream out("compressed.bin", ios::binary);
    for (int i = 0; i < encoded.size(); i += 8) {
        string byte = encoded.substr(i, 8);
        while (byte.size() < 8)
            byte += '0';
        char b = bitset<8>(byte).to_ulong();
        out.write(&b, 1);
    }
    out.close();

    // Size calculations
    long originalSize = getFileSize(inputFile);
    long compressedSize = getFileSize("compressed.bin");

    double reduction = 
        ((double)(originalSize - compressedSize) / originalSize) * 100;

    cout << "\nOriginal File Size   : " << originalSize << " bytes";
    cout << "\nCompressed File Size : " << compressedSize << " bytes";
    cout << "\nCompression Ratio    : " 
         << fixed << setprecision(2)
         << (double)originalSize / compressedSize;
    cout << "\nFile Size Reduction  : " 
         << reduction << " %\n";

    cout << "\nFile compressed successfully!\n";
}

// Decompress File
void decompressFile() {
    unordered_map<string, char> reverseCode;
    ifstream codeFile("codes.txt");
    char ch;
    string code;

    while (codeFile >> ch >> code)
        reverseCode[code] = ch;
    codeFile.close();

    ifstream in("compressed.bin", ios::binary);
    string binary = "";
    char byte;

    while (in.read(&byte, 1))
        binary += bitset<8>(byte).to_string();
    in.close();

    ofstream out("output.txt");
    string temp = "";

    for (char bit : binary) {
        temp += bit;
        if (reverseCode.count(temp)) {
            out << reverseCode[temp];
            temp = "";
        }
    }
    out.close();

    cout << "File decompressed successfully!\n";
}

// Main
int main() {
    string inputFile = "input.txt";
    compressFile("input.txt");
    decompressFile();
    return 0;
}
