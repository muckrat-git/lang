#pragma once

#include <iostream>
#include <cstring>
#include <unordered_map>
#include <initializer_list>

#include "List.cpp"

namespace Muck {
    template <typename key_t, typename value_t, typename hasher_t = std::hash<key_t>>
    class Dict {
        private:
        // Null reference
        value_t NULLREF;
    
        struct Elem {
            size_t hash;
            key_t key;
            value_t value;
        };

        struct Pair {
            key_t key;
            value_t value;
        };
        
        long _binarySearch(size_t hash, long min, long max) {
            // Avoid unfound
            if(min == max) {
                if(content[min].hash == hash) return min;
                return -1;
            }

            // Get next pos
            long pos = (min + max) / 2;

            // Check pos
            if(content[pos].hash == hash) return pos;

            // Update min and max
            if(content[pos].hash > hash) max = pos;
            else min = pos+1;
 
            return _binarySearch(hash, min, max);
        }

        public:
        List<Elem> content;
        // - Functions -

        // Returns the value of at 'key'
        value_t * Get(key_t key) {
            // Return null if length is 0
            if(content.length == 0) return nullptr;
            
            // Get hash of key
            size_t hash = hasher_t{}(key);

            // Binary search the content
            long pos = _binarySearch(hash, 0, content.length - 1);
            if(pos == -1) return nullptr;
            
            // Return value
            return &content[pos].value;
        }

        // Add item to dict (returns index)
        long Add(key_t key, value_t value) {
            // Get hash of key
            size_t hash = hasher_t{}(key);

            // Check if key should be appended
            if(content.length == 0 || content[content.length - 1].hash < hash) {
                content.Append((Elem){hash, key, value});
                return content.length - 1;
            }
            else {
                // Add to start
                content >> 1;
                content[0] = (Elem){hash, key, value};
                return 0;
            }
        }

        // Set item at 'key' to 'value' without creation
        bool Set(key_t key, value_t value) {
            // Get hash of key
            size_t hash = hasher_t{}(key);

            // Binary search the content
            long pos = _binarySearch(hash, 0, content.length - 1);
            if(pos == -1) return false;

            // Set value
            content[pos].value = value;

            return true;
        }

        // Remove item at 'key'
        void Remove(key_t key) {
            // Get hash of key
            size_t hash = hasher_t{}(key);

            // Binary search the content
            long pos = _binarySearch(hash, 0, content.length - 1);
            if(pos == -1) return;

            // Remove item
            content.Remove(pos);
        }

        // Covert to printable string
        std::string ToString(std::string (*key2str)(key_t), std::string (*val2str)(value_t)) {
            std::string str = "{\n";
            for(long i = 0; i < content.length; i++) {
                str += "\t" + key2str(content[i].key) + ": " + val2str(content[i].value) + "\n";
            }
            str += "}";
            return str;
        }

        // - Operator Overloads -

        // Get/Set operator
        value_t & operator[](key_t key) {
            // Get hash of key
            size_t hash = hasher_t{}(key);

            // Binary search the content
            long pos = _binarySearch(hash, 0, content.length - 1);
            
            // Add elem if not present
            if(pos == -1) {
                pos = Add(key, NULLREF);
            }

            // Return value
            return content[pos].value;
        }

        // - Constructors -

        Dict() = default;

        // Array initilizer constructor
        Dict(std::initializer_list<Pair> pairs) {
            for(Pair pair : pairs) Add(pair.key, pair.value);
        }
    };
}