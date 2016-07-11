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
    const int _minAlphabetLength = 16;
    const double _sepDiv = 3.5;
    const int _guardDiv = 12;
    int _minHashLength = 0;
    std::string _guards;

    std::string _consistentShuffle(std::string alphabet, std::string salt);
    std::string _encode(std::vector<int> numbers);
    std::vector<int> _decode(std::string hash, std::string alphabet);
    std::string _hash(int input, std::string alphabet);
    int _unhash(std::string input, std::string alphabet);

public:
    explicit Hashids(std::string salt, int minHashLength = 0, std::string alphabet = "");
    std::string encode(int number);
    std::string encode(std::initializer_list<int> numbers);
    std::vector<int> decode(std::string hash);
};

#endif  // HASHIDS_H_
