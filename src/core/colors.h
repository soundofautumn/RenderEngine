//
// Created by Autumn Sound on 2024/9/6.
//

#ifndef RENDERENGINE_COLORS_H
#define RENDERENGINE_COLORS_H

#include "vector.h"

using Color = Vector4f;

namespace Colors {
    const Color Black{0, 0, 0, 1};
    const Color White{1, 1, 1, 1};
    const Color Red{1, 0, 0, 1};
    const Color Green{0, 1, 0, 1};
    const Color Blue{0, 0, 1, 1};
    const Color Yellow{1, 1, 0, 1};
    const Color Cyan{0, 1, 1, 1};
    const Color Magenta{1, 0, 1, 1};
    const Color Gray{0.5, 0.5, 0.5, 1};
    const Color LightGray{0.75, 0.75, 0.75, 1};
    const Color DarkGray{0.25, 0.25, 0.25, 1};
}
#endif //RENDERENGINE_COLORS_H
