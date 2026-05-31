#ifndef ACTIONTABLE_H
#define ACTIONTABLE_H

#include <map>
#include <utility>

template <typename Owner, typename Command, typename State>
class ActionTable {
private:
    using Action = void (Owner::*)();
    using ActionKey = std::pair<Command, State>;

    std::map<ActionKey, Action> actions;

public:
    void add(Command command, State state, Action action) { //
        actions[{command, state}] = action;
    }

    void execute(Owner* owner, Command command, State state) const { //
        auto action = actions.find({command, state});
        if (action != actions.end()) {
            (owner->*action->second)();
        }
    }
};

#endif // ACTIONTABLE_H
