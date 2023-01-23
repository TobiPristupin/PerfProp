#include "include/PmuEvent.h"

PmuEvent::PmuEvent(std::string name, PmuEvent::Type type)
: name(std::move(name)), type(type) {
    std::string::size_type colonPos = this->name.find(':');
    if (colonPos != std::string::npos){
        this->modifiers = this->name.substr(colonPos + 1);
    }
}

bool PmuEvent::operator==(const PmuEvent &e) const {
    return (this->name == e.name) && (this->type == e.type);
}

std::string PmuEvent::getName() const {
    return name;
}

std::string PmuEvent::getModifiers() const {
    return modifiers;
}

PmuEvent::Type PmuEvent::getType() const {
    return type;
}

bool PmuEvent::operator<(const PmuEvent& e) const {
    if (this->name == e.name){
        if (this->modifiers == e.modifiers){
            return this->type < e.type;
        }
        return this->modifiers < e.modifiers;
    }
    return this->name < e.name;
}




