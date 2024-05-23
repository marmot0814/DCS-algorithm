#pragma once

#include<bits/stdc++.h>
#include <fstream>

class RandomStringGenerator {
public:
    RandomStringGenerator() {
        std::srand(std::time(nullptr));
    }

    std::string gen_rand_string(int n) {
        const char nucleotides[] = {'A', 'T', 'C', 'G'};
        std::string result;
        result.reserve(n);
        for (int i = 0; i < n; ++i) {
            result += nucleotides[std::rand() % 4];
        }
        return result;
    }

    std::string gen_special_string(int n) {
        if (n <= 4) {
            return gen_rand_string(n);
        }

        const char nucleotides[] = {'A', 'T', 'C', 'G'};
        std::string result;
        result.reserve(n);
        
        int segment_length = std::rand() % (n / 2) + 1; 
        segment_length=3;
        std::string segment;
        segment.reserve(segment_length);
        
        for (int i = 0; i < segment_length; ++i) {
            segment += nucleotides[std::rand() % 4];
        }
        
        while (result.length() + segment_length <= n) {
            result += segment;
        }
        
        while (result.length() < n) {
            result += nucleotides[std::rand() % 4];
        }
        
        return result;
    }

    std::string read_file(int n) {
      const char nucleotides[] = {'A', 'T', 'C', 'G'};
      std::string file_path="ecoli.txt";
      std::ifstream file(file_path);
      std::string result;
      result.reserve(n);

      if (file.is_open()) {
          char ch;
          int count = 0;

          while (count < n) {
              if (!file.get(ch)) {
                  
                  file.clear();
                  file.seekg(0, std::ios::beg);
                  continue;
              }
              if (ch == '\n' || ch == '\r') {
                  continue; 
              }
              if (ch == 'A' || ch == 'T' || ch == 'C' || ch == 'G') {
                  result += ch;
              } else {
                  result += nucleotides[std::rand() % 4];
              }
              count++;
          }
          file.close();
      } else {
          std::cerr << "Unable to open file: " << file_path << std::endl;
      }

      
      while (result.length() < n) {
          result += nucleotides[std::rand() % 4];
      }

      return result;
    }

    std::string all_A(int n){
        std::string tmp="";
        for(int i=0;i<n;i++)
        {
            tmp+="A";
        }
        return tmp;
    }
};
