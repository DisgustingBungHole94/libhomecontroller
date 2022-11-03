#include "homecontroller/util/id.h"

#include <random>

namespace hc {
namespace util {
    
    void id::generate() {
        static const char CHAR_SET[] = 
        {   
            '0','1','2','3','4',
            '5','6','7','8','9',
            'A','B','C','D','E','F',
            'G','H','I','J','K',
            'L','M','N','O','P',
            'Q','R','S','T','U',
            'V','W','X','Y','Z',
            'a','b','c','d','e','f',
            'g','h','i','j','k',
            'l','m','n','o','p',
            'q','r','s','t','u',
            'v','w','x','y','z'
        };

        static const int ID_LENGTH = 32;

        std::default_random_engine re(std::random_device{}());
        std::uniform_int_distribution<int> dist(0, sizeof(CHAR_SET) - 1);

        std::string id;
        id.reserve(ID_LENGTH);

        for (int i = 0; i < ID_LENGTH; i++) {
            id += CHAR_SET[dist(re)];
        }

        m_id = id;
    }

}
}