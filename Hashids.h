#include <string>
#include <vector>
#include <initializer_list>

#ifndef HASHIDS_H_
#define HASHIDS_H_

class Hashids {
private:
    std::string _alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
    std::string _salt;
    std::string _separators = "cfhistuCFHISTU";
    int _minAlphabetLength = 16;
    double _sepDiv = 3.5;
    int _guardDiv = 12;
    int _minHashLength = 0;
    std::string _guards;

    std::string _consistentShuffle(std::string alphabet, std::string salt);
    std::string _encode(std::vector<int> numbers);
    std::string _hash(int input, std::string alphabet);

public:
    Hashids(std::string salt, int minHashLength = 0, std::string alphabet = "");
    std::string encode(int number);
    std::string encode(std::initializer_list<int> numbers);
};

#endif  // HASHIDS_H_
