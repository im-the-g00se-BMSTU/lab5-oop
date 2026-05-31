#ifndef STATENAMETABLE_H
#define STATENAMETABLE_H

#include <map>
#include <string>
#include <utility>

template <typename State>
class StateNameTable {
private:
    std::map<State, std::string> names;
public:
    void add(State state, std::string name) {
        names[state] = std::move(name);
    }
    std::string getName(State state) const {
        std::string result;
        auto name = names.find(state);
        if (name != names.end())
            result = name->second;
        return result;
    }
};

#endif // STATENAMETABLE_H
