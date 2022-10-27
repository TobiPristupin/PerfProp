#include "include/Event.h"

Event::Event(int id, std::string name) : id(id), name(std::move(name)) {}

bool Event::operator==(const Event &e) const {
    return this->id == e.id;
}


