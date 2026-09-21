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

// Returns UTC Julian Date from CCSDS OMM EPOCH, or zero for an invalid epoch.
double omm_epoch_jd_utc(const astro::OmmRecord& elements) noexcept;

std::string to_string(Sgp4Error error);

}
