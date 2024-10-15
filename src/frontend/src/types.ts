export interface IPoint {
    x: number;
    y: number;
    type?:
        | "click"
        | "current"
        | "drag"
        | "sliding"
        | "ending"
        | "dragable"
        | "view"
        | "bounder"
        | "center"
        | "rotate";
}

export interface IDrawFuncParams {
    pointers: IPoint[];
    color?: string;
    algorithm?: number;
}

export type IDrawApiParamType = "point" | "func" | "multi_points" | "unknown";

export interface IDrawApiParam {
    type: IDrawApiParamType;
    name?: string;
    func?: (...pointers: IPoint[]) => number | object;
}

export interface IPrimitive {
    apiEndpoint: string;
    params: (IDrawApiParam & {
        value: any;
    })[];
    index: number;
    type?: string;
}
