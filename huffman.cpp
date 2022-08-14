#include "huffman.hpp"

// Create frequency array
void Huffman::create_freq_array()
{
  for (int i = 0; i < 128; i++)
  {
    freq_arr.push_back(new Node());
    freq_arr[i]->data = i;
    freq_arr[i]->freq = 0;
  }
}

// Create minHeap
void Huffman::create_minHeap()
{
  char ch;

  in_file.open(input_file, ios::in);

  in_file.get(ch);
  while (!in_file.eof())
  {
    freq_arr[ch]->freq++;
    in_file.get(ch);
  }

  in_file.close();

  for (int i = 0; i < 128; i++)
  {
    if (freq_arr[i]->freq > 0)
    {
      minHeap.push(freq_arr[i]);
    }
  }
}

// Create Huffman Tree
void Huffman::create_huffmanTree()
{
  Node *node1, *node2;
  priority_queue<Node *, vector<Node *>, Comp> tempQ(minHeap);
  while (tempQ.size() != 1)
  {
    node1 = tempQ.top();
    tempQ.pop();

    node2 = tempQ.top();
    tempQ.pop();

    root = new Node();
    root->freq = node1->freq + node2->freq;

    root->left = node1;
    root->right = node2;
    tempQ.push(root);
  }
}

void Huffman::assign_codes_helper(Node *rt, string str)
{
  if (rt->left == NULL && rt->right == NULL)
  {
    rt->code = str;
    return;
  }

  assign_codes_helper(rt->left, str + '0');
  assign_codes_helper(rt->right, str + '1');
}

// Assign huffman codes to nodes
void Huffman::assign_codes()
{
  assign_codes_helper(root, "");
}

int Huffman::bin_to_dec(string str)
{
  int res = 0;
  for (auto c : str)
  {
    res = res * 2 + c - '0';
  }
  return res;
}

// Adds minHeap size and data to encoded data
void Huffman::add_meta_data()
{
  string temp = "";
  encoded_data = "";

  encoded_data += (char)minHeap.size();
  priority_queue<Node *, vector<Node *>, Comp> tempQ(minHeap);
  while (!tempQ.empty())
  {
    Node *cur = tempQ.top();
    encoded_data += cur->data;

    temp.assign(127 - cur->code.length(), '0');
    temp += '1';
    temp += cur->code;

    encoded_data += (char)bin_to_dec(temp.substr(0, 8));
    for (int i = 0; i < 15; i++)
    {
      temp = temp.substr(8);
      encoded_data += (char)bin_to_dec(temp.substr(0, 8));
    }

    tempQ.pop();
  }
}

// Adds encoded input data in encoded data
void Huffman::add_encoded_input()
{
  char ch;
  string temp = "";
  in_file.open(input_file, ios::in);

  in_file.get(ch);
  while (!in_file.eof())
  {
    temp += freq_arr[ch]->code;

    while (temp.length() > 8)
    {
      encoded_data += (char)bin_to_dec(temp.substr(0, 8));
      temp = temp.substr(8);
    }

    in_file.get(ch);
  }

  int count = 8 - temp.length();
  if (temp.length() < 8)
  {
    temp.append(count, '0');
  }
  encoded_data += (char)bin_to_dec(temp);
  encoded_data += (char)count;

  in_file.close();
}

// Creates new file with .huf extension and saves the encoded data
void Huffman::save_encoded_data()
{
  out_file.open(output_file, ios::out | ios::binary);
  out_file.write(encoded_data.c_str(), encoded_data.size());
  out_file.close();
}

string Huffman::dec_to_bin(int num)
{
  string temp = "", res = "";
  while (num > 0)
  {
    temp += (num % 2 + '0');
    num /= 2;
  }
  res.append(8 - temp.length(), '0');
  for (int i = temp.length() - 1; i >= 0; i--)
  {
    res += temp[i];
  }
  return res;
}

void Huffman::build_tree(char data_value, string &hcode)
{
  Node *cur = root;
  for (int i = 0; i < hcode.size(); i++)
  {
    if (hcode[i] == '0')
    {
      if (cur->left == NULL)
      {
        cur->left = new Node();
      }
      cur = cur->left;
    }
    else if (hcode[i] == '1')
    {
      if (cur->right == NULL)
      {
        cur->right = new Node();
      }
      cur = cur->right;
    }
  }
  cur->data = data_value;
}

// Build the tree according to encoded values from encoded data file
void Huffman::get_huffmanTree()
{
  in_file.open(input_file, ios::in | ios::binary);

  unsigned char size;
  in_file.read(reinterpret_cast<char *>(&size), 1);
  root = new Node();

  for (int i = 0; i < size; i++)
  {
    unsigned char huffman_code[16];
    char data_value;
    in_file.read(&data_value, 1);
    in_file.read(reinterpret_cast<char *>(huffman_code), 16);

    string hcode = "";
    for (int i = 0; i < 16; i++)
    {
      hcode += dec_to_bin(huffman_code[i]);
    }
    int j = 0;
    while (hcode[j] == '0')
    {
      j++;
    }
    hcode = hcode.substr(j + 1);
    build_tree(data_value, hcode);
  }
  in_file.close();
}

// Get the decoded data of the original data by removing meta data from encoded data
void Huffman::get_decoded_data()
{
  in_file.open(input_file, ios::in | ios::binary);

  unsigned char size;
  in_file.read(reinterpret_cast<char *>(&size), 1);

  in_file.seekg(1 + 17 * size, ios::beg);
  unsigned char chunk;
  in_file.read(reinterpret_cast<char *>(&chunk), 1);
  while (!in_file.eof())
  {
    decoded_data.push_back(chunk);
    in_file.read(reinterpret_cast<char *>(&chunk), 1);
  }

  in_file.close();
}

// Save the decompressed file
void Huffman::save_decoded_file()
{
  out_file.open(output_file, ios::out);

  in_file.open(input_file, ios::in | ios::binary);
  char count;
  in_file.seekg(-1, ios::end);
  in_file.read(&count, 1);
  in_file.close();

  Node *cur = root;
  string path;
  for (int i = 0; i < decoded_data.size() - 1; i++)
  {
    path = dec_to_bin(decoded_data[i]);
    if (i == decoded_data.size() - 2)
    {
      path = path.substr(0, 8 - count);
    }

    for (int j = 0; j < path.size(); j++)
    {
      if (path[j] == '0')
      {
        cur = cur->left;
      }
      else
      {
        cur = cur->right;
      }

      if (cur->left == NULL && cur->right == NULL)
      {
        out_file.put(cur->data);
        cur = root;
      }
    }
  }
  out_file.close();
}

// Compresses input file and saves the encoded file
void Huffman::compress()
{
  create_minHeap();
  create_huffmanTree();
  assign_codes();
  add_meta_data();
  add_encoded_input();
  save_encoded_data();
}

// Decompresses the encoded file and saves it
void Huffman::decompress()
{
  get_huffmanTree();
  get_decoded_data();
  save_decoded_file();
}