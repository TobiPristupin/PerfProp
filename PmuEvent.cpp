#include "include/PmuEvent.h"

PmuEvent::PmuEvent(std::string name, PmuEvent::Type type, std::string modifiers)
: name(std::move(name)), type(type), modifiers(std::move(modifiers)) {}

bool PmuEvent::operator==(const PmuEvent &e) const {
    return (this->name == e.name) && (this->modifiers == e.modifiers) && (this->type == e.type);
}

const std::string &PmuEvent::getName() const {
    return name;
}

const std::string &PmuEvent::getModifiers() const {
    return modifiers;
}

PmuEvent::Type PmuEvent::getType() const {
    return type;
}

std::optional<double> PmuEvent::getMean() const {
    return mean;
}

std::optional<double> PmuEvent::getVariance() const {
    return variance;
}

void PmuEvent::setMean(double m) {
    mean = m;
}

void PmuEvent::setVariance(double var) {
    variance = var;
}


