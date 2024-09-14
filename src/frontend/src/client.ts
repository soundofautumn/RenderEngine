import axios from "axios";
import { randomEngineName } from "./utils";

export const backend_endpoint = "localhost:3000";

export const engine_name = randomEngineName();

export const fps = 1;

export const client = axios.create({
    baseURL: "http://" + backend_endpoint,
    method: "POST",
    headers: {
        "Content-Type": "application/json",
        "Engine-Name": engine_name,
    },
});
