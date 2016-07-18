#include <algorithm>
#include <string>
#include <cmath>
#include <vector>
#include <initializer_list>
#include <sstream>

#include "../include/Hashids.h"

std::string Hashids::_consistentShuffle(std::string alphabet, std::string salt) {
    int integer, j, i, v, p;
    char temp;

    if (salt.length() == 0) {
        return alphabet;
    }

    for (i = alphabet.length() - 1, v = 0, p = 0; i > 0; i--, v++) {
        v %= salt.length();
        p += integer = static_cast<int>(salt[v]);
        j = (integer + v + p) % i;

        temp = alphabet[j];
        alphabet = alphabet.substr(0, j) + alphabet[i] + alphabet.substr(j + 1);
        alphabet = alphabet.substr(0, i) + temp + alphabet.substr(i + 1);
    }

    return alphabet;
}

std::string Hashids::_encode(std::vector<int> numbers) {
    int numbersHashInt = 0;

    for (int i = 0; i < numbers.size(); i++) {
        numbersHashInt += (numbers[i] % (i + 100));
    }

    std::string lottery, ret, alphabet = _alphabet, buffer, last;
    lottery = ret = alphabet[numbersHashInt % alphabet.length()];

    int number, numbersSize = numbers.size(), sepsIndex;
    for (int i = 0; i < numbers.size(); i++) {
        number = numbers[i];
        buffer = lottery + _salt + alphabet;

        alphabet = _consistentShuffle(alphabet, buffer.substr(0, alphabet.length()));
        last = _hash(number, alphabet);

        ret += last;

        if (i + 1 < numbersSize) {
            number %= (static_cast<int>(last[0]) + i);
            sepsIndex = number % _separators.length();
            ret += _separators[sepsIndex];
        }
    }

    int guardIndex;
    std::string guard;
    if (ret.length() < _minHashLength) {
        guardIndex = (numbersHashInt + static_cast<int>(ret[0])) % _guards.length();
        guard = _guards[guardIndex];

        ret = guard + ret;

        if (ret.length() < _minHashLength) {
            guardIndex = (numbersHashInt + static_cast<int>(ret[2])) % _guards.length();
            guard = _guards[guardIndex];

            ret += guard;
        }
    }

    int halfLength = alphabet.length() / 2, excess;
    while (ret.length() < _minHashLength) {
        alphabet = _consistentShuffle(alphabet, alphabet);
        ret = alphabet.substr(halfLength) + ret + alphabet.substr(0, halfLength);

        excess = ret.length() - _minHashLength;
        if (excess > 0) {
            ret = ret.substr(excess / 2, _minHashLength);
        }
    }

    return ret;
}

std::vector<int> Hashids::_decode(std::string hash, std::string alphabet) {
    std::vector<int> ret;
    int i = 0;
    std::string hashBreakDown = hash;

    for (int j = 0; j < _guards.length(); j++) {
        int position = hashBreakDown.find(_guards[j]);
        if (position != std::string::npos) {
            std::replace(hashBreakDown.begin(), hashBreakDown.end(), _guards[j], ' ');
        }
    }

    // hashBreakDown = hash;

    std::stringstream ss(hashBreakDown);
    std::string s;
    std::vector<std::string> hashArray;
    while (ss >> s) {
        hashArray.push_back(s);
    }

    if (hashArray.size() == 3 || hashArray.size() == 2) {
        i = 1;
    }

    hashBreakDown = hashArray[i];
    std::string lottery, subHash, buffer;

    if (hashBreakDown[0] != NULL) {
        lottery = hashBreakDown[0];
        hashBreakDown = hashBreakDown.substr(1);

        for (int j = 0; j < _separators.length(); j++) {
            int position = hashBreakDown.find(_separators[j]);
            if (position != std::string::npos) {
                std::replace(hashBreakDown.begin(), hashBreakDown.end(), _separators[j], ' ');
            }
        }

        std::stringstream ss(hashBreakDown);
        std::string s;
        std::vector<std::string> hashArray;
        while (ss >> s) {
            hashArray.push_back(s);
        }

        for (int i = 0; i < hashArray.size(); i++) {
            subHash = hashArray[i];
            buffer = lottery + _salt + alphabet;

            alphabet = _consistentShuffle(alphabet, buffer.substr(0, alphabet.length()));
            ret.push_back(_unhash(subHash, alphabet));
        }

        if (_encode(ret) != hash) {
            ret = {};
        }
    }

    return ret;
}

std::string Hashids::_hash(int input, std::string alphabet) {
    std::string hash = "";
    int alphabetLength = alphabet.length();

    do {
        hash = alphabet[input % alphabetLength] + hash;
        input = input / alphabetLength;
    } while (input);

    return hash;
}

int Hashids::_unhash(std::string input, std::string alphabet) {
    int number = 0, pos;

    for (int i = 0; i < input.length(); i++) {
        pos = alphabet.find(input[i]);
        number += pos * std::pow(alphabet.length(), input.length() - i - 1);
    }

    return number;
}

Hashids::Hashids(std::string salt, int minHashLength, std::string alphabet) {
    _salt = salt;
    if (minHashLength > 0) {
        _minHashLength = minHashLength;
    }
    if (alphabet != "") {
        _alphabet = alphabet;
    }

    std::string uniqueAlphabet = "";
    for (size_t i = 0; i < _alphabet.length(); i++) {
        if (uniqueAlphabet.find(_alphabet[i]) == std::string::npos) {
            uniqueAlphabet += _alphabet[i];
        }
    }
    _alphabet = uniqueAlphabet;

    for (size_t i = 0; i < _separators.length(); i++) {
        int j = _alphabet.find(_separators[i]);
        if (j == std::string::npos) {
            _separators = _separators.substr(0, i) + " " + _separators.substr(i + 1);
        } else {
            _alphabet = _alphabet.substr(0, j) + " " + _alphabet.substr(j + 1);
        }
    }

    _alphabet.erase(std::remove_if(_alphabet.begin(), _alphabet.end(), isspace),
        _alphabet.end());
    _separators.erase(std::remove_if(_separators.begin(), _separators.end(),
        isspace), _separators.end());

    _separators = _consistentShuffle(_separators, _salt);

    if (!_separators.length() || (static_cast<double>(_alphabet.length()) /
        _separators.length()) > _sepDiv) {
        int sepsLength = std::ceil(static_cast<double>(_alphabet.length()) / _sepDiv);

        if (sepsLength == 1) {
            sepsLength++;
        }

        if (sepsLength > _separators.length()) {
            int diff = sepsLength - _separators.length();
            _separators += _alphabet.substr(0, diff);
            _alphabet = _alphabet.substr(diff);

        } else {
            _separators = _separators.substr(0, sepsLength);
        }
    }

    _alphabet = _consistentShuffle(_alphabet, _salt);

    int guardCount = std::ceil(static_cast<double>(_alphabet.length()) / _guardDiv);

    if (_alphabet.length() < 3) {
        _guards = _separators.substr(0, guardCount);
        _separators = _separators.substr(guardCount);
    } else {
        _guards = _alphabet.substr(0, guardCount);
        _alphabet = _alphabet.substr(guardCount);
    }
}

std::string Hashids::encode(int number) {
    return _encode(std::vector<int>{number});
}

std::string Hashids::encode(std::initializer_list<int> numbers) {
    return _encode(numbers);
}

std::vector<int> Hashids::decode(std::string hash) {
    return _decode(hash, _alphabet);
}
