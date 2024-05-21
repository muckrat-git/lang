#include <cstddef>

namespace Muck {
    template <typename x> 
    class LinkedList {
        public:
        class Node {
            public:
            x data;
            Node * next;

            Node(x data) {
                this->data = data;
                this->next = nullptr;
            }
            Node(void){}
        };
        Node * first = nullptr;

        // Append operator
        x operator+=(x rhs) {
            // Handle empty case
            if(first == nullptr) {
                first = new Node(rhs);
                return first->data;
            }

            // Set next available node
            Node * current = first;
            while(current->next != nullptr) current = current->next;
            current->next = new Node(rhs);
            return current->next->data;
        }

        // Get the size of the list
        size_t size() {
            Node * current = first;
            size_t count = 0;
            while(current != nullptr) {
                current = current->next;
                ++count;
            }
            return count;
        }
    };
}