//
// Created by Autumn Sound on 2024/9/25.
//

#ifndef RENDERENGINE_CONFIG_HPP
#define RENDERENGINE_CONFIG_HPP

#ifdef RENDERENGINE_UNIMPLEMENTED
#undef RENDERENGINE_UNIMPLEMENTED
#endif  // RENDERENGINE_UNIMPLEMENTED

#ifdef RENDERENGINE_DEBUG
#define RENDERENGINE_UNIMPLEMENTED throw std::runtime_error("Not implemented");
#else
#define RENDERENGINE_UNIMPLEMENTED void(0);
#endif  // RENDERENGINE_DEBUG

namespace RenderCore {

template <typename... Args>
void ignore_unused(Args&&...) {}

template <typename... Args>
void ignore_unused() {}

}  // namespace RenderCore

#endif  // RENDERENGINE_CONFIG_HPP
