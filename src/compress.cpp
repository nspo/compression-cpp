#include <iostream>
#include <fstream>
#include "Huffman.h"
#include "LZW.h"
#include "external/argagg.h"

int main(int argc, char** argv) {
    // Parse arguments
    argagg::parser argparser {{
                              {"help", {"-h", "--help"}, "Show this help message", 0},
                              {"lzw", {"-l", "--lzw"}, "Use LZW compression instead of Huffman", 0},
                              {"extract", {"-x", "--extract"}, "Extract input file instead of compressing it", 0},
                              }};
    argagg::parser_results args;
    try {
        args = argparser.parse(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    if (args["help"] || args.pos.size() != 1) {
        argagg::fmt_ostream fmt(std::cerr);
        const auto program = argv[0];
        fmt << "Usage: " << program << " [options] INPUT_FILENAME\n" << argparser;
        fmt << "\nExamples:\n";
        fmt << program << " input.txt\t\tCompress input.txt with Huffman\n";
        fmt << program << " -l input.txt\t\tCompress input.txt with LZW\n";
        fmt << program << " -xl input.txt.lzw\tExtract input.txt.lzw with LZW\n";
        return 1;
    }

    const std::string file_in = args.pos[0];


    if (args.options["lzw"]) {
        // lzw
        std::cout << "Using LZW compression...\n";

        if (args.options["extract"]) {
            const std::string file_out = file_in+".orig";
            std::cout<<"Extracting "<<file_in<<" to "<<file_out<<"\n";

            std::ifstream ifs(file_in);
            std::ofstream ofs(file_out);

            if (!ifs || !ofs) {
                std::cerr << "Error opening input or output file.\n";
                return 1;
            }

            lzw::expand(ifs, ofs);
        } else {
            // compress
            const std::string file_out = file_in+".lzw";

            std::cout<<"Compressing "<<file_in<<" to "<<file_out<<"\n";

            std::ifstream ifs(file_in);
            std::ofstream ofs(file_out);

            if (!ifs || !ofs) {
                std::cerr << "Error opening input or output file.\n";
                return 1;
            }

            lzw::compress(ifs, ofs);
        }
    }else {
        // huffman
        std::cout << "Using Huffman compression...\n";

        if (args.options["extract"]) {
            const std::string file_out = file_in+".orig";
            std::cout<<"Extracting "<<file_in<<" to "<<file_out<<"\n";

            std::ifstream ifs(file_in);
            std::ofstream ofs(file_out);

            if (!ifs || !ofs) {
                std::cerr << "Error opening input or output file.\n";
                return 1;
            }

            huffman::expand(ifs, ofs);
        } else {
            // compress
            const std::string file_out = file_in+".huffman";
            std::cout<<"Compressing "<<file_in<<" to "<<file_out<<"\n";

            std::ifstream ifs1(file_in);
            std::ifstream ifs2(file_in);
            std::ofstream ofs(file_out);

            if (!ifs1 || !ifs2 || !ofs) {
                std::cerr << "Error opening input or output file.\n";
                return 1;
            }

            huffman::compress(ifs1, ifs2, ofs);
        }
    }

    return 0;
}