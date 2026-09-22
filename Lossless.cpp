#include <iostream>
#include <fstream>
#include <unordered_map>
#include <queue>
#include <string>
#include <vector>
#include <iomanip>

using namespace std;

struct Node {
    char data;
    unsigned freq;
    Node *left, *right;
    Node(char d, unsigned f) : data(d), freq(f), left(nullptr), right(nullptr) {}
};

struct Compare {
    bool operator()(Node* l, Node* r) { 
        return l->freq > r->freq; 
    }
};

class HuffmanCompressor {
private:
    unordered_map<char, string> huffmanCode;
    unordered_map<char, unsigned> freqMap;
    Node* root;

    void generateCodes(Node* node, string str) {
        if (!node) return;
        if (!node->left && !node->right) {
            huffmanCode[node->data] = (str.empty()) ? "0" : str;
            return;
        }
        generateCodes(node->left, str + "0");
        generateCodes(node->right, str + "1");
    }

    void buildTree() {
        freeTree(root);
        root = nullptr;
        
        priority_queue<Node*, vector<Node*>, Compare> pq;
        for (auto pair : freqMap) {
            pq.push(new Node(pair.first, pair.second));
        }

        if (pq.size() == 1) {
            Node* only = pq.top(); pq.pop();
            root = new Node('$', only->freq);
            root->left = only;
            generateCodes(root, "");
            return;
        }

        while (pq.size() > 1) {
            Node *left = pq.top(); pq.pop();
            Node *right = pq.top(); pq.pop();
            Node *top = new Node('$', left->freq + right->freq);
            top->left = left;
            top->right = right;
            pq.push(top);
        }
        root = pq.empty() ? nullptr : pq.top();
        generateCodes(root, "");
    }

    void freeTree(Node* node) {
        if (!node) return;
        freeTree(node->left);
        freeTree(node->right);
        delete node;
    }

public:
    HuffmanCompressor() : root(nullptr) {}
    ~HuffmanCompressor() { freeTree(root); }

    void compress(const string& inputFile, const string& outputFile) {
        ifstream inFile(inputFile, ios::binary);
        if (!inFile) {
            cerr << "Error: Cannot open " << inputFile << endl;
            return;
        }

        freqMap.clear();
        huffmanCode.clear();

        char ch;
        unsigned totalChars = 0;
        while (inFile.get(ch)) {
            freqMap[ch]++;
            totalChars++;
        }
        inFile.clear();
        inFile.seekg(0);

        if (totalChars == 0) {
            cout << "Warning: Empty file provided." << endl;
            return;
        }

        buildTree();

        ofstream outFile(outputFile, ios::binary);
        
        int mapSize = freqMap.size();
        outFile.write(reinterpret_cast<const char*>(&mapSize), sizeof(mapSize));
        outFile.write(reinterpret_cast<const char*>(&totalChars), sizeof(totalChars));
        
        for (const auto& pair : freqMap) {
            outFile.write(&pair.first, sizeof(pair.first));
            outFile.write(reinterpret_cast<const char*>(&pair.second), sizeof(pair.second));
        }

        string bitString = "";
        while (inFile.get(ch)) {
            bitString += huffmanCode[ch];
            while (bitString.length() >= 8) {
                char byte = 0;
                for (int i = 0; i < 8; ++i) {
                    if (bitString[i] == '1') byte |= (1 << (7 - i));
                }
                outFile.write(&byte, 1);
                bitString = bitString.substr(8);
            }
        }

        if (!bitString.empty()) {
            char byte = 0;
            for (size_t i = 0; i < bitString.length(); ++i) {
                if (bitString[i] == '1') byte |= (1 << (7 - i));
            }
            outFile.write(&byte, 1);
        }

        inFile.close();
        
        long inSize = totalChars;
        long outSize = outFile.tellp();
        outFile.close();

        cout << "\n================ COMPRESSION SUMMARY ================" << endl;
        cout << "Original Size   : " << inSize << " bytes" << endl;
        cout << "Compressed Size : " << outSize << " bytes" << endl;
        double reduction = (1.0 - (double)outSize / inSize) * 100.0;
        cout << "Space Saved     : " << fixed << setprecision(2) << reduction << "%" << endl;
        cout << "====================================================\n" << endl;
    }

    void decompress(const string& inputFile, const string& outputFile) {
        ifstream inFile(inputFile, ios::binary);
        if (!inFile) {
            cerr << "Error: Cannot open " << inputFile << endl;
            return;
        }

        int mapSize;
        unsigned totalChars;
        inFile.read(reinterpret_cast<char*>(&mapSize), sizeof(mapSize));
        inFile.read(reinterpret_cast<char*>(&totalChars), sizeof(totalChars));

        freqMap.clear();
        for (int i = 0; i < mapSize; ++i) {
            char key;
            unsigned value;
            inFile.read(&key, sizeof(key));
            inFile.read(reinterpret_cast<char*>(&value), sizeof(value));
            freqMap[key] = value;
        }

        buildTree();

        ofstream outFile(outputFile, ios::binary);
        Node* curr = root;
        unsigned charactersDecoded = 0;
        char byte;

        while (charactersDecoded < totalChars && inFile.read(&byte, 1)) {
            for (int i = 0; i < 8; ++i) {
                bool bit = (byte >> (7 - i)) & 1;
                curr = bit ? curr->right : curr->left;

                if (!curr->left && !curr->right) {
                    outFile.put(curr->data);
                    charactersDecoded++;
                    curr = root;
                    if (charactersDecoded == totalChars) break;
                }
            }
        }

        inFile.close();
        outFile.close();
        cout << "Decompression finished. Recovered " << charactersDecoded << " bytes cleanly." << endl;
    }
};

int main() {
    ofstream sampleIn("input.txt");
    for (int i = 0; i < 1000; ++i) {
        sampleIn << "IIT Delhi Computer Science and Mathematics Huffman Compression Test. ";
    }
    sampleIn.close();

    HuffmanCompressor hc;
    cout << "Compressing 'input.txt' -> 'compressed.bin'..." << endl;
    hc.compress("input.txt", "compressed.bin");

    cout << "Decompressing 'compressed.bin' -> 'output.txt'..." << endl;
    hc.decompress("compressed.bin", "output.txt");

    return 0;
}