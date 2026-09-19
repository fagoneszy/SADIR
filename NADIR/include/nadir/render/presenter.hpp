#pragma once

#include <string>
#include <vector>

#include <nadir/render/phosphor_buffer.hpp>
#include <nadir/core/terminal.hpp>

namespace nadir::render {

struct HUDState {
    double fps{};
    int frame{};
    std::string utc{};
    std::string frame_mode{};
    std::string camera_mode{};
    bool trails{false};
    bool grid{false};
    bool stars{false};
    bool hud{false};
    int entities{0};
};

class Presenter {
public:
    Presenter(int width, int height);
    void resize(int width, int height);
    void render(const PhosphorBuffer& phosphor, const HUDState& hud);
    void present(::nadir::TerminalSession& terminal);

private:
    void render_phosphor_to_cells(const PhosphorBuffer& phosphor);
    void render_hud(const HUDState& hud);
    void compute_diff();
    void write_diff(::nadir::TerminalSession& terminal);

    int w_{};
    int h_{};
    int virtual_w_{};
    int virtual_h_{};
    std::vector<TerminalCell> current_;
    std::vector<TerminalCell> previous_;
    std::vector<TerminalCell> hud_current_;
    std::vector<TerminalCell> hud_previous_;
    std::string hud_text_;
};

}
