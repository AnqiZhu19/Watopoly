#ifndef OBSERVER_H
#define OBSERVER_H

#include <vector>
#include <algorithm>

class Observer {
public:
    virtual void notify() = 0;
    virtual ~Observer() = default;
};

class Subject {
    std::vector<Observer*> observers;
public:
    void attach(Observer* o) { observers.push_back(o); }
    void detach(Observer* o) {
        observers.erase(std::remove(observers.begin(), observers.end(), o), observers.end());
    }
    void notifyObservers() {
        for (auto o : observers) o->notify();
    }
    virtual ~Subject() = default;
};

#endif
