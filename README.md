# TextRank-Keywords

This is a C++ implementation of the Text-Rank algorithm to extract keywords from an input text.

## Features
- Reads UTF8-encoded text files
- Optimized for quick execution. For a 10-pages text, it takes about 0.2 seconds to launch the program, read the 700.000 words dictionary and extract keywords from it.
- Based on POS-tagging: considers only nouns and adjectives as potential keywords,
- Lemmatization of words

## Usage

```bash
./keywords path_to_dictionary [path_to_input_file]
```

- `path_do_dictionary` is the name of a binary dictionary with part-of-speech tagging and lemmatization of the words
- `path_to_input_file` is the name of the input file whose keywords shall be extracted. It may be omitted, and standard input will be used.

## Installation
For better performance, a custom binary dictionary format is used. To ensure the binary format is compatible with the computer on which the program is run, it is advised to generate the binary dictionary on the same computer.

Program `prepare.cpp` is designed to generate a binary dictionary from a text dictionary. The text dictionary should have the same format as the one [here](https://github.com/frodonh/french-words).

```bash
./prepare path_to_text_dictionary path_to_binary_dictionary
```

The format of the binary format is the following. For each dictionary entry, the binary file has the following bytes:

- one byte for the POS tag, either 'N' for nouns or 'A' for adjectives, 'S' for non-significant words (verb, adverb, preposition...) and ' ' if the POS tag is unknown ;
- four bytes which make an int32_t value corresponding to the number of the associated lemma entry in the file (starting with 0) ;
- one byte which stands for the length (read as unsigned char) of the word in UTF-8 encoding ;
- n bytes for the word, n being the number read in the previous step ;

The entries must be sorted in alphabetical order of the words (using the current locale).
