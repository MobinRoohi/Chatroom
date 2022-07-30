#include "helper.h"

vector<string>* split_message(string message, string wrapper, int pos_start) {
    vector<string>* result = new vector<string>();
    int pos_end, wrapper_len = wrapper.length();
    while ((pos_end = message.find (wrapper, pos_start)) != string::npos) {
        result->push_back(message.substr(pos_start, pos_end - pos_start));
        pos_start = pos_end + wrapper_len;
    }
    result->push_back (message.substr (pos_start));
    return result;
}

void check_message_size(vector<string>* message_splitted, int i) {
    if (message_splitted->size() == 0 || (i != -1 && message_splitted->size() != i))
        throw "The message format is incorrect";
}