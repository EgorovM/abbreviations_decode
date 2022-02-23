#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <set>
#include <list>


#define dataframe std::map<std::wstring, std::vector<std::wstring>>


void splitString(std::vector<std::wstring> &output, const std::wstring& str) {
    output.clear();

    size_t start = 0;
    size_t end = str.find_first_of(' ');

    while (end <= std::wstring::npos)
    {
        output.emplace_back(str.substr(start, end-start));

        if (end == std::wstring::npos)
            break;

        start = end + 1;
        end = str.find_first_of(' ', start);
    }
}

void string_lower(std::wstring &s)
{
    std::locale::global(std::locale(""));
    std::wcout.imbue(std::locale());
    auto& facet = std::use_facet<std::ctype<wchar_t>>(std::locale());
    facet.tolower(reinterpret_cast<wchar_t *>(&s[0]), reinterpret_cast<const wchar_t *>(&s[0] + s.size()));
}

void extractTermContexts(
    std::vector<std::vector<std::wstring>> &contextList,
    const std::vector<std::wstring>& termWords,
    const std::vector<std::wstring>& words,
    int window=5) {

    contextList.clear();

    int termLength = int(termWords.size());
    int context_size = window * 2 + termLength + termLength % 2;

    std::list<std::wstring> currentContext = std::list<std::wstring>(
            words.begin(),
            std::min(words.begin() + context_size, words.end()));

    int start_pos, end_pos;

    for(int i = context_size; i < words.size(); i++) {
        start_pos = window - termLength / 2;
        end_pos = window + termLength / 2 + int(termLength % 2 != 0);

        bool isCentered = true;

        for(int j = start_pos; j < end_pos; j++) {
            auto l_front = currentContext.begin();
            std::advance(l_front, j);

            if(*l_front != termWords[j - start_pos]) {
                isCentered = false;
                break;
            }
        }

        if(isCentered) {
            contextList.emplace_back(currentContext.begin(), currentContext.end());
        }

        currentContext.push_back(words[i]);
        currentContext.erase(currentContext.begin());
    }
}

void join_vector(std::vector<std::wstring> &row, std::wstring &result, wchar_t delimiter) {
    result.clear();

    for(auto & token : row) {
        result += token + delimiter;
    }
}

void extract(std::wstring& term, std::vector<std::wstring>& termWords, std::vector<std::vector<std::wstring>>& texts, dataframe& contexts_df) {
    std::vector<std::vector<std::wstring>> term_contexts;
    std::wstring context;

    int new_lines = 0;

    for(auto &text : texts) {
        extractTermContexts(term_contexts, termWords, text);

        for(auto & term_context : term_contexts) {
            contexts_df[L"term"].push_back(term);

            join_vector(term_context, context, ' ');
            contexts_df[L"context"].push_back(context);

            new_lines++;
        }
    }

    std::wcout << "new lines: " << new_lines << std::endl;
}

void read_line(const std::wstring& line, std::vector<std::wstring>& row, wchar_t delimiter) {
    std::wstring word;

    std::wstringstream str(line);

    while (getline(str, word, delimiter))
        row.push_back(word);
}

void read_csv(
        dataframe &content,
        const std::string fname,
        wchar_t delimiter)
{
    std::vector<std::wstring> headers;
	std::vector<std::wstring> row;
	std::wstring line;
 
	std::wfstream file (fname, std::ios::in);

	if(file.is_open())
	{
        getline(file, line);
        read_line(line, headers, delimiter);

		while(getline(file, line))
		{
			row.clear();

            read_line(line, row, delimiter);

            for(int i = 0; i < row.size(); i++) {
                content[headers[i]].push_back(row[i]);
            }
		}
	}
	else
		std::cout<<"Could not open the file\n";

    file.close();
}

void tests() {
    std::vector<std::vector<std::wstring>> term_contexts;
    std::wstring text = L"as adhklfj sdj dj djd ud dj dd ad is я не знаю is not a good reason sl lsj j sj s";
    std::vector<std::wstring> termWords;
    std::vector<std::wstring> textWords;

    splitString(textWords, text);

    std::wstring term = L"ad";
    splitString(termWords, term);
    extractTermContexts(term_contexts, termWords, textWords);
    assert(term_contexts.size() == 1);

    term = L"ad is";
    splitString(termWords, term);
    extractTermContexts(term_contexts, termWords, textWords);
    assert(term_contexts.size() == 1);

    term = L"asf ksgkn";
    splitString(termWords, term);
    extractTermContexts(term_contexts, termWords, textWords);
    assert(term_contexts.empty());
}

void to_csv(dataframe &df, const std::string filename, char delimiter) {
    std::wfstream file(filename, std::ios::out);

    std::vector<std::wstring> headers;
    headers.reserve(df.size());

    for (auto const& element : df) {
        headers.push_back(element.first);
        file << element.first << delimiter;
    }
    file << "\n";

    for (int i = 0; i < df[headers[0]].size(); i++) {
        for (auto const& header : headers) {
            file << df[header][i] << delimiter;
        }
        if(i != df[headers[0]].size() - 1) file << "\n";
    }

    file.close();
}

int main() {
    setlocale(LC_ALL, "Russian");

    tests();

    std::cout << "started reading files..." << std::endl;

    dataframe terms_df;
    dataframe records_df;

    read_csv(terms_df,
            "./data/abbr_term.csv",
            '\t');

    read_csv(records_df,
            "./data/dialogue_data.csv",
            ';');

    std::vector<std::wstring> terms;
    terms.assign(terms_df[L"term"].begin(), terms_df[L"term"].end());
    std::vector<std::wstring> texts = records_df[L"text"];

    std::cout << "finished." << std::endl;

    std::vector<std::vector<std::wstring>> termsWords(terms.size(), std::vector<std::wstring>());
    std::vector<std::vector<std::wstring>> textsWords(texts.size(), std::vector<std::wstring>());

    for(int i = 0; i < terms.size(); i++) {
        string_lower(terms_df[L"term"][i]);
        splitString(termsWords[i], terms_df[L"term"][i]);
    }
//
//    for(int i = 0; i < texts.size(); i++) {
//        string_lower(texts[i]);
//        splitString(textsWords[i], texts[i]);
//    }
//
//    std::set<std::wstring> allWords;
//    for(const auto& words : textsWords) {
//        for(const std::wstring & word : words) {
//            allWords.insert(word);
//        }
//    }
//
//    std::cout << "started finding context..." << std::endl;
//
//    dataframe contexts_df;
//
//    for(int i = 0; i < terms.size(); i++) {
//        std::wcout << terms_df[L"term"][i] << " " << i + 1 << "'st term of " << terms.size() << " " << std::endl;
//
//        bool containTerm = true;
//
//        for(const std::wstring& word : termsWords[i]) {
//            if(allWords.count(word) == 0) {
//                containTerm = false;
//                break;
//            }
//        }
//
//        if(!containTerm) {
//            std::wcout << "there is no term" << std::endl;
//            continue;
//        }
//
//        try {
//            extract(terms[i], termsWords[i], textsWords, contexts_df);
//        }catch (std::exception& e) {
//            std::wcout << "caused problem" << std::endl;
//        }
//    }
//
//    std::cout << "finished." << std::endl;
//
//    to_csv(contexts_df, "contexts.csv", '\t');
}