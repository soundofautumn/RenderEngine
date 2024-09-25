export interface IPoint {
    x: number;
    y: number;
    type?: "click" | "current" | "drag" | "sliding" | "ending";
}
