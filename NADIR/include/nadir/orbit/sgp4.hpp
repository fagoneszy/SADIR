#pragma once

#include <string>

#include <nadir/astro/omm.hpp>
#include <nadir/orbit/state.hpp>

namespace nadir::orbit {

enum class Sgp4Error {
    None,
    MeanEccentricity,
    MeanMotion,
    PerturbedEccentricity,
    SemiLatusRectum,
    Decayed,
    InvalidElements,
    NumericalFailure
};

struct Sgp4Result {
    State state;
    Sgp4Error error{Sgp4Error::None};

    constexpr explicit operator bool() const noexcept {
        return error == Sgp4Error::None;
    }
};

Sgp4Result propagate_sgp4(
    const astro::OmmRecord& elements,
    double minutes_since_epoch
);

std::string to_string(Sgp4Error error);

}