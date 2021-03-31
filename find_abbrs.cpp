#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>
#include <string>

#include "rapidcsv.h"

std::map<std::string, int> abbr;


void initAbbr()
{
    rapidcsv::Document doc("term_abbrs.csv");

    std::vector<std::string> col = doc.GetColumn<std::string>("term");
    
    for(int i = 0; i < col.size(); i++){
        abbr[col[i]] = i;
    }
}

std::vector<std::string> readParts(){
    rapidcsv::Document doc("../miac/data/subparts_ariadna_norm_head.csv", rapidcsv::LabelParams(0, 0),
                        rapidcsv::SeparatorParams('\t'));

    std::vector<std::string> col = doc.GetColumn<std::string>("norm_part");

    return col;
}

std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

int main(){
    initAbbr();

    std::vector<std::string> parts = readParts();
    std::vector<std::pair<int, std::string>> termIndex;
    
    for(int i = 0; i < parts.size(); i++){
        std::string text = parts[i];

        int j = 0;
        size_t pos = 0;
        std::string token;
        std::string prevWord, newWord;

        while ((pos = text.find(' ')) != std::string::npos) {
            token = text.substr(0, pos);
            text.erase(0, pos + 1);

            j++;

            if(j == 2){
                newWord = prevWord + " " + token;
                ReplaceAll(newWord, std::string("."), std::string(""));

                if(abbr.count(newWord) > 0){
                    termIndex.push_back({i, newWord});
                }

                j = 0;
            }

            prevWord = token;
        }
    }

    std::ofstream myfile;
    myfile.open ("term_indexes.csv");

    for(int i = 0; i < termIndex.size(); i++){
        myfile << termIndex[i].first << ";" << termIndex[i].second << "\n";
    }
    
    myfile.close();
}