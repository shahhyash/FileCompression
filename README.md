# File Compression (CS214 // Assignment 2)
#### Contributors: Seth Karten & Yash Shah

## Overview
In this assignment, we were asked to design a program which uses Huffman Codes to compress and decompress files. The assignment's primary objective served to help us learn and use the file system interface to read files and directories. Using the `readdir()` and `opendir()` functions, we were able to recursively explore the entire directory tree and eventually use `open()` and `read()` functions to read and analyze the contents of each file. 

## Design
This assignment consists of three major parts: (1) building the codebook using Huffman codes on tokens read from files, (2) compressing the files, and (3) decompressing the compressed files.

### Building the Codebook

When building the codebook, we used three primary data structures in our implementation to optimize the process:

- AVL Tree
- MinHeap
- Huffman Tree

The `build_codebook` routine performs the following operations:

<img src="./diagrams/build-codebook.svg">

We start the build proces by using the AVL tree to store tokens. We use the AVL tree for the `O(log(n))` time complexity it provides for insertion and lookup operations. This reduces our overhead as we can quickly append items from a file buffer without having repeated elements, while being able to count the frequency of each token occuring.

Next, we build a MinHeap by traversing through this AVL tree of elements with the priority factor as the frequency of occurence of each token. The MinHeap's `O(log(n))` time complexity allows us to quickly sort the tokens to prepare them for encoding using Huffman Codes as it serves as an ordered list of tokens.

FInally, we build the Huffman Tree as it's structure allows us to quickly determine each token's encoding string. As we traverse through the entire tree, we keep track of left/right moves and build up the encoding string until we reach a leaf node. This operation gives us an `O(n)` time complexity for encoding keys as we only ever visit each token once. 

### Compressing Files



### Decompressing Files

## Implementation
