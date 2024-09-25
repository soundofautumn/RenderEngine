import axios from "axios";
import { randomEngineName } from "./utils";

export const backend_endpoint =
    import.meta.env.VITE_RENDER_ENGINE_ENDPOINT || "localhost:3000";

export const engine_name = randomEngineName();

export const engine_fps = 30;

export const client = axios.create({
    baseURL: "http://" + backend_endpoint,
    method: "POST",
    headers: {
        "Content-Type": "application/json",
        "Engine-Name": engine_name,
    },
});
