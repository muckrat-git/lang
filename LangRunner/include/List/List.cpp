#pragma once

#include <iostream>
#include <cstring>

namespace Muck {
    template <typename type> 
    class List {
        private:
        type * content;
    
        public:
        size_t length = 0;     // Number of elements contained in list
    
        // - Constructors -
    
        // Create a new list from a pointer array
        List(type * content, size_t length) {
            // Allocate content
            this->content = new type[length];
            
            // Copy memory from content to this->content
            memcpy(this->content, content, sizeof(type) * length);
            this->length = length;
        }
        // Create an empty list of size 'length'
        List(size_t length) {
            // Allocate content
            this->content = new type[length];
            this->length = length;
        }
        // Null constructor
        List() = default;
    
        // - Destructor -
        ~List() {
            if(this->length > 0) delete[] this->content;
        }
    
        // - Functions -
    
        // Check if 'index' is valid within the list
        bool HasIndex(size_t index) {
            return index >= 0 && index < this->length;
        }
    
        // Returns index of first occurrence of 'value' in the list
        size_t IndexOf(type value) {
            for(size_t i = 0; i < this->length; ++i) {
                if(content[i] == value) return i;
            }
            return this->length;
        }
    
        // Add item 'value' to the end of the list
        type & Append(type value) {
            // Check if empty
            if(this->length == 0) {
                this->content = new type[1];
                this->content[0] = value;
                this->length = 1;
                return this->content[0];
            }

            // Calculate size
            size_t size = sizeof(type) * this->length;
            
            // Copy old content to buffer
            type * buffer = new type[this->length];
            memcpy(buffer, this->content, size);
    
            // Reallocate new content
            delete[] this->content;
            this->content = new type[this->length + 1];
            
            // Copy old content to new content
            memcpy(this->content, buffer, size);
            
            // Deallocate buffer
            delete[] buffer;
            
            // Add new value to end of list
            return content[this->length++] = value;
        }
    
        // Remove item at 'index' from list
        bool Remove(size_t index) {
            // Ensure index is valid
            if(!HasIndex(index)) return false;
            
            // Copy old content to buffer
            type * buffer = new type[this->length];
            memcpy(buffer, this->content, sizeof(type) * this->length);
    
            // Reallocate new content
            delete[] this->content;
            this->content = new type[this->length - 1];
    
            // Copy over data
            for(size_t i = 0; i < this->length; ++i)
                this->content[i] = buffer[i - (i>index ? 1 : 0)];
    
            // Deallocate buffer
            delete[] buffer;
            return true;
        }
    
        // Remove first occurance of 'value' from the list
        bool RemoveItem(type value) {
            // Locate item
            size_t index = IndexOf(value);
            if(index == this->length) return false;
    
            // Remove item
            return Remove(index);
        }
    
        // Replace item at index
        bool Replace(size_t index, type value) {
            // Ensure index is valid
            if(!HasIndex(index)) return false;
    
            // Replace item
            content[index] = value;
            return true;
        }
    
        // Convert list to string representation
        std::string ToString() {
            std::string result = "[";
            for(int i = 0; i < this->length; ++i) {
                result += std::to_string(content[i]);
                if(i != this->length - 1) result += ", ";
            }
            return result + "]";
        }
    
        // - Operator Overloads -
    
        // Get item at index    
        type & operator[](size_t index) {    
            return this->content[index];    
        }    
        // Append item to list    
        type & operator+=(type value) {    
            return Append(value);    
        } 
        // Remove first occurance of item    
        type & operator-=(type value) {    
            return RemoveItem(value);    
        }    
        // Remove last element    
        void operator--() const {    
            Remove(this->length - 1);    
        }    
        // Shift elements by 'places' up    
        void operator>>(size_t places) {    
            // Append last element    
            Append(content[this->length - 1]);    
    
            // Shift elements    
            for(size_t i = this->length - 1; i > 0; --i    )    
                content[i] = content[i - 1];    
        }    
        // Shift elements by 'places' down    
        void operator<<(size_t places) {    
            // Remove first element    
            Remove(0);    
        }    

        // - Iterator functions -
        type * begin() {
            return content;
        }
        type * end() {
            return content + this->length;
        }
    };
};