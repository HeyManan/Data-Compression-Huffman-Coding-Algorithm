#include <bits/stdc++.h>
using namespace std;

// Defining Huffman Tree Node
struct Node
{
  char data;
  unsigned freq;
  string code;
  Node *left, *right;

  Node()
  {
    left = right = NULL;
  }
};

class Huffman
{
private:
  string input_file, output_file, encoded_data;

  vector<Node *> freq_arr;

  vector<unsigned char> decoded_data;

  fstream in_file, out_file;

  Node *root;

  class Comp
  {
  public:
    bool operator()(Node *l, Node *r)
    {
      return l->freq > r->freq;
    }
  };

  priority_queue<Node *, vector<Node *>, Comp> minHeap;

  // Create frequency array
  void create_freq_array();

  // Create minHeap
  void create_minHeap();

  // Create Huffman Tree
  void create_huffmanTree();

  void assign_codes_helper(Node *, string);

  // Assign huffman codes to nodes
  void assign_codes();

  int bin_to_dec(string);

  // Adds minHeap size and data to encoded data
  void add_meta_data();

  // Adds encoded input data in encoded data
  void add_encoded_input();

  // Creates new file with .huf extension and saves the encoded data
  void save_encoded_data();

  string dec_to_bin(int num);

  void build_tree(char, string &);

  // Build the tree according to encoded values from encoded data file
  void get_huffmanTree();

  // Get the decoded data of the original data by removing meta data from encoded data
  void get_decoded_data();

  // Save the decompressed file
  void save_decoded_file();

public:
  // Constructor
  Huffman(string input_file, string output_file)
  {
    this->input_file = input_file;
    this->output_file = output_file;
    create_freq_array();
  }

  // Compresses input file and saves the encoded file
  void compress();

  // Decompresses the encoded file and saves it
  void decompress();
};