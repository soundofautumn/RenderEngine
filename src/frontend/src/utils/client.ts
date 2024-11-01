import axios from "axios";
import { randomEngineName } from ".";

export const backend_endpoint =
    import.meta.env.VITE_RENDER_ENGINE_ENDPOINT || "localhost:3000";

export const engine_name =
    sessionStorage.getItem("engine_name") || randomEngineName();

export const game_engine_name = randomEngineName();

export const engine_fps = 30;

export const game_engine_fps = 3;

export const retry_max_times = 114514;

export const client = axios.create({
    baseURL: "//" + backend_endpoint,
    method: "POST",
    headers: {
        "Content-Type": "application/json",
        "Engine-Name": engine_name,
    },
});
