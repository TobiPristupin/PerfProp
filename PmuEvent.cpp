#include "include/PmuEvent.h"

PmuEvent::PmuEvent(std::string name, PmuEvent::Type type, std::string modifiers) : name(std::move(name)), type(type), modifiers(std::move(modifiers)) {}

bool PmuEvent::operator==(const PmuEvent &e) const {
    return (this->name == e.name) && (this->modifiers == e.modifiers) && (this->type == e.type);
}


