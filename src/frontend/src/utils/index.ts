export function randomEngineName() {
    const rtn = Math.random().toString(36).substring(2, 8);
    sessionStorage.setItem("engine_name", rtn);
    return rtn;
}
