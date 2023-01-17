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



