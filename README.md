# Design-And-Implementation-Of-Huffman-Compiler-Decoder
Huffman编/译码器的设计与实现，数据结构课程设计

Design a Huffman coding and decoding system.The characters in a text file are coded by Huffman, and the coding file is generated.In turn, the encoding file decoder can be restored to a text file.

(1) read an English essay (file extension TXT);

(2) count and output the frequency of different characters appearing in the article (Spaces, line breaks, punctuation, etc., are also processed according to characters);

(3) construct Huffman tree according to character frequency, and give Huffman code of each character;

(4) output Huffman tree and Huffman coding;

(5) use the established hufman tree to encode text files and generate compressed files (the suffix of encoding files is.huf);

(6) compare the file size stored with the input text file size with Huffman coding, and calculate the file compression rate;

(7) decoding, decoding huf file to TXT file, and compared with the original TXT file.
Test data: text file of your choice, containing at least 3000 characters.