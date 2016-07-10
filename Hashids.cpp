#include <algorithm>
#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <initializer_list>
using std::cout;
using std::endl;

#include "Hashids.h"

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
            sepsIndex = number % _seps.length();
            ret += _seps[sepsIndex];
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

std::string Hashids::_hash(int input, std::string alphabet) {
    std::string hash = "";
    int alphabetLength = alphabet.length();

    do {
        hash = alphabet[input % alphabetLength] + hash;
        input = input / alphabetLength;
    } while (input);

    return hash;
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

    for (size_t i = 0; i < _seps.length(); i++) {
        int j = _alphabet.find(_seps[i]);
        if (j == std::string::npos) {
            _seps = _seps.substr(0, i) + " " + _seps.substr(i + 1);
        } else {
            _alphabet = _alphabet.substr(0, j) + " " + _alphabet.substr(j + 1);
        }
    }

    _alphabet.erase(remove_if(_alphabet.begin(), _alphabet.end(), isspace), _alphabet.end());
    _seps.erase(remove_if(_seps.begin(), _seps.end(), isspace), _seps.end());

    _seps = _consistentShuffle(_seps, _salt);

    if (!_seps.length() || (static_cast<double>(_alphabet.length()) / _seps.length()) > _sepDiv) {
        int sepsLength = ceil(static_cast<double>(_alphabet.length()) / _sepDiv);

        if (sepsLength == 1) {
            sepsLength++;
        }

        if (sepsLength > _seps.length()) {
            int diff = sepsLength - _seps.length();
            _seps += _alphabet.substr(0, diff);
            _alphabet = _alphabet.substr(diff);

        } else {
            _seps = _seps.substr(0, sepsLength);
        }
    }

    _alphabet = _consistentShuffle(_alphabet, _salt);

    int guardCount = ceil(static_cast<double>(_alphabet.length()) / _guardDiv);

    if (_alphabet.length() < 3) {
        _guards = _seps.substr(0, guardCount);
        _seps = _seps.substr(guardCount);
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

int main() {
    auto hashids = Hashids("this is my salt");

    cout << hashids.encode(12345) << endl;
    cout << hashids.encode({1, 2, 3, 4, 5}) << endl;

    return 0;
}
