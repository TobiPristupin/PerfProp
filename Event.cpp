#include "include/Event.h"

Event::Event(std::string name, std::string modifiers) : name(std::move(name)), modifiers(std::move(modifiers)) {}

bool Event::operator==(const Event &e) const {
    return (this->name == e.name) && (this->modifiers == e.modifiers);
}


