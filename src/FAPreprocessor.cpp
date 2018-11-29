#include "stdafx.h"
#include <fstream>
#include <streambuf>
#include <memory>
#include <iostream>

#include "Utilities.h"

#include "FATokenizer.h"
#include "FAPreprocessor.h"


FlexASM::Preprocessor::Preprocessor()
{
    CommentChar = ';';
    StringChar = 0x27;
}


FlexASM::Preprocessor::~Preprocessor()
{
}

std::string FlexASM::Preprocessor::RemoveComments(std::string input)
{
    std::string result = "";

    bool inComment = false;
    bool inString = false;
    for (char& currentChar : input)
    {
        if (!inString && currentChar == StringChar)
        {
            inString = true;
        }
        else
        {
            if (inString && currentChar == StringChar)
            {
                inString = false;
            }
        }

        if (!inComment)
        {
            if (currentChar == CommentChar && !inString)
            {
                inComment = true;
                continue;
            }
            else
            {
                result += currentChar;
                continue;
            }
        }

        if (currentChar == 10)
            inComment = false;
    }

    return result;
}

std::string FlexASM::Preprocessor::IncludeFiles(std::string input, std::string filePath)
{
    return std::string();
}


///

std::string FlexASM::Preprocessor::Preprocess(std::string fileName)
{
    // prepare result
    std::string result = "";

    // load file into temp string
    std::string temp = read_file(fileName);

    // preprocess (remove) comments
    temp = RemoveComments(temp);

    // tokenize
    std::unique_ptr<Tokenizer> tokenizer = std::make_unique<Tokenizer>();
    tokenizer->Tokenize(temp);

    std::string includeStrigs = "";
    // parse through tokens and look for things to preprocess
    while (tokenizer->IsInRange())
    {
        Token token = tokenizer->GetCurrentToken();
        if (token.Type == ttCompilerCommand)
        {
            if (token.Value == "%include")
            {
                Token nextToken = tokenizer->GetNextToken();

                if (nextToken.Type == ttConstString)
                {
                    std::string filePath = nextToken.Value.substr(1, nextToken.Value.length() - 2);
                    std::string fileSource = Preprocess(filePath);

                    includeStrigs += fileSource;
                }
                tokenizer->NextToken();
            }
        }
        else
        {
            result += tokenizer->GetCurrentToken().Value + ' ';
        }
        tokenizer->NextToken();
    }
    result += includeStrigs;
    return result;
}
