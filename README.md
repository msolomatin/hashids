[![Build Status](https://travis-ci.org/msolomatin/hashids.cpp.svg?branch=master)](https://travis-ci.org/msolomatin/hashids.cpp)

# Hashids

## Description
C++11 port of a [JavaScript library](http://hashids.org/javascript/) to generate YouTube-like hashids from one or many numbers.

## Alpha
This library is at the earliest stage of development and is being developed and tested on Mac OS X 10.10.5.

## Requirements
C++11 compatible compiler.

## Basic Example

```C++
#include "Hashids.h"
auto hashids = Hashids("this is my salt", 8);
auto id = hashids.encode(12345);
```
```id``` is going to be
```
B0NkK9A5
```

You can also pass several numbers
```C++
#include "Hashids.h"
auto hashids = Hashids("this is my salt", 8);
auto id = hashids.encode({1, 2, 3, 4, 5});
```
In this case ```id``` is going to be
```
zoHWuNhktp
```
